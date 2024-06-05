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
  static LineSegment2D Make(Point2D const& p0, Point2D const& p1, int decimal_precision = DP_THREE);
  LineSegment2D(LineSegment2D const&) = default;
  LineSegment2D(LineSegment2D&&) = default;
  ~LineSegment2D() = default;

  inline Point2D const& First() const { return P0; }
  inline Point2D const& Last() const { return P1; }

  bool AlmostEquals(LineSegment2D const& other, int decimal_precision = DP_THREE) const;
  Line2D ToLine(int decimal_precision = DP_THREE) const;
  double Length() const;
  double DistanceTo(Point2D const& point, int decimal_precision = DP_THREE) const;
  double Location(Point2D const& point, int decimal_precision = DP_THREE) const;
  Point2D Interpolate(double pct) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static LineSegment2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static LineSegment2D FromFile(std::string const& path);

  LineSegment2D& operator=(LineSegment2D const& other);

#pragma region Geometrical Operations
  bool Contains(Point2D const& point, int decimal_precision = DP_THREE) const;
  using ReturnSet = std::optional<std::variant<Point2D>>;
  bool Intersects(Line2D const& line, int decimal_precision = DP_THREE) const;
  bool Intersects(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  bool Intersects(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Line2D const& line, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(LineSegment2D const& other, int decimal_precision = DP_THREE) const;
#pragma endregion

 private:
  Point2D P0, P1;

  LineSegment2D(Point2D const& p0, Point2D const& p1);
};

#pragma region Operator Overloading

bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs);

#pragma endregion

}  // namespace geompp