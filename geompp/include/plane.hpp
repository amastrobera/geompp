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
  static Plane From3Points(Point3D p1, Point3D p2, Point3D p3, int decimal_precision = DP_THREE);
  static Plane FromOriginAndAxes(Point3D origin, Vector3D u, Vector3D v, int decimal_precision = DP_THREE);
  static Plane FromOriginAndNormal(Point3D origin, Vector3D normal, int decimal_precision = DP_THREE);

  Plane(Plane const&) = default;
  Plane(Plane&&) = default;
  ~Plane() = default;

  Point3D inline const origin() const { return Origin; }
  Vector3D inline const normal() const { return Normal; }
  Vector3D inline const axis_u() const { return AxisU; }
  Vector3D inline const axis_v() const { return AxisV; }

  bool AlmostEquals(Plane const& other, int decimal_precision = DP_THREE) const;
  Plane& operator=(Plane const& other);

#pragma region Geometrial Operations
  double DistanceTo(Point3D const& p, int decimal_precision = DP_THREE) const;
  Point3D ProjectOnto(Point3D const& p, int decimal_precision = DP_THREE) const;
  Point2D ProjectInto(Point3D const& p, int decimal_precision = DP_THREE) const;
  Point3D Evaluate(Point2D const& p, int decimal_precision = DP_THREE) const;

  bool Contains(Point3D const& point, int decimal_precision = DP_THREE) const;
  using ReturnSet = std::optional<std::variant<Point3D>>;
  bool Intersects(Line3D const& line, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Line3D const& line, int decimal_precision = DP_THREE) const;

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