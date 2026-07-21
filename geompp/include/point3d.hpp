#pragma once

#include "constants.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace geompp {

class Point3D {
 public:
  Point3D(double x, double y, double z);
  Point3D(Point3D const&) = default;
  Point3D(Vector3D const&);
  Point3D(Point3D&&) = default;
  ~Point3D() = default;

  double x() const;
  double y() const;
  double z() const;

  Vector3D ToVector() const;
  bool AlmostEquals(Point3D const& other, double epsilon = DOUBLE_EPSILON) const;
  double DistanceTo(Point3D const& other) const;

  std::string ToWkt() const;
  static Point3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Point3D FromFile(std::string const& path);

  Point3D& operator=(Point3D const& other);

  static Point3D Zero();

 private:
  double X, Y, Z;
};

#pragma region Collection Operations

bool are_collinear(Point3D const& p1, Point3D const& p2, Point3D const& p3);

std::vector<Point3D> remove_consecutive_duplicates(std::vector<Point3D> const& points);

std::vector<Point3D> remove_duplicates(std::vector<Point3D> const& points);

std::vector<Point3D> remove_collinear(std::vector<Point3D> const& points);

Point3D linear_combination(std::vector<Point3D> const& points, std::vector<double> const& weights);

Point3D average(std::vector<Point3D> const& points);

std::vector<Point3D> convex_hull(std::vector<Point3D> const& points, std::optional<Vector3D> normal = std::nullopt);

/// @brief Linear interpolation: P0 + t * (P1 - P0). Not clamped — t outside [0, 1] extrapolates past P0/P1.
Point3D lerp(Point3D const& P0, Point3D const& P1, double t);

#pragma endregion

#pragma region Operators Overloading

bool operator==(Point3D const& lhs, Point3D const& rhs);

Point3D operator+(Point3D const& lhs, Vector3D const& rhs);
Point3D& operator+=(Point3D& lhs, Vector3D const& rhs);
Vector3D operator-(Point3D const& lhs, Point3D const& rhs);
Point3D operator-(Point3D const& lhs, Vector3D const& rhs);

Point3D operator*(Point3D const& lhs, double a);
Point3D operator*(double a, Point3D const& rhs);
Point3D operator*(Point3D const& lhs, Point3D const& rhs) = delete;
Point3D operator+(Point3D const& lhs, Point3D const& rhs) = delete;

Point3D operator/(Point3D const& lhs, Point3D const& rhs) = delete;
Point3D operator/(Point3D const& lhs, double a);

std::ostream& operator<<(std::ostream& os, Point3D const& g);

#pragma endregion

#pragma region Formatter

// #include <format>
//  template <>
//  struct std::formatter<Point3D> {
//    constexpr auto parse(std::format_parse_context& ctx) {
//      return ctx.end();
//    }
//
//    auto format(Point3D const& p, std::format_context& ctx) {
//      return std::format_to(ctx.out(), "{}", p.ToWkt());
//    }
//  };

#pragma endregion

#pragma region Inlined Functions

inline Point3D::Point3D(double x, double y, double z) : X(x), Y(y), Z(z) {}
inline Point3D::Point3D(Vector3D const& v) : X(v.x()), Y(v.y()), Z(v.z()) {}
inline double Point3D::x() const { return X; }
inline double Point3D::y() const { return Y; }
inline double Point3D::z() const { return Z; }
inline Point3D Point3D::Zero() { return Point3D(0, 0, 0); }

#pragma endregion

}  // namespace geompp
