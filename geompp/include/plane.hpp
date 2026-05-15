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

  Point3D inline const origin() const { return Origin; }
  Vector3D inline const normal() const { return Normal; }
  Vector3D inline const axis_u() const { return AxisU; }
  Vector3D inline const axis_v() const { return AxisV; }

  bool AlmostEquals(Plane const& other, double epsilon = DOUBLE_EPSILON) const;
  Plane& operator=(Plane const& other);

#pragma region Geometrial Operations
  double SignedDistanceTo(Point3D const& p) const;
  double DistanceTo(Point3D const& p) const;
  Point3D ProjectOnto(Point3D const& p) const;
  Point2D ProjectInto(Point3D const& p) const;
  Point3D Evaluate(Point2D const& p) const;

  bool Contains(Point3D const& point) const;

  using ReturnSet = std::optional<std::variant<Point3D, Line3D, LineSegment3D>>;

  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& ray) const;
  bool Intersects(LineSegment3D const& segment) const;
  bool Intersects(Plane const& plane) const;
  bool Intersects(Triangle3D const& triangle) const;

  ReturnSet Intersection(Line3D const& line) const;
  ReturnSet Intersection(Ray3D const& ray) const;
  ReturnSet Intersection(LineSegment3D const& segment) const;
  ReturnSet Intersection(Plane const& plane) const;
  ReturnSet Intersection(Triangle3D const& triangle) const;

  bool IsParallel(Line3D const& line) const;
  bool IsParallel(Ray3D const& ray) const;
  bool IsParallel(LineSegment3D const& segment) const;
  bool IsCoplanar(Line3D const& line) const;
  bool IsCoplanar(Ray3D const& ray) const;
  bool IsCoplanar(LineSegment3D const& segment) const;

#pragma endregion

  static inline Plane XY() { return Plane(Point3D::Zero(), Vector3D::BasisZ()); }
  static inline Plane YZ() { return Plane(Point3D::Zero(), Vector3D::BasisX()); }
  static inline Plane ZX() { return Plane(Point3D::Zero(), Vector3D::BasisY()); }

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
