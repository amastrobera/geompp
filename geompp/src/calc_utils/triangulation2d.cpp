#include "calc_utils/triangulation2d.hpp"

#include "calc_utils/convex_hull2d.hpp"
#include "calc_utils/convex_hull3d.hpp"
#include "calc_utils/polygon_ops2d.hpp"
#include "calc_utils/polygon_queries2d.hpp"
#include "calc_utils/triangulation3d.hpp"
#include "grid_cell2d.hpp"
#include "grid_cell3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "triangle2d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace geompp {

inline namespace geometry {
namespace detail {

namespace view {

namespace helpers {

// helper lambdas
template <typename PointT>
double area2(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  auto [x0, y0] = view.xy(a);
  auto [x1, y1] = view.xy(b);
  auto [x2, y2] = view.xy(c);
  return (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
}

template <typename PointT>
bool is_reflex(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  return compare(helpers::area2(a, b, c, view), 0) < 0;
}

// Inclusive (>= 0, not > 0): a point exactly ON one of the candidate ear's edges must still
// disqualify it, not just a point strictly inside. Without this, a non-adjacent vertex that happens
// to be collinear with two of the ear's vertices (e.g. a reflex vertex sitting exactly on the
// prev-next diagonal, as in an L-shaped polygon whose notch corner lies on that diagonal) is
// invisible to a strict test, so the algorithm accepts a diagonal that actually exits the polygon.
template <typename PointT>
bool is_left_or_on(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  auto u_x = view.x(b) - view.x(a);
  auto u_y = view.y(b) - view.y(a);

  auto v_x = view.x(c) - view.x(a);
  auto v_y = view.y(c) - view.y(a);

  return compare(u_x * v_y - u_y * v_x, 0) >= 0;
};

template <typename PointT>
bool is_point_in_triangle(PointT const& a, PointT const& b, PointT const& c, PointT const& p, View2D const& view) {
  return helpers::is_left_or_on(a, b, p, view) && helpers::is_left_or_on(b, c, p, view) &&
         helpers::is_left_or_on(c, a, p, view);
};

template <typename PointT>
bool are_collinear(PointT const& a, PointT const& b, PointT const& c, View2D const& view) {
  return compare(helpers::area2(a, b, c, view), 0.0) == 0;
};

}  // namespace helpers

// helper predicates

template <typename PointT>
bool is_y_monotone(std::vector<PointT> const& input, View2D const& view) {
  std::size_t n = input.size();
  if (n < 3) {
    return true;
  }
  int max_count = 0, min_count = 0;
  for (std::size_t i = 0; i < n; ++i) {
    double y_curr = view.y(input[i]);
    double y_prev = view.y(input[(i + n - 1) % n]);
    double y_next = view.y(input[(i + 1) % n]);
    if (compare(y_curr, y_prev) > 0 && compare(y_curr, y_next) > 0) {
      ++max_count;
    }
    if (compare(y_curr, y_prev) < 0 && compare(y_curr, y_next) < 0) {
      ++min_count;
    }
  }
  return max_count <= 1 && min_count <= 1;
}

template bool is_y_monotone(std::vector<Point2D> const& input, View2D const& view);
template bool is_y_monotone(std::vector<Point3D> const& input, View2D const& view);

template <typename PointT>
bool in_circumcircle(PointT const& a, PointT const& b, PointT const& c, PointT const& p, View2D const& view) {
  double ax = view.x(a) - view.x(p);
  double ay = view.y(a) - view.y(p);
  double bx = view.x(b) - view.x(p);
  double by = view.y(b) - view.y(p);
  double cx = view.x(c) - view.x(p);
  double cy = view.y(c) - view.y(p);
  double det = ax * (by * (cx * cx + cy * cy) - cy * (bx * bx + by * by)) -
               ay * (bx * (cx * cx + cy * cy) - cx * (bx * bx + by * by)) +
               (ax * ax + ay * ay) * (bx * cy - by * cx);
  return compare(det, 0.0) > 0;
}

template bool in_circumcircle(Point2D const& a, Point2D const& b, Point2D const& c, Point2D const& p,
                               View2D const& view);
template bool in_circumcircle(Point3D const& a, Point3D const& b, Point3D const& c, Point3D const& p,
                               View2D const& view);

// triangulation functions

// EarClipping: walks the ring and clips the first valid ear it finds, in traversal order. O(n^2)
// worst case, but very often close to O(n) in practice for well-behaved (mostly-convex) polygons,
// since clipping at i frequently leaves i_next immediately clippable too. Doesn't optimize triangle
// shape -- a legitimate but geometrically thin ear can get clipped just because it was encountered
// first, producing a sliver triangle even on perfectly ordinary input. See EarClippingBestFit below
// for the shape-aware alternative and its own, steeper cost.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<PointT> const& input,
                                                                   View2D const& view) {
  std::size_t n = input.size();

  // temporary container for output (excellent for unknown size list)
  std::deque<std::array<std::size_t, 3>> triangles;

  // temporary containers for calculations
  //  \_ previous and next indices (keep them as they are)
  std::vector<std::size_t> next_id(n), prev_id(n);
  for (std::size_t i = 0; i < n; ++i) {
    next_id[i] = (i + 1) % n;
    prev_id[i] = (i + n - 1) % n;
  }

  //  \_ reflex indices to analyze (reduce with a "swap and pop", the order doesn't matter)
  // NOTE: this set also includes exactly-flat (180 degree, collinear-with-neighbors) vertices, not
  // just strictly-reflex ones. The "only reflex vertices can lie inside a candidate ear" theorem the
  // is_ear scan below relies on assumes a proper simple polygon with no 3 consecutive collinear
  // points; a flat vertex is neither reflex nor a valid ear itself, but it CAN sit exactly on another
  // candidate ear's boundary (is_point_in_triangle is inclusive of the boundary), so it must still be
  // scanned as a blocker or that ear gets wrongly accepted and the flat vertex's own two edges never
  // make it into the triangulation. Under Collinearity::Enforce/Assert this never comes up (no flat
  // vertices reach here); Guaranteed callers (e.g. fix_adjacency()'s re-triangulation of a facet with
  // a spliced-in collinear vertex) are exactly the case this guards.
  std::deque<std::size_t> reflex_indices;
  std::vector<std::uint8_t> is_reflex(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    if (helpers::is_reflex(input[prev_id[i]], input[i], input[next_id[i]], view) ||
        helpers::are_collinear(input[prev_id[i]], input[i], input[next_id[i]], view)) {
      reflex_indices.push_back(i);
      is_reflex[i] = 1;
    }
  }

  auto lambda_modify_reflex_status = [&](std::size_t idx) {
    bool is_now_reflex = helpers::is_reflex(input[prev_id[idx]], input[idx], input[next_id[idx]], view) ||
                         helpers::are_collinear(input[prev_id[idx]], input[idx], input[next_id[idx]], view);

    if (!is_now_reflex && is_reflex[idx]) {  // the opposite can never happen by a theorem: once an ear is cut,
                                             // the prev/next vertex can become convex, but never reflex
      auto it = std::find(reflex_indices.begin(), reflex_indices.end(), idx);
      if (it != reflex_indices.end()) {
        // remove with a quick O(1) swap and pop
        *it = reflex_indices.back();
        reflex_indices.pop_back();
      }

      is_reflex[idx] = 0;
    }
  };

  // main loop: while we have more than 3 vertices, try to find an ear and clip it
  std::size_t i = 0, i_prev = prev_id[i], i_next = next_id[i];
  while (n > 3) {
    if (!is_reflex[i]) {  // the triplet could be an ear
      PointT const& p_cur = input[i];
      PointT const& p_prev = input[i_prev];
      PointT const& p_next = input[i_next];

      // it can happen that this vertex is not reflex, but also not convex!
      // the points may be collinear, making the triangle check fail, we want to avoid that
      if (!helpers::are_collinear(p_prev, p_cur, p_next, view)) {
        // check if the triangle is an ear
        bool is_ear = true;
        for (auto const& j : reflex_indices) {
          if (j == i_prev || j == i_next) {  // quick exit (we don't care of the prev/next indices to be reflex)
            continue;
          }
          if (helpers::is_point_in_triangle(p_prev, p_cur, p_next, input[j], view)) {
            is_ear = false;
            break;
          }
        }

        if (is_ear) {
          triangles.push_back({i_prev, i, i_next});

          // set the prev/next indices to skip the current vertex
          next_id[i_prev] = i_next;
          prev_id[i_next] = i_prev;

          // update reflex status of the previous and next vertices
          lambda_modify_reflex_status(i_prev);
          lambda_modify_reflex_status(i_next);

          --n;
        }
      }
    }

    // update the index to the next vertex
    i = i_next;
    i_prev = prev_id[i];
    i_next = next_id[i];
  }

  // exactly 3 vertices remain, still linked via prev_id/next_id — the loop above only clips ears down to
  // n == 3 and never emits this last, leftover triangle itself.
  triangles.push_back({i_prev, i, i_next});

  // transform the deque in vector (1 allocation, using move)
  std::vector<std::array<std::size_t, 3>> triangles_output(std::make_move_iterator(triangles.begin()),
                                                           std::make_move_iterator(triangles.end()));

  return triangles_output;
}

template std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<Point2D> const& input,
                                                                            View2D const& view);
template std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<Point3D> const& input,
                                                                            View2D const& view);

// EarClippingBestFit: like EarClipping, but each outer-loop iteration does a full lap over the
// *current* ring to find the best-scoring valid ear (by shape quality, not just the first one found)
// before clipping exactly one. Guaranteed termination via the same Two Ears Theorem EarClipping relies
// on -- this only ever reorders which valid ear gets picked, never rejects one outright, so it can't
// get stuck the way a strict angle/area floor would. Trades speed for shape: unconditionally ~O(n^2)
// (a full O(current n) rescan per clip, every time), where EarClipping's O(n^2) is only a worst case.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(std::vector<PointT> const& input,
                                                                            View2D const& view) {
  std::size_t n = input.size();

  // temporary container for output (excellent for unknown size list)
  std::deque<std::array<std::size_t, 3>> triangles;

  // temporary containers for calculations
  //  \_ previous and next indices (keep them as they are)
  std::vector<std::size_t> next_id(n), prev_id(n);
  for (std::size_t i = 0; i < n; ++i) {
    next_id[i] = (i + 1) % n;
    prev_id[i] = (i + n - 1) % n;
  }

  //  \_ reflex indices to analyze (reduce with a "swap and pop", the order doesn't matter)
  // NOTE: this set also includes exactly-flat (180 degree, collinear-with-neighbors) vertices, not
  // just strictly-reflex ones -- see the matching comment in ear_clipping_triangulation() for why a
  // flat vertex must still be scanned as a potential ear-blocker even though it's never a valid ear.
  std::deque<std::size_t> reflex_indices;
  std::vector<std::uint8_t> is_reflex(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    if (helpers::is_reflex(input[prev_id[i]], input[i], input[next_id[i]], view) ||
        helpers::are_collinear(input[prev_id[i]], input[i], input[next_id[i]], view)) {
      reflex_indices.push_back(i);
      is_reflex[i] = 1;
    }
  }

  auto lambda_modify_reflex_status = [&](std::size_t idx) {
    bool is_now_reflex = helpers::is_reflex(input[prev_id[idx]], input[idx], input[next_id[idx]], view) ||
                         helpers::are_collinear(input[prev_id[idx]], input[idx], input[next_id[idx]], view);

    if (!is_now_reflex && is_reflex[idx]) {  // the opposite can never happen by a theorem: once an ear is cut,
                                             // the prev/next vertex can become convex, but never reflex
      auto it = std::find(reflex_indices.begin(), reflex_indices.end(), idx);
      if (it != reflex_indices.end()) {
        // remove with a quick O(1) swap and pop
        *it = reflex_indices.back();
        reflex_indices.pop_back();
      }

      is_reflex[idx] = 0;
    }
  };

  auto lambda_ear_quality = [&view](PointT const& a, PointT const& b, PointT const& c) -> double {
    auto [x0, y0] = view.xy(a);  // p_prev
    auto [x1, y1] = view.xy(b);  // p_cur
    auto [x2, y2] = view.xy(c);  // p_next

    // loose collinearity check already computes
    double area2 = std::abs(helpers::area2(a, b, c, view));

    double a2 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);  // |cur  - next|^2, opposite p_prev
    double b2 = (x2 - x0) * (x2 - x0) + (y2 - y0) * (y2 - y0);  // |prev - next|^2, opposite p_cur
    double c2 = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);  // |cur  - prev|^2, opposite p_next

    // the real formula is
    //    ear_quality = 4*sqrt(3)* abs(cross(a,b,c)/2) / (a*a + b*b + c*c)
    // however we remove 4*, sqrt(3), and /2 to save time
    // because we only need this value for comparison and the real variables are the
    // numerator cross(a,b,c) and the denominator (a2 + b2 + c2)
    return area2 / (a2 + b2 + c2);
  };

  // main loop: while we have more than 3 vertices, try to find an ear and clip it
  std::size_t i = 0, i_prev = prev_id[i], i_next = next_id[i];
  while (n > 3) {  // LOOP(1): main of the Ear Cutting Algorithm

    double best_ear_score = -1.0;  // sin(min-angle) or similar, always >= 0 for a valid ear
    std::array<std::size_t, 3> best_ear;
    std::size_t start = i;

    do {                    // LOOP(2): improvement step making this a greedy algorithm with same O(n2)
                            //          finds the best ear amongst all possible ones (based on area)
      if (!is_reflex[i]) {  // the triplet could be an ear
        PointT const& p_cur = input[i];
        PointT const& p_prev = input[i_prev];
        PointT const& p_next = input[i_next];

        // it can happen that this vertex is not reflex, but also not convex!
        // the points may be collinear, making the triangle check fail, we want to avoid that
        if (!helpers::are_collinear(p_prev, p_cur, p_next, view)) {
          // check if the triangle is an ear
          bool is_ear = true;
          for (auto const& j : reflex_indices) {
            if (j == i_prev || j == i_next) {  // quick exit (we don't care of the prev/next indices to be reflex)
              continue;
            }
            if (helpers::is_point_in_triangle(p_prev, p_cur, p_next, input[j], view)) {
              is_ear = false;
              break;
            }
          }

          if (is_ear) {
            // find the best cuttable ear (by area)
            double score = lambda_ear_quality(p_prev, p_cur, p_next);
            if (compare(score, best_ear_score) > 0) {
              best_ear_score = score;
              best_ear = {i_prev, i, i_next};
            }
          }
        }
      }

      i = i_next, i_prev = prev_id[i], i_next = next_id[i];

    } while (i != start);

    i_prev = best_ear[0], i = best_ear[1], i_next = best_ear[2];

    triangles.push_back(best_ear);

    // set the prev/next indices to skip the current vertex
    next_id[i_prev] = i_next;
    prev_id[i_next] = i_prev;

    // update reflex status of the previous and next vertices
    lambda_modify_reflex_status(i_prev);
    lambda_modify_reflex_status(i_next);

    // remove one vertex from the list
    --n;

    // update the index to the next vertex
    i = i_next, i_prev = prev_id[i], i_next = next_id[i];
  }

  // exactly 3 vertices remain, still linked via prev_id/next_id — the loop above only clips ears down to
  // n == 3 and never emits this last, leftover triangle itself.
  triangles.push_back({i_prev, i, i_next});

  // transform the deque in vector (1 allocation, using move)
  std::vector<std::array<std::size_t, 3>> triangles_output(std::make_move_iterator(triangles.begin()),
                                                           std::make_move_iterator(triangles.end()));

  return triangles_output;
}

template std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(std::vector<Point2D> const& input,
                                                                                     View2D const& view);
template std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(std::vector<Point3D> const& input,
                                                                                     View2D const& view);

// Partition a simple, CCW-wound ring into y-monotone pieces (de Berg, "Computational Geometry" §3.2).
// See this function's own header doc for the algorithm summary, complexity trade-off, and the known
// horizontal-edge degenerate case.
template <typename PointT>
std::vector<std::vector<std::size_t>> partition_monotone_polygon(std::vector<PointT> const& input,
                                                                  View2D const& view) {
  std::size_t n = input.size();
  if (n < 3) {
    throw std::invalid_argument("partition_monotone_polygon: input must have at least 3 points");
  }

  // Total sweep order: a is "above" b iff y(a) > y(b), or (tie) x(a) < x(b), or (tie) a's own index <
  // b's -- the index tiebreak only ever matters for exact-duplicate coordinates, which shouldn't reach
  // here under any Collinearity setting other than Guaranteed.
  auto above = [&](std::size_t a, std::size_t b) -> bool {
    auto cy = compare(view.y(input[a]), view.y(input[b]));
    if (cy != 0) {
      return cy > 0;
    }
    auto cx = compare(view.x(input[a]), view.x(input[b]));
    if (cx != 0) {
      return cx < 0;
    }
    return a < b;
  };

  std::vector<std::size_t> next_id(n), prev_id(n);
  for (std::size_t i = 0; i < n; ++i) {
    next_id[i] = (i + 1) % n;
    prev_id[i] = (i + n - 1) % n;
  }

  enum class VType { Start, End, Split, Merge, Regular };
  std::vector<VType> vtype(n);
  for (std::size_t i = 0; i < n; ++i) {
    std::size_t p = prev_id[i], q = next_id[i];
    bool prev_above = above(p, i);
    bool next_above = above(q, i);
    // A collinear (exactly 180-degree) split/merge vertex is treated as convex here, same as every
    // other caller of helpers::is_reflex assumes non-degenerate input; Collinearity::Enforce/Assert
    // strip these before this code ever runs, so this only matters under Collinearity::Guaranteed.
    bool reflex = helpers::is_reflex(input[p], input[i], input[q], view);
    if (!prev_above && !next_above) {
      vtype[i] = reflex ? VType::Split : VType::Start;
    } else if (prev_above && next_above) {
      vtype[i] = reflex ? VType::Merge : VType::End;
    } else {
      vtype[i] = VType::Regular;
    }
  }

  std::vector<std::size_t> order(n);
  for (std::size_t i = 0; i < n; ++i) {
    order[i] = i;
  }
  std::sort(order.begin(), order.end(), above);

  // Status structure T: currently active descending edges (edge k := (input[k], input[next_id[k]])),
  // each with a helper vertex and whether that helper is a merge vertex. Linear-scan based -- see this
  // function's header doc for the O(n^2)-worst-case trade-off this implies.
  struct ActiveEdge {
    std::size_t edge_start;
    std::size_t helper;
    bool helper_is_merge;
  };
  std::vector<ActiveEdge> status;

  // x-coordinate of edge e at the y-height of vertex v -- used only to rank/search the status
  // structure. A horizontal edge (top.y == bot.y, which by `above`'s own tiebreak always has
  // top.x < bot.x) collapses to its top endpoint's x: see this function's header @note.
  auto edge_x_at = [&](ActiveEdge const& e, std::size_t v) -> double {
    std::size_t top = e.edge_start, bot = next_id[e.edge_start];
    double y_top = view.y(input[top]), y_bot = view.y(input[bot]);
    if (compare(y_top, y_bot) == 0) {
      return view.x(input[top]);
    }
    double t = (view.y(input[v]) - y_bot) / (y_top - y_bot);
    return view.x(input[bot]) + t * (view.x(input[top]) - view.x(input[bot]));
  };

  auto find_left_edge = [&](std::size_t v) -> std::size_t {
    double vx = view.x(input[v]);
    std::size_t best = status.size();
    double best_x = -std::numeric_limits<double>::infinity();
    for (std::size_t k = 0; k < status.size(); ++k) {
      double ex = edge_x_at(status[k], v);
      if (compare(ex, vx) < 0 && compare(ex, best_x) > 0) {
        best_x = ex;
        best = k;
      }
    }
    if (best == status.size()) {
      throw std::logic_error("partition_monotone_polygon: no active edge found to the left of a split/merge vertex");
    }
    return best;
  };

  auto find_status_index = [&](std::size_t edge_start) -> std::size_t {
    for (std::size_t k = 0; k < status.size(); ++k) {
      if (status[k].edge_start == edge_start) {
        return k;
      }
    }
    throw std::logic_error("partition_monotone_polygon: expected active edge not found in status structure");
  };

  std::vector<std::pair<std::size_t, std::size_t>> diagonals;

  for (std::size_t v : order) {
    switch (vtype[v]) {
      case VType::Start: {
        status.push_back({v, v, false});
        break;
      }
      case VType::Split: {
        std::size_t j = find_left_edge(v);
        diagonals.push_back({v, status[j].helper});
        status[j].helper = v;
        status[j].helper_is_merge = false;
        status.push_back({v, v, false});
        break;
      }
      case VType::End: {
        std::size_t idx = find_status_index(prev_id[v]);
        if (status[idx].helper_is_merge) {
          diagonals.push_back({v, status[idx].helper});
        }
        status.erase(status.begin() + static_cast<std::ptrdiff_t>(idx));
        break;
      }
      case VType::Merge: {
        std::size_t idx = find_status_index(prev_id[v]);
        if (status[idx].helper_is_merge) {
          diagonals.push_back({v, status[idx].helper});
        }
        status.erase(status.begin() + static_cast<std::ptrdiff_t>(idx));

        std::size_t j = find_left_edge(v);
        if (status[j].helper_is_merge) {
          diagonals.push_back({v, status[j].helper});
        }
        status[j].helper = v;
        status[j].helper_is_merge = true;
        break;
      }
      case VType::Regular: {
        bool interior_right = !above(next_id[v], v);  // next(v) is below v
        if (interior_right) {
          std::size_t idx = find_status_index(prev_id[v]);
          if (status[idx].helper_is_merge) {
            diagonals.push_back({v, status[idx].helper});
          }
          status.erase(status.begin() + static_cast<std::ptrdiff_t>(idx));
          status.push_back({v, v, false});
        } else {
          std::size_t j = find_left_edge(v);
          if (status[j].helper_is_merge) {
            diagonals.push_back({v, status[j].helper});
          }
          status[j].helper = v;
          status[j].helper_is_merge = false;
        }
        break;
      }
    }
  }

  // Apply every collected diagonal to a running list of pieces (each a list of ORIGINAL `input`
  // indices, CCW order). Every diagonal is a non-crossing chord, so incrementally splitting whichever
  // current piece holds both of its endpoints -- in any order -- always yields a correct result: two
  // non-crossing chords of a simple polygon can never have their endpoints straddle two different
  // pieces created by an earlier split (a straight segment connecting across two disjoint pieces would
  // have to cross the very diagonal that separated them).
  std::vector<std::vector<std::size_t>> pieces;
  {
    std::vector<std::size_t> full(n);
    for (std::size_t i = 0; i < n; ++i) {
      full[i] = i;
    }
    pieces.push_back(std::move(full));
  }

  for (auto const& [a, b] : diagonals) {
    // Index-based loop, NOT range-based: the body below may pieces.push_back() a new piece, which can
    // reallocate `pieces` and invalidate any reference/iterator taken into it beforehand.
    for (std::size_t pi = 0; pi < pieces.size(); ++pi) {
      auto& piece = pieces[pi];
      auto it_a = std::find(piece.begin(), piece.end(), a);
      auto it_b = std::find(piece.begin(), piece.end(), b);
      if (it_a == piece.end() || it_b == piece.end()) {
        continue;
      }
      std::size_t pos_a = static_cast<std::size_t>(std::distance(piece.begin(), it_a));
      std::size_t pos_b = static_cast<std::size_t>(std::distance(piece.begin(), it_b));
      std::size_t m = piece.size();
      std::size_t fwd_diff = (pos_b + m - pos_a) % m;
      if (fwd_diff == 1 || fwd_diff == m - 1) {
        // Already adjacent in this piece (can happen when a merge vertex's two helper checks resolve
        // to the same target vertex) -- a no-op split, nothing further to do for this diagonal.
        break;
      }

      std::vector<std::size_t> piece_a, piece_b;
      for (std::size_t k = pos_a;; k = (k + 1) % m) {
        piece_a.push_back(piece[k]);
        if (k == pos_b) {
          break;
        }
      }
      for (std::size_t k = pos_b;; k = (k + 1) % m) {
        piece_b.push_back(piece[k]);
        if (k == pos_a) {
          break;
        }
      }
      pieces[pi] = std::move(piece_a);
      pieces.push_back(std::move(piece_b));
      break;
    }
  }

  return pieces;
}

template std::vector<std::vector<std::size_t>> partition_monotone_polygon(std::vector<Point2D> const& input,
                                                                          View2D const& view);
template std::vector<std::vector<std::size_t>> partition_monotone_polygon(std::vector<Point3D> const& input,
                                                                          View2D const& view);

template <typename PointT>
std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(std::vector<PointT> const& input,
                                                                       View2D const& view) {
  std::size_t n = input.size();

  // Step 1: find top (max y, tie-break max x) and bottom (min y, tie-break min x) vertices.
  std::size_t top_idx = 0, bot_idx = 0;
  for (std::size_t i = 1; i < n; ++i) {
    double yi = view.y(input[i]);
    double yt = view.y(input[top_idx]);
    double yb = view.y(input[bot_idx]);
    if (compare(yi, yt) > 0 || (compare(yi, yt) == 0 && compare(view.x(input[i]), view.x(input[top_idx])) > 0)) {
      top_idx = i;
    }
    if (compare(yi, yb) < 0 || (compare(yi, yb) == 0 && compare(view.x(input[i]), view.x(input[bot_idx])) < 0)) {
      bot_idx = i;
    }
  }

  // Step 2: build left and right chains (both start at top_idx, end at bot_idx).
  // left_chain: walk CCW (forward, next = (i+1)%n)
  // right_chain: walk CW (backward, prev = (i+n-1)%n)
  std::vector<std::size_t> left_chain, right_chain;
  {
    std::size_t i = top_idx;
    while (i != bot_idx) {
      left_chain.push_back(i);
      i = (i + 1) % n;
    }
    left_chain.push_back(bot_idx);
  }
  {
    std::size_t i = top_idx;
    while (i != bot_idx) {
      right_chain.push_back(i);
      i = (i + n - 1) % n;
    }
    right_chain.push_back(bot_idx);
  }

  // Step 3: merge left_chain[1:] and right_chain[1:] into merged, y-descending, tie-break x-ascending.
  // Tag each entry with which chain it came from.
  enum class Chain { Left, Right };
  struct MergedEntry {
    std::size_t idx;
    Chain chain;
  };

  std::vector<MergedEntry> merged;
  merged.reserve(n);
  merged.push_back({top_idx, Chain::Left});

  std::size_t li = 1, ri = 1;
  while (li < left_chain.size() - 1 || ri < right_chain.size() - 1) {
    bool take_left = false;
    if (li >= left_chain.size() - 1) {
      take_left = false;
    } else if (ri >= right_chain.size() - 1) {
      take_left = true;
    } else {
      double yl = view.y(input[left_chain[li]]);
      double yr = view.y(input[right_chain[ri]]);
      if (compare(yl, yr) > 0) {
        take_left = true;
      } else if (compare(yl, yr) < 0) {
        take_left = false;
      } else {
        take_left = true;  // tie: left chain first
      }
    }
    if (take_left) {
      merged.push_back({left_chain[li], Chain::Left});
      ++li;
    } else {
      merged.push_back({right_chain[ri], Chain::Right});
      ++ri;
    }
  }
  merged.push_back({bot_idx, Chain::Left});

  // Step 4: stack-based triangulation.
  std::vector<std::array<std::size_t, 3>> result;

  auto emit = [&](std::size_t a, std::size_t b, std::size_t c) {
    if (compare(helpers::area2(input[a], input[b], input[c], view), 0.0) >= 0) {
      result.push_back({a, b, c});
    } else {
      result.push_back({a, c, b});
    }
  };

  std::vector<MergedEntry> stk;
  stk.push_back(merged[0]);
  stk.push_back(merged[1]);

  for (std::size_t j = 2; j < merged.size() - 1; ++j) {
    MergedEntry curr = merged[j];
    if (curr.chain != stk.back().chain) {
      while (stk.size() > 1) {
        MergedEntry top = stk.back();
        stk.pop_back();
        emit(curr.idx, top.idx, stk.back().idx);
      }
      stk.pop_back();
      stk.push_back(merged[j - 1]);
      stk.push_back(curr);
    } else {
      MergedEntry last_popped = stk.back();
      stk.pop_back();
      while (!stk.empty()) {
        double a = helpers::area2(input[curr.idx], input[last_popped.idx], input[stk.back().idx], view);
        bool valid = (curr.chain == Chain::Left) ? compare(a, 0.0) < 0 : compare(a, 0.0) > 0;
        if (valid) {
          emit(curr.idx, last_popped.idx, stk.back().idx);
          last_popped = stk.back();
          stk.pop_back();
        } else {
          break;
        }
      }
      stk.push_back(last_popped);
      stk.push_back(curr);
    }
  }

  // Handle last vertex (bot_idx).
  MergedEntry last_vert = merged.back();
  while (stk.size() > 1) {
    MergedEntry top = stk.back();
    stk.pop_back();
    emit(last_vert.idx, top.idx, stk.back().idx);
  }

  return result;
}

template std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(std::vector<Point2D> const& input,
                                                                                View2D const& view);
template std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(std::vector<Point3D> const& input,
                                                                                View2D const& view);

namespace helpers {

// Lawson edge flipping, in place: every edge shared by exactly 2 triangles whose opposite vertex lies
// strictly inside the neighboring triangle's circumcircle is flipped, until none remains. An edge owned
// by a single triangle (a polygon ring edge, or a point set's hull edge) is never flipped -- for a CDT
// that is exactly what keeps the boundary as a constraint. Each flip strictly increases the sorted
// angle vector, so the loop terminates; O(n²) flips worst case, each O(1).
// @param tris CCW index triplets into @p input, a valid triangulation (no overlaps, no zero-area).
template <typename PointT>
void lawson_flip(std::vector<PointT> const& input, View2D const& view, std::vector<std::array<std::size_t, 3>>& tris) {
  // undirected edge -> the (at most 2) triangles sharing it.
  constexpr std::size_t none = std::numeric_limits<std::size_t>::max();
  auto key = [](std::size_t a, std::size_t b) -> std::uint64_t {
    if (a > b) {
      std::swap(a, b);
    }
    return (static_cast<std::uint64_t>(a) << 32) | static_cast<std::uint64_t>(b);
  };
  std::unordered_map<std::uint64_t, std::array<std::size_t, 2>> owners;
  owners.reserve(tris.size() * 3);
  auto add_owner = [&](std::size_t a, std::size_t b, std::size_t t) {
    auto [it, inserted] = owners.try_emplace(key(a, b), std::array<std::size_t, 2>{t, none});
    if (!inserted) {
      it->second[1] = t;
    }
  };
  auto replace_owner = [&](std::size_t a, std::size_t b, std::size_t old_t, std::size_t new_t) {
    auto& o = owners.at(key(a, b));
    if (o[0] == old_t) {
      o[0] = new_t;
    } else if (o[1] == old_t) {
      o[1] = new_t;
    }
  };
  for (std::size_t t = 0; t < tris.size(); ++t) {
    add_owner(tris[t][0], tris[t][1], t);
    add_owner(tris[t][1], tris[t][2], t);
    add_owner(tris[t][2], tris[t][0], t);
  }

  // stack of edges to (re)check, seeded with every shared edge.
  std::vector<std::array<std::size_t, 2>> stack;
  stack.reserve(owners.size());
  for (auto const& [k, o] : owners) {
    if (o[1] != none) {
      stack.push_back({static_cast<std::size_t>(k >> 32), static_cast<std::size_t>(k & 0xFFFFFFFFu)});
    }
  }

  while (!stack.empty()) {
    auto [u, v] = stack.back();
    stack.pop_back();

    auto it = owners.find(key(u, v));
    if (it == owners.end() || it->second[1] == none) {
      continue;  // already flipped away, or a single-owner (constraint / hull) edge
    }
    std::size_t t1 = it->second[0];
    std::size_t t2 = it->second[1];

    // Rotate t1 to (a, b, c) so that a->b is the shared edge in t1's own CCW order; t2 then holds b->a,
    // and d is t2's vertex opposite the shared edge.
    auto r1 = tris[t1];
    while (!((r1[0] == u && r1[1] == v) || (r1[0] == v && r1[1] == u))) {
      std::rotate(r1.begin(), r1.begin() + 1, r1.end());
    }
    std::size_t a = r1[0], b = r1[1], c = r1[2];
    std::size_t d = none;
    for (auto idx : tris[t2]) {
      if (idx != a && idx != b) {
        d = idx;
      }
    }

    if (!in_circumcircle(input[a], input[b], input[c], input[d], view)) {
      continue;  // locally Delaunay
    }
    // Only flip when the quad a-d-b-c is strictly convex, i.e. both new triangles are strictly CCW.
    // Always true for a genuinely illegal edge; guards against tolerance-level near-degenerate cases.
    if (compare(area2(input[a], input[d], input[c], view), 0.0) <= 0 ||
        compare(area2(input[d], input[b], input[c], view), 0.0) <= 0) {
      continue;
    }

    // Flip a-b into c-d: t1 = (a, b, c) -> (a, d, c), t2 = (b, a, d) -> (d, b, c).
    tris[t1] = {a, d, c};
    tris[t2] = {d, b, c};
    owners.erase(it);
    owners[key(c, d)] = {t1, t2};
    replace_owner(a, d, t2, t1);
    replace_owner(b, c, t1, t2);

    // The 4 outer edges of the quad may have become illegal.
    stack.push_back({a, d});
    stack.push_back({d, b});
    stack.push_back({b, c});
    stack.push_back({c, a});
  }
}

// Scan triangulation of a point set: points are inserted in lexicographic (x, then y) order, so each
// new point lies outside the current convex hull, and is joined to every hull edge it strictly sees.
// Only orientation tests -- no super-triangle, so no far-away vertices to bias the result. Points
// coinciding (within DECIMAL_PRECISION) with an earlier one, or lying on the current hull boundary
// within tolerance, are skipped. O(n²) worst case (linear hull scan per insertion).
// @returns CCW index triplets into @p input covering its convex hull; empty if every point is collinear.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> scan_triangulation(std::vector<PointT> const& input, View2D const& view) {
  std::vector<std::array<std::size_t, 3>> tris;

  // Lexicographic order on the projected coordinates (exact comparisons: a strict weak ordering).
  std::vector<std::size_t> order(input.size());
  for (std::size_t i = 0; i < order.size(); ++i) {
    order[i] = i;
  }
  std::sort(order.begin(), order.end(), [&](std::size_t i, std::size_t j) {
    auto [xi, yi] = view.xy(input[i]);
    auto [xj, yj] = view.xy(input[j]);
    return xi < xj || (xi == xj && yi < yj);
  });

  // Drop near-duplicates: an earlier point within tolerance always has an x within tolerance too.
  std::vector<std::size_t> q;
  q.reserve(order.size());
  for (auto i : order) {
    auto [xi, yi] = view.xy(input[i]);
    bool duplicate = false;
    for (auto k = q.rbegin(); k != q.rend(); ++k) {
      auto [xk, yk] = view.xy(input[*k]);
      if (compare(xk, xi) != 0) {
        break;
      }
      if (compare(yk, yi) == 0) {
        duplicate = true;
        break;
      }
    }
    if (!duplicate) {
      q.push_back(i);
    }
  }
  if (q.size() < 3) {
    return tris;
  }

  auto orient = [&](std::size_t a, std::size_t b, std::size_t c) {
    return compare(area2(input[a], input[b], input[c], view), 0.0);
  };

  // Seed: q[0..k-1] collinear (and sorted along their line), q[k] the first point off that line.
  std::size_t k = 2;
  while (k < q.size() && orient(q[0], q[1], q[k]) == 0) {
    ++k;
  }
  if (k == q.size()) {
    return tris;  // all collinear
  }
  bool left = orient(q[0], q[1], q[k]) > 0;
  std::vector<std::size_t> hull;  // CCW
  for (std::size_t j = 0; j + 1 < k; ++j) {
    if (left) {
      tris.push_back({q[j], q[j + 1], q[k]});
    } else {
      tris.push_back({q[j + 1], q[j], q[k]});
    }
  }
  if (left) {
    hull.assign(q.begin(), q.begin() + static_cast<std::ptrdiff_t>(k));
  } else {
    hull.assign(q.rbegin() + static_cast<std::ptrdiff_t>(q.size() - k), q.rend());
  }
  hull.push_back(q[k]);

  std::vector<bool> visible;
  for (std::size_t s = k + 1; s < q.size(); ++s) {
    std::size_t p = q[s];
    std::size_t m = hull.size();
    visible.assign(m, false);
    for (std::size_t e = 0; e < m; ++e) {
      visible[e] = orient(hull[e], hull[(e + 1) % m], p) < 0;
    }
    // the visible edges form one contiguous run around the hull: find where it starts.
    std::size_t start = m;
    for (std::size_t e = 0; e < m; ++e) {
      if (visible[e] && !visible[(e + m - 1) % m]) {
        start = e;
        break;
      }
    }
    if (start == m) {
      continue;  // nothing strictly visible: p lies on the hull boundary within tolerance
    }
    std::size_t run = 0;
    while (run < m && visible[(start + run) % m]) {
      std::size_t u = hull[(start + run) % m];
      std::size_t w = hull[(start + run + 1) % m];
      tris.push_back({u, p, w});  // (u, w, p) is CW, so (u, p, w) is CCW
      ++run;
    }
    // hull: ..., hull[start], p, hull[start + run], ... (the run's interior vertices are now inside).
    std::vector<std::size_t> next;
    next.reserve(m + 1);
    next.push_back(hull[start]);
    next.push_back(p);
    for (std::size_t j = run; j < m; ++j) {
      next.push_back(hull[(start + j) % m]);
    }
    hull = std::move(next);
  }
  return tris;
}

}  // namespace helpers

template <typename PointT>
std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<PointT> const& input, View2D const& view) {
  auto tris = helpers::scan_triangulation(input, view);
  helpers::lawson_flip(input, view, tris);
  return tris;
}

template std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<Point2D> const& input,
                                                                        View2D const& view);
template std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<Point3D> const& input,
                                                                        View2D const& view);

template <typename PointT>
std::vector<std::array<std::size_t, 3>> constrained_delaunay_triangulation(std::vector<PointT> const& input,
                                                                           View2D const& view) {
  // Any valid triangulation of the ring keeps every ring edge and is strictly interior; ring edges then
  // have a single owning triangle, so lawson_flip never touches them -- they are the constraints.
  auto tris = ear_clipping_best_fit_triangulation(input, view);
  helpers::lawson_flip(input, view, tris);
  return tris;
}

template std::vector<std::array<std::size_t, 3>> constrained_delaunay_triangulation(
    std::vector<Point2D> const& input, View2D const& view);
template std::vector<std::array<std::size_t, 3>> constrained_delaunay_triangulation(
    std::vector<Point3D> const& input, View2D const& view);

template <typename PointT>
std::vector<std::array<PointT, 3>> triangulate_impl(std::vector<PointT> const& input, View2D const& view,
                                                    TriangulationParams const& settings) {
  if (input.size() < 3) {
    throw std::invalid_argument("triangulate: input must have at least 3 points");
  }

  if (input.size() == 3) {
    return {std::array<PointT, 3>{input[0], input[1], input[2]}};
  }

  // Working copy: Collinearity::Enforce may shrink it, Winding::Enforce may reverse it — every check below
  // and the final index-back-into-points step at the bottom all operate on this, never on the original
  // `input`.
  std::vector<PointT> working = input;

  switch (settings.collinearity) {
    case TriangulationParams::Collinearity::Guaranteed: {
      break;
    }
    case TriangulationParams::Collinearity::Assert: {
      if (detail::view::has_collinears(working, view)) {
        throw std::invalid_argument("triangulate: input has collinear (or duplicate) points");
      }
      break;
    }
    case TriangulationParams::Collinearity::Enforce: {
      if (detail::view::has_collinears(working, view)) {
        // remove_collinear() also catches duplicates as a byproduct: are_collinear(p0, p1, p2) is trivially
        // true whenever any two of the three coincide, so a run of duplicate points collapses right along
        // with genuinely collinear ones — no separate remove_consecutive_duplicates() pass needed.
        working = remove_collinear(std::move(working));
        if (working.size() < 3) {
          throw std::invalid_argument("triangulate: fewer than 3 points remain after removing collinear points");
        }
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown collinearity strategy");
    }
  }

  switch (settings.ccw_winding) {
    case TriangulationParams::Winding::Guaranteed: {
      break;
    }
    case TriangulationParams::Winding::Assert: {
      if (!detail::view::is_ccw(working, view)) {
        throw std::invalid_argument("triangulate: input is not CCW wound");
      }
      break;
    }
    case TriangulationParams::Winding::Enforce: {
      if (!detail::view::is_ccw(working, view)) {
        std::reverse(working.begin(), working.end());
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown winding strategy");
    }
  }

  // param check and creation of simple loops
  std::vector<std::vector<PointT>> simple_loops;
  switch (settings.simplicity) {
    case TriangulationParams::Simplicity::Guaranteed: {
      simple_loops.push_back(working);
      break;
    }
    case TriangulationParams::Simplicity::Assert: {
      if (!detail::view::is_simple(working, view)) {
        throw std::invalid_argument("triangulate: input is not simple");
      }
      simple_loops.push_back(working);
      break;
    }
    case TriangulationParams::Simplicity::Enforce: {
      if (detail::view::is_simple(working, view)) {
        simple_loops.push_back(working);
      } else if constexpr (std::is_same_v<PointT, Point3D>) {
        // TODO: simplify_rings() only ever returns 2D-projected rings (Point2D), so unprojecting them back
        // to Point3D needs plane-aware reconstruction not wired up here yet.
        throw std::runtime_error("not yet implemented");
      } else {  // simplified rings are also guaranteed to be CCW sorted
        for (auto const& loop : detail::view::simplify_rings(working, {}, view)) {
          simple_loops.push_back(loop);
        }
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown simplicity strategy");
    }
  }

  std::vector<std::array<std::size_t, 3>> tri_indices;

  switch (settings.strategy) {
    case TriangulationParams::Strategy::EarClipping: {
      for (auto const& loop : simple_loops) {
        auto loop_tri_indices = ear_clipping_triangulation(loop, view);
        tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
      }
      break;
    }
    case TriangulationParams::Strategy::EarClippingBestFit: {
      for (auto const& loop : simple_loops) {
        auto loop_tri_indices = ear_clipping_best_fit_triangulation(loop, view);
        tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
      }
      break;
    }
    case TriangulationParams::Strategy::MonotonePolygon: {
      for (auto const& loop : simple_loops) {
        switch (settings.monotonicity) {
          case TriangulationParams::Monotonicity::Guaranteed: {
            auto loop_tri_indices = monotone_polygon_triangulation(loop, view);
            tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
            break;
          }
          case TriangulationParams::Monotonicity::Assert: {
            if (!is_y_monotone(loop, view)) {
              throw std::invalid_argument("triangulate: input is not y-monotone");
            }
            auto loop_tri_indices = monotone_polygon_triangulation(loop, view);
            tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
            break;
          }
          case TriangulationParams::Monotonicity::Enforce: {
            if (is_y_monotone(loop, view)) {
              auto loop_tri_indices = monotone_polygon_triangulation(loop, view);
              tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
            } else {
              // Decompose into y-monotone pieces (each a list of indices into `loop`), triangulate
              // each piece independently with the existing monotone-polygon algorithm, then map each
              // piece-local triangle index back to a `loop`-relative index before concatenating --
              // `loop`-relative indices are what every other Strategy branch (and the final
              // `working[tri[k]]` lookup below) expects here.
              auto pieces = partition_monotone_polygon(loop, view);
              for (auto const& piece : pieces) {
                std::vector<PointT> piece_pts;
                piece_pts.reserve(piece.size());
                for (auto idx : piece) {
                  piece_pts.push_back(loop[idx]);
                }
                auto local_tris = monotone_polygon_triangulation(piece_pts, view);
                for (auto const& lt : local_tris) {
                  tri_indices.push_back({piece[lt[0]], piece[lt[1]], piece[lt[2]]});
                }
              }
            }
            break;
          }
          default: {
            throw std::invalid_argument("triangulate: unknown monotonicity strategy");
          }
        }
      }
      break;
    }
    case TriangulationParams::Strategy::ConstrainedDelaunay: {
      for (auto const& loop : simple_loops) {
        auto loop_tri_indices = constrained_delaunay_triangulation(loop, view);
        tri_indices.insert(tri_indices.end(), loop_tri_indices.begin(), loop_tri_indices.end());
      }
      break;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown triangulation strategy");
    }
  }

  std::vector<std::array<PointT, 3>> result;
  result.reserve(tri_indices.size());
  for (auto const& tri : tri_indices) {
    result.push_back({working[tri[0]], working[tri[1]], working[tri[2]]});
  }
  return result;
}

template std::vector<std::array<Point2D, 3>> triangulate_impl(std::vector<Point2D> const& input, View2D const& view,
                                                              TriangulationParams const& settings);
template std::vector<std::array<Point3D, 3>> triangulate_impl(std::vector<Point3D> const& input, View2D const& view,
                                                              TriangulationParams const& settings);
}  // namespace view
}  // namespace detail

std::vector<Triangle2D> triangulate(std::vector<Point2D> const& input, TriangulationParams const& settings) {
  auto tris = detail::view::triangulate_impl(input, View2D::XY(), settings);
  std::vector<Triangle2D> result;
  result.reserve(tris.size());
  for (auto const& t : tris) {
    result.push_back(Triangle2D::Make(t[0], t[1], t[2]));
  }
  return result;
}

std::vector<Triangle2D> delaunay(std::vector<Point2D> const& points) {
  if (points.size() < 3) {
    throw std::invalid_argument("delaunay: less than 3 points");
  }
  auto tri_indices = detail::view::delaunay_triangulation(points, View2D::XY());
  std::vector<Triangle2D> result;
  result.reserve(tri_indices.size());
  for (auto const& t : tri_indices) {
    result.push_back(Triangle2D::Make(points[t[0]], points[t[1]], points[t[2]]));
  }
  return result;
}

bool in_circumcircle(Point2D const& a, Point2D const& b, Point2D const& c, Point2D const& p) {
  return detail::view::in_circumcircle(a, b, c, p, View2D::XY());
}

bool in_circumcircle(Point3D const& a, Point3D const& b, Point3D const& c, Point3D const& p) {
  std::vector<Point3D> tri_pts{a, b, c};
  auto frame = principal_axes(tri_pts);
  Axis dax = frame.Z.DominantAxis();
  View2D v = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
  return detail::view::in_circumcircle(a, b, c, p, v);
}

namespace {

// PointT-specific segment-containment test, dispatched by overload resolution -- native per dimension,
// deliberately not View2D-projected (see AdjacencyViolation's own docs for why that would be wrong here).
bool segment_contains(Point2D const& a, Point2D const& b, Point2D const& v) {
  return LineSegment2D::Make(a, b).Contains(v);
}
bool segment_contains(Point3D const& a, Point3D const& b, Point3D const& v) {
  return LineSegment3D::Make(a, b).Contains(v);
}

// Supercover grid-cell traversal for segment a->b: visits every GridCell2D the segment's path touches,
// including a cell it only clips at a corner -- plain Bresenham can skip such a cell, which would
// silently reintroduce a missed-T-junction false negative (the exact bug class the grid-bucketed pass
// exists to catch). Standard incremental (Amanatides-Woo style) grid DDA: track the parametric t at which
// the line next crosses an x boundary and a y boundary, step whichever is smaller. On a tie -- the
// segment crosses exactly at a cell corner -- both single-axis neighbor cells are visited in addition to
// the diagonal cell, not just the diagonal one. Visiting an extra cell here is harmless: segment_contains
// below will correctly reject any vertex that isn't actually on the segment.
std::vector<GridCell2D> cells_along_segment(Point2D const& a, Point2D const& b, double epsilon) {
  std::vector<GridCell2D> result;
  std::unordered_set<GridCell2D, detail::GridCell2DHash> visited;
  auto visit = [&](std::int64_t cx, std::int64_t cy) {
    if (visited.insert({cx, cy}).second) {
      result.push_back({cx, cy});
    }
  };

  double ax = a.x(), ay = a.y();
  double dx = b.x() - ax, dy = b.y() - ay;

  std::int64_t x = static_cast<std::int64_t>(std::floor(ax / epsilon));
  std::int64_t y = static_cast<std::int64_t>(std::floor(ay / epsilon));
  std::int64_t const x_end = static_cast<std::int64_t>(std::floor(b.x() / epsilon));
  std::int64_t const y_end = static_cast<std::int64_t>(std::floor(b.y() / epsilon));

  visit(x, y);

  int const step_x = (dx > 0.0) ? 1 : (dx < 0.0 ? -1 : 0);
  int const step_y = (dy > 0.0) ? 1 : (dy < 0.0 ? -1 : 0);

  double const inf = std::numeric_limits<double>::infinity();
  double t_max_x = inf, t_delta_x = inf;
  if (step_x != 0) {
    double boundary_x = (step_x > 0) ? static_cast<double>(x + 1) * epsilon : static_cast<double>(x) * epsilon;
    t_max_x = (boundary_x - ax) / dx;
    t_delta_x = epsilon / std::fabs(dx);
  }
  double t_max_y = inf, t_delta_y = inf;
  if (step_y != 0) {
    double boundary_y = (step_y > 0) ? static_cast<double>(y + 1) * epsilon : static_cast<double>(y) * epsilon;
    t_max_y = (boundary_y - ay) / dy;
    t_delta_y = epsilon / std::fabs(dy);
  }

  // Iteration count is bounded by the Chebyshev-ish cell distance to x_end/y_end; the *2 margin plus hard
  // cap is a defensive guard against floating-point drift in t_max, not something normal input should hit.
  std::size_t const max_iterations = static_cast<std::size_t>(std::llabs(x_end - x) + std::llabs(y_end - y)) * 2 + 4;
  for (std::size_t iter = 0; (x != x_end || y != y_end) && iter < max_iterations; ++iter) {
    double t_min = std::min(t_max_x, t_max_y);
    if (t_min == inf) {
      break;  // no axis has a pending crossing left (degenerate a==b is handled by the loop guard above)
    }
    double tie_tol = 1e-9 * std::max({std::fabs(t_max_x), std::fabs(t_max_y), 1.0});
    bool const tied_x = step_x != 0 && (t_max_x - t_min) <= tie_tol;
    bool const tied_y = step_y != 0 && (t_max_y - t_min) <= tie_tol;

    std::int64_t const new_x = tied_x ? x + step_x : x;
    std::int64_t const new_y = tied_y ? y + step_y : y;

    // Visit every {old, new} combination across the tied axes: collapses to just the one stepped cell
    // for a plain single-axis crossing, and covers both axis-neighbors plus the diagonal on a corner tie.
    for (std::int64_t cx : {x, new_x}) {
      for (std::int64_t cy : {y, new_y}) {
        visit(cx, cy);
      }
    }

    if (tied_x) {
      t_max_x += t_delta_x;
    }
    if (tied_y) {
      t_max_y += t_delta_y;
    }
    x = new_x;
    y = new_y;
  }
  visit(x_end, y_end);  // safety net in case the iteration guard above ever trips early
  return result;
}

// 3D counterpart of cells_along_segment above -- the standard 3D-DDA / Amanatides-Woo voxel traversal,
// with the same both-neighbors-on-a-tie handling generalized: a tie between 2 axes (segment crosses
// exactly on a cell edge) visits the 2 axis-neighbors plus the diagonal; a tie between all 3 axes
// (segment passes exactly through a cell corner) visits all 7 neighbors sharing that corner plus the
// diagonal, not just the corner-diagonal cell.
std::vector<GridCell3D> cells_along_segment(Point3D const& a, Point3D const& b, double epsilon) {
  std::vector<GridCell3D> result;
  std::unordered_set<GridCell3D, detail::GridCell3DHash> visited;
  auto visit = [&](std::int64_t cx, std::int64_t cy, std::int64_t cz) {
    if (visited.insert({cx, cy, cz}).second) {
      result.push_back({cx, cy, cz});
    }
  };

  double ax = a.x(), ay = a.y(), az = a.z();
  double dx = b.x() - ax, dy = b.y() - ay, dz = b.z() - az;

  std::int64_t x = static_cast<std::int64_t>(std::floor(ax / epsilon));
  std::int64_t y = static_cast<std::int64_t>(std::floor(ay / epsilon));
  std::int64_t z = static_cast<std::int64_t>(std::floor(az / epsilon));
  std::int64_t const x_end = static_cast<std::int64_t>(std::floor(b.x() / epsilon));
  std::int64_t const y_end = static_cast<std::int64_t>(std::floor(b.y() / epsilon));
  std::int64_t const z_end = static_cast<std::int64_t>(std::floor(b.z() / epsilon));

  visit(x, y, z);

  int const step_x = (dx > 0.0) ? 1 : (dx < 0.0 ? -1 : 0);
  int const step_y = (dy > 0.0) ? 1 : (dy < 0.0 ? -1 : 0);
  int const step_z = (dz > 0.0) ? 1 : (dz < 0.0 ? -1 : 0);

  double const inf = std::numeric_limits<double>::infinity();
  auto axis_setup = [inf](std::int64_t c, int step, double a_coord, double d, double eps, double& t_max,
                          double& t_delta) {
    if (step != 0) {
      double boundary = (step > 0) ? static_cast<double>(c + 1) * eps : static_cast<double>(c) * eps;
      t_max = (boundary - a_coord) / d;
      t_delta = eps / std::fabs(d);
    } else {
      t_max = inf;
      t_delta = inf;
    }
  };

  double t_max_x, t_delta_x, t_max_y, t_delta_y, t_max_z, t_delta_z;
  axis_setup(x, step_x, ax, dx, epsilon, t_max_x, t_delta_x);
  axis_setup(y, step_y, ay, dy, epsilon, t_max_y, t_delta_y);
  axis_setup(z, step_z, az, dz, epsilon, t_max_z, t_delta_z);

  std::size_t const max_iterations =
      static_cast<std::size_t>(std::llabs(x_end - x) + std::llabs(y_end - y) + std::llabs(z_end - z)) * 2 + 6;
  for (std::size_t iter = 0; (x != x_end || y != y_end || z != z_end) && iter < max_iterations; ++iter) {
    double t_min = std::min({t_max_x, t_max_y, t_max_z});
    if (t_min == inf) {
      break;  // no axis has a pending crossing left
    }
    double tie_tol = 1e-9 * std::max({std::fabs(t_max_x), std::fabs(t_max_y), std::fabs(t_max_z), 1.0});
    bool const tied_x = step_x != 0 && (t_max_x - t_min) <= tie_tol;
    bool const tied_y = step_y != 0 && (t_max_y - t_min) <= tie_tol;
    bool const tied_z = step_z != 0 && (t_max_z - t_min) <= tie_tol;

    std::int64_t const new_x = tied_x ? x + step_x : x;
    std::int64_t const new_y = tied_y ? y + step_y : y;
    std::int64_t const new_z = tied_z ? z + step_z : z;

    // Visit every {old, new} combination across the tied axes: 2 cells for a plain single-axis crossing,
    // 4 for a 2-axis edge tie, 8 for a 3-axis corner tie -- the full supercover set sharing that
    // boundary, not just the diagonal landing cell.
    for (std::int64_t cx : {x, new_x}) {
      for (std::int64_t cy : {y, new_y}) {
        for (std::int64_t cz : {z, new_z}) {
          visit(cx, cy, cz);
        }
      }
    }

    if (tied_x) {
      t_max_x += t_delta_x;
    }
    if (tied_y) {
      t_max_y += t_delta_y;
    }
    if (tied_z) {
      t_max_z += t_delta_z;
    }
    x = new_x;
    y = new_y;
    z = new_z;
  }
  visit(x_end, y_end, z_end);  // safety net in case the iteration guard above ever trips early
  return result;
}

template <typename PointT>
std::vector<AdjacencyViolation<PointT>> validate_adjacency_impl(std::vector<std::vector<PointT>> const& facet_rings) {
  // return type
  std::vector<AdjacencyViolation<PointT>> violations;

  // helper functions and structures
  struct Edge {
    std::size_t facet;
    PointT a, b;
    auto to_segment() const {  // Make()'s AlmostEquals check now runs once per edge
      if constexpr (std::is_same_v<PointT, Point2D>) {
        return LineSegment2D::Make(a, b);
      } else {
        return LineSegment3D::Make(a, b);
      }
    };
  };

  // build edges
  std::vector<Edge> edges;
  for (std::size_t f = 0; f < facet_rings.size(); ++f) {
    auto const& ring = facet_rings[f];
    std::size_t n = ring.size();
    for (std::size_t i = 0; i < n; ++i) {
      edges.push_back({f, ring[i], ring[(i + 1) % n]});
    }
  }

  // Pass 1: non-manifold edges -- group edges that are the exact same undirected segment; a group of
  // more than 2 means more than 1 facet neighbors that edge (a normal boundary edge groups to 1, a
  // normal shared interior edge groups to 2).
  std::vector<bool> used(edges.size(), false);
  for (std::size_t i = 0; i < edges.size(); ++i) {
    if (used[i]) {
      continue;
    }
    std::vector<std::size_t> group{i};
    used[i] = true;
    for (std::size_t j = i + 1; j < edges.size(); ++j) {
      if (used[j]) {
        continue;
      }
      bool same_dir = edges[i].a.AlmostEquals(edges[j].a) && edges[i].b.AlmostEquals(edges[j].b);
      bool rev_dir = edges[i].a.AlmostEquals(edges[j].b) && edges[i].b.AlmostEquals(edges[j].a);
      if (same_dir || rev_dir) {
        group.push_back(j);
        used[j] = true;
      }
    }

    if (group.size() > 2) {
      std::vector<std::size_t> facet_indices;
      for (auto gi : group) {
        facet_indices.push_back(edges[gi].facet);
      }
      violations.push_back({edges[group[0]].a, edges[group[0]].b, facet_indices, edges[group[0]].a});
    }
  }

  // Pass 2: T-junctions - find vertices that lie in the middle of other facets' edges
  //                       those will imply a re-formatting of that containing facet
  // --- helper functions ----
  // the aim is to avoid passing 2+ time the same vertex in "violations" vector
  using GridCellT = std::conditional_t<std::is_same_v<PointT, Point2D>, GridCell2D, GridCell3D>;
  using GridCellTHash =
      std::conditional_t<std::is_same_v<PointT, Point2D>, detail::GridCell2DHash, detail::GridCell3DHash>;
  struct ViolatingPoint {
    std::size_t face_index;   // index of the facet containing it
    std::size_t point_index;  // index of the point in that facet
  };
  std::unordered_map<GridCellT, ViolatingPoint, GridCellTHash> point_grid_map;
  for (std::size_t f = 0; f < facet_rings.size(); ++f) {
    for (std::size_t p = 0; p < facet_rings[f].size(); ++p) {
      auto gc = GridCellT::FromPoint(facet_rings[f][p]);
      if (point_grid_map.find(gc) == point_grid_map.end()) {
        point_grid_map.insert({gc, ViolatingPoint{f, p}});
      }
    }
  }
  for (auto const& edge : edges) {
    auto seg = edge.to_segment();

    for (auto const& [gc, vp] : point_grid_map) {
      auto const& v = facet_rings[vp.face_index][vp.point_index];
      if (v.AlmostEquals(edge.a) || v.AlmostEquals(edge.b)) {
        continue;
      }
      if (seg.Contains(v)) {
        violations.push_back({edge.a, edge.b, {edge.facet}, v});
      }
    }
  }

  return violations;
}

template <typename PointT>
std::vector<std::vector<PointT>> fix_adjacency_impl(std::vector<std::vector<PointT>> const& facet_rings) {
  auto violations = validate_adjacency_impl(facet_rings);

  struct SpliceKey {
    std::size_t facet;
    PointT a, b;
  };
  std::vector<std::pair<SpliceKey, std::vector<PointT>>> splices;

  for (auto const& v : violations) {
    std::size_t coarse_facet = v.facet_indices[0];

    bool found = false;
    for (auto& [key, pts] : splices) {
      if (key.facet == coarse_facet && key.a.AlmostEquals(v.edge_p0) && key.b.AlmostEquals(v.edge_p1)) {
        pts.push_back(v.on_vertex);
        found = true;
        break;
      }
    }
    if (!found) {
      splices.push_back({{coarse_facet, v.edge_p0, v.edge_p1}, {v.on_vertex}});
    }
  }

  std::vector<std::vector<PointT>> new_perimeters = facet_rings;

  for (auto const& [key, pts] : splices) {
    auto& perim = new_perimeters[key.facet];
    for (std::size_t i = 0; i < perim.size(); ++i) {
      if (perim[i].AlmostEquals(key.a)) {
        std::vector<PointT> sorted_pts = pts;
        std::sort(sorted_pts.begin(), sorted_pts.end(),
                  [&](PointT const& x, PointT const& y) { return key.a.DistanceTo(x) < key.a.DistanceTo(y); });
        perim.insert(perim.begin() + static_cast<std::ptrdiff_t>(i) + 1, sorted_pts.begin(), sorted_pts.end());
        break;
      }
    }
  }

  // NOTE: deliberately NOT rebuilt via Polygon2D/3D::Make() -- Make() unconditionally strips collinear
  // points, which would immediately undo the splice above. These raw rings are the fix.
  return new_perimeters;
}

// View2D to project a ring through for the diagonal-cut math below -- native XY for 2D, PCA-fitted
// dominant-axis view for 3D (same pattern triangulate(vector<Point3D>, settings) uses when no normal
// is supplied), dispatched by overload resolution like segment_contains() above.
View2D view_for_ring(std::vector<Point2D> const&) { return View2D::XY(); }

View2D view_for_ring(std::vector<Point3D> const& ring) {
  auto frame = principal_axes(ring);
  Axis dax = frame.Z.DominantAxis();
  return (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
}

// Standard proper (strict) segment-segment intersection: both segments must straddle each other.
// Touching only at a shared endpoint, or running collinear/overlapping, does NOT count -- a valid
// polygon diagonal is allowed to meet other edges only at its own two endpoints.
template <typename PointT>
bool segments_properly_intersect(PointT const& p1, PointT const& q1, PointT const& p2, PointT const& q2,
                                 View2D const& view) {
  double d1 = detail::view::helpers::area2(p2, q2, p1, view);
  double d2 = detail::view::helpers::area2(p2, q2, q1, view);
  double d3 = detail::view::helpers::area2(p1, q1, p2, view);
  double d4 = detail::view::helpers::area2(p1, q1, q2, view);
  bool straddles_p2q2 =
      (compare(d1, 0.0) > 0 && compare(d2, 0.0) < 0) || (compare(d1, 0.0) < 0 && compare(d2, 0.0) > 0);
  bool straddles_p1q1 =
      (compare(d3, 0.0) > 0 && compare(d4, 0.0) < 0) || (compare(d3, 0.0) < 0 && compare(d4, 0.0) > 0);
  return straddles_p2q2 && straddles_p1q1;
}

// Is (ring[i], ring[j]) a valid polygon diagonal? i is always a just-spliced, exactly-flat (180 degree)
// vertex here, never a normal sharp one -- so unlike the general O'Rourke-style Diagonal()/InCone() test,
// there's no "is b within i's interior angle cone" step: a flat vertex's cone IS the whole interior
// half-plane, so any b on the interior side works there. What's still needed: (1) the segment can't
// properly cross any OTHER edge of the ring, (2) it can't run collinear along i's own (flat) edge --
// a "diagonal" along the boundary itself doesn't cut anything, (3) its midpoint must be strictly inside
// the ring (winding-number test, robust to the ring's own other flat/collinear vertices).
template <typename PointT>
bool is_valid_diagonal(std::vector<PointT> const& ring, std::size_t i, std::size_t j, View2D const& view) {
  std::size_t n = ring.size();
  std::size_t i_next = (i + 1) % n;
  std::size_t i_prev = (i + n - 1) % n;
  if (j == i || j == i_next || j == i_prev) {
    return false;
  }

  PointT const& a = ring[i];
  PointT const& b = ring[j];

  if (detail::view::helpers::are_collinear(ring[i_prev], a, b, view)) {
    return false;
  }

  for (std::size_t k = 0; k < n; ++k) {
    std::size_t k_next = (k + 1) % n;
    if (k == i || k == j || k_next == i || k_next == j) {
      continue;
    }
    if (segments_properly_intersect(a, b, ring[k], ring[k_next], view)) {
      return false;
    }
  }

  double mx = (view.x(a) + view.x(b)) / 2.0;
  double my = (view.y(a) + view.y(b)) / 2.0;
  return detail::view::polygon_contains(ring, std::vector<std::vector<PointT>>{}, view, mx, my);
}

// Splits a simple ring into two simple rings sharing the diagonal (ring[i], ring[j]) as an edge.
template <typename PointT>
std::pair<std::vector<PointT>, std::vector<PointT>> split_ring_at(std::vector<PointT> const& ring, std::size_t i,
                                                                  std::size_t j) {
  std::size_t n = ring.size();
  std::vector<PointT> ring_a, ring_b;
  for (std::size_t k = i;; k = (k + 1) % n) {
    ring_a.push_back(ring[k]);
    if (k == j) {
      break;
    }
  }
  for (std::size_t k = j;; k = (k + 1) % n) {
    ring_b.push_back(ring[k]);
    if (k == i) {
      break;
    }
  }
  return {ring_a, ring_b};
}

// The Polygon2D/3D flavor of fix_adjacency(): unlike the raw splice fix_adjacency_impl() above (still
// used by the Triangle2D/3D overload as a first step before re-triangulating), this actually splits a
// coarse facet into multiple facets -- one straight diagonal cut per spliced T-junction vertex, to its
// nearest ring vertex that forms a valid diagonal. This is the standard "diagonal-to-nearest-vertex"
// polygon-splitting technique (a guaranteed-valid diagonal always exists from any vertex of a simple
// polygon with >= 4 vertices), same guarantee ear-clipping itself relies on. An unaffected facet passes
// through unchanged.
template <typename PointT>
std::vector<std::vector<PointT>> split_facets_at_junctions_impl(std::vector<std::vector<PointT>> const& facet_rings) {
  auto violations = validate_adjacency_impl(facet_rings);

  struct SpliceKey {
    std::size_t facet;
    PointT a, b;
  };
  std::vector<std::pair<SpliceKey, std::vector<PointT>>> splices;

  for (auto const& v : violations) {
    std::size_t coarse_facet = v.facet_indices[0];

    bool found = false;
    for (auto& [key, pts] : splices) {
      if (key.facet == coarse_facet && key.a.AlmostEquals(v.edge_p0) && key.b.AlmostEquals(v.edge_p1)) {
        pts.push_back(v.on_vertex);
        found = true;
        break;
      }
    }
    if (!found) {
      splices.push_back({{coarse_facet, v.edge_p0, v.edge_p1}, {v.on_vertex}});
    }
  }

  std::vector<std::vector<PointT>> result;

  for (std::size_t f = 0; f < facet_rings.size(); ++f) {
    std::vector<PointT> ring = facet_rings[f];
    std::vector<PointT> spliced_points;

    for (auto const& [key, pts] : splices) {
      if (key.facet != f) {
        continue;
      }
      for (std::size_t i = 0; i < ring.size(); ++i) {
        if (ring[i].AlmostEquals(key.a)) {
          std::vector<PointT> sorted_pts = pts;
          std::sort(sorted_pts.begin(), sorted_pts.end(),
                    [&](PointT const& x, PointT const& y) { return key.a.DistanceTo(x) < key.a.DistanceTo(y); });
          ring.insert(ring.begin() + static_cast<std::ptrdiff_t>(i) + 1, sorted_pts.begin(), sorted_pts.end());
          spliced_points.insert(spliced_points.end(), sorted_pts.begin(), sorted_pts.end());
          break;
        }
      }
    }

    if (spliced_points.empty()) {
      result.push_back(std::move(ring));
      continue;
    }

    View2D view = view_for_ring(ring);
    std::vector<std::vector<PointT>> pieces = {std::move(ring)};

    for (auto const& v : spliced_points) {
      for (std::size_t pi = 0; pi < pieces.size(); ++pi) {
        auto& piece = pieces[pi];

        std::size_t vi = piece.size();
        for (std::size_t k = 0; k < piece.size(); ++k) {
          if (piece[k].AlmostEquals(v)) {
            vi = k;
            break;
          }
        }
        if (vi == piece.size()) {
          continue;  // v doesn't belong to this piece -- keep searching the others
        }

        std::vector<std::size_t> candidates;
        for (std::size_t k = 0; k < piece.size(); ++k) {
          if (k != vi) {
            candidates.push_back(k);
          }
        }
        std::sort(candidates.begin(), candidates.end(),
                  [&](std::size_t ca, std::size_t cb) { return v.DistanceTo(piece[ca]) < v.DistanceTo(piece[cb]); });

        std::size_t chosen = piece.size();
        double chosen_dist = 0.0;
        for (auto c : candidates) {
          if (is_valid_diagonal(piece, vi, c, view)) {
            chosen = c;
            chosen_dist = v.DistanceTo(piece[c]);
            break;
          }
        }
        if (chosen == piece.size()) {
          throw std::logic_error(
              "fix_adjacency: found no valid diagonal from a spliced T-junction vertex -- "
              "every simple polygon with >= 4 vertices has one, so this indicates a bug");
        }

        // TODO: tie-break policy not decided yet (see conversation) -- disabled for now rather than
        // silently committing to whichever candidate std::sort's unstable ordering happens to put
        // first. Refuse loudly instead of producing a non-reproducible split.
        for (auto c : candidates) {
          if (c != chosen && compare(v.DistanceTo(piece[c]), chosen_dist) == 0 && is_valid_diagonal(piece, vi, c, view)) {
            throw std::logic_error(
                "fix_adjacency: found multiple equally-near valid diagonals from a spliced T-junction "
                "vertex -- tie-breaking is not yet defined, refusing to pick one arbitrarily");
          }
        }

        auto [ring_a, ring_b] = split_ring_at(piece, vi, chosen);
        pieces.erase(pieces.begin() + static_cast<std::ptrdiff_t>(pi));
        pieces.push_back(std::move(ring_a));
        pieces.push_back(std::move(ring_b));
        break;  // this spliced vertex is handled -- move on to the next one
      }
    }

    result.insert(result.end(), std::make_move_iterator(pieces.begin()), std::make_move_iterator(pieces.end()));
  }

  return result;
}

}  // namespace

std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<Polygon2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<Triangle2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<std::vector<Point2D>> const& facet_rings) {
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<Polygon3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<Triangle3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }
  return validate_adjacency_impl(facet_rings);
}

std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<std::vector<Point3D>> const& facet_rings) {
  return validate_adjacency_impl(facet_rings);
}

std::vector<std::vector<Point2D>> fix_adjacency(std::vector<Polygon2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return split_facets_at_junctions_impl(facet_rings);
}

std::vector<std::vector<Point3D>> fix_adjacency(std::vector<Polygon3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    facet_rings.push_back(f.Perimeter());
  }
  return split_facets_at_junctions_impl(facet_rings);
}

std::vector<Triangle2D> fix_adjacency(std::vector<Triangle2D> const& facets) {
  std::vector<std::vector<Point2D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }

  auto fixed_rings = fix_adjacency_impl(facet_rings);

  std::vector<Triangle2D> result;
  TriangulationParams guaranteed_collinearity;
  guaranteed_collinearity.collinearity = TriangulationParams::Collinearity::Guaranteed;
  for (auto const& ring : fixed_rings) {
    if (ring.size() == 3) {
      result.push_back(Triangle2D::Make(ring[0], ring[1], ring[2]));
      continue;
    }
    auto sub_triangles = triangulate(ring, guaranteed_collinearity);
    result.insert(result.end(), std::make_move_iterator(sub_triangles.begin()),
                  std::make_move_iterator(sub_triangles.end()));
  }
  return result;
}

std::vector<Triangle3D> fix_adjacency(std::vector<Triangle3D> const& facets) {
  std::vector<std::vector<Point3D>> facet_rings;
  facet_rings.reserve(facets.size());
  for (auto const& f : facets) {
    auto [p0, p1, p2] = f.Vertices();
    facet_rings.push_back({p0, p1, p2});
  }

  auto fixed_rings = fix_adjacency_impl(facet_rings);

  std::vector<Triangle3D> result;
  TriangulationParams guaranteed_collinearity;
  guaranteed_collinearity.collinearity = TriangulationParams::Collinearity::Guaranteed;
  for (auto const& ring : fixed_rings) {
    if (ring.size() == 3) {
      result.push_back(Triangle3D::Make(ring[0], ring[1], ring[2]));
      continue;
    }
    // The ring may now span more than 3 points (still planar -- the splice only added collinear
    // points), so fit its normal via PCA rather than assuming a caller-supplied one.
    auto sub_triangles = triangulate(ring, guaranteed_collinearity);
    result.insert(result.end(), std::make_move_iterator(sub_triangles.begin()),
                  std::make_move_iterator(sub_triangles.end()));
  }
  return result;
}

std::vector<Triangle2D> triangulate(std::vector<Polygon2D> const& polygons, TriangulationParams const& settings) {
  switch (settings.conformity) {
    case AdjacencyConformity::Guaranteed: {
      std::vector<Triangle2D> result;
      for (auto const& poly : polygons) {
        auto tris = triangulate(poly.Perimeter(), settings);
        result.insert(result.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
      }
      return result;
    }
    case AdjacencyConformity::Assert: {
      auto violations = validate_adjacency(polygons);
      if (!violations.empty()) {
        auto const& v = violations.front();
        throw std::invalid_argument(
            "triangulate: facets violate adjacency conformity at edge (" + v.edge_p0.ToWkt() + " -> " +
            v.edge_p1.ToWkt() + "), touched by " + std::to_string(v.facet_indices.size()) + " facet(s)");
      }
      std::vector<Triangle2D> result;
      for (auto const& poly : polygons) {
        auto tris = triangulate(poly.Perimeter(), settings);
        result.insert(result.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
      }
      return result;
    }
    case AdjacencyConformity::Enforce: {
      // fix_adjacency() now actually splits a coarse facet via a diagonal cut per T-junction vertex
      // (§10.5), rather than just splicing a flat vertex into its ring, so every returned ring is
      // already a simple polygon with no leftover collinear points -- no need to force
      // Collinearity::Guaranteed here the way an earlier version of this function had to.
      auto fixed_rings = fix_adjacency(polygons);

      std::vector<Triangle2D> result;
      for (auto const& ring : fixed_rings) {
        auto tris = triangulate(ring, settings);
        result.insert(result.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
      }
      return result;
    }
    default: {
      throw std::invalid_argument("triangulate: unknown adjacency conformity");
    }
  }
}

namespace detail {

template <typename PointT>
void assert_adjacency(std::vector<AdjacencyViolation<PointT>> const& violations) {
  if (violations.empty()) {
    return;
  }
  auto const& v = violations.front();
  std::string facets_str;
  for (std::size_t i = 0; i < v.facet_indices.size(); ++i) {
    facets_str += std::to_string(v.facet_indices[i]);
    if (i + 1 < v.facet_indices.size()) {
      facets_str += ", ";
    }
  }

  throw std::invalid_argument("facet(s) " + facets_str + " violate adjacency conformity at edge (" +
                              v.edge_p0.ToWkt() + " -> " + v.edge_p1.ToWkt() + ")");
}

template void assert_adjacency(std::vector<AdjacencyViolation<Point2D>> const&);
template void assert_adjacency(std::vector<AdjacencyViolation<Point3D>> const&);

}  // namespace detail

}  // namespace geometry

}  // namespace geompp
