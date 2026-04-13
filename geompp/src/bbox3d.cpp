#include "bbox3d.hpp"

// #include "line_segment3d.hpp"
// #include "polygon3d.hpp"
// #include "polyline3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <unordered_set>

namespace geompp {

BBox3D::BBox3D(Point3D const& min, Point3D const& max) : MIN(min), MAX(max) {}

BBox3D::BBox3D(LineSegment3D const& s) {
  double ax = s.First().x(), bx = s.Last().x();
  double ay = s.First().y(), by = s.Last().y();
  double az = s.First().z(), bz = s.Last().z();

  MIN = {compare(ax, bx) < 0 ? ax : bx, compare(ay, by) < 0 ? ay : by, compare(az, bz) < 0 ? az : bz};
  MAX = {compare(ax, bx) > 0 ? ax : bx, compare(ay, by) > 0 ? ay : by, compare(az, bz) > 0 ? az : bz};
}

BBox3D::BBox3D(Polyline3D const& s) {
  if (s.Size() == 0) {
    throw std::runtime_error("cannot make bounding box of empty polyline");
  }

  double max_x = s[0].x(), min_x = s[0].x();
  double max_y = s[0].y(), min_y = s[0].y();
  double max_z = s[0].z(), min_z = s[0].z();

  for (int i = 1; i < s.Size(); ++i) {
    double x = s[i].x(), y = s[i].y(), z = s[i].z();

    if (compare(x, max_x) > 0) {
      max_x = x;
    }
    if (compare(y, max_y) > 0) {
      max_y = y;
    }
    if (compare(z, max_z) > 0) {
      max_z = z;
    }

    if (compare(x, min_x) < 0) {
      min_x = x;
    }
    if (compare(y, min_y) < 0) {
      min_y = y;
    }
    if (compare(z, min_z) < 0) {
      min_z = z;
    }
  }

  MIN = {min_x, min_y, min_z};
  MAX = {max_x, max_y, max_z};
}

BBox3D::BBox3D(Polygon3D const& s) {
  if (s.Size() == 0) {
    throw std::runtime_error("cannot make bounding box of empty polygon");
  }

  double max_x = s[0].x(), min_x = s[0].x();
  double max_y = s[0].y(), min_y = s[0].y();
  double max_z = s[0].z(), min_z = s[0].z();

  for (std::size_t i = 1; i < s.Size(); ++i) {
    double x = s[i].x(), y = s[i].y(), z = s[i].z();

    if (compare(x, max_x) > 0) {
      max_x = x;
    }
    if (compare(y, max_y) > 0) {
      max_y = y;
    }
    if (compare(z, max_z) > 0) {
      max_z = z;
    }

    if (compare(x, min_x) < 0) {
      min_x = x;
    }
    if (compare(y, min_y) < 0) {
      min_y = y;
    }
    if (compare(z, min_z) < 0) {
      min_z = z;
    }
  }

  MIN = {min_x, min_y, min_z};
  MAX = {max_x, max_y, max_z};
}

BBox3D::BBox3D(Triangle3D const& s) {
  auto [p0, p1, p2] = s.Vertices();

  double max_x = p0.x(), min_x = p0.x();
  double max_y = p0.y(), min_y = p0.y();
  double max_z = p0.z(), min_z = p0.z();

  for (Point3D const& p : {p1, p2}) {
    double x = p.x(), y = p.y(), z = p.z();

    if (compare(x, max_x) > 0) {
      max_x = x;
    }
    if (compare(y, max_y) > 0) {
      max_y = y;
    }
    if (compare(z, max_z) > 0) {
      max_z = z;
    }

    if (compare(x, min_x) < 0) {
      min_x = x;
    }
    if (compare(y, min_y) < 0) {
      min_y = y;
    }
    if (compare(z, min_z) < 0) {
      min_z = z;
    }
  }

  MIN = {min_x, min_y, min_z};
  MAX = {max_x, max_y, max_z};
}

BBox3D::BBox3D(BBox3D const& b) : MIN(b.MIN), MAX(b.MAX) {}

BBox3D& BBox3D::operator=(BBox3D const& other) {
  if (this != &other) {
    MIN = other.MIN;
    MAX = other.MAX;
  }
  return *this;
}

bool BBox3D::AlmostEquals(BBox3D const& other, double epsilon) const {
  return MIN.AlmostEquals(other.MIN, epsilon) && MAX.AlmostEquals(other.MAX, epsilon);
}

#pragma region Geometrical Operations

bool BBox3D::Contains(Point3D const& p) const {
  return compare(p.x(), MIN.x()) >= 0 && compare(p.x(), MAX.x()) <= 0 && compare(p.y(), MIN.y()) >= 0 &&
         compare(p.y(), MAX.y()) <= 0 && compare(p.z(), MIN.z()) >= 0 && compare(p.z(), MAX.z()) <= 0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(BBox3D const& lhs, BBox3D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

}  // namespace geompp
