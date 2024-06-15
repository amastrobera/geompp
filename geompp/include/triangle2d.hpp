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
class LineSegment2D;
class Polyline2D;

class Triangle2D {
 public:
  static Triangle2D Make(Point2D const& p0, Point2D const& p1, Point2D const& p2, int decimal_precision = DP_THREE);
  Triangle2D(Triangle2D const&) = default;
  Triangle2D(Triangle2D&&) = default;
  ~Triangle2D() = default;

  // inline Point2D const& First() const { return P0; }
  // inline Point2D const& Last() const { return P1; }

  bool AlmostEquals(Triangle2D const& other, int decimal_precision = DP_THREE) const;
  // Line2D ToLine(int decimal_precision = DP_THREE) const;
  // double Length() const;
  // double DistanceTo(Point2D const& point, int decimal_precision = DP_THREE) const;
  // double Location(Point2D const& point, int decimal_precision = DP_THREE) const;
  // Point2D Interpolate(double pct) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Triangle2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Triangle2D FromFile(std::string const& path);

  Triangle2D& operator=(Triangle2D const& other);

#pragma region Geometrical Operations
  // bool Contains(Point2D const& point, int decimal_precision = DP_THREE) const;
  // using ReturnSet = std::optional<std::variant<Point2D>>;
  // bool Intersects(Line2D const& line, int decimal_precision = DP_THREE) const;
  // bool Intersects(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  // bool Intersects(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(Line2D const& line, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(LineSegment2D const& other, int decimal_precision = DP_THREE) const;
#pragma endregion

 private:
  Point2D P0, P1, P2;

  Triangle2D(Point2D const& p0, Point2D const& p1, Point2D const& p2);
};

#pragma region Operator Overloading

bool operator==(Triangle2D const& lhs, Triangle2D const& rhs);

#pragma endregion

}  // namespace geompp