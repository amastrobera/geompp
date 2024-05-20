#include "vector2d.hpp"

namespace geompp {

Vector2D::Vector2D(double x, double y) : X(x), Y(y) {}

Vector2D Vector2D::operator+(Vector2D const &vec) {
  return Vector2D(X + vec.X, Y + vec.Y);
}

Point2D Vector2D::operator+(Point2D const &point) {
  return Point2D(point.x() + X, point.y() + Y);
}

} // namespace geompp
