#pragma once

#include "point2d.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Line3D;
class Ray3D;
class LineSegment3D;
class Triangle3D;

class Plane {
 public:
  static Plane From3Points(Point3D p1, Point3D p2, Point3D p3);
  static Plane FromOriginAndAxes(Point3D origin, Vector3D u, Vector3D v);
  static Plane FromOriginAndNormal(Point3D origin, Vector3D normal);

  Plane(Plane const&) = default;
  Plane(Plane&&) = default;
  ~Plane() = default;

  Point3D const origin() const;
  Vector3D const normal() const;
  Vector3D const axis_u() const;
  Vector3D const axis_v() const;

  bool AlmostEquals(Plane const& other, double epsilon = DOUBLE_EPSILON) const;
  Plane& operator=(Plane const& other);

#pragma region Geometrial Operations

  /// @brief Signed perpendicular distance from a point to the plane.
  /// @param p The point.
  /// @return (p - origin) · normal. Positive on the normal's side, negative on the other.
  double SignedDistanceTo(Point3D const& p) const;

  /// @brief Unsigned perpendicular distance from a point to the plane.
  /// @param p The point.
  /// @return Absolute value of @ref SignedDistanceTo.
  double DistanceTo(Point3D const& p) const;

  /// @brief Orthogonal projection of a point onto this plane (3D position).
  /// @param p The point to project.
  /// @return Closest point on the plane to @p p, expressed in world (3D) coordinates.
  Point3D ProjectOnto(Point3D const& p) const;

  /// @brief Orthogonal projection of a point into this plane's local 2D basis (axis_u, axis_v).
  /// @param p The point to project.
  /// @return 2D coordinates (u, v) such that p ≈ origin + u·axis_u + v·axis_v + (distance · normal).
  Point2D ProjectInto(Point3D const& p) const;

  /// @brief Inverse of @ref ProjectInto — lift a local 2D point onto the plane in world coordinates.
  /// @param p The local (u, v) point in the plane's basis.
  /// @return World-space 3D point: origin + p.x · axis_u + p.y · axis_v.
  Point3D Evaluate(Point2D const& p) const;

  /// @brief Tests whether a point lies on the plane.
  /// @param point The point to test.
  /// @return true if @p point's signed distance to the plane is zero within decimal precision.
  bool Contains(Point3D const& point) const;

  /// @brief Tests whether a line meets this plane.
  /// @param line The line.
  /// @return true if the line crosses the plane or lies in it.
  bool Intersects(Line3D const& line) const;

  /// @brief Tests whether a ray meets this plane within its domain.
  /// @param ray The ray.
  /// @return true if the ray crosses the plane ahead of its origin, or lies in it.
  bool Intersects(Ray3D const& ray) const;

  /// @brief Tests whether a segment crosses or touches this plane.
  /// @param segment The segment.
  /// @return true if some part of the segment is on the plane.
  bool Intersects(LineSegment3D const& segment) const;

  /// @brief Tests whether another plane meets this one.
  /// @param plane The other plane.
  /// @return true unless the two planes are parallel and distinct.
  bool Intersects(Plane const& plane) const;

  /// @brief Tests whether a triangle meets this plane.
  /// @param triangle The triangle.
  /// @return true if the plane cuts through the triangle or touches it.
  bool Intersects(Triangle3D const& triangle) const;

  using ReturnSet = std::optional<std::variant<Point3D, Line3D, LineSegment3D>>;

  /// @brief Intersection of this plane with a line.
  /// @param line The line.
  /// @return A Point3D (one crossing), a Line3D (line lies in the plane), or std::nullopt for parallel-distinct.
  ReturnSet Intersection(Line3D const& line) const;

  /// @brief Intersection of this plane with a ray.
  /// @param ray The ray.
  /// @return Point3D, Ray-as-Line3D when the ray lies in the plane, or std::nullopt if the ray points away.
  ReturnSet Intersection(Ray3D const& ray) const;

  /// @brief Intersection of this plane with a segment.
  /// @param segment The segment.
  /// @return Point3D crossing, LineSegment3D (the segment lies in the plane), or std::nullopt if disjoint.
  ReturnSet Intersection(LineSegment3D const& segment) const;

  /// @brief Intersection of two planes.
  /// @param plane The other plane.
  /// @return The shared line as Line3D, or std::nullopt for parallel-distinct planes.
  ReturnSet Intersection(Plane const& plane) const;

  /// @brief Intersection of this plane with a triangle.
  /// @param triangle The triangle.
  /// @return A Point3D (touches a vertex), a LineSegment3D (cuts through interior), or std::nullopt if disjoint.
  ReturnSet Intersection(Triangle3D const& triangle) const;

  /// @brief Tests whether a line is parallel to this plane (no convergence).
  /// @param line The line.
  /// @return true if line.direction · plane.normal == 0. Lines lying *in* the plane also return true.
  bool IsParallel(Line3D const& line) const;

  /// @brief Tests whether a ray is parallel to this plane.
  /// @param ray The ray.
  /// @return true if ray.direction · plane.normal == 0.
  bool IsParallel(Ray3D const& ray) const;

  /// @brief Tests whether a segment is parallel to this plane.
  /// @param segment The segment.
  /// @return true if the segment's direction is perpendicular to the plane's normal.
  bool IsParallel(LineSegment3D const& segment) const;

  /// @brief Tests whether a line lies entirely in this plane.
  /// @param line The line.
  /// @return true if the line is parallel to the plane AND any point of it is on the plane.
  bool IsCoplanar(Line3D const& line) const;

  /// @brief Tests whether a ray lies entirely in this plane.
  /// @param ray The ray.
  /// @return true if the ray is parallel to the plane AND its origin lies on the plane.
  bool IsCoplanar(Ray3D const& ray) const;

  /// @brief Tests whether a segment lies entirely in this plane.
  /// @param segment The segment.
  /// @return true if both segment endpoints lie on the plane.
  bool IsCoplanar(LineSegment3D const& segment) const;

#pragma endregion

  static Plane XY();
  static Plane YZ();
  static Plane ZX();

 private:
  Point3D Origin;
  Vector3D Normal;
  Vector3D AxisU;
  Vector3D AxisV;

  Plane(Point3D origin, Vector3D normal);
  Plane(Point3D origin, Vector3D u, Vector3D v);
};

#pragma region Operator Overloading

bool operator==(Plane const& lhs, Plane const& rhs);

#pragma endregion

#pragma region Inlined Functions

inline Point3D const Plane::origin() const { return Origin; }
inline Vector3D const Plane::normal() const { return Normal; }
inline Vector3D const Plane::axis_u() const { return AxisU; }
inline Vector3D const Plane::axis_v() const { return AxisV; }
inline Plane Plane::XY() { return Plane(Point3D::Zero(), Vector3D::BasisZ()); }
inline Plane Plane::YZ() { return Plane(Point3D::Zero(), Vector3D::BasisX()); }
inline Plane Plane::ZX() { return Plane(Point3D::Zero(), Vector3D::BasisY()); }

#pragma endregion

#pragma region Collection Operations

bool are_coplanar(std::vector<Point3D> const& points);

// returns the XY, YZ or ZX world plane whose normal is closest to the normal of the points' plane
Plane closest_world_plane_to(std::vector<Point3D> const& points);

// computes the signed area of the points according to a ref-plane (if provided)
//    or the plane defined by the first three non-collinear points (otherwise)
// the area is positive if the points are CCW, negative if they are CW, and zero if they are collinear
double signed_area(std::vector<Point3D> const& points, std::optional<Plane> plane = std::nullopt);

// if ref_plane is provided, it will be used to determine the orientation of the points, otherwise the plane will be
// determined by the first three non-collinear points
bool are_ccw(std::vector<Point3D> const& points, std::optional<Plane> ref_plane = std::nullopt);

// if ref_plane is provided, it will be used to determine the orientation of the points, otherwise the plane will be
// determined by the first three non-collinear points
bool are_cw(std::vector<Point3D> const& points, std::optional<Plane> ref_plane = std::nullopt);

Point3D centroid(std::vector<Point3D> const& points, std::optional<Plane> plane = std::nullopt);

#pragma endregion

}  // namespace geompp
