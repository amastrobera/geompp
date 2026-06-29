#include "bball3d.hpp"

#include "utils.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <unordered_set>

namespace geompp {

BBall3D::BBall3D(Point3D const& center, double radius) : CENTER(center), RADIUS(radius) {}

BBall3D::BBall3D(std::vector<Point3D> const& points) {
  std::size_t n = points.size();
  if (n == 0) {
    throw std::invalid_argument("cannot make bounding ball from an empty point cloud");
  }

  Point3D c = {0.0, 0.0, 0.0};
  double rad, rad2;
  double x_min, x_max, y_min, y_max, z_min, z_max;
  std::size_t i_x_min, i_x_max, i_y_min, i_y_max, i_z_min, i_z_max;

  // find the largest diameter to start with
  x_min = x_max = points[0].x();
  y_min = y_max = points[0].y();
  z_min = z_max = points[0].z();
  i_x_min = i_x_max = i_y_min = i_y_max = i_z_min = i_z_max = 0;
  for (std::size_t i = 1; i < n; ++i) {
    // update the values of the (x,y, z) min and max
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
    if (compare(points[i].z(), z_min) < 0) {
      z_min = points[i].z();
      i_z_min = i;
    }
    if (compare(points[i].z(), z_max) > 0) {
      z_max = points[i].z();
      i_z_max = i;
    }
  }

  // select the largest extent as an initial ball diameter
  Vector3D dx = points[i_x_max] - points[i_x_min];
  Vector3D dy = points[i_y_max] - points[i_y_min];
  Vector3D dz = points[i_z_max] - points[i_z_min];
  double dx2 = dx.Dot(dx);  // do not use sqrt yet to save time
  double dy2 = dy.Dot(dy);  // do not use sqrt yet to save time
  double dz2 = dz.Dot(dz);  // do not use sqrt yet to save time
  if (compare(dx2, dy2) >= 0) {
    if (compare(dx2, dz2) >= 0) {
      c = points[i_x_min] + dx / 2.0;
      rad2 = (points[i_x_max] - c).Dot(points[i_x_max] - c);  // do not use sqrt yet to save time
    } else {
      c = points[i_z_min] + dz / 2.0;
      rad2 = (points[i_z_max] - c).Dot(points[i_z_max] - c);  // do not use sqrt yet to save time
    }
  } else {
    if (compare(dy2, dz2) >= 0) {
      c = points[i_y_min] + dy / 2.0;
      rad2 = (points[i_y_max] - c).Dot(points[i_y_max] - c);  // do not use sqrt yet to save time
    } else {
      c = points[i_z_min] + dz / 2.0;
      rad2 = (points[i_z_max] - c).Dot(points[i_z_max] - c);  // do not use sqrt yet to save time
    }
  }
  rad = sqrt(rad2);

  // check that all points are contained in the ball
  // ... and if not, change the radius and the center accordingly
  double dist, dist2;
  for (std::size_t i = 0; i < n; ++i) {
    Vector3D dp = points[i] - c;
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

BBall3D::BBall3D(BBall3D const& b) : CENTER(b.CENTER), RADIUS(b.RADIUS) {}

Point3D BBall3D::center() const { return CENTER; }
double BBall3D::radius() const { return RADIUS; }

BBall3D& BBall3D::operator=(BBall3D const& other) {
  if (this != &other) {
    CENTER = other.CENTER;
    RADIUS = other.RADIUS;
  }
  return *this;
}

bool BBall3D::AlmostEquals(BBall3D const& other, double epsilon) const {
  return CENTER.AlmostEquals(other.CENTER, epsilon) && compare(RADIUS, other.RADIUS, epsilon) == 0;
}

#pragma region Geometrical Operations

bool BBall3D::Contains(Point3D const& p) const { return compare(CENTER.DistanceTo(p), RADIUS) <= 0; }

#pragma endregion

#pragma region Operator Overloading

bool operator==(BBall3D const& lhs, BBall3D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

}  // namespace geompp
