#pragma once

#include "point2d.hpp"

namespace geompp {

class Point2D;

class Vector2D {
 public:
  Vector2D(double x, double y);
  Vector2D(Vector2D const&) = default;
  Vector2D(Vector2D&&) = default;
  ~Vector2D() = default;

  double inline const x() const { return X; }
  double inline const y() const { return Y; }

  double Length() const;
  bool AlmostEquals(Vector2D const& other, int decimal_precision = DP_THREE) const;

  double Dot(Vector2D const& v);
  double Cross(Vector2D const& v);

 private:
  double X, Y;
};

// operators
bool operator==(Vector2D const& lhs, Vector2D const& rhs);

Point2D operator+(Vector2D const& lhs, Point2D const& point);
Vector2D operator+(Vector2D const& lhs, Vector2D const& vec);

Vector2D operator-(Vector2D const& lhs, Vector2D const& vec);

Vector2D operator*(Vector2D const& lhs, double a);
Vector2D operator*(double a, Vector2D const& rhs);
Vector2D operator*(Vector2D const& lhs, Vector2D const& vec) = delete;

Vector2D operator/(Vector2D const& lhs, Vector2D const& vec) = delete;

}  // namespace geompp