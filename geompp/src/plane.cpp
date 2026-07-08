#include "plane.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "ray3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <cmath>
#include <format>
#include <fstream>

namespace geompp {

Plane Plane::From3Points(Point3D p1, Point3D p2, Point3D p3) { return FromOriginAndAxes(p1, p2 - p1, p3 - p1); }

Plane Plane::FromOriginAndAxes(Point3D origin, Vector3D u, Vector3D v) {
  if (compare(u.Length(), 0) == 0) {
    throw new std::runtime_error("zero length axis u");
  }
  if (compare(v.Length(), 0) == 0) {
    throw new std::runtime_error("zero length axis v");
  }
  return {origin, u, v};
}

Plane Plane::FromOriginAndNormal(Point3D origin, Vector3D normal) {
  if (compare(normal.Length(), 0) == 0) {
    throw new std::runtime_error("zero length normal");
  }
  return {origin, normal};
}

Plane::Plane(Point3D origin, Vector3D normal)
    : Origin(origin), Normal(normal), AxisU(normal.Perp().Normalize()), AxisV(normal.Cross(AxisU).Normalize()) {}

Plane::Plane(Point3D origin, Vector3D u, Vector3D v)
    : Origin(origin), Normal(u.Cross(v).Normalize()), AxisU(u.Normalize()), AxisV(Normal.Cross(AxisU).Normalize()) {}

bool Plane::AlmostEquals(Plane const& other, double epsilon) const {
  return (
      // same normal (same winding order CCW or CW)
      Normal.AlmostEquals(other.Normal, epsilon) &&
      // and, either equal origins
      (Origin.AlmostEquals(other.Origin, epsilon) ||
       // or on the same plane
       compare(Normal.Dot(Origin - other.Origin), epsilon) == 0));
}

Plane& Plane::operator=(Plane const& other) {
  if (this != &other) {
    Origin = other.Origin;
    Normal = other.Normal;
    AxisU = other.AxisU;
    AxisV = other.AxisV;
  }
  return *this;
}

#pragma region Geometrical Operations

double Plane::SignedDistanceTo(Point3D const& p) const { return (p - Origin).Dot(Normal); }

double Plane::DistanceTo(Point3D const& p) const { return std::abs(SignedDistanceTo(p)); }

Point3D Plane::ProjectOnto(Point3D const& p) const { return p - SignedDistanceTo(p) * Normal; }

Point2D Plane::ProjectInto(Point3D const& p) const {
  auto p_to_orig = p - Origin;
  double u = p_to_orig.Dot(AxisU);
  double v = p_to_orig.Dot(AxisV);
  return {u, v};
}

Point3D Plane::Evaluate(Point2D const& p) const { return Origin + AxisU * p.x() + AxisV * p.y(); }

bool Plane::Contains(Point3D const& point) const { return compare((point - Origin).Dot(Normal), 0) == 0; }

bool Plane::Intersects(Line3D const& line) const { return Intersection(line).has_value(); }

bool Plane::Intersects(Ray3D const& ray) const { return Intersection(ray).has_value(); }

bool Plane::Intersects(LineSegment3D const& segment) const { return Intersection(segment).has_value(); }

bool Plane::Intersects(Plane const& plane) const { return Intersection(plane).has_value(); }

bool Plane::Intersects(Triangle3D const& triangle) const { return Intersection(triangle).has_value(); }

Plane::ReturnSet Plane::Intersection(Line3D const& line) const {
  auto U = line.Last() - line.First();
  auto W = line.First() - Origin;
  auto denominator = U.Dot(Normal);
  if (compare(denominator, 0) == 0) {
    if (compare(W.Dot(Normal), 0) == 0) {
      return line;  // the line lies entirely in the plane
    }
    return std::nullopt;  // parallel and distinct
  }
  double s = -W.Dot(Normal) / denominator;
  return ProjectOnto(line.First() + s * U);  // snap to plane: division by small denominator can accumulate error
}

Plane::ReturnSet Plane::Intersection(Ray3D const& ray) const {
  auto line_intersection = Intersection(ray.ToLine());
  if (!line_intersection.has_value()) {
    return std::nullopt;
  }

  if (std::holds_alternative<Line3D>(*line_intersection)) {
    return line_intersection;  // the ray's line lies in the plane, so the ray does too — report it as-is
  }

  auto const& intersection_point = std::get<Point3D>(*line_intersection);
  if (!ray.IsAhead(intersection_point)) {
    return std::nullopt;
  }
  return intersection_point;
}

Plane::ReturnSet Plane::Intersection(LineSegment3D const& segment) const {
  auto line_intersection = Intersection(segment.ToLine());
  if (!line_intersection.has_value()) {
    return std::nullopt;
  }

  if (std::holds_alternative<Line3D>(*line_intersection)) {
    return segment;  // the segment's line lies in the plane, so the segment does too
  }

  auto const& intersection_point = std::get<Point3D>(*line_intersection);
  if (!segment.Contains(intersection_point)) {
    return std::nullopt;
  }
  return intersection_point;
}

Plane::ReturnSet Plane::Intersection(Plane const& plane) const {
  auto n_cross = Normal.Cross(plane.Normal);
  if (compare(n_cross.Length(), 0) == 0) {
    // parallel or same plane
    return std::nullopt;
  }

  // line of intersection is defined by the cross product of the normals, and a point that is in both planes
  // using the "plane-intersect-point" formula, we can find a point that is in both planes

  double d1 = -Normal.Dot(Origin.ToVector());
  double d2 = -plane.Normal.Dot(plane.Origin.ToVector());

  Point3D P0 = (d2 * Normal - d1 * plane.Normal).Cross(n_cross) / n_cross.Dot(n_cross);

  return Line3D::Make(P0, n_cross);
}

Plane::ReturnSet Plane::Intersection(Triangle3D const& triangle) const {
  // Delegate to Triangle3D::Intersection(Plane). Plane-triangle intersection can only produce a
  // Point3D or a LineSegment3D — both are also valid alternatives of Plane::ReturnSet, so we
  // unwrap and rewrap rather than returning the triangle-side variant directly (the two variants
  // have different alternative sets).
  auto result = triangle.Intersection(*this);
  if (!result.has_value()) {
    return std::nullopt;
  }
  if (std::holds_alternative<Point3D>(*result)) {
    return std::get<Point3D>(*result);
  }
  if (std::holds_alternative<LineSegment3D>(*result)) {
    return std::get<LineSegment3D>(*result);
  }
  return std::nullopt;
}

bool Plane::IsParallel(Line3D const& line) const { return compare(line.Direction().Dot(Normal), 0) == 0; }

bool Plane::IsParallel(Ray3D const& ray) const { return compare(ray.Direction().Dot(Normal), 0) == 0; }

bool Plane::IsParallel(LineSegment3D const& segment) const {
  return compare((segment.Last() - segment.First()).Dot(Normal), 0) == 0;
}

bool Plane::IsCoplanar(Line3D const& line) const { return IsParallel(line) && Contains(line.First()); }

bool Plane::IsCoplanar(Ray3D const& ray) const { return IsParallel(ray) && Contains(ray.Origin()); }

bool Plane::IsCoplanar(LineSegment3D const& segment) const { return IsParallel(segment) && Contains(segment.First()); }

#pragma endregion

#pragma region Operator Overloading

bool operator==(Plane const& lhs, Plane const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

#pragma region Collection Operations

bool are_coplanar(std::vector<Point3D> const& points) {
  auto unique_points = remove_collinear(points);
  if (unique_points.size() < 4) {
    return true;
  }

  // avoid building a plane and making a constructor
  auto normal = (unique_points[1] - unique_points[0]).Cross(unique_points[2] - unique_points[0]);

  // if normal and Pi-P0 are not orthogonal, then the point is not in the plane defined by P0, P1 and P2
  for (int i = 3; i < unique_points.size(); ++i) {
    if (compare(normal.Dot(unique_points[i] - unique_points[0]), 0) != 0) {
      return false;
    }
  }

  return true;
}

Plane closest_world_plane_to(std::vector<Point3D> const& points) {
  auto unique_points = remove_collinear(points);
  if (unique_points.size() < 3) {
    throw std::runtime_error("closest_world_plane_to requires at least 3 non-collinear points");
  }
  Vector3D points_normal = Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]).normal();
  Vector3D world_normal = {0, 0, 0};

  // Find the absolute largest component of the normal
  double absX = std::abs(points_normal.x());
  double absY = std::abs(points_normal.y());
  double absZ = std::abs(points_normal.z());

  if (compare(absZ, absX) >= 0 && compare(absZ, absY) >= 0) {
    world_normal = Vector3D::BasisZ();

  } else {
    world_normal = compare(absX, absY) >= 0 ? Vector3D::BasisX() : Vector3D::BasisY();
  }

  return Plane::FromOriginAndNormal(Point3D::Zero(), world_normal);
}

// Snyder & Barr [1987] approach: pick the dominant normal axis, and project the polygon there
//                                (simple drop of coordinate)
//                                then compute the 2D signed area, and multiply by the normal
// This makes the calculation quicker and removes the overhead of re-constructing a vector of 2D points
// Note: this function could have been written more elegantly projecting on 2D all points and re-using
//       the 2D signed area function, thus avoiding code-redundancy. However, we preferred to invest in
//       performance, avoiding the filling of a vector of 2D points and calling many constructors, I
//       re-wrote the shoelace formuala in 3D.
double signed_area(std::vector<Point3D> const& points, std::optional<Plane> plane) {
  auto unique_points = remove_collinear(points);
  if (unique_points.size() < 3) {
    throw std::runtime_error(
        std::format("cannot compute area of a set of points with less than 3 unique points; points are too close with "
                    "{} decimals precision",
                    DECIMAL_PRECISION));
  }
  if (!plane.has_value()) {
    plane = closest_world_plane_to(unique_points);
  }

  double signed_area = 0;
  std::size_t n = unique_points.size();

  // select the larges coordinate of the normal
  auto dominant_coord = plane->normal().DominantAxis();
  switch (dominant_coord) {
    case Axis::X: {  // Project onto the YZ plane at zero cost
      // simplification of the Shoelace formula, using only 1 multiplication and 1 subtraction per point, instead of 2
      // multiplications and 1 subtraction for the general case
      for (int i = 0; i < n; ++i) {
        auto const& p0 = unique_points[(n + i - 1) % n];
        auto const& p1 = unique_points[i];
        auto const& p2 = unique_points[(i + 1) % n];
        signed_area += p1.y() * (p2.z() - p0.z());
      }
    } break;

    case Axis::Y: {  // Project onto the ZX plane at zero cost
      // simplification of the Shoelace formula, using only 1 multiplication and 1 subtraction per point, instead of 2
      // multiplications and 1 subtraction for the general case
      for (int i = 0; i < n; ++i) {
        auto const& p0 = unique_points[(n + i - 1) % n];
        auto const& p1 = unique_points[i];
        auto const& p2 = unique_points[(i + 1) % n];
        signed_area += p1.z() * (p2.x() - p0.x());
      }

    } break;

    case Axis::Z: {  // Project onto the XY plane at zero cost
      // simplification of the Shoelace formula, using only 1 multiplication and 1 subtraction per point, instead of 2
      // multiplications and 1 subtraction for the general case
      for (int i = 0; i < n; ++i) {
        auto const& p0 = unique_points[(n + i - 1) % n];
        auto const& p1 = unique_points[i];
        auto const& p2 = unique_points[(i + 1) % n];
        signed_area += p1.x() * (p2.y() - p0.y());
      }
    } break;
  }

  // scale to get area before projection and get the correct sign, then divide by 2
  double normal_length = plane->normal().Length();
  switch (dominant_coord) {
    case Axis::X: {
      signed_area *= normal_length / (2 * plane->normal().x());
    } break;

    case Axis::Y: {
      signed_area *= normal_length / (2 * plane->normal().y());
    } break;

    case Axis::Z: {
      signed_area *= normal_length / (2 * plane->normal().z());
    } break;
  }

  return signed_area;
}

bool are_ccw(std::vector<Point3D> const& points, std::optional<Plane> plane) {
  return compare(signed_area(points, plane), 0) > 0;
}

bool are_cw(std::vector<Point3D> const& points, std::optional<Plane> plane) {
  return compare(signed_area(points, plane), 0) < 0;
}

// Calculates the centroid of the polygon
// Note: this function could have been written more elegantly projecting on 2D all points and re-using
//       the 2D signed area function, thus avoiding code-redundancy. However, we preferred to invest in
//       performance, avoiding the filling of a vector of 2D points and calling many constructors, I
//       re-wrote the shoelace formuala in 3D.
Point3D centroid(std::vector<Point3D> const& points, std::optional<Plane> plane) {
  auto unique_points = remove_collinear(points);
  if (unique_points.size() <= 3) {
    return average(unique_points);
  }

  if (!plane.has_value()) {
    plane = Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]);
  }

  double sa = 0;
  double cx = 0;
  double cy = 0;
  double cz = 0;
  std::size_t n = unique_points.size();

  // the plane having formula
  //   Ax + By + Cz + D = 0
  //  where (A, B, C) is the normal of the plane,
  //       and D is the distance from the origin to the plane along the normal (D = -N.dot(P0))
  Vector3D normal = plane->normal();
  double A = normal.x();
  double B = normal.y();
  double C = normal.z();
  double D = -normal.Dot(unique_points[0].ToVector());
  //  we will be able to evaluate the 3D coordinate of the centroid based on the other two, using
  //     cy and cz known => cx = (-B*cy - C*cz - D) / A
  //     cx and cz known => cy = (-A*cx - C*cz - D) / B
  //     cx and cy known => cz = (-A*cx - B*cy - D) / C
  //
  // ... in fact the Plane::Evaluate() function cannot be used in this case.
  //     it would require us to project all points on the points->plane() in order to make sense
  //     (and we explicitely don't want to project all points on that plane, as we want to save time and memory)

  // select the larges coordinate of the normal
  auto dominant_coord = plane->normal().DominantAxis();
  switch (dominant_coord) {
    case Axis::X: {  // Project onto the YZ plane at zero cost

      for (int i = 0; i < n; ++i) {
        auto const& p1 = unique_points[i];
        auto const& p2 = unique_points[(i + 1) % n];
        double shoelace = p1.y() * p2.z() - p2.y() * p1.z();
        // we do not need the adjustment for the dominant coordinate as we did in siged_area
        // ... because the adjument happens both on the numerator and the denominator of
        //     the centroid (Cx and Cy) formula and it cancels out
        cy += (p1.y() + p2.y()) * shoelace;
        cz += (p1.z() + p2.z()) * shoelace;
        sa += shoelace;
      }
      sa /= 2.0;

      if (compare(sa, 0) == 0) {
        throw std::runtime_error("centroid of a set of points with zero area (YZ projection)");
      }

      cy /= (6 * sa);
      cz /= (6 * sa);
      cx = (-B * cy - C * cz - D) / A;

    } break;

    case Axis::Y: {  // Project onto the ZX plane at zero cost

      for (int i = 0; i < n; ++i) {
        auto const& p1 = unique_points[i];
        auto const& p2 = unique_points[(i + 1) % n];
        double shoelace = p1.z() * p2.x() - p2.z() * p1.x();
        // we do not need the adjustment for the dominant coordinate as we did in siged_area
        // ... because the adjument happens both on the numerator and the denominator of
        //     the centroid (Cx and Cy) formula and it cancels out
        cz += (p1.z() + p2.z()) * shoelace;
        cx += (p1.x() + p2.x()) * shoelace;
        sa += shoelace;
      }
      sa /= 2.0;

      if (compare(sa, 0) == 0) {
        throw std::runtime_error("centroid of a set of points with zero area (ZX projection)");
      }

      cz /= (6 * sa);
      cx /= (6 * sa);
      cy = (-A * cx - C * cz - D) / B;

    } break;

    case Axis::Z: {  // Project onto the XY plane at zero cost

      for (int i = 0; i < n; ++i) {
        auto const& p1 = unique_points[i];
        auto const& p2 = unique_points[(i + 1) % n];
        double shoelace = p1.x() * p2.y() - p2.x() * p1.y();
        // we do not need the adjustment for the dominant coordinate as we did in siged_area
        // ... because the adjument happens both on the numerator and the denominator of
        //     the centroid (Cx and Cy) formula and it cancels out
        cx += (p1.x() + p2.x()) * shoelace;
        cy += (p1.y() + p2.y()) * shoelace;
        sa += shoelace;
      }
      sa /= 2.0;

      if (compare(sa, 0) == 0) {
        throw std::runtime_error("centroid of a set of points with zero area (XY projection)");
      }

      cx /= (6 * sa);
      cy /= (6 * sa);
      cz = (-A * cx - B * cy - D) / C;

    } break;
  }

  return Point3D(cx, cy, cz);
}

#pragma endregion

}  // namespace geompp
