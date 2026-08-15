#include "calc_utils/polygon_queries2d.hpp"

#include "calc_utils/convex_hull2d.hpp"
#include "line2d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <type_traits>

namespace geompp {

inline namespace geometry {
namespace detail {

int winding_number(std::vector<Point2D> const& vertices, Point2D const& p) {
  int wn = 0;
  int n = static_cast<int>(vertices.size());
  for (int i1 = 0; i1 < n; ++i1) {
    int i2 = (i1 + 1) % n;
    auto const& v1 = vertices[i1];
    auto const& v2 = vertices[i2];
    if (compare(v1.y(), p.y()) <= 0) {   // edge starts below p
      if (compare(v2.y(), p.y()) > 0) {  //   ends above p (upward crossing)
        if (is_left(v1, v2, p)) {        //   p left of the edge
          ++wn;
        }
      }
    } else {                              // edge starts above p
      if (compare(v2.y(), p.y()) <= 0) {  //   ends below p (downward crossing)
        if (is_right(v1, v2, p)) {        //   p right of the edge
          --wn;
        }
      }
    }
  }
  return wn;
}

namespace view {

template <PointContainer Points>
bool is_on_perimeter(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py) {
  Point2D test_pt(px, py);
  auto check_ring = [&](auto const& ring) -> bool {
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      Point2D v1(view.x(ring[i]), view.y(ring[i]));
      Point2D v2(view.x(ring[(i + 1) % n]), view.y(ring[(i + 1) % n]));
      if (LineSegment2D::Make(v1, v2).Contains(test_pt)) {
        return true;
      }
    }
    return false;
  };
  if (check_ring(outer)) {
    return true;
  }
  for (auto const& hole : holes) {
    if (check_ring(hole)) {
      return true;
    }
  }
  return false;
}

template bool is_on_perimeter(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&, View2D const&,
                              double, double);
template bool is_on_perimeter(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&, View2D const&,
                              double, double);

template <PointContainer Points>
bool polygon_contains(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py) {
  auto ring_winding = [&](auto const& ring) -> int {
    int wn = 0;
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      double v1y = view.y(ring[i]);
      double v2y = view.y(ring[(i + 1) % n]);
      if (compare(v1y, py) <= 0) {
        if (compare(v2y, py) > 0) {
          double v1x = view.x(ring[i]), v2x = view.x(ring[(i + 1) % n]);
          if (compare((v2x - v1x) * (py - v1y) - (v2y - v1y) * (px - v1x), 0.0) > 0) {
            ++wn;
          }
        }
      } else {
        if (compare(v2y, py) <= 0) {
          double v1x = view.x(ring[i]), v2x = view.x(ring[(i + 1) % n]);
          if (compare((v2x - v1x) * (py - v1y) - (v2y - v1y) * (px - v1x), 0.0) < 0) {
            --wn;
          }
        }
      }
    }
    return wn;
  };

  int wn = ring_winding(outer);
  if (wn == 0) {  // early terminate if outside
    return false;
  }

  for (auto const& hole : holes) {
    wn += ring_winding(hole);

    if (wn == 0) {  // since valid holes cannot overlap each other
                    // early terminate if inside one of the holes
      return false;
    }
  }
  return wn != 0;
}

template bool polygon_contains(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&, View2D const&,
                               double, double);
template bool polygon_contains(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&, View2D const&,
                               double, double);

template <PointContainer Points, Point P>
std::vector<std::pair<double, double>> compute_parametric_intersection_intervals(
    Points const& outer_coplanar_ccw, std::vector<Points> const& holes_coplanar_cw, bool is_convex_input,
    P const& line_p0, P const& line_p1, View2D const& view) {
  // Debug-mode invariant checks. Disabled in Release (NDEBUG defined). Qualified as geompp::are_ccw/
  // are_coplanar/are_cw (not plain unqualified calls): this function lives inside namespace detail, so an
  // unqualified call would be ambiguous between this scope's own detail:: overloads and the public ones
  // pulled in via ADL on Points::value_type -- and these asserts exist specifically to VERIFY the
  // collinear-free/CCW/coplanar invariant the caller claims, so the full-checking public path is correct.
  assert(geompp::are_ccw(outer_coplanar_ccw));
  if constexpr (std::is_same_v<typename Points::value_type, Point3D>) {
    assert(geompp::are_coplanar(outer_coplanar_ccw));
  }
  assert(!is_convex_input || is_convex(outer_coplanar_ccw, view));
  assert(!is_convex_input || holes_coplanar_cw.empty());
  for (auto const& hole : holes_coplanar_cw) {
    assert(geompp::are_cw(hole));
    if constexpr (std::is_same_v<typename Points::value_type, Point3D>) {
      assert(geompp::are_coplanar(hole));
    }
  }

  // actual algorithm

  // Project the line endpoints to 2D once; all geometry lives in this view's plane.
  // edge e(i): v_0 = outer[i], v_1 = outer[(i+1)%n]  (CCW order)
  //   ex = v1x - v0x,  ey = v1y - v0y
  //   outward normal for CCW ring: n = (ey, -ex)
  //   N = -n · (P0 - Vi)  =  -(ey, -ex) · (p0x - v0x,  p0y - v0y)
  //                       = -(ey * (p0x - v0x) + (-ex) * (p0y - v0y))
  //                       = -(p0x - v0x) * ey + (p0y - v0y)*ex
  //   D = n · (P1 - P0)   =  dx*ey - dy*ex
  double p0x = view.x(line_p0), p0y = view.y(line_p0);
  double dx = view.x(line_p1) - p0x;
  double dy = view.y(line_p1) - p0y;

  auto edge_ND = [&](P const& v0, P const& v1) -> std::pair<double, double> {
    double v0x = view.x(v0), v0y = view.y(v0);
    double ex = view.x(v1) - v0x, ey = view.y(v1) - v0y;
    double N = -(p0x - v0x) * ey + (p0y - v0y) * ex;
    double D = dx * ey - dy * ex;
    return {N, D};
  };

  std::vector<std::pair<double, double>> t_list;

  if (is_convex_input) {  // quick exit on entering and leaving edges
    double t_e = std::numeric_limits<double>::min();
    bool is_t_e_set = false;
    double t_l = std::numeric_limits<double>::max();
    bool is_t_l_set = false;
    std::size_t n = outer_coplanar_ccw.size();

    for (std::size_t i = 0; i < n; ++i) {
      auto const& v_0 = outer_coplanar_ccw[i];
      auto const& v_1 = outer_coplanar_ccw[(i + 1) % n];
      auto [N, D] = edge_ND(v_0, v_1);

      auto D_compare_to_0 = compare(D, 0);
      if (D_compare_to_0 == 0) {  // line is parallel to edge e(i)
        if (compare(N, 0) < 0) {  // P0 is outside the edge e(i)
          break;                  // early terminate (valid for convex: one miss = total miss)

        } else {
          continue;  // ignore edge e(i)
        }
      }

      double t = N / D;
      if (D_compare_to_0 < 0) {  // the line is ENTERING polygon through the edge e(i)
        if (!is_t_e_set) {       // init
          t_e = t;
          is_t_e_set = true;

        } else {
          if (compare(t, t_e) > 0) {  // update our t_e to the greatest of the old and new value
            t_e = t;
          }
        }

        if (is_t_e_set && is_t_l_set &&  // if both are set
            compare(t_e, t_l) > 0) {     // quick rejection if not t_e < t_l
          break;
        }

      } else {              // the line is LEAVING polygon through the edge e(i), on D_compare_to_0 > 0
        if (!is_t_l_set) {  // init
          t_l = t;
          is_t_l_set = true;

        } else {
          if (compare(t, t_l) < 0) {  // update our t_l to the smallest of the old and new value
            t_l = t;
          }
        }

        if (is_t_e_set && is_t_l_set &&  // if both are set
            compare(t_e, t_l) > 0) {     // quick rejection if not t_e < t_l
          break;
        }
      }
    }

    // only if we have both an ENTERING edge and a LEAVING edge we have an intersection,
    // otherwise it's an overlap (OnPerimeter)
    if (is_t_e_set && is_t_l_set && compare(t_e, t_l) < 0) {
      t_list.emplace_back(t_e, t_l);
    }

  } else {  // Collect, Sort, Parity

    // we don't need to care about separating t_e from t_l
    // the Jordan Curve Theorem guarantees that an infinite line crossing a closed shape will always alternate:
    // Enter-> Leave-> Enter -> Leave, always paired up
    // (try it graphically on a piece of paper: outer CCW and inner CW guarantee this)

    // the list of ts will always have an even number
    // (mathematically guaranteed if we close the for-loop of points)
    // so we can expect to always have something like this once sorted (<t_e, t_l>, <t_e, t_l>, <t_e, t_l>, ...)

    // special case: overlap with vertex: the values of (sorted) t_e and t_l are equal: we will remove them

    std::vector<double> t_all;
    std::size_t n = outer_coplanar_ccw.size();

    for (std::size_t i = 0; i < n; ++i) {
      auto const& v_0 = outer_coplanar_ccw[i];
      auto const& v_1 = outer_coplanar_ccw[(i + 1) % n];  // guarantees the loop of points to be closed
      auto [N, D] = edge_ND(v_0, v_1);

      auto D_compare_to_0 = compare(D, 0);
      if (D_compare_to_0 == 0) {  // line is parallel to edge e(i)
        continue;                 // skip — parallel to one edge does not mean the line misses the whole concave polygon
      }

      double t = N / D;
      t_all.push_back(t);
    }

    // Hole edges must feed the same t_all list as the outer ring: the Jordan Curve Theorem guarantee
    // above (outer CCW + inner CW -> crossings always alternate solid/hole/solid) only holds once every
    // boundary ring -- not just the outer one -- has contributed its crossings.
    for (auto const& hole : holes_coplanar_cw) {
      std::size_t nh = std::ranges::size(hole);
      for (std::size_t i = 0; i < nh; ++i) {
        auto const& v_0 = hole[i];
        auto const& v_1 = hole[(i + 1) % nh];
        auto [N, D] = edge_ND(v_0, v_1);

        auto D_compare_to_0 = compare(D, 0);
        if (D_compare_to_0 == 0) {
          continue;
        }

        double t = N / D;
        t_all.push_back(t);
      }
    }

    // sort the list
    if (!t_all.empty()) {
      // sort ASC, using the std::partial_ordering and the optimized ranges algorithm
      std::ranges::sort(t_all);
      // remove consecutive duplicates
      remove_all_duplicated_elements(t_all);

      if (!t_all.empty()) {
        std::size_t m = t_all.size();
        if (m % 2 != 0) {  // impossible (mathematically) to get an odd number
          throw std::runtime_error("found an odd number of t_all intervals in sorted vector");
        }

        for (std::size_t i = 0; i < m - 1; i += 2) {
          t_list.emplace_back(t_all[i], t_all[i + 1]);
        }
      }
    }
  }

  return t_list;
}

std::vector<std::pair<double, double>> compute_intersection_intervals_2d(
    std::vector<Point2D> const& outer_coplanar_ccw, std::vector<std::vector<Point2D>> const& holes_coplanar_cw,
    bool is_convex_input, Point2D const& line_p0, Point2D const& line_p1, View2D const& view) {
  return compute_parametric_intersection_intervals(outer_coplanar_ccw, holes_coplanar_cw, is_convex_input, line_p0,
                                                   line_p1, view);
}

std::vector<std::pair<double, double>> compute_intersection_intervals_3d(
    std::vector<Point3D> const& outer_coplanar_ccw, std::vector<std::vector<Point3D>> const& holes_coplanar_cw,
    bool is_convex_input, Point3D const& line_p0, Point3D const& line_p1, View2D const& view) {
  return compute_parametric_intersection_intervals(outer_coplanar_ccw, holes_coplanar_cw, is_convex_input, line_p0,
                                                   line_p1, view);
}

template <PointContainer Points, Point P>
double distance_to(Points const& outer_loop, bool is_convex, P const& line_p0, P const& line_p1, View2D const& view) {
  std::size_t n = std::ranges::size(outer_loop);
  if (n == 0) {
    throw std::invalid_argument("distance_to: empty vertex range");
  }

  double lx0 = view.x(line_p0), ly0 = view.y(line_p0);
  double ldx = view.x(line_p1) - lx0, ldy = view.y(line_p1) - ly0;
  double llen = std::sqrt(ldx * ldx + ldy * ldy);
  if (compare(llen, 0.0) == 0) {
    throw std::invalid_argument("distance_to: line_p0, line_p1 project onto a zero-length line");
  }
  double nx = -ldy / llen, ny = ldx / llen;  // unit, perpendicular to the (projected) line

  auto signed_dist = [&](std::size_t i) {
    return (view.x(outer_loop[i]) - lx0) * nx + (view.y(outer_loop[i]) - ly0) * ny;
  };

  // convex: binary search based O(LogN) algorithm
  if (is_convex) {
    // Daniel Sunday's O(log n) extreme-vertex binary search (cf. detail::extreme_points), reimplemented
    // here on view-projected scalars: view.x()/y() have no native-vector equivalent to dot a direction
    // against, so the native detail::extreme_points can't be reused for the projected case.
    long const ln = static_cast<long>(n);
    auto proj = [&](long i) { return signed_dist(static_cast<std::size_t>(((i % ln) + ln) % ln)); };
    auto sgn = [](double d) -> int { return static_cast<int>(d > 0) - static_cast<int>(d < 0); };
    auto search = [&](double s) -> std::size_t {
      auto g = [&](long i) { return s * proj(i); };
      auto cmp = [&](long i, long j) { return sgn(g(j) - g(i)); };
      auto extr = [&](long i) { return cmp(i + 1, i) >= 0 && cmp(i, i - 1) < 0; };
      if (extr(0)) {
        return 0;
      }
      long lo = 0, hi = ln;
      while (lo + 1 < hi) {
        long m = (lo + hi) / 2;
        if (extr(m)) {
          return static_cast<std::size_t>(m);
        }
        int ls = cmp(lo + 1, lo);
        int ms = cmp(m + 1, m);
        bool go_hi = (ls < ms) || (ls == ms && ls == cmp(lo, m));
        if (go_hi) {
          hi = m;
        } else {
          lo = m;
        }
      }
      return static_cast<std::size_t>(lo);
    };

    double d_max = signed_dist(search(1.0));
    double d_min = signed_dist(search(-1.0));

    // polygon straddles the line if the extreme signed offsets have opposite sign (or touch)
    if (compare(d_min, 0) <= 0 && compare(d_max, 0) >= 0) {
      return 0.0;
    }

    return std::min(std::abs(d_min), std::abs(d_max));
  }

  // non convex: brute force O(N) algorithm
  double min_d = std::numeric_limits<double>::infinity();
  for (std::size_t i = 0; i < n; ++i) {
    double d0 = signed_dist(i);
    double d1 = signed_dist((i + 1) % n);

    // the signed distance is affine along the edge, so its rate of change is exactly d1 - d0;
    // solving d0 + t*(d1-d0) = 0 gives the parametric crossing point without any extra dot products
    double denom = d1 - d0;
    if (compare(denom, 0.0) != 0) {
      double t = -d0 / denom;
      if (is_in_range(t, 0.0, 1.0)) {
        return 0.0;  // this edge crosses the line
      }
    }

    // no crossing: the closest point on this edge to the line is one of its endpoints
    min_d = std::min({min_d, std::abs(d0), std::abs(d1)});
  }

  return min_d;
}

template double distance_to(std::vector<Point2D> const&, bool, Point2D const&, Point2D const&, View2D const&);
template double distance_to(std::vector<Point3D> const&, bool, Point3D const&, Point3D const&, View2D const&);

}  // namespace view

double distance_to(std::vector<Point2D> const& outer_loop, bool is_convex, Point2D const& line_p0,
                   Point2D const& line_p1) {
  return view::distance_to(outer_loop, is_convex, line_p0, line_p1, View2D::XY());
}

namespace view {

namespace {

// Cross-product orientation tests, projected through `view` (dimension-agnostic: works for Point2D or Point3D).
template <Point P>
bool is_left(P const& v1, P const& v2, P const& p, View2D const& view) {
  double cross =
      (view.x(v2) - view.x(v1)) * (view.y(p) - view.y(v1)) - (view.y(v2) - view.y(v1)) * (view.x(p) - view.x(v1));
  return compare(cross, 0.0) > 0;
}

template <Point P>
bool is_right(P const& v1, P const& v2, P const& p, View2D const& view) {
  double cross =
      (view.x(v2) - view.x(v1)) * (view.y(p) - view.y(v1)) - (view.y(v2) - view.y(v1)) * (view.x(p) - view.x(v1));
  return compare(cross, 0.0) < 0;
}

// tells if v1 is above v2, with respect to point p (ref)
template <Point P>
bool is_above(P const& p, P const& v1, P const& v2, View2D const& view) {
  return is_left(p, v1, v2, view);
}
// tells if v1 is below v2, with respect to point p (ref)
template <Point P>
bool is_below(P const& p, P const& v1, P const& v2, View2D const& view) {
  return is_right(p, v1, v2, view);
}

/// @brief finds the index of outer_loop corresponding to the RIGHT tangent from point p (Dan Sunday's O(log n)
/// binary search). Assumes outer_loop is CONVEX and CCW, projected through `view`.
template <PointContainer Points, Point P>
std::size_t r_tangent(Points const& outer_loop, P const& p, View2D const& view) {
  std::size_t n = outer_loop.size();
  if (n < 3) {
    throw std::invalid_argument("r_tangent requires at least 3 vertices");
  }

  // test if the first vertex is a local max
  if (is_below(p, outer_loop[1], outer_loop[0], view) && !is_above(p, outer_loop[n - 1], outer_loop[0], view)) {
    return 0;
  }

  std::size_t a = 0, b = n;
  std::size_t max_iter = n + 1;  // each iteration strictly halves [a,b], so this is a generous safety bound
  while (max_iter-- > 0) {
    if (b - a <= 1) {  // narrowed to a single edge: whichever endpoint is more "above" wins
      return is_above(p, outer_loop[a % n], outer_loop[b % n], view) ? a % n : b % n;
    }
    std::size_t c = (a + b) / 2;
    bool dnC = is_below(p, outer_loop[(c + 1) % n], outer_loop[c % n], view);
    if (dnC && !is_above(p, outer_loop[(c + n - 1) % n], outer_loop[c % n], view)) {  // found the max tangent point
      return c % n;
    }
    // no max yet, continue the binary search, in either sub-chain [a,c] or [c,b]
    bool upA = is_above(p, outer_loop[(a + 1) % n], outer_loop[a % n], view);
    if (upA) {
      if (dnC) {
        b = c;
      } else if (is_above(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    } else {
      if (!dnC) {
        a = c;
      } else if (is_below(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    }
  }

  throw std::logic_error("loop above max_iter without finding c in binary search (r_tangent)");
}

/// @brief finds the index of outer_loop corresponding to the LEFT tangent from point p (Dan Sunday's O(log n)
/// binary search). Assumes outer_loop is CONVEX and CCW, projected through `view`.
template <PointContainer Points, Point P>
std::size_t l_tangent(Points const& outer_loop, P const& p, View2D const& view) {
  std::size_t n = outer_loop.size();
  if (n < 3) {
    throw std::invalid_argument("l_tangent requires at least 3 vertices");
  }

  // test if the first vertex is a local min
  if (!is_below(p, outer_loop[1], outer_loop[0], view) && is_above(p, outer_loop[n - 1], outer_loop[0], view)) {
    return 0;
  }

  std::size_t a = 0, b = n;
  std::size_t max_iter = n + 1;  // each iteration strictly halves [a,b], so this is a generous safety bound
  while (max_iter-- > 0) {
    if (b - a <= 1) {  // narrowed to a single edge: whichever endpoint is more "below" wins
      return is_below(p, outer_loop[a % n], outer_loop[b % n], view) ? a % n : b % n;
    }
    std::size_t c = (a + b) / 2;
    bool dnC = is_below(p, outer_loop[(c + 1) % n], outer_loop[c % n], view);
    if (!dnC && is_above(p, outer_loop[(c + n - 1) % n], outer_loop[c % n], view)) {  // found the min tangent point
      return c % n;
    }
    // no min yet, continue the binary search, in either sub-chain [a,c] or [c,b]
    bool dnA = is_below(p, outer_loop[(a + 1) % n], outer_loop[a % n], view);
    if (dnA) {
      if (!dnC) {
        b = c;
      } else if (is_below(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    } else {
      if (dnC) {
        a = c;
      } else if (is_above(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    }
  }

  throw std::logic_error("loop above max_iter without finding c in binary search (l_tangent)");
}

// Reduces `loop` to its convex hull (projected through `view`) when it isn't already convex. Returns the hull
// points alongside the indices (into the ORIGINAL loop) each hull point came from, so callers can map hull-local
// results back to the caller's index space. When `already_convex`, both are trivial identity pass-throughs.
template <PointContainer Points>
std::pair<std::vector<std::ranges::range_value_t<Points>>, std::vector<std::size_t>> to_convex_loop(
    Points const& loop, bool already_convex, View2D const& view) {
  using PointT = std::ranges::range_value_t<Points>;

  std::vector<std::size_t> idx;
  if (already_convex) {
    idx.resize(loop.size());
    std::iota(idx.begin(), idx.end(), std::size_t{0});
  } else {
    idx = convex_hull_monotone_chain(loop, view);
  }

  std::vector<PointT> pts;
  pts.reserve(idx.size());
  for (auto i : idx) {
    pts.push_back(loop[i]);
  }
  return {std::move(pts), std::move(idx)};
}

}  // namespace

template <PointContainer Points, Point P>
std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(Points const& outer_loop, bool is_convex, P const& p,
                                                             View2D const& view) {
  // if the polygon is convex we can use the binary search, O(logN)
  if (is_convex) {
    return {l_tangent(outer_loop, p, view), r_tangent(outer_loop, p, view)};
  }

  // otherwise reduce to the convex hull first — a tangent from an external point can only ever touch a hull
  // vertex — then map the hull-local result back to an index into the original outer_loop
  auto [hull_pts, hull_idx] = to_convex_loop(outer_loop, /*already_convex=*/false, view);
  return {hull_idx[l_tangent(hull_pts, p, view)], hull_idx[r_tangent(hull_pts, p, view)]};
}

template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const&, bool, Point2D const&,
                                                                      View2D const&);
template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point3D> const&, bool, Point3D const&,
                                                                      View2D const&);

template <PointContainer Points>
std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(Points const& loop1, bool is_convex1, Points const& loop2,
                                                            bool is_convex2, View2D const& view) {
  auto [cv1, idx1] = to_convex_loop(loop1, is_convex1, view);
  auto [cv2, idx2] = to_convex_loop(loop2, is_convex2, view);

  std::size_t m = cv1.size();
  std::size_t n = cv2.size();
  if (m < 3 || n < 3) {
    throw std::invalid_argument("poly_poly_RL_tangent_to requires polygons with at least 3 vertices");
  }

  std::size_t i1 = r_tangent(cv1, cv2[0], view);
  std::size_t i2 = l_tangent(cv2, cv1[i1], view);

  std::size_t max_iter = m * n + 1;
  bool done = false;
  while (!done && max_iter-- > 0) {
    done = true;

    std::size_t guard1 = 0;
    while (!is_left(cv2[i2], cv1[i1], cv1[(i1 + 1) % m], view)) {
      i1 = (i1 + 1) % m;
      if (++guard1 > m) {
        throw std::logic_error("poly_poly_RL_tangent_to: i1 walk failed to converge");
      }
    }

    std::size_t guard2 = 0;
    while (!is_right(cv1[i1], cv2[i2], cv2[(i2 + n - 1) % n], view)) {
      i2 = (i2 + n - 1) % n;
      done = false;
      if (++guard2 > n) {
        throw std::logic_error("poly_poly_RL_tangent_to: i2 walk failed to converge");
      }
    }
  }
  if (!done) {
    throw std::logic_error("poly_poly_RL_tangent_to exceeded max iterations without converging");
  }

  return {idx1[i1], idx2[i2]};
}

template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const&, bool,
                                                                     std::vector<Point2D> const&, bool, View2D const&);
template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point3D> const&, bool,
                                                                     std::vector<Point3D> const&, bool, View2D const&);

}  // namespace view

std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const& outer_loop, bool is_convex,
                                                             Point2D const& p) {
  return view::point_poly_tangent_lr_to(outer_loop, is_convex, p, View2D::XY());
}

std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const& loop1, bool is_convex1,
                                                            std::vector<Point2D> const& loop2, bool is_convex2) {
  return view::poly_poly_RL_tangent_to(loop1, is_convex1, loop2, is_convex2, View2D::XY());
}

}  // namespace detail

double distance_to(Polygon2D const& polygon, Line2D const& line) {
  return detail::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.First(), line.Last());
}

PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Point2D const& p) {
  auto [left_i, right_i] = detail::point_poly_tangent_lr_to(polygon.Perimeter(), polygon.IsConvex(), p);
  return {LineSegment2D::Make(p, polygon[left_i]), LineSegment2D::Make(p, polygon[right_i])};
}

PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Polygon2D const& other) {
  auto [RL_poly_i, RL_other_i] =
      detail::poly_poly_RL_tangent_to(polygon.Perimeter(), polygon.IsConvex(), other.Perimeter(), other.IsConvex());
  auto [LR_other_i, LR_poly_i] =
      detail::poly_poly_RL_tangent_to(other.Perimeter(), other.IsConvex(), polygon.Perimeter(), polygon.IsConvex());
  return {LineSegment2D::Make(polygon[RL_poly_i], other[RL_other_i]),
          LineSegment2D::Make(polygon[LR_poly_i], other[LR_other_i])};
}

}  // namespace geometry

}  // namespace geompp
