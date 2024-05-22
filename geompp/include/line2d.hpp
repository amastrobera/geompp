#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>

namespace geompp {

class Line2D {
 public:
  static Line2D Make(Point2D const& p0, Point2D const& p1, int decimal_precision = DP_THREE);
  static Line2D Make(Point2D const& orig, Vector2D const& dir, int decimal_precision = DP_THREE);
  Line2D(Line2D const&) = default;
  Line2D(Line2D&&) = default;
  ~Line2D() = default;

  inline Point2D const& First() const { return P0; }
  inline Point2D const& Last() const { return P1; }
  inline Point2D const& Origin() const { return P0; }
  inline Vector2D const& Direction() const { return DIR; }

#pragma region Geometrical Operations
  bool Contains(Point2D const& point, int decimal_precision = DP_THREE) const;
  std::optional<Shape2D> Intersection(Line2D const& other, int decimal_precision = DP_THREE);
  bool Intersects(Line2D const& other, int decimal_precision = DP_THREE);
#pragma endregion

 private:
  Point2D P0, P1;
  Vector2D DIR;

  Line2D(Point2D const& p0, Point2D const& p1);
  Line2D(Point2D const& orig, Vector2D const& dir);
};



}  // namespace geompp