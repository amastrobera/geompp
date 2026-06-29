#include "bball2d.hpp"

#include "utils.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <unordered_set>

namespace geompp {

BBall2D::BBall2D(Point2D const& center, double radius) : CENTER(center), RADIUS(radius) {}

BBall2D::BBall2D(std::vector<Point2D> const& points) {
  std::size_t n = points.size();
  if (n == 0) {
    throw std::invalid_argument("cannot make bounding ball from an empty point cloud");
  }

  Point2D c = {0.0, 0.0};
  double rad, rad2;
  double x_min, x_max, y_min, y_max;
  std::size_t i_x_min, i_x_max, i_y_min, i_y_max;

  // find the largest diameter to start with
  x_min = x_max = points[0].x();
  y_min = y_max = points[0].y();
  i_x_min = i_x_max = i_y_min = i_y_max = 0;
  for (std::size_t i = 1; i < n; ++i) {
    // update the values of the (x,y) min and max
    if (compare(points[i].x(), x_min) < 0) {
      x_min = points[i].x();
      i_x_min = i;
    }
    if (compare(points[i].x(), x_max) > 0) {
      x_max = points[i].x();
      i_x_max = i;
    }
    if (compare(points[i].y(), y_min) < 0) {
      y_min = points[i].y();
      i_y_min = i;
    }
    if (compare(points[i].y(), y_max) > 0) {
      y_max = points[i].y();
      i_y_max = i;
    }
  }

  // select the largest extent as an initial ball diameter
  Vector2D dx = points[i_x_max] - points[i_x_min];
  Vector2D dy = points[i_y_max] - points[i_y_min];
  double dx2 = dx.Dot(dx);  // do not use sqrt yet to save time
  double dy2 = dy.Dot(dy);  // do not use sqrt yet to save time
  if (compare(dx2, dy2) >= 0) {
    c = points[i_x_min] + dx / 2.0;
    rad2 = (points[i_x_max] - c).Dot(points[i_x_max] - c);  // do not use sqrt yet to save time
  } else {
    c = points[i_y_min] + dy / 2.0;
    rad2 = (points[i_y_max] - c).Dot(points[i_y_max] - c);  // do not use sqrt yet to save time
  }
  rad = sqrt(rad2);

  // check that all points are contained in the ball
  // ... and if not, change the radius and the center accordingly
  double dist, dist2;
  for (std::size_t i = 0; i < n; ++i) {
    Vector2D dp = points[i] - c;
    dist2 = dp.Dot(dp);  // do not use sqrt yet to save time

    if (compare(dist2, rad2) <= 0) {
      continue;
    }

    // this point is not in the ball, let's change the ball's parameters
    dist = sqrt(dist2);
    rad = (rad + dist) / 2.0;  // move the new radius halfway towards the new max point distance
    rad2 = rad * rad;
    c = c + (dist - rad) * dp / dist;  // also move the center halfway towards the new max point
  }

  CENTER = c;
  RADIUS = rad;
}

BBall2D::BBall2D(BBall2D const& b) : CENTER(b.CENTER), RADIUS(b.RADIUS) {}

Point2D BBall2D::center() const { return CENTER; }
double BBall2D::radius() const { return RADIUS; }

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

}  // namespace geompp
