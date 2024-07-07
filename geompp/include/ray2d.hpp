#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Line2D;
class LineSegment2D;

class Ray2D {
 public:
  static Ray2D Make(Point2D const& orig, Vector2D const& dir, int decimal_precision = DP_THREE);
  Ray2D(Ray2D const&) = default;
  Ray2D(Ray2D&&) = default;
  ~Ray2D() = default;

  inline Point2D const& Origin() const { return ORIGIN; }
  inline Vector2D const& Direction() const { return DIR; }
  bool AlmostEquals(Ray2D const& other, int decimal_precision = DP_THREE) const;
  bool IsAhead(Point2D const& point, int decimal_precision = DP_THREE) const;
  bool IsBehind(Point2D const& point, int decimal_precision = DP_THREE) const;
  Line2D ToLine(int decimal_precision = DP_THREE) const;
  double DistanceTo(Point2D const& point, int decimal_precision = DP_THREE) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Ray2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Ray2D FromFile(std::string const& path);

  Ray2D& operator=(Ray2D const& other);

#pragma region Geometrical Operations
  bool Contains(Point2D const& point, int decimal_precision = DP_THREE) const;
  using ReturnSet = std::optional<std::variant<Point2D>>;
  bool Intersects(Line2D const& line, int decimal_precision = DP_THREE) const;
  bool Intersects(Ray2D const& other, int decimal_precision = DP_THREE) const;
  bool Intersects(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Line2D const& line, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(Ray2D const& other, int decimal_precision = DP_THREE) const;
  ReturnSet Intersection(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
#pragma endregion

 private:
  Point2D ORIGIN;
  Vector2D DIR;  // unit

  Ray2D(Point2D const& orig, Vector2D const& dir);
};

#pragma region Operator Overloading

bool operator==(Ray2D const& lhs, Ray2D const& rhs);

std::ostream& operator<<(std::ostream& os, Ray2D const& g);

#pragma endregion

}  // namespace geompp