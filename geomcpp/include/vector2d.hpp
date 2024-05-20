#pragma once

#include "point2d.hpp"

namespace geompp {

class Point2D;

class Vector2D {
  double X, Y;

public:
  Vector2D(double x, double y);
  Vector2D(Vector2D const &) = default;
  Vector2D(Vector2D &&) = default;
  ~Vector2D() = default;

  double inline const x() const { return X; }
  double inline const y() const { return Y; }

  Point2D operator+(Point2D const &point);
  Vector2D operator+(Vector2D const &vec);
};

} // namespace geompp