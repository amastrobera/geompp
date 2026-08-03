#pragma once

#include "generic_concepts.hpp"
#include "constants.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace geompp {

class Vector3D;
class LineSegment3D;
class Polyline3D;
class Polygon3D;
class Triangle3D;

class BBox3D {
 public:
  /// @brief XYZ Axis Aligned 3D bounding box. Built in O(N) from the min/max x, y, z coordinate of a cloud of points.
  /// It is ideal and quick for rejecting containment or intersection operations
  BBox3D(Point3D const& min, Point3D const& max);
  BBox3D(LineSegment3D const& s);
  BBox3D(Polyline3D const& s);
  BBox3D(Polygon3D const& s);
  BBox3D(Triangle3D const& s);
  /// @brief Builds the tight axis-aligned box from any random-access sized range of Point3D-compatible elements.
  template <PointContainer Points>
  BBox3D(Points const& points);

  BBox3D(BBox3D const&) = default;
  BBox3D(BBox3D&&) = default;
  ~BBox3D() = default;

  Point3D min() const;
  Point3D max() const;

  bool AlmostEquals(BBox3D const& other, double epsilon = DOUBLE_EPSILON) const;
  BBox3D& operator=(BBox3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).
  /// @param p The point to test.
  /// @return true if @p p falls within the closed box [min, max] along each axis.
  bool Contains(Point3D const& p) const;

#pragma endregion

 private:
  Point3D MIN{0.0, 0.0, 0.0}, MAX{0.0, 0.0, 0.0};
};

#pragma region Operators Overloading

bool operator==(BBox3D const& lhs, BBox3D const& rhs);

#pragma endregion

#pragma region Inlined Functions

inline Point3D BBox3D::min() const { return MIN; }
inline Point3D BBox3D::max() const { return MAX; }
inline BBox3D::BBox3D(Point3D const& min, Point3D const& max) : MIN(min), MAX(max) {}

#pragma endregion

extern template BBox3D::BBox3D(std::vector<Point3D> const&);

}  // namespace geompp
