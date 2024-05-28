#include "vector2d.hpp"

#include "utils.hpp"

#include <cmath>
#include <format>

namespace geompp {

Vector2D::Vector2D(double x, double y) : X(x), Y(y) {}

double Vector2D::Length() const { return sqrt(pow(X, 2) + pow(Y, 2)); }

bool Vector2D::AlmostEquals(Vector2D const& other, int decimal_precision) const {
  return round_to(X - other.X, decimal_precision) == 0 && round_to(Y - other.Y, decimal_precision) == 0;
}

double Vector2D::Dot(Vector2D const& v) const { return (X * v.X + Y * v.Y); }

double Vector2D::Cross(Vector2D const& v) const { return (-Y * v.X + X * v.Y); }

Vector2D Vector2D::Perp() const { return {-Y, X}; }

Vector2D Vector2D::Normalize() const {
  double len = Length();
  return {X / len, Y / len};
}

// operators

Vector2D Vector2D::operator-() { return {-X, -Y}; }

bool operator==(Vector2D const& lhs, Vector2D const& rhs) { return lhs.AlmostEquals(rhs); }

Vector2D operator+(Vector2D const& lhs, Vector2D const& rhs) { return {lhs.x() + rhs.x(), lhs.y() + rhs.y()}; }

Vector2D operator-(Vector2D const& lhs, Vector2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }

Point2D operator+(Vector2D const& lhs, Point2D const& rhs) { return {lhs.x() + rhs.x(), lhs.y() + rhs.y()}; }

Vector2D operator*(Vector2D const& lhs, double a) { return {lhs.x() * a, lhs.y() * a}; }

Vector2D operator*(double a, Vector2D const& rhs) { return rhs * a; }

double operator*(Vector2D const& lhs, Vector2D const& rhs) { return lhs.Dot(rhs); }

#pragma region Formatting

std::string Vector2D::ToWkt(int decimal_precision) const {
  return std::format("VECTOR ({} {})", round_to(X, decimal_precision), round_to(Y, decimal_precision));
}

Vector2D Vector2D::FromWkt(std::string wkt) { throw; }

#pragma endregion

}  // namespace geompp
