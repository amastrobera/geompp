#pragma once

#include "generic_concepts.hpp"
#include "constants.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <cmath>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class BBall3D {
 public:
  BBall3D(Point3D const& center, double radius);
  /// @brief uses the algorithm Fast Ball (Ritter 1990) to compute the center and a radius of a cloud of points in O(N)
  /// time. Accepts any random-access sized range of Point3D-compatible elements.
  template <PointContainer Points>
  BBall3D(Points const& points);

  BBall3D(BBall3D const&) = default;
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

#pragma region Inlined Functions

inline Point3D BBall3D::center() const { return CENTER; }
inline double BBall3D::radius() const { return RADIUS; }
inline BBall3D::BBall3D(Point3D const& center, double radius) : CENTER(center), RADIUS(radius) {}

#pragma endregion

extern template BBall3D::BBall3D(std::vector<Point3D> const&);

}  // namespace geometry

}  // namespace geompp
