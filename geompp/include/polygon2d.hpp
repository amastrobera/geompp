#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "segment_iterator2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <variant>

namespace geompp {

class Line2D;
class Ray2D;
class LineSegment2D;
class Polyline2D;
class Triangle2D;

class Polygon2D {
 public:
  static Polygon2D Make(std::vector<Point2D> const& points);
  static Polygon2D Make(std::vector<Point2D> const& points, std::vector<std::vector<Point2D>> const& holes);
  Polygon2D(Polygon2D const&) = default;
  Polygon2D(Polygon2D&&) = default;
  ~Polygon2D() = default;

  inline std::size_t Size() const { return VERTICES.size(); }
  Point2D const& operator[](int i) const;

  bool AlmostEquals(Polygon2D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange2D ToSegments() const;
  Point2D Centroid() const;
  double Area() const;
  inline double Perimeter() const { return PERIMETER; }
  double DistanceTo(Point2D const& point) const;

  std::string ToWkt() const;
  static Polygon2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polygon2D FromFile(std::string const& path);

  Polygon2D& operator=(Polygon2D const& other);

#pragma region Geometrical Operations
  bool Contains(Point2D const& point) const;
  using ReturnSet = std::optional<std::variant<Point2D>>;
  bool Intersects(Line2D const& line) const;
  bool Intersects(Ray2D const& ray) const;
  bool Intersects(LineSegment2D const& segment) const;
  ReturnSet Intersection(Line2D const& line) const;
  ReturnSet Intersection(Ray2D const& ray) const;
  ReturnSet Intersection(LineSegment2D const& other) const;
#pragma endregion

 private:
  std::vector<Point2D> VERTICES;
  std::vector<std::vector<Point2D>> HOLES;
  double PERIMETER;

  Polygon2D(std::vector<Point2D> const& points, double perimeter);
  Polygon2D(std::vector<Point2D> const& points, double perimeter, std::vector<std::vector<Point2D>> const& holes);
};

#pragma region Operator Overloading

bool operator==(Polygon2D const& lhs, Polygon2D const& rhs);

std::ostream& operator<<(std::ostream& os, Polygon2D const& g);

#pragma endregion

}  // namespace geompp
