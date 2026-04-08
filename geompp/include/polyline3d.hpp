#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Line3D;
class Ray3D;
class LineSegment3D;

class Polyline3D {
 public:
  // TODO: it would be nice to use variadic templates,
  //       BUT they don't support the optional parameter decimal_precision,
  //       AND must be definied in the header!
  static Polyline3D Make(std::vector<Point3D> const& points);
  Polyline3D(Polyline3D const&) = default;
  Polyline3D(Polyline3D&&) = default;
  ~Polyline3D() = default;

  inline int Size() const { return KNOTS.size(); }
  Point3D const& operator[](size_t i) const;
  // TODO: it would be nice to have a "generator" with coroutines that "yields" point by point

  bool AlmostEquals(Polyline3D const& other, double epsilon = DOUBLE_EPSILON) const;
  std::vector<LineSegment3D> ToSegments() const;
  double Length() const;
  double DistanceTo(Point3D const& point) const;
  double Location(Point3D const& point) const;
  Point3D Interpolate(double pct) const;

  std::string ToWkt() const;
  static Polyline3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polyline3D FromFile(std::string const& path);

  Polyline3D& operator=(Polyline3D const& other);

#pragma region Geometrical Operations
  bool Contains(Point3D const& point) const;
  using MultiPoint = std::vector<Point3D>;
  using ReturnSet = std::optional<std::variant<Point3D, MultiPoint>>;
  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& ray) const;
  bool Intersects(LineSegment3D const& segment) const;
  bool Intersects(Polyline3D const& other) const;
  ReturnSet Intersection(Line3D const& line) const;
  ReturnSet Intersection(Ray3D const& ray) const;
  ReturnSet Intersection(LineSegment3D const& segment) const;
  ReturnSet Intersection(
      Polyline3D const& other) const;  // TODO: this is the brute force O(N2), replace with the proper
                                       // algorithm for intersection of a set of segments O(N*LogN)
#pragma endregion

 private:
  std::vector<Point3D> KNOTS;

  Polyline3D(std::vector<Point3D>&& points);
};

#pragma region Operator Overloading

bool operator==(Polyline3D const& lhs, Polyline3D const& rhs);

std::ostream& operator<<(std::ostream& os, Polyline3D const& g);

#pragma endregion

}  // namespace geompp
