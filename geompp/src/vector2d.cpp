#include "vector2d.hpp"
#include "utils.hpp"

#include <cmath>

namespace geompp {

Vector2D::Vector2D(double x, double y) : X(x), Y(y) {}

double Vector2D::Length() const { return sqrt(pow(X, 2) + pow(Y, 2)); }

bool Vector2D::AlmostEquals(Vector2D const& other, int decimal_precision) const {
  return round((X - other.X) * pow(10, decimal_precision)) == 0 &&
         round((Y - other.Y) * pow(10, decimal_precision)) == 0;
}

double Vector2D::Dot(Vector2D const& v) const { return (X * v.X + Y * v.Y); }

double Vector2D::Cross(Vector2D const& v) const { return (-Y * v.X + X * v.Y); }

Vector2D Vector2D::Perp() const { return Vector2D(-Y, X); }

Vector2D Vector2D::Normalize() const {
    double len = Length();
    return Vector2D(X / len, Y / len);
}

// operators

Vector2D Vector2D::operator-() { return Vector2D(-X, -Y); }

bool operator==(Vector2D const& lhs, Vector2D const& rhs) { return lhs.AlmostEquals(rhs); }

Vector2D operator+(Vector2D const& lhs, Vector2D const& rhs) { return Vector2D(lhs.x() + rhs.x(), lhs.y() + rhs.y()); }

Vector2D operator-(Vector2D const& lhs, Vector2D const& rhs) { return Vector2D(lhs.x() - rhs.x(), lhs.y() - rhs.y()); }

Point2D operator+(Vector2D const& lhs, Point2D const& rhs) { return Point2D(lhs.x() + rhs.x(), lhs.y() + rhs.y()); }

Vector2D operator*(Vector2D const& lhs, double a) { return Vector2D(lhs.x() * a, lhs.y() * a); }

Vector2D operator*(double a, Vector2D const& rhs) { return rhs * a; }

double operator*(Vector2D const& lhs, Vector2D const& rhs) { return lhs.Dot(rhs); }

}  // namespace geompp
