#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <string>
#include <variant>

namespace geompp {

class Line3D;
class Ray3D;

class LineSegment3D {
 public:
  static LineSegment3D Make(Point3D const& p0, Point3D const& p1);
  LineSegment3D(LineSegment3D const&) = default;
  LineSegment3D(LineSegment3D&&) = default;
  ~LineSegment3D() = default;

  inline Point3D const& First() const { return P0; }
  inline Point3D const& Last() const { return P1; }

  bool AlmostEquals(LineSegment3D const& other) const;
  Line3D ToLine() const;
  double Length() const;
  double DistanceTo(Point3D const& point) const;
  double Location(Point3D const& point) const;
  Point3D Interpolate(double pct) const;

  std::string ToWkt() const;
  static LineSegment3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static LineSegment3D FromFile(std::string const& path);

  LineSegment3D& operator=(LineSegment3D const& other);

#pragma region Geometrical Operations
  bool Contains(Point3D const& point) const;
  using ReturnSet = std::optional<std::variant<Point3D>>;
  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& ray) const;
  bool Intersects(LineSegment3D const& segment) const;
  ReturnSet Intersection(Line3D const& line) const;
  ReturnSet Intersection(Ray3D const& ray) const;
  ReturnSet Intersection(LineSegment3D const& other) const;
#pragma endregion

 private:
  Point3D P0, P1;

  LineSegment3D(Point3D const& p0, Point3D const& p1);
};

#pragma region Operator Overloading

bool operator==(LineSegment3D const& lhs, LineSegment3D const& rhs);

std::ostream& operator<<(std::ostream& os, LineSegment3D const& g);

#pragma endregion

}  // namespace geompp
