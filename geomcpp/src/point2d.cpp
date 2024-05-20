#include "point2d.hpp"
#include <cmath>

namespace geompp {

Point2D::Point2D(double x, double y) : X(x), Y(y) {}

Point2D Point2D::operator+(Vector2D const &vec) {
  return Point2D(X + vec.x(), Y + vec.y());
}

bool Point2D::operator==(Point const &other, int decimal_precision) {
  return round((X - other.X) * 10 ^ decimal_precision) == 0 &&
         round((Y - other.Y) * 10 ^ decimal_precision) == 0;
}

} // namespace geompp
