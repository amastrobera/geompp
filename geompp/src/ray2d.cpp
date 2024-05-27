#include "ray2d.hpp"

#include "constants.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "utils.hpp"

#include <format>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

Ray2D Ray2D::Make(Point2D const& p0, Vector2D const& dir, int decimal_precision) {
  if (round_to(dir.Length(), decimal_precision) == 0) {
    throw std::runtime_error(std::format("the direction is almost zero with {} decimals precision", decimal_precision));
  }
  return {p0, dir};
}

Ray2D::Ray2D(Point2D const& orig, Vector2D const& dir) : ORIGIN(orig), DIR(dir.Normalize()) {}

bool Ray2D::IsAhead(Point2D const& point, int decimal_precision) const {
  return round_to(DIR.Dot(point - ORIGIN), decimal_precision) >= 0.0;
}

bool Ray2D::IsBehind(Point2D const& point, int decimal_precision) const {
  return round_to(DIR.Dot(point - ORIGIN), decimal_precision) < 0.0;
}

#pragma endregion

#pragma region Geometrical Operations

bool Ray2D::Contains(Point2D const& point, int decimal_precision) const {
  return round_to((point - ORIGIN).Cross(DIR), decimal_precision) == 0.0 && IsAhead(point, decimal_precision);
}

bool Ray2D::Intersects(Line2D const& line, int decimal_precision) const {
  return Intersection(line, decimal_precision).has_value();
}

bool Ray2D::Intersects(Ray2D const& other, int decimal_precision) const {
  return Intersection(other, decimal_precision).has_value();
}

bool Ray2D::Intersects(LineSegment2D const& segment, int decimal_precision) const {
  return segment.Intersects(*this, decimal_precision);
}

Ray2D::ReturnSet Ray2D::Intersection(Line2D const& line, int decimal_precision) const {
  auto u = DIR;
  auto v = line.Direction();
  auto vp = v.Perp();
  auto w = (ORIGIN - line.First());

  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);

  // verify that the intersection is ahead of the ray
  auto inter_p = ORIGIN + t * u;
  if (!IsAhead(inter_p, decimal_precision)) {
    return std::nullopt;
  }

  return inter_p;
}

Ray2D::ReturnSet Ray2D::Intersection(Ray2D const& other, int decimal_precision) const {
  auto u = DIR;
  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto v = other.DIR;
  auto vp = v.Perp();
  auto w = (ORIGIN - other.ORIGIN);

  // testing on this ray
  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);
  auto inter_t = ORIGIN + t * u;
  if (!IsAhead(inter_t, decimal_precision)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (round_to(v * up, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = other.ORIGIN + s * v;
  if (!other.IsAhead(inter_s, decimal_precision)) {
    return std::nullopt;
  }

  return inter_t;
}

Ray2D::ReturnSet Ray2D::Intersection(LineSegment2D const& segment, int decimal_precision) const {
  return segment.Intersection(*this, decimal_precision);
}

#pragma endregion

}  // namespace geompp
