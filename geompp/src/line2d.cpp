#include "line2d.hpp"
#include "utils.hpp"

#include <format>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Line2D Line2D::Make(Point2D const& p0, Point2D const& p1, int decimal_precision) {
  if (p0.AlmostEquals(p1, decimal_precision)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision",
                                         p0.ToWkt(decimal_precision), p1.ToWkt(decimal_precision), decimal_precision));
  }
  return Line2D(p0, p1);
}

Line2D Line2D::Make(Point2D const& p0, Vector2D const& dir, int decimal_precision) {
  if (round_to(dir.Length(), decimal_precision) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", decimal_precision));
  }
  return Line2D(p0, dir);
}

Line2D::Line2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1), DIR(p1 - p0) {}

Line2D::Line2D(Point2D const& orig, Vector2D const& dir) : P0(orig), DIR(dir), P1(orig + dir) {}

#pragma endregion

#pragma region Geometrical Operations

bool Line2D::Contains(Point2D const& point, int decimal_precision) const {
    return round_to((point - P0).Cross(DIR), decimal_precision) == 0.0;
}

bool Line2D::Intersects(Line2D const& other, int decimal_precision) {
    // very easy to verify in 2D plane
    return round_to(DIR.Cross(other.DIR), decimal_precision) != 0.0;
}

std::optional<Shape2D> Line2D::Intersection(Line2D const& other, int decimal_precision ) {

    auto u = DIR;
    // auto up = u.Perp(); // equivalent (calc, on the other side)
    auto v = other.DIR;
    auto vp = v.Perp();
    auto w = (P0 - other.P0);

    if (round_to(u * vp, decimal_precision) == 0.0) {
        return std::nullopt;
    }
    double t = (-w * vp) / (u * vp);

    //if (round_to(v * up, decimal_precision) == 0.0) {
    //    return std::nullopt;
    //}
    // double s = (w * up) / (v * up); // equivalent (calc on the other side)

    return P0 + t * u;
}


#pragma endregion



}  // namespace geompp
