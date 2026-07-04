#include "bbox2d.hpp"

#include "line_segment2d.hpp"
#include "polygon2d.hpp"
#include "polyline2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include <vector>

namespace geompp {

BBox2D::BBox2D(LineSegment2D const& s) {
  double max_x = compare(s.First().x(), s.Last().x()) >= 0 ? s.First().x() : s.Last().x();
  double max_y = compare(s.First().y(), s.Last().y()) >= 0 ? s.First().y() : s.Last().y();

  double min_x = compare(s.First().x(), s.Last().x()) <= 0 ? s.First().x() : s.Last().x();
  double min_y = compare(s.First().y(), s.Last().y()) <= 0 ? s.First().y() : s.Last().y();

  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

BBox2D::BBox2D(Polyline2D const& s) {
  if (s.Size() == 0) {
    throw new std::runtime_error("cannot make bounding box of empty polyline");
  }

  double max_x = s[0].x();
  double max_y = s[0].y();
  double min_x = s[0].x();
  double min_y = s[0].y();

  for (int i = 1; i < s.Size(); ++i) {
    max_x = compare(s[i].x(), max_x) >= 0 ? s[i].x() : max_x;
    max_y = compare(s[i].y(), max_y) >= 0 ? s[i].y() : max_y;

    min_x = compare(s[i].x(), min_x) <= 0 ? s[i].x() : min_x;
    min_y = compare(s[i].y(), min_y) <= 0 ? s[i].y() : min_y;
  }

  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

BBox2D::BBox2D(Polygon2D const& s) {
  if (s.Size() == 0) {
    throw new std::runtime_error("cannot make bounding box of empty polyline");
  }

  double max_x = s[0].x();
  double max_y = s[0].y();
  double min_x = s[0].x();
  double min_y = s[0].y();

  for (int i = 1; i < s.Size(); ++i) {
    double x = s[i].x();
    double y = s[i].y();

    if (compare(x, max_x) > 0) {
      max_x = x;
    }
    if (compare(y, max_y) > 0) {
      max_y = y;
    }

    if (compare(x, min_x) < 0) {
      min_x = x;
    }
    if (compare(y, min_y) < 0) {
      min_y = y;
    }
  }

  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

BBox2D::BBox2D(Triangle2D const& s) : BBox2D(s.ToPolygon()) {}

BBox2D& BBox2D::operator=(BBox2D const& other) {
  if (this != &other) {
    MIN = other.MIN;
    MAX = other.MAX;
  }
  return *this;
}

bool BBox2D::AlmostEquals(BBox2D const& other, double epsilon) const {
  return MIN.AlmostEquals(other.MIN, epsilon) && MAX.AlmostEquals(other.MAX, epsilon);
}

#pragma region Geometrical Operations

bool BBox2D::Contains(Point2D const& p) const {
  return compare(p.x(), MIN.x()) >= 0 && compare(p.x(), MAX.x()) <= 0 && compare(p.y(), MIN.y()) >= 0 &&
         compare(p.y(), MAX.y()) <= 0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(BBox2D const& lhs, BBox2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

template <PointContainer Points>
BBox2D::BBox2D(Points const& points) {
  if (std::ranges::empty(points)) {
    throw std::runtime_error("cannot make bounding box of empty point cloud");
  }
  auto b = std::ranges::begin(points);
  double max_x = (*b).x(), min_x = (*b).x();
  double max_y = (*b).y(), min_y = (*b).y();
  ++b;
  for (auto e = std::ranges::end(points); b != e; ++b) {
    double x = (*b).x(), y = (*b).y();
    if (compare(x, max_x) > 0) {
      max_x = x;
    }
    if (compare(y, max_y) > 0) {
      max_y = y;
    }
    if (compare(x, min_x) < 0) {
      min_x = x;
    }
    if (compare(y, min_y) < 0) {
      min_y = y;
    }
  }
  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

template BBox2D::BBox2D(std::vector<Point2D> const&);

}  // namespace geompp
