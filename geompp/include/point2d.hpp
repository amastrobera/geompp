#pragma once

#include "constants.hpp"
#include "vector2d.hpp"

#include <ostream>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class Point2D {
 public:
  Point2D(double x, double y);
  Point2D(Point2D const&) = default;
  Point2D(Vector2D const&);
  Point2D(Point2D&&) = default;
  ~Point2D() = default;

  double x() const;
  double y() const;

  Vector2D ToVector() const;
  bool AlmostEquals(Point2D const& other, double epsilon = DOUBLE_EPSILON) const;
  double DistanceTo(Point2D const& other) const;

  std::string ToWkt() const;
  static Point2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Point2D FromFile(std::string const& path);

  Point2D& operator=(Point2D const& other);

  static Point2D Zero();

 private:
  double X, Y;
};

#pragma region Collection Operations

/// @brief tells whether a point p is on the left of a line defined by (v1,v2).
///        alternatively, it tells whether the vector (p-v1) forms a counter clockwise rotation to the vector (v2-v1)
bool is_left(Point2D const& v1, Point2D const& v2, Point2D const& p);

/// @brief tells whether a point p is on the right of a line defined by (v1,v2).
///        alternatively, it tells whether the vector (p-v1) forms a clockwise rotation to the vector (v2-v1)
bool is_right(Point2D const& v1, Point2D const& v2, Point2D const& p);

bool are_collinear(Point2D const& p1, Point2D const& p2, Point2D const& p3);

std::vector<Point2D> remove_consecutive_duplicates(std::vector<Point2D> const& points);
/// @brief Same as the const& overload, but compacts @p points in place (no fresh vector allocated)
/// instead of copying survivors into a new one.
std::vector<Point2D> remove_consecutive_duplicates(std::vector<Point2D>&& points);

std::vector<Point2D> remove_duplicates(std::vector<Point2D> const& points);

std::vector<Point2D> remove_collinear(std::vector<Point2D> const& points);
/// @brief Same as the const& overload, but compacts @p points in place (no fresh vector allocated)
/// instead of copying survivors into a new one.
std::vector<Point2D> remove_collinear(std::vector<Point2D>&& points);

Point2D linear_combination(std::vector<Point2D> const& points, std::vector<double> const& weights);

Point2D average(std::vector<Point2D> const& points);

/// @brief Sorts an arbitrary (not necessarily ring-ordered) set of points into counter-clockwise
/// angular order around their arithmetic mean.
/// @param points The points to sort. Order on input does not matter.
/// @return @p points unchanged if it has fewer than 3 points (a warning is logged); otherwise a copy
/// sorted by ascending angle (atan2) around `average(points)`.
std::vector<Point2D> sort_ccw(std::vector<Point2D> const& points);

std::vector<Point2D> convex_hull(std::vector<Point2D> const& points);

/// @brief Linear interpolation: P0 + t * (P1 - P0). Not clamped — t outside [0, 1] extrapolates past P0/P1.
Point2D lerp(Point2D const& P0, Point2D const& P1, double t);

#pragma endregion

#pragma region Operators Overloading

bool operator==(Point2D const& lhs, Point2D const& rhs);

Point2D operator+(Point2D const& lhs, Vector2D const& rhs);
Point2D& operator+=(Point2D& lhs, Vector2D const& rhs);

Vector2D operator-(Point2D const& lhs, Point2D const& rhs);
Point2D operator-(Point2D const& lhs, Vector2D const& rhs);

Point2D operator*(Point2D const& lhs, double a);
Point2D operator*(double a, Point2D const& rhs);
Point2D operator*(Point2D const& lhs, Point2D const& rhs) = delete;
Point2D operator+(Point2D const& lhs, Point2D const& rhs) = delete;

Point2D operator/(Point2D const& lhs, Point2D const& rhs) = delete;
Point2D operator/(Point2D const& lhs, double a);

std::ostream& operator<<(std::ostream& os, Point2D const& g);

#pragma endregion

#pragma region Formatter

// #include <format>
//  template <>
//  struct std::formatter<Point2D> {
//    constexpr auto parse(std::format_parse_context& ctx) {
//      return ctx.end();
//    }
//
//    auto format(Point2D const& p, std::format_context& ctx) {
//      return std::format_to(ctx.out(), "{}", p.ToWkt());
//    }
//  };

#pragma endregion

#pragma region Collection Operations

double signed_area(std::vector<Point2D> const& points);

bool are_ccw(std::vector<Point2D> const& points);

bool are_cw(std::vector<Point2D> const& points);

Point2D centroid(std::vector<Point2D> const& points);

#pragma endregion

#pragma region Inlined Functions

inline Point2D::Point2D(double x, double y) : X(x), Y(y) {}
inline Point2D::Point2D(Vector2D const& v) : X(v.x()), Y(v.y()) {}
inline double Point2D::x() const { return X; }
inline double Point2D::y() const { return Y; }
inline Point2D Point2D::Zero() { return Point2D(0, 0); }

#pragma endregion

}  // namespace geometry

}  // namespace geompp
