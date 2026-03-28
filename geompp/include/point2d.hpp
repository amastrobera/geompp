#pragma once

#include "constants.hpp"

#include <ostream>
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
  bool AlmostEquals(Point2D const& other) const;
  double DistanceTo(Point2D const& other) const;

  std::string ToWkt() const;
  static Point2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Point2D FromFile(std::string const& path);

  Point2D& operator=(Point2D const& other);

  static inline Point2D Origin() { return Point2D(); }

 private:
  double X, Y;
};

#pragma region Collection Operations

bool are_collinear(Point2D const& p1, Point2D const& p2, Point2D const& p3);

std::vector<Point2D> remove_duplicates(std::vector<Point2D> const& points);

std::vector<Point2D> remove_collinear(std::vector<Point2D> const& points);

Point2D linear_combination(std::vector<Point2D> const& points, std::vector<double> const& weights);

Point2D average(std::vector<Point2D> const& points);

#pragma endregion

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

}  // namespace geompp
