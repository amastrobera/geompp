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

  bool AlmostEquals(Plane const& other) const;
  Plane& operator=(Plane const& other);

#pragma region Geometrial Operations
  double SignedDistanceTo(Point3D const& p) const;
  double DistanceTo(Point3D const& p) const;
  Point3D ProjectOnto(Point3D const& p) const;
  Point2D ProjectInto(Point3D const& p) const;
  Point3D Evaluate(Point2D const& p) const;

  bool Contains(Point3D const& point) const;
  using ReturnSet = std::optional<std::variant<Point3D>>;
  bool Intersects(Line3D const& line) const;
  ReturnSet Intersection(Line3D const& line) const;

#pragma endregion

  static inline Plane XY() { return Plane(Point3D{0, 0, 0}, Vector3D{0, 0, 1}); }
  static inline Plane YZ() { return Plane(Point3D{0, 0, 0}, Vector3D{1, 0, 0}); }
  static inline Plane ZX() { return Plane(Point3D{0, 0, 0}, Vector3D{0, 1, 0}); }

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

}  // namespace geompp
