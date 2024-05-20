#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

namespace geompp {

class Line2D {
public:
  static Line2D Make(Point2D const& p0, Point2D const &p1, int decimal_precision = DP_THREE);
  static Line2D Make(Point2D const& orig, Vector2D const& dir, int decimal_precision = DP_THREE);
  Line2D(Line2D const &) = default;
  Line2D(Line2D &&) = default;
  ~Line2D() = default;

  Point2D inline const First() { return P0; }
  Point2D inline const Last() const { return P1; }
  Point2D inline const Origin() { return P0; }
  Vector2D inline const Direction() const { return DIR; }

private:
  Point2D P0, P1;
  Vector2D DIR;

  Line2D(Point2D const &p0, Point2D const &p1);
  Line2D(Point2D const &orig, Vector2D const &dir);
};

} // namespace geompp