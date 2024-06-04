#pragma once

#include "utils.hpp"

#include <string>

namespace geompp {

class Point2D;

class Vector2D {
 public:
  Vector2D(double x = 0.0, double y = 0.0);
  Vector2D(Vector2D const&) = default;
  Vector2D(Vector2D&&) = default;
  ~Vector2D() = default;

  double inline const x() const { return X; }
  double inline const y() const { return Y; }

  double Length() const;
  bool AlmostEquals(Vector2D const& other, int decimal_precision = DP_THREE) const;
  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Vector2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Vector2D FromFile(std::string const& path);

  Vector2D& operator=(Vector2D const& other);

  double Dot(Vector2D const& v) const;
  double Cross(Vector2D const& v) const;
  Vector2D Perp() const;
  Vector2D Normalize() const;

  Vector2D operator-();

 private:
  double X, Y;
};

#pragma region Operator Overloading

bool operator==(Vector2D const& lhs, Vector2D const& rhs);

Point2D operator+(Vector2D const& lhs, Point2D const& point);
Vector2D operator+(Vector2D const& lhs, Vector2D const& vec);

Vector2D operator-(Vector2D const& lhs, Vector2D const& vec);

Vector2D operator*(Vector2D const& lhs, double a);
Vector2D operator*(double a, Vector2D const& rhs);
double operator*(Vector2D const& lhs, Vector2D const& vec);

Vector2D operator/(Vector2D const& lhs, Vector2D const& vec) = delete;

#pragma endregion

}  // namespace geompp