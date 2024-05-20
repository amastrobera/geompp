#pragma once

#include "constants.hpp"
#include "vector2d.hpp"


namespace geompp {

class Vector2D;

class Point2D {
  double X, Y;

public:
  Point2D(double x, double y);
  Point2D(Point2D const &) = default;
  Point2D(Point2D &&) = default;
  ~Point2D() = default;

  double inline const x() const { return X; }
  double inline const y() const { return Y; }

  Point2D operator+(Point2D const &point) = delete;
  Point2D operator+(Vector2D const &vec);

  bool operator==(Point const &other, int decimal_precision = DP_THREE);
};

} // namespace geompp