#include "bprism3d.hpp"

#include "utils.hpp"

#include <cmath>
#include <limits>
#include <vector>

namespace geompp {

BPrism3D& BPrism3D::operator=(BPrism3D const& other) {
  if (this != &other) {
    CENTER = other.CENTER;
    AXIS_U = other.AXIS_U;
    AXIS_V = other.AXIS_V;
    AXIS_W = other.AXIS_W;
    HALF_LEN_U = other.HALF_LEN_U;
    HALF_LEN_V = other.HALF_LEN_V;
    HALF_LEN_W = other.HALF_LEN_W;
  }
  return *this;
}

bool BPrism3D::AlmostEquals(BPrism3D const& other, double epsilon) const {
  return CENTER.AlmostEquals(other.CENTER, epsilon) && AXIS_U.AlmostEquals(other.AXIS_U, epsilon) &&
         AXIS_V.AlmostEquals(other.AXIS_V, epsilon) && AXIS_W.AlmostEquals(other.AXIS_W, epsilon) &&
         compare(HALF_LEN_U, other.HALF_LEN_U, epsilon) == 0 && compare(HALF_LEN_V, other.HALF_LEN_V, epsilon) == 0 &&
         compare(HALF_LEN_W, other.HALF_LEN_W, epsilon) == 0;
}

std::array<Point3D, 8> BPrism3D::Corners() const {
  auto hu = HALF_LEN_U * AXIS_U;
  auto hv = HALF_LEN_V * AXIS_V;
  auto hw = HALF_LEN_W * AXIS_W;
  return {
      CENTER + hu + hv + hw, CENTER - hu + hv + hw, CENTER - hu - hv + hw, CENTER + hu - hv + hw,
      CENTER + hu + hv - hw, CENTER - hu + hv - hw, CENTER - hu - hv - hw, CENTER + hu - hv - hw,
  };
}

#pragma region Geometrical Operations

bool BPrism3D::Contains(Point3D const& p) const {
  auto dp = p - CENTER;
  double pu = std::abs(dp.Dot(AXIS_U));
  double pv = std::abs(dp.Dot(AXIS_V));
  double pw = std::abs(dp.Dot(AXIS_W));
  return compare(pu, HALF_LEN_U) <= 0 && compare(pv, HALF_LEN_V) <= 0 && compare(pw, HALF_LEN_W) <= 0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(BPrism3D const& lhs, BPrism3D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

template <PointContainer Points>
BPrism3D::BPrism3D(Points const& points) {
  std::size_t n = std::ranges::size(points);
  if (n < 3) {
    throw std::invalid_argument("cannot make oriented bounding prism from less than 3 points");
  }

  auto frame = principal_axes(points);
  AXIS_U = frame.X;
  AXIS_V = frame.Y;
  AXIS_W = frame.Z;

  Point3D average_pt = average(points);

  auto plane = Plane::FromOriginAndAxes(average_pt, frame.X, frame.Y);
  auto view = View2D::OnPlane(plane);

  auto hull_idx = detail::convex_hull_monotone_chain(points, view);
  std::size_t m = hull_idx.size();
  if (m < 3) {
    throw std::invalid_argument("cannot make oriented bounding prism from collinear or coincident points");
  }

  auto mbr = detail::min_bounding_rect(hull_idx, points, view);
  AXIS_U = (mbr.u_axis_x * frame.X + mbr.u_axis_y * frame.Y).Normalize();
  AXIS_V = (mbr.v_axis_x * frame.X + mbr.v_axis_y * frame.Y).Normalize();
  HALF_LEN_U = mbr.half_len_u;
  HALF_LEN_V = mbr.half_len_v;
  double cx_2d = mbr.origin_x + mbr.center_u * mbr.u_axis_x + mbr.center_v * mbr.v_axis_x;
  double cy_2d = mbr.origin_y + mbr.center_u * mbr.u_axis_y + mbr.center_v * mbr.v_axis_y;

  double w_min = std::numeric_limits<double>::max();
  double w_max = -std::numeric_limits<double>::max();
  for (auto const& p : points) {
    double pw = (p - average_pt).Dot(AXIS_W);
    if (pw < w_min) {
      w_min = pw;
    }
    if (pw > w_max) {
      w_max = pw;
    }
  }
  HALF_LEN_W = (w_max - w_min) / 2.0;
  if (compare(HALF_LEN_W, 0.0) == 0) {
    HALF_LEN_W = DOUBLE_EPSILON;
  }
  double w_center = (w_min + w_max) / 2.0;

  CENTER = average_pt + cx_2d * frame.X + cy_2d * frame.Y + w_center * AXIS_W;
}

template BPrism3D::BPrism3D(std::vector<Point3D> const&);

}  // namespace geompp
