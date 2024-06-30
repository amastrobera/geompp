#pragma once

#include "constants.hpp"

#include <string>
#include <vector>

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
  double DistanceTo(Point2D const& other, int decimal_precision = DP_THREE) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Point2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Point2D FromFile(std::string const& path);

  Point2D& operator=(Point2D const& other);

  static inline Point2D Origin() { return Point2D(); }

#pragma region Collection Operations

  static std::vector<Point2D> remove_duplicates(std::vector<Point2D> const& points, int decimal_precision = DP_THREE);

  static std::vector<Point2D> remove_collinear(std::vector<Point2D> const& points, int decimal_precision = DP_THREE);

  static Point2D average(std::vector<Point2D> const& points);

#pragma endregion

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

//#include <format>
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

#pragma endregion

}  // namespace geompp
