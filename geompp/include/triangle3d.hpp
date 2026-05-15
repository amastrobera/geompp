#pragma once

#include "constants.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <variant>

namespace geompp {

class Line3D;
class Ray3D;
// class Polyline3D;

class Triangle3D {
 public:
  static Triangle3D Make(Point3D const& p0, Point3D const& p1, Point3D const& p2);
  Triangle3D(Triangle3D const&) = default;
  Triangle3D(Triangle3D&&) = default;
  ~Triangle3D() = default;

  inline std::tuple<Point3D, Point3D, Point3D> const Vertices() const { return {P0, P1, P2}; }

  bool AlmostEquals(Triangle3D const& other, double epsilon = DOUBLE_EPSILON) const;
  Point3D Centroid() const;
  Polygon3D ToPolygon() const;  // useful for ToWkt() polygon
  Plane ToPlane() const;
  Vector3D AreaVector() const;
  Vector3D Normal() const;
  double SignedArea(Vector3D const& ref_normal)
      const;  // if negative the order of points is clock-wise, otherwise it's counter-clockwise
  double Area() const;
  double Perimeter() const;
  bool IsCCW(Vector3D const& ref_normal) const;

#pragma region line operations

  double DistanceTo(Point3D const& point) const;
  std::tuple<Vector3D, Vector3D> ToAxis()
      const;  // returnx the axis U and axis V of the triangle (U = P1-P0, V = P2-P0)
  std::optional<Point3D> Interpolate(double s, double t) const;
  std::optional<std::tuple<double, double>> Location(Point3D const& point) const;

#pragma endregion

  std::string ToWkt() const;
  static Triangle3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Triangle3D FromFile(std::string const& path);

  Triangle3D& operator=(Triangle3D const& other);

#pragma region Geometrical Operations
  bool Contains(Point3D const& point) const;
  using ReturnSet = std::optional<std::variant<Point3D, LineSegment3D, Triangle3D, Polygon3D>>;
  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& ray) const;
  bool Intersects(LineSegment3D const& segment) const;
  bool Intersects(Plane const& plane) const;
  bool Intersects(Triangle3D const& other) const;

  ReturnSet Intersection(Line3D const& line) const;
  ReturnSet Intersection(Ray3D const& ray) const;
  ReturnSet Intersection(LineSegment3D const& segment) const;
  ReturnSet Intersection(Plane const& plane) const;
  ReturnSet Intersection(Triangle3D const& other) const;
#pragma endregion

 private:
  Point3D P0, P1, P2;

  Triangle3D(Point3D const& p0, Point3D const& p1, Point3D const& p2);
};

#pragma region Operator Overloading

bool operator==(Triangle3D const& lhs, Triangle3D const& rhs);

std::ostream& operator<<(std::ostream& os, Triangle3D const& g);

#pragma endregion

}  // namespace geompp
