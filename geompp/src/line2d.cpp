#include "line2d.hpp"
#include "utils.hpp"

#include <stdexcept>
#include <format>

namespace geompp {

Line2D Line2D::Make(Point2D const& p0, Point2D const& p1, int decimal_precision) {
  if (p0.AlmostEquals(p1, decimal_precision)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision", p0.ToWkt(decimal_precision), 
        p1.ToWkt(decimal_precision), decimal_precision));
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

}  // namespace geompp

