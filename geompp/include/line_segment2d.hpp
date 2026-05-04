#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <string>
#include <variant>

namespace geompp {

class Line2D;
class Ray2D;

class LineSegment2D {
 public:
  static LineSegment2D Make(Point2D const& p0, Point2D const& p1);
  LineSegment2D(LineSegment2D const&) = default;
  LineSegment2D(LineSegment2D&&) = default;
  ~LineSegment2D() = default;

  inline Point2D const& First() const { return P0; }
  inline Point2D const& Last() const { return P1; }

  bool AlmostEquals(LineSegment2D const& other, double epsilon = DOUBLE_EPSILON) const;
  Line2D ToLine() const;
  double Length() const;

#pragma region line operations

  Point2D ProjectOnto(Point2D const& point) const;
  double DistanceTo(Point2D const& point) const;
  double Location(Point2D const& point) const;
  Point2D Interpolate(double pct) const;

#pragma endregion

  std::string ToWkt() const;
  static LineSegment2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static LineSegment2D FromFile(std::string const& path);

  LineSegment2D& operator=(LineSegment2D const& other);

#pragma region Geometrical Operations
  bool Contains(Point2D const& point) const;
  using ReturnSet = std::optional<std::variant<Point2D>>;
  bool Intersects(Line2D const& line) const;
  bool Intersects(Ray2D const& ray) const;
  bool Intersects(LineSegment2D const& segment) const;
  ReturnSet Intersection(Line2D const& line) const;
  ReturnSet Intersection(Ray2D const& ray) const;
  ReturnSet Intersection(LineSegment2D const& other) const;
#pragma endregion

 private:
  Point2D P0, P1;

  LineSegment2D(Point2D const& p0, Point2D const& p1);
};

#pragma region Operator Overloading

bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs);

std::ostream& operator<<(std::ostream& os, LineSegment2D const& g);

#pragma endregion

}  // namespace geompp
