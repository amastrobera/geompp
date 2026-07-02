#pragma once

#include "concepts.hpp"
#include "constants.hpp"
#include "point2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <cmath>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace geompp {

class BBall2D {
 public:
  BBall2D(Point2D const& center, double radius);
  /// @brief uses the algorithm Fast Ball (Ritter 1990) to compute the center and a radius of a cloud of points in O(N)
  /// time. Accepts any random-access sized range of Point2D-compatible elements.
  template <PointContainer Points>
  BBall2D(Points const& points);

  BBall2D(BBall2D const&) = default;
  BBall2D(BBall2D&&) = default;
  ~BBall2D() = default;

  Point2D center() const;
  double radius() const;

  bool AlmostEquals(BBall2D const& other, double epsilon = DOUBLE_EPSILON) const;
  BBall2D& operator=(BBall2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside this bounding ball (inclusive of the boundary).
  /// @param p The point to test.
  /// @return true if @p p falls within the closed ball of radius RADIUS centered at CENTER.
  bool Contains(Point2D const& p) const;

#pragma endregion

 private:
  Point2D CENTER{0.0, 0.0};
  double RADIUS;
};

#pragma region Operators Overloading

bool operator==(BBall2D const& lhs, BBall2D const& rhs);

#pragma endregion

#pragma region Inlined Functions

inline Point2D BBall2D::center() const { return CENTER; }
inline double BBall2D::radius() const { return RADIUS; }
inline BBall2D::BBall2D(Point2D const& center, double radius) : CENTER(center), RADIUS(radius) {}

#pragma endregion

extern template BBall2D::BBall2D(std::vector<Point2D> const&);

}  // namespace geompp
