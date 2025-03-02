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

BBox2D::BBox2D(LineSegment2D const& s, int decimal_precision) {
  double max_x = round_to(s.First().x() - s.Last().x(), decimal_precision) >= 0 ? s.First().x() : s.Last().x();
  double max_y = round_to(s.First().y() - s.Last().y(), decimal_precision) >= 0 ? s.First().y() : s.Last().y();

  double min_x = round_to(s.First().x() - s.Last().x(), decimal_precision) <= 0 ? s.First().x() : s.Last().x();
  double min_y = round_to(s.First().y() - s.Last().y(), decimal_precision) <= 0 ? s.First().y() : s.Last().y();

  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

BBox2D::BBox2D(Polyline2D const& s, int decimal_precision) {
  if (s.Size() == 0) {
    throw new std::runtime_error("cannot make bounding box of empty polyline");
  }

  double max_x = s[0].x();
  double max_y = s[0].y();
  double min_x = s[0].x();
  double min_y = s[0].y();

  for (int i = 1; i < s.Size(); ++i) {
    max_x = round_to(s[i].x() - max_x, decimal_precision) >= 0 ? s[i].x() : max_x;
    max_y = round_to(s[i].y() - max_y, decimal_precision) >= 0 ? s[i].y() : max_y;

    min_x = round_to(s[i].x() - min_x, decimal_precision) <= 0 ? s[i].x() : min_x;
    min_x = round_to(s[i].x() - min_y, decimal_precision) <= 0 ? s[i].y() : min_y;
  }

  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

BBox2D::BBox2D(Polygon2D const& s, int decimal_precision) {
  if (s.Size() == 0) {
    throw new std::runtime_error("cannot make bounding box of empty polyline");
  }

  double max_x = s[0].x();
  double max_y = s[0].y();
  double min_x = s[0].x();
  double min_y = s[0].y();

  for (int i = 1; i < s.Size(); ++i) {
    max_x = round_to(s[i].x() - max_x, decimal_precision) >= 0 ? s[i].x() : max_x;
    max_y = round_to(s[i].y() - max_y, decimal_precision) >= 0 ? s[i].y() : max_y;

    min_x = round_to(s[i].x() - min_x, decimal_precision) <= 0 ? s[i].x() : min_x;
    min_x = round_to(s[i].x() - min_y, decimal_precision) <= 0 ? s[i].y() : min_y;
  }

  MIN = {min_x, min_y};
  MAX = {max_x, max_y};
}

BBox2D::BBox2D(Triangle2D const& s, int decimal_precision) : BBox2D(s.ToPolygon(), decimal_precision) {}

BBox2D::BBox2D(BBox2D const& b) : MIN(b.MAX), MAX(b.MAX) {}

bool BBox2D::AlmostEquals(BBox2D const& other, int decimal_precision) const {
  return MIN.AlmostEquals(other.MIN, decimal_precision) && MAX.AlmostEquals(other.MAX, decimal_precision);
}

#pragma region Geometrical Operations

bool BBox2D::Contains(Point2D const& p, int decimal_precision) const {
  return round_to(p.x() - MIN.x(), decimal_precision) >= 0 && round_to(p.x() - MAX.x(), decimal_precision) <= 0 &&
         round_to(p.y() - MIN.y(), decimal_precision) >= 0 && round_to(p.y() - MAX.y(), decimal_precision) <= 0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(BBox2D const& lhs, BBox2D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

}  // namespace geompp
