#pragma once

#include "constants.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "triangle3d.hpp"

#include <ostream>
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
  BBox3D(Point3D const& min, Point3D const& max);
  BBox3D(LineSegment3D const& s);
  BBox3D(Polyline3D const& s);
  BBox3D(Polygon3D const& s);
  BBox3D(Triangle3D const& s);

  BBox3D(BBox3D const&);
  BBox3D(BBox3D&&) = default;
  ~BBox3D() = default;

  inline Point3D min() const { return MIN; }
  inline Point3D max() const { return MAX; }

  bool AlmostEquals(BBox3D const& other, double epsilon = DOUBLE_EPSILON) const;
  BBox3D& operator=(BBox3D const& other);

#pragma region Geometrical Operations

  bool Contains(Point3D const& p) const;

#pragma endregion

 private:
  Point3D MIN{0.0, 0.0, 0.0}, MAX{0.0, 0.0, 0.0};
};

#pragma region Operators Overloading

bool operator==(BBox3D const& lhs, BBox3D const& rhs);

#pragma endregion

}  // namespace geompp
