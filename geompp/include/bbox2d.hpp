#pragma once

#include "constants.hpp"
#include "point2d.hpp"

#include <ostream>
#include <string>
#include <vector>

namespace geompp {

class Vector2D;
class LineSegment2D;
class Polyline2D;
class Polygon2D;
class Triangle2D;

class BBox2D {
 public:
  BBox2D(Point2D const& min, Point2D const& max);
  BBox2D(LineSegment2D const& s);
  BBox2D(Polyline2D const& s);
  BBox2D(Polygon2D const& s);
  BBox2D(Triangle2D const& s);

  BBox2D(BBox2D const&);
  BBox2D(BBox2D&&) = default;
  ~BBox2D() = default;

  inline Point2D min() const { return MIN; }
  inline Point2D max() const { return MAX; }

  bool AlmostEquals(BBox2D const& other, double epsilon = DOUBLE_EPSILON) const;
  BBox2D& operator=(BBox2D const& other);

#pragma region Geometrical Operations

  bool Contains(Point2D const& p) const;

#pragma endregion

 private:
  Point2D MIN, MAX;
};

#pragma region Operators Overloading

bool operator==(BBox2D const& lhs, BBox2D const& rhs);

#pragma endregion

}  // namespace geompp
