#pragma once

#include "calc_utils2d.hpp"
#include "generic_concepts.hpp"
#include "constants.hpp"
#include "point2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <array>
#include <limits>
#include <ostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class BRect2D {
 public:
  /// @brief Builds the minimum oriented bounding rectangle using the rotating-calipers method
  /// (Freeman & Shapira 1975 / Toussaint 1983). For non-convex input, computes the convex hull
  /// first (Andrew's monotone chain, O(n log n)), then rotates calipers around each hull edge to
  /// find the minimum-area rectangle. Accepts any random-access sized range of Point2D-compatible elements.
  template <PointContainer Points>
  BRect2D(Points const& points);

  BRect2D(BRect2D const&) = default;
  BRect2D(BRect2D&&) = default;
  ~BRect2D() = default;

  Point2D center() const;
  Vector2D axis_u() const;
  Vector2D axis_v() const;
  double half_len_u() const;
  double half_len_v() const;
  double width() const;
  double height() const;
  double area() const;
  std::array<Point2D, 4> Corners() const;

  bool AlmostEquals(BRect2D const& other, double epsilon = DOUBLE_EPSILON) const;
  BRect2D& operator=(BRect2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside this oriented bounding rectangle (inclusive of the boundary).
  /// Projects the point onto the rectangle's local axes; returns true when both projections are
  /// within [-half_len_u, half_len_u] x [-half_len_v, half_len_v].
  bool Contains(Point2D const& p) const;

#pragma endregion

 private:
  Point2D CENTER{0.0, 0.0};
  Vector2D AXIS_U{1.0, 0.0};
  Vector2D AXIS_V{0.0, 1.0};
  double HALF_LEN_U{0.0};
  double HALF_LEN_V{0.0};
};

#pragma region Operators Overloading

bool operator==(BRect2D const& lhs, BRect2D const& rhs);

#pragma endregion

#pragma region Inlined Functions

inline Point2D BRect2D::center() const { return CENTER; }
inline Vector2D BRect2D::axis_u() const { return AXIS_U; }
inline Vector2D BRect2D::axis_v() const { return AXIS_V; }
inline double BRect2D::half_len_u() const { return HALF_LEN_U; }
inline double BRect2D::half_len_v() const { return HALF_LEN_V; }
inline double BRect2D::width() const { return 2.0 * HALF_LEN_U; }
inline double BRect2D::height() const { return 2.0 * HALF_LEN_V; }
inline double BRect2D::area() const { return 4.0 * HALF_LEN_U * HALF_LEN_V; }

#pragma endregion

extern template BRect2D::BRect2D(std::vector<Point2D> const&);

}  // namespace geometry

}  // namespace geompp
