#pragma once

#include "utils.hpp"

#include <ostream>
#include <string>

namespace geompp {

inline namespace geometry {

class Point2D;

class Vector2D {
 public:
  Vector2D(double x, double y);
  Vector2D(Vector2D const&) = default;
  Vector2D(Vector2D&&) = default;
  ~Vector2D() = default;

  double x() const;
  double y() const;

  Point2D ToPoint() const;

  double Length() const;
  bool AlmostEquals(Vector2D const& other, double epsilon = DOUBLE_EPSILON) const;
  std::string ToWkt() const;
  static Vector2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Vector2D FromFile(std::string const& path);

  Vector2D& operator=(Vector2D const& other);

  double Dot(Vector2D const& v) const;
  double Cross(Vector2D const& v) const;  // aka perp-product
  Vector2D Perp() const;
  Vector2D Normalize() const;
  bool IsParallel(Vector2D const& other) const;

  Vector2D operator-() const;

  static Vector2D BasisX();
  static Vector2D BasisY();

 private:
  double X, Y;
};

#pragma region Operator Overloading

bool operator==(Vector2D const& lhs, Vector2D const& rhs);

Point2D operator+(Vector2D const& lhs, Point2D const& point);
Vector2D operator+(Vector2D const& lhs, Vector2D const& vec);
Vector2D operator+=(Vector2D& lhs, Vector2D const& vec);

Vector2D operator-(Vector2D const& lhs, Vector2D const& vec);

Vector2D operator*(Vector2D const& lhs, double a);
Vector2D operator*(double a, Vector2D const& rhs);
double operator*(Vector2D const& lhs, Vector2D const& vec);

Vector2D operator/(Vector2D const& lhs, Vector2D const& vec) = delete;
Vector2D operator/(Vector2D const& lhs, double a);

std::ostream& operator<<(std::ostream& os, Vector2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline Vector2D::Vector2D(double x, double y) : X(x), Y(y) {}
inline double Vector2D::x() const { return X; }
inline double Vector2D::y() const { return Y; }
inline Vector2D Vector2D::BasisX() { return Vector2D(1, 0); }
inline Vector2D Vector2D::BasisY() { return Vector2D(0, 1); }

#pragma endregion

}  // namespace geometry

}  // namespace geompp
