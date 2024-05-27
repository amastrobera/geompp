#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <variant>

namespace geompp {

class Line2D;
class LineSegment2D;
class Shape2D;

class Ray2D {
 public:
  static Ray2D Make(Point2D const& orig, Vector2D const& dir, int decimal_precision = DP_THREE);
  Ray2D(Ray2D const&) = default;
  Ray2D(Ray2D&&) = default;
  ~Ray2D() = default;

  inline Point2D const& Origin() const { return ORIGIN; }
  inline Vector2D const& Direction() const { return DIR; }
  bool IsAhead(Point2D const& point, int decimal_precision = DP_THREE) const;
  bool IsBehind(Point2D const& point, int decimal_precision = DP_THREE) const;

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

}  // namespace geompp