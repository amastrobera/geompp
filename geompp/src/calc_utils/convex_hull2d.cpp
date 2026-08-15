#include "calc_utils/convex_hull2d.hpp"

#include "calc_utils/polygon_ops2d.hpp"
#include "calc_utils/self_intersections2d.hpp"
#include "line2d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "vector2d.hpp"
#include "vector3d.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>

namespace geompp {

inline namespace geometry {
namespace detail {

namespace view {

template <PointContainer Points>
std::vector<std::size_t> convex_hull_monotone_chain(Points const& points, View2D const& view) {
  std::size_t n = std::ranges::size(points);
  if (n < 3) {
    return {};
  }

  std::vector<std::size_t> indices(n);
  std::iota(indices.begin(), indices.end(), std::size_t{0});

  std::sort(indices.begin(), indices.end(), [&](std::size_t i, std::size_t j) {
    double xi = view.x(points[i]), xj = view.x(points[j]);
    if (xi != xj) {
      return xi < xj;
    }
    return view.y(points[i]) < view.y(points[j]);
  });

  auto is_left = [&](std::size_t o, std::size_t a, std::size_t b) {
    auto [ox, oy] = view.xy(points[o]);
    auto [ax, ay] = view.xy(points[a]);
    auto [bx, by] = view.xy(points[b]);
    return compare((ax - ox) * (by - oy) - (ay - oy) * (bx - ox), 0) > 0;
  };

  std::vector<std::size_t> hull;
  hull.reserve(2 * n);

  // Lower hull
  for (std::size_t i = 0; i < n; ++i) {
    while (hull.size() >= 2 && !is_left(hull[hull.size() - 2], hull.back(), indices[i])) {
      hull.pop_back();
    }
    hull.push_back(indices[i]);
  }

  // Upper hull
  std::size_t lower_hull_size = hull.size();
  for (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(n) - 2; i >= 0; --i) {
    while (hull.size() > lower_hull_size && !is_left(hull[hull.size() - 2], hull.back(), indices[i])) {
      hull.pop_back();
    }
    hull.push_back(indices[i]);
  }

  if (!hull.empty()) {
    hull.pop_back();
  }
  return hull;
}

// template specialization
template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point2D> const&, View2D const&);

template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point3D> const&, View2D const&);

}  // namespace view

template <VectorType V, ProjectablePointContainerWith<V> R>
std::pair<std::size_t, std::size_t> extreme_points(R const& vertices, bool is_convex, V const& dir) {
  long const n = static_cast<long>(std::ranges::distance(vertices));
  if (n == 0) {
    throw std::invalid_argument("extreme_points: empty vertex range");
  }

  // projection of vertex i (cyclic index) onto the direction
  auto proj = [&](long i) -> double {
    return vertices[static_cast<std::size_t>(((i % n) + n) % n)].ToVector().Dot(dir);
  };

  // convex fast-path — Daniel Sunday's O(log n) binary search for the extreme vertex of a convex CCW
  // polygon in a direction (cf. the KACTL "extrVertex"). search(+1) returns argmax(proj); search(-1)
  // returns argmax(-proj) = argmin(proj). Ties (an edge perpendicular to dir) resolve to one endpoint.
  if (is_convex) {
    auto sgn = [](double d) -> int { return static_cast<int>(d > 0) - static_cast<int>(d < 0); };

    auto search = [&](double s) -> std::size_t {
      auto g = [&](long i) { return s * proj(i); };                                 // maximise s * proj
      auto cmp = [&](long i, long j) { return sgn(g(j) - g(i)); };                  // sign( g(j) - g(i) )
      auto extr = [&](long i) { return cmp(i + 1, i) >= 0 && cmp(i, i - 1) < 0; };  // i is the peak
      if (extr(0)) {
        return 0;
      }
      long lo = 0, hi = n;
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

    std::size_t max_i = search(1.0);
    std::size_t min_i = search(-1.0);
    return {min_i, max_i};
  }

  // brute force O(n): a concave ring has no monotone structure to exploit
  std::size_t min_i = 0, max_i = 0;
  double min_v = proj(0), max_v = proj(0);
  for (long i = 1; i < n; ++i) {
    double v = proj(i);
    if (v < min_v) {
      min_v = v;
      min_i = static_cast<std::size_t>(i);
    }
    if (v > max_v) {
      max_v = v;
      max_i = static_cast<std::size_t>(i);
    }
  }
  return {min_i, max_i};
}

template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point2D> const&, bool, Vector2D const&);
template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point3D> const&, bool, Vector3D const&);

namespace view {

template <PointContainer Points>
MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const& hull_indices, Points const& points,
                                        View2D const& view) {
  std::size_t m = hull_indices.size();

  double best_area = std::numeric_limits<double>::max();
  double best_ux = 1.0, best_uy = 0.0, best_vx = 0.0, best_vy = 1.0;
  double best_umin = 0.0, best_umax = 0.0, best_vmin = 0.0, best_vmax = 0.0;
  double best_ox = 0.0, best_oy = 0.0;

  for (std::size_t i = 0; i < m; ++i) {
    auto [p0x, p0y] = view.xy(points[hull_indices[i]]);
    auto [p1x, p1y] = view.xy(points[hull_indices[(i + 1) % m]]);

    double ex = p1x - p0x;
    double ey = p1y - p0y;
    double elen = std::sqrt(ex * ex + ey * ey);
    if (compare(elen, 0.0) == 0) {
      continue;
    }

    double ux = ex / elen;
    double uy = ey / elen;
    double vx = -uy;
    double vy = ux;

    double u_min = std::numeric_limits<double>::max();
    double u_max = -std::numeric_limits<double>::max();
    double v_min = std::numeric_limits<double>::max();
    double v_max = -std::numeric_limits<double>::max();

    for (std::size_t j = 0; j < m; ++j) {
      auto [qx, qy] = view.xy(points[hull_indices[j]]);
      double dx = qx - p0x;
      double dy = qy - p0y;
      double pu = dx * ux + dy * uy;
      double pv = dx * vx + dy * vy;
      if (pu < u_min) {
        u_min = pu;
      }
      if (pu > u_max) {
        u_max = pu;
      }
      if (pv < v_min) {
        v_min = pv;
      }
      if (pv > v_max) {
        v_max = pv;
      }
    }

    double rect_area = (u_max - u_min) * (v_max - v_min);
    if (compare(rect_area, best_area) < 0) {
      best_area = rect_area;
      best_ux = ux;
      best_uy = uy;
      best_vx = vx;
      best_vy = vy;
      best_umin = u_min;
      best_umax = u_max;
      best_vmin = v_min;
      best_vmax = v_max;
      best_ox = p0x;
      best_oy = p0y;
    }
  }

  double cu = (best_umin + best_umax) / 2.0;
  double cv = (best_vmin + best_vmax) / 2.0;

  return MinBoundingRectResult{
      best_ux, best_uy, best_vx, best_vy, (best_umax - best_umin) / 2.0, (best_vmax - best_vmin) / 2.0,
      cu,      cv,      best_ox, best_oy,
  };
}

template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point2D> const&,
                                                 View2D const&);
template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point3D> const&,
                                                 View2D const&);

}  // namespace view

std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points) {
  return view::convex_hull_monotone_chain(points, View2D::XY());
}

namespace view {

template <PointContainer Points>
bool is_ccw(Points const& points, View2D const& view) {
  std::size_t n = std::ranges::size(points);
  double area2 = 0.0;
  for (std::size_t i = 0; i < n; ++i) {
    double x0 = view.x(points[i]), y0 = view.y(points[i]);
    double x1 = view.x(points[(i + 1) % n]), y1 = view.y(points[(i + 1) % n]);
    area2 += x0 * y1 - x1 * y0;
  }
  return compare(area2, 0.0) > 0;
}

template bool is_ccw(std::vector<Point2D> const&, View2D const&);
template bool is_ccw(std::vector<Point3D> const&, View2D const&);

template <PointContainer Points>
bool has_collinears(Points const& points, View2D const& view) {
  std::size_t n = std::ranges::size(points);
  for (std::size_t i = 0; i < n; ++i) {
    auto [x0, y0] = view.xy(points[i]);
    auto [x1, y1] = view.xy(points[(i + 1) % n]);
    auto [x2, y2] = view.xy(points[(i + 2) % n]);

    // A duplicate needs no separate check: if points[i] == points[i+1] (or points[i+1] == points[i+2]),
    // one of the two edge vectors below is the zero vector, so the cross product is already trivially zero.
    double cross = (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);  // zero turn at points[i+1]

    if (compare(cross, 0.0) == 0) {
      return true;
    }
  }
  return false;
}

template bool has_collinears(std::vector<Point2D> const&, View2D const&);
template bool has_collinears(std::vector<Point3D> const&, View2D const&);

template <PointContainer Points>
bool is_convex(Points const& vertices, View2D const& view) {
  int n = static_cast<int>(vertices.size());
  bool seen_positive = false;
  bool seen_negative = false;
  for (int i = 0; i < n; ++i) {
    double x0 = view.x(vertices[i]), y0 = view.y(vertices[i]);
    double x1 = view.x(vertices[(i + 1) % n]), y1 = view.y(vertices[(i + 1) % n]);
    double x2 = view.x(vertices[(i + 2) % n]), y2 = view.y(vertices[(i + 2) % n]);
    double cross = (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
    auto ord = compare(cross, 0.0);
    if (ord == 0) {
      continue;
    }
    if (ord > 0) {
      seen_positive = true;
    } else {
      seen_negative = true;
    }
    if (seen_positive && seen_negative) {
      return false;
    }
  }
  return true;
}

template bool is_convex(std::vector<Point2D> const&, View2D const&);
template bool is_convex(std::vector<Point3D> const&, View2D const&);

template <PointContainer Points>
bool is_simple(Points const& points, View2D const& view) {
  auto segs = collect_ring_segments(points, std::vector<Points>{}, view);
  return !has_intersections(segs);
}

template bool is_simple(std::vector<Point2D> const&, View2D const&);
template bool is_simple(std::vector<Point3D> const&, View2D const&);

}  // namespace view

bool is_convex(std::vector<Point2D> const& vertices, std::vector<std::vector<Point2D>> const& holes) {
  if (!holes.empty() || vertices.size() < 3) {
    return false;
  }
  return view::is_convex(vertices, View2D::XY());
}

}  // namespace detail

ExtremePoints<Point2D> find_extreme_points(Polygon2D const& polygon, Line2D const& line) {
  auto [min_i, max_i] = detail::extreme_points(polygon.Perimeter(), polygon.IsConvex(), line.Direction());
  return {polygon[min_i], polygon[max_i]};
}

}  // namespace geometry

}  // namespace geompp
