#include "calc_utils/polygon_queries3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "polygon3d.hpp"
#include "utils.hpp"
#include "view2d.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace geompp {

inline namespace geometry {

namespace {

// Minimum distance from a fixed 3D line to a closed ring of points (one polygon boundary loop —
// outer ring or a hole). Point-to-line distance has nested convex (ellipsoidal) level sets centered
// on the line, so its minimum over any closed planar region is always attained on the region's
// boundary, regardless of whether that region is convex — so an exact per-edge scan suffices for
// both convex and non-convex rings alike; no separate convex fast path is needed here.
// Per edge P0->P1 (E = P1-P0, A = P0-line_origin, D = line direction, unit), the squared distance
// from the edge point P0+t*E to the line is the quadratic c0 + c1*t + c2*t^2 derived from
// |A+tE|^2 - ((A+tE).D)^2; minimizing over t in [0,1] is a standard clamped-vertex search.
double ring_distance_to_line(std::vector<Point3D> const& ring, Point3D const& line_origin, Vector3D const& line_dir) {
  std::size_t n = ring.size();
  double min_d2 = -1;
  for (std::size_t i = 0; i < n; ++i) {
    Point3D const& p0 = ring[i];
    Point3D const& p1 = ring[(i + 1) % n];
    Vector3D E = p1 - p0;
    Vector3D A = p0 - line_origin;

    double AD = A.Dot(line_dir);
    double ED = E.Dot(line_dir);
    double c0 = A.Dot(A) - AD * AD;
    double c1 = 2.0 * (A.Dot(E) - AD * ED);
    double c2 = E.Dot(E) - ED * ED;

    double t = 0.0;
    if (compare(c2, 0.0) != 0) {
      t = -c1 / (2.0 * c2);
    } else if (c1 < 0) {
      t = 1.0;
    }
    t = std::clamp(t, 0.0, 1.0);

    double d2 = c0 + c1 * t + c2 * t * t;
    if (compare(min_d2, 0) < 0 || d2 < min_d2) {
      min_d2 = d2;
    }
  }
  return std::sqrt(std::max(min_d2, 0.0));
}

}  // namespace

/// @brief computes the distance between a polygon and a line (the distance is zero if they intersect)
double distance_to(Polygon3D const& polygon, Line3D const& line) {
  auto poly_plane = polygon.GetPlane();
  bool plane_is_parallel_to_line = poly_plane.IsParallel(line);

  // cases that can be reduced to 2D: a line parallel to the plane has a constant perpendicular
  // offset `h` to it, so the true 3D distance is sqrt(h^2 + d2d^2), where d2d is the in-plane
  // distance between the polygon and the line's projection onto the plane. Coplanarity is just the
  // special case h == 0, which this formula already handles without a separate branch.
  if (plane_is_parallel_to_line) {
    double h = poly_plane.DistanceTo(line.Origin());

    double d2d;
    // these world planes will make the maths very quick downstream - if they can be used
    if (poly_plane == Plane::XY()) {
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::XY());
    } else if (poly_plane == Plane::YZ()) {
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::YZ());
    } else if (poly_plane == Plane::ZX()) {
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::ZX());
    } else {
      // otherwise we will have to go slower and use the plane's own (Custom) basis
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::OnPlane(poly_plane));
    }

    return std::sqrt(h * h + d2d * d2d);
  }

  // cases that are purely 3D
  // the line is skewed, therefore it must intersect the plane in one point
  auto plane_p = poly_plane.Intersection(line);
  if (!plane_p.has_value() || !std::holds_alternative<Point3D>(*plane_p)) {
    throw std::logic_error("skew line does not intersect plane as expected");
  }
  auto const& plane_pp = std::get<Point3D>(*plane_p);

  // if the point is inside the polygon the distance is zero
  if (polygon.Contains(plane_pp)) {
    return 0.0;
  }

  // otherwise, the true minimum is on the polygon's boundary (see ring_distance_to_line above) —
  // scan the outer ring and every hole, in native 3D, no projection needed.
  double min_d = ring_distance_to_line(polygon.Perimeter(), line.Origin(), line.Direction());
  for (auto const& hole : polygon.Holes()) {
    min_d = std::min(min_d, ring_distance_to_line(hole, line.Origin(), line.Direction()));
  }
  return min_d;
}

namespace {

// Cheapest View2D that can represent `plane`: one of the fast world-plane views when `plane` is parallel
// to that world plane (whatever its offset — e.g. z = 5 still qualifies for XY), otherwise the plane's own
// (Custom) basis. Mirrors the selection distance_to(Polygon3D, Line3D) makes above. Subsumes the old
// exact-through-origin check (Plane::XY() etc.): that's just the offset = 0 case of the same test.
View2D view_for_plane(Plane const& plane) {
  if (plane.normal().IsParallel(Vector3D::BasisZ())) {
    return View2D::XY(plane.origin().z());
  }
  if (plane.normal().IsParallel(Vector3D::BasisX())) {
    return View2D::YZ(plane.origin().x());
  }
  if (plane.normal().IsParallel(Vector3D::BasisY())) {
    return View2D::ZX(plane.origin().y());
  }
  return View2D::OnPlane(plane);
}

}  // namespace

PolygonTangents<LineSegment3D> tangents_to(Polygon3D const& polygon, Point3D const& p) {
  auto poly_plane = polygon.GetPlane();
  if (!poly_plane.Contains(p)) {
    throw std::logic_error("tangents_to(Polygon3D, Point3D) — point is not coplanar with the polygon");
  }

  auto [left_i, right_i] =
      detail::view::point_poly_tangent_lr_to(polygon.Perimeter(), polygon.IsConvex(), p, view_for_plane(poly_plane));
  return {LineSegment3D::Make(p, polygon[left_i]), LineSegment3D::Make(p, polygon[right_i])};
}

PolygonTangents<LineSegment3D> tangents_to(Polygon3D const& polygon, Polygon3D const& other) {
  auto poly_plane = polygon.GetPlane();
  if (!(poly_plane == other.GetPlane())) {
    throw std::logic_error("tangents_to(Polygon3D, Polygon3D) — polygons are not coplanar");
  }
  View2D view = view_for_plane(poly_plane);

  auto [RL_poly_i, RL_other_i] = detail::view::poly_poly_RL_tangent_to(polygon.Perimeter(), polygon.IsConvex(),
                                                                       other.Perimeter(), other.IsConvex(), view);
  auto [LR_other_i, LR_poly_i] = detail::view::poly_poly_RL_tangent_to(other.Perimeter(), other.IsConvex(),
                                                                       polygon.Perimeter(), polygon.IsConvex(), view);
  return {LineSegment3D::Make(polygon[RL_poly_i], other[RL_other_i]),
          LineSegment3D::Make(polygon[LR_poly_i], other[LR_other_i])};
}

}  // namespace geometry

}  // namespace geompp
