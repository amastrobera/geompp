#include "bball2d.hpp"

#include "utils.hpp"

#include <cmath>
#include <vector>

namespace geompp {


BBall2D& BBall2D::operator=(BBall2D const& other) {
  if (this != &other) {
    CENTER = other.CENTER;
    RADIUS = other.RADIUS;
  }
  return *this;
}

bool BBall2D::AlmostEquals(BBall2D const& other, double epsilon) const {
  return CENTER.AlmostEquals(other.CENTER, epsilon) && compare(RADIUS, other.RADIUS, epsilon) == 0;
}

#pragma region Geometrical Operations

bool BBall2D::Contains(Point2D const& p) const { return compare(CENTER.DistanceTo(p), RADIUS) <= 0; }

#pragma endregion

#pragma region Operator Overloading

bool operator==(BBall2D const& lhs, BBall2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

template <PointContainer Points>
BBall2D::BBall2D(Points const& points) {
  std::size_t n = std::ranges::size(points);
  if (n == 0) {
    throw std::invalid_argument("cannot make bounding ball from an empty point cloud");
  }
  auto b = std::ranges::begin(points);

  Point2D c = {0.0, 0.0};
  double rad, rad2;
  double x_min = b[0].x(), x_max = b[0].x();
  double y_min = b[0].y(), y_max = b[0].y();
  std::size_t i_x_min = 0, i_x_max = 0, i_y_min = 0, i_y_max = 0;

  for (std::size_t i = 1; i < n; ++i) {
    if (compare(b[i].x(), x_min) < 0) {
      x_min = b[i].x();
      i_x_min = i;
    }
    if (compare(b[i].x(), x_max) > 0) {
      x_max = b[i].x();
      i_x_max = i;
    }
    if (compare(b[i].y(), y_min) < 0) {
      y_min = b[i].y();
      i_y_min = i;
    }
    if (compare(b[i].y(), y_max) > 0) {
      y_max = b[i].y();
      i_y_max = i;
    }
  }

  auto dx = b[i_x_max] - b[i_x_min];
  auto dy = b[i_y_max] - b[i_y_min];
  double dx2 = dx.Dot(dx);
  double dy2 = dy.Dot(dy);
  if (compare(dx2, dy2) >= 0) {
    c = b[i_x_min] + dx / 2.0;
    rad2 = (b[i_x_max] - c).Dot(b[i_x_max] - c);
  } else {
    c = b[i_y_min] + dy / 2.0;
    rad2 = (b[i_y_max] - c).Dot(b[i_y_max] - c);
  }
  rad = std::sqrt(rad2);

  for (std::size_t i = 0; i < n; ++i) {
    auto dp = b[i] - c;
    double dist2 = dp.Dot(dp);
    if (compare(dist2, rad2) <= 0) {
      continue;
    }
    double dist = std::sqrt(dist2);
    rad = (rad + dist) / 2.0;
    rad2 = rad * rad;
    c = c + (dist - rad) * dp / dist;
  }

  CENTER = c;
  RADIUS = rad;
}

template BBall2D::BBall2D(std::vector<Point2D> const&);

}  // namespace geompp
