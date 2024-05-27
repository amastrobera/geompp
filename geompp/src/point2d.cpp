#include "point2d.hpp"

#include "vector2d.hpp"

#include <cmath>

namespace geompp {

Point2D::Point2D(double x, double y) : X(x), Y(y) {}

Point2D::Point2D(Point2D const& p) : X(p.X), Y(p.Y) {}

bool Point2D::AlmostEquals(Point2D const& other, int decimal_precision) const {
  return round((X - other.X) * pow(10, decimal_precision)) == 0.0 &&
         round((Y - other.Y) * pow(10, decimal_precision)) == 0.0;
}

Vector2D Point2D::ToVector() { return {X, Y}; }

#pragma region Operator Overloading
bool operator==(Point2D const& lhs, Point2D const& rhs) { return lhs.AlmostEquals(rhs); }

Point2D operator+(Point2D const& lhs, Vector2D const& rhs) { return {lhs.x() + rhs.x(), lhs.y() + rhs.y()}; }

Vector2D operator-(Point2D const& lhs, Point2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }
Point2D operator-(Point2D const& lhs, Vector2D const& rhs) { return {lhs.x() - rhs.x(), lhs.y() - rhs.y()}; }

Point2D operator*(Point2D const& lhs, double a) { return {lhs.x() * a, lhs.y() * a}; }
Point2D operator*(double a, Point2D const& rhs) { return rhs * a; }

#pragma endregion

#pragma region Formatting

std::string Point2D::ToWkt(int decimal_precision) const { return std::format("POINT ({} {})", X, Y); }

#pragma endregion

}  // namespace geompp
