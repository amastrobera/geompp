#include "brect2d.hpp"

#include "utils.hpp"

#include <cmath>
#include <vector>

namespace geompp {

BRect2D& BRect2D::operator=(BRect2D const& other) {
  if (this != &other) {
    CENTER = other.CENTER;
    AXIS_U = other.AXIS_U;
    AXIS_V = other.AXIS_V;
    HALF_LEN_U = other.HALF_LEN_U;
    HALF_LEN_V = other.HALF_LEN_V;
  }
  return *this;
}

bool BRect2D::AlmostEquals(BRect2D const& other, double epsilon) const {
  return CENTER.AlmostEquals(other.CENTER, epsilon) &&
         AXIS_U.AlmostEquals(other.AXIS_U, epsilon) &&
         AXIS_V.AlmostEquals(other.AXIS_V, epsilon) &&
         compare(HALF_LEN_U, other.HALF_LEN_U, epsilon) == 0 &&
         compare(HALF_LEN_V, other.HALF_LEN_V, epsilon) == 0;
}

std::array<Point2D, 4> BRect2D::Corners() const {
  auto hu = HALF_LEN_U * AXIS_U;
  auto hv = HALF_LEN_V * AXIS_V;
  return {
    CENTER + hu + hv,
    CENTER - hu + hv,
    CENTER - hu - hv,
    CENTER + hu - hv,
  };
}

#pragma region Geometrical Operations

bool BRect2D::Contains(Point2D const& p) const {
  auto dp = p - CENTER;
  double pu = std::abs(dp.Dot(AXIS_U));
  double pv = std::abs(dp.Dot(AXIS_V));
  return compare(pu, HALF_LEN_U) <= 0 && compare(pv, HALF_LEN_V) <= 0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(BRect2D const& lhs, BRect2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

template <PointContainer Points>
BRect2D::BRect2D(Points const& points) {
  std::size_t n = std::ranges::size(points);
  if (n < 3) {
    throw std::invalid_argument("cannot make oriented bounding rectangle from less than 3 points");
  }
  auto hull_idx = detail::convex_hull_indices(points);
  std::size_t m = hull_idx.size();
  if (m < 3) {
    throw std::invalid_argument("cannot make oriented bounding rectangle from collinear or coincident points");
  }
  auto mbr = detail::view::min_bounding_rect(hull_idx, points, View2D::XY());
  AXIS_U = Vector2D{mbr.u_axis_x, mbr.u_axis_y}.Normalize();
  AXIS_V = Vector2D{mbr.v_axis_x, mbr.v_axis_y}.Normalize();
  HALF_LEN_U = mbr.half_len_u;
  HALF_LEN_V = mbr.half_len_v;
  CENTER = Point2D(mbr.origin_x, mbr.origin_y) + mbr.center_u * AXIS_U + mbr.center_v * AXIS_V;
}

template BRect2D::BRect2D(std::vector<Point2D> const&);

}  // namespace geompp
