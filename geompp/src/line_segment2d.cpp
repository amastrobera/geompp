#include "line_segment2d.hpp"

#include "line2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include <format>
#include <limits>
#include <stdexcept>

namespace geompp {

#pragma region Constructors

LineSegment2D LineSegment2D::Make(Point2D const& p0, Point2D const& p1, int decimal_precision) {
  if (p0.AlmostEquals(p1, decimal_precision)) {
    throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision",
                                         p0.ToWkt(decimal_precision), p1.ToWkt(decimal_precision), decimal_precision));
  }
  return {p0, p1};
}

LineSegment2D::LineSegment2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1) {}

double LineSegment2D::Length() const { return (P1 - P0).Length(); }

Line2D LineSegment2D::ToLine(int decimal_precision) const { return Line2D::Make(P0, P1, decimal_precision); }

double LineSegment2D::Location(Point2D const& point, int decimal_precision) const {
  if (!ToLine().Contains(point, decimal_precision)) {
    return std::numeric_limits<double>::infinity();
  }
  return sign((point - P0).Dot(P1 - P0), decimal_precision) * (point - P0).Length() / Length();
}

#pragma endregion

#pragma region Geometrical Operations

bool LineSegment2D::Contains(Point2D const& point, int decimal_precision) const {
  double t = Location(point, decimal_precision);
  return round_to(t, decimal_precision) >= 0 && round_to(t - 1, decimal_precision) <= 0;
}

bool LineSegment2D::Intersects(Line2D const& line, int decimal_precision) const {
  return Intersection(line, decimal_precision).has_value();
}

bool LineSegment2D::Intersects(Ray2D const& ray, int decimal_precision) const {
  return Intersection(ray, decimal_precision).has_value();
}

bool LineSegment2D::Intersects(LineSegment2D const& other, int decimal_precision) const {
  return Intersection(other, decimal_precision).has_value();
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(Line2D const& line, int decimal_precision) const {
  auto u = P1 - P0;
  auto v = line.Direction();
  auto vp = v.Perp();
  auto w = (P0 - line.First());

  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);

  // verify that the intersection is ahead of the ray
  auto inter_p = P0 + t * u;
  if (!Contains(inter_p, decimal_precision)) {
    return std::nullopt;
  }

  return inter_p;
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(Ray2D const& ray, int decimal_precision) const {
  auto u = P1 - P0;
  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto v = ray.Direction();
  auto vp = v.Perp();
  auto w = (P0 - ray.Origin());

  // testing on this ray
  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);
  auto inter_t = P0 + t * u;
  if (!Contains(inter_t, decimal_precision)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (round_to(v * up, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = ray.Origin() + s * v;
  if (!ray.IsAhead(inter_s, decimal_precision)) {
    return std::nullopt;
  }

  return inter_t;
}

LineSegment2D::ReturnSet LineSegment2D::Intersection(LineSegment2D const& other, int decimal_precision) const {
  auto u = P1 - P0;
  auto up = u.Perp();  // equivalent (calc, on the other side)
  auto v = (other.P1 - other.P0);
  auto vp = v.Perp();
  auto w = (P0 - other.P0);

  // testing on this ray
  if (round_to(u * vp, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double t = (-w * vp) / (u * vp);
  auto inter_t = P0 + t * u;
  if (!Contains(inter_t, decimal_precision)) {
    return std::nullopt;
  }

  // testing on the other ray
  if (round_to(v * up, decimal_precision) == 0.0) {
    return std::nullopt;
  }
  double s = (w * up) / (v * up);  // equivalent (calc on the other side)
  auto inter_s = other.P0 + s * v;
  if (!other.Contains(inter_s, decimal_precision)) {
    return std::nullopt;
  }

  return inter_t;
}

#pragma endregion

}  // namespace geompp
