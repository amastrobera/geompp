#include "bbox2d.hpp"

#include "line_segment2d.hpp"
#include "polygon2d.hpp"
#include "polyline2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>  // TODO: replace with logger lib
#include <unordered_set>

namespace geompp {

BBox2D::BBox2D(Point2D const& min, Point2D const& max) : MIN(min), MAX(max) {}

BBox2D::BBox2D(LineSegment2D const& s) {
  double max_x = round(s.First().x() - s.Last().x()) >= 0 ? s.First().x() : s.Last().x();
  double max_y = round(s.First().y() - s.Last().y()) >= 0 ? s.First().y() : s.Last().y();

  double min_x = round(s.First().x() - s.Last().x()) <= 0 ? s.First().x() : s.Last().x();
  double min_y = round(s.First().y() - s.Last().y()) <= 0 ? s.First().y() : s.Last().y();

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
    max_x = round(s[i].x() - max_x) >= 0 ? s[i].x() : max_x;
    max_y = round(s[i].y() - max_y) >= 0 ? s[i].y() : max_y;

    min_x = round(s[i].x() - min_x) <= 0 ? s[i].x() : min_x;
    min_x = round(s[i].x() - min_y) <= 0 ? s[i].y() : min_y;
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
    max_x = round(s[i].x() - max_x) >= 0 ? s[i].x() : max_x;
    max_y = round(s[i].y() - max_y) >= 0 ? s[i].y() : max_y;

    min_x = round(s[i].x() - min_x) <= 0 ? s[i].x() : min_x;
    min_x = round(s[i].x() - min_y) <= 0 ? s[i].y() : min_y;
  }

  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

BBox2D::BBox2D(Triangle2D const& s) : BBox2D(s.ToPolygon()) {}

BBox2D::BBox2D(BBox2D const& b) : MIN(b.MAX), MAX(b.MAX) {}

bool BBox2D::AlmostEquals(BBox2D const& other) const {
  return MIN.AlmostEquals(other.MIN) && MAX.AlmostEquals(other.MAX);
}

#pragma region Geometrical Operations

bool BBox2D::Contains(Point2D const& p) const {
  return round(p.x() - MIN.x()) >= 0 && round(p.x() - MAX.x()) <= 0 && round(p.y() - MIN.y()) >= 0 &&
         round(p.y() - MAX.y()) <= 0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(BBox2D const& lhs, BBox2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

}  // namespace geompp
