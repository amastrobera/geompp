#pragma once

#include "constants.hpp"
#include "vector2d.hpp"

#include <format>

namespace geompp {

class Vector2D;

class Point2D {
 public:
  Point2D(double x = 0.0, double y = 0.0);
  Point2D(Point2D const&);
  Point2D(Point2D&&) = default;
  ~Point2D() = default;

  inline double x() const { return X; }
  inline double y() const { return Y; }

  Vector2D ToVector();

  bool AlmostEquals(Point2D const& other, int decimal_precision = DP_THREE) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;

 private:
  double X, Y;
};

#pragma region Operators Overloading

bool operator==(Point2D const& lhs, Point2D const& rhs);

Point2D operator+(Point2D const& lhs, Vector2D const& rhs);

Vector2D operator-(Point2D const& lhs, Point2D const& rhs);
Point2D operator-(Point2D const& lhs, Vector2D const& rhs);

Point2D operator*(Point2D const& lhs, double a);
Point2D operator*(double a, Point2D const& rhs);
Point2D operator*(Point2D const& lhs, Point2D const& rhs) = delete;
Point2D operator+(Point2D const& lhs, Point2D const& rhs) = delete;

Point2D operator/(Point2D const& lhs, Point2D const& rhs) = delete;

#pragma endregion

#pragma region Formatter

// template <>
// struct std::formatter<Point2D> {
//   constexpr auto parse(std::format_parse_context& ctx) {
//     return ctx.end();
//   }
//
//   auto format(Point2D const& p, std::format_context& ctx) {
//     return std::format_to(ctx.out(), "{}", p.ToWkt());
//   }
// };

}  // namespace geompp

#pragma endregion