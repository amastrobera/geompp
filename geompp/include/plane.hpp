#pragma once

#include "utils.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <ostream>
#include <string>


namespace geompp {


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


  bool AlmostEquals(Plane const& other, int decimal_precision = DP_THREE) const;
  Plane& operator=(Plane const& other);

  double DistanceTo(Point3D const& p, int decimal_precision) const;
  Point3D ProjectOnto(Point3D const& p) const;
  Point2D ProjectInto(Point3D const& p) const;
  Point3D Evaluate(Point2D const& p) const;

  static inline Plane XY() { return Plane(Point3D {0,0,0}, Vector3D {0,0,1}); }
  static inline Plane YZ() { return Plane(Point3D {0,0,0}, Vector3D {1,0,0}); }
  static inline Plane ZX() { return Plane(Point3D {0,0,0}, Vector3D {0,1,0}); }

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