#pragma once

#include "generic_concepts.hpp"
#include "constants.hpp"
#include "point2d.hpp"
#include "utils.hpp"

#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class Vector2D;
class LineSegment2D;
class Polyline2D;
class Polygon2D;
class Triangle2D;

class BBox2D {
 public:
  /// @brief XY Axis Aligned 2D bounding box. Built in O(N) from the min/max x, y coordinate of a cloud of points.
  /// It is ideal and quick for rejecting containment or intersection operations
  BBox2D(Point2D const& min, Point2D const& max);
  BBox2D(LineSegment2D const& s);
  BBox2D(Polyline2D const& s);
  BBox2D(Polygon2D const& s);
  BBox2D(Triangle2D const& s);
  /// @brief Builds the tight axis-aligned box from any random-access sized range of Point2D-compatible elements.
  template <PointContainer Points>
  BBox2D(Points const& points);

  BBox2D(BBox2D const&) = default;
  BBox2D(BBox2D&&) = default;
  ~BBox2D() = default;

  Point2D min() const;
  Point2D max() const;

  bool AlmostEquals(BBox2D const& other, double epsilon = DOUBLE_EPSILON) const;
  BBox2D& operator=(BBox2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).
  /// @param p The point to test.
  /// @return true if @p p falls within the closed box [min, max] along each axis.
  bool Contains(Point2D const& p) const;

#pragma endregion

 private:
  Point2D MIN{0.0, 0.0}, MAX{0.0, 0.0};
};

#pragma region Operators Overloading

bool operator==(BBox2D const& lhs, BBox2D const& rhs);

#pragma endregion

#pragma region Inlined Functions

inline Point2D BBox2D::min() const { return MIN; }
inline Point2D BBox2D::max() const { return MAX; }
inline BBox2D::BBox2D(Point2D const& min, Point2D const& max) : MIN(min), MAX(max) {}

#pragma endregion

extern template BBox2D::BBox2D(std::vector<Point2D> const&);

}  // namespace geometry

}  // namespace geompp
