#pragma once

#include "constants.hpp"
#include "point3d.hpp"

#include <ostream>
#include <string>
#include <vector>

namespace geompp {

class BBall3D {
 public:
  BBall3D(Point3D const& center, double radius);
  /// @brief uses the algorithm Fast Ball (Ritter 1990) to compute the center and a radius of a cloud of points in O(N)
  /// time
  /// @param points
  BBall3D(std::vector<Point3D> const& points);

  BBall3D(BBall3D const&);
  BBall3D(BBall3D&&) = default;
  ~BBall3D() = default;

  Point3D center() const;
  double radius() const;

  bool AlmostEquals(BBall3D const& other, double epsilon = DOUBLE_EPSILON) const;
  BBall3D& operator=(BBall3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside this bounding ball (inclusive of the boundary).
  /// @param p The point to test.
  /// @return true if @p p falls within the closed ball of radius RADIUS centered at CENTER.
  bool Contains(Point3D const& p) const;

#pragma endregion

 private:
  Point3D CENTER{0.0, 0.0, 0.0};
  double RADIUS;
};

#pragma region Operators Overloading

bool operator==(BBall3D const& lhs, BBall3D const& rhs);

#pragma endregion

}  // namespace geompp
