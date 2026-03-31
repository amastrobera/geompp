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

// BBox3D::BBox3D(LineSegment3D const& s) {
//   double max_x = round(s.First().x() - s.Last().x()) >= 0 ? s.First().x() : s.Last().x();
//   double max_y = round(s.First().y() - s.Last().y()) >= 0 ? s.First().y() : s.Last().y();
//
//   double min_x = round(s.First().x() - s.Last().x()) <= 0 ? s.First().x() : s.Last().x();
//   double min_y = round(s.First().y() - s.Last().y()) <= 0 ? s.First().y() : s.Last().y();
//
//   MIN = {min_x, min_y};
//   MAX = {max_x, max_y};
// }
//
// BBox3D::BBox3D(Polyline3D const& s) {
//   if (s.Size() == 0) {
//     throw new std::runtime_error("cannot make bounding box of empty polyline");
//   }
//
//   double max_x = s[0].x();
//   double max_y = s[0].y();
//   double min_x = s[0].x();
//   double min_y = s[0].y();
//
//   for (int i = 1; i < s.Size(); ++i) {
//     max_x = round(s[i].x() - max_x) >= 0 ? s[i].x() : max_x;
//     max_y = round(s[i].y() - max_y) >= 0 ? s[i].y() : max_y;
//
//     min_x = round(s[i].x() - min_x) <= 0 ? s[i].x() : min_x;
//     min_x = round(s[i].x() - min_y) <= 0 ? s[i].y() : min_y;
//   }
//
//   MIN = {min_x, min_y};
//   MAX = {max_x, max_y};
// }
//
// BBox3D::BBox3D(Polygon3D const& s) {
//   if (s.Size() == 0) {
//     throw new std::runtime_error("cannot make bounding box of empty polyline");
//   }
//
//   double max_x = s[0].x();
//   double max_y = s[0].y();
//   double min_x = s[0].x();
//   double min_y = s[0].y();
//
//   for (int i = 1; i < s.Size(); ++i) {
//     max_x = round(s[i].x() - max_x) >= 0 ? s[i].x() : max_x;
//     max_y = round(s[i].y() - max_y) >= 0 ? s[i].y() : max_y;
//
//     min_x = round(s[i].x() - min_x) <= 0 ? s[i].x() : min_x;
//     min_x = round(s[i].x() - min_y) <= 0 ? s[i].y() : min_y;
//   }
//
//   MIN = {min_x, min_y};
//   MAX = {max_x, max_y};
// }
//
// BBox3D::BBox3D(Triangle3D const& s) : BBox3D(s.ToPolygon()) {}

BBox3D::BBox3D(BBox3D const& b) : MIN(b.MAX), MAX(b.MAX) {}

bool BBox3D::AlmostEquals(BBox3D const& other, int decimal_precision) const {
  return MIN.AlmostEquals(other.MIN, decimal_precision) && MAX.AlmostEquals(other.MAX, decimal_precision);
}

#pragma region Geometrical Operations

bool BBox3D::Contains(Point3D const& p) const {
  return round(p.x() - MIN.x()) >= 0 && round(p.x() - MAX.x()) <= 0 && round(p.y() - MIN.y()) >= 0 &&
         round(p.y() - MAX.y()) <= 0;
}

#pragma endregion

#pragma region Operator Overloading

bool operator==(BBox3D const& lhs, BBox3D const& rhs) { return lhs.AlmostEquals(rhs); }

#pragma endregion

}  // namespace geompp
