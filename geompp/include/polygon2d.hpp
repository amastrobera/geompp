#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
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
  static Polygon2D Make(std::vector<Point2D> const& points, int decimal_precision = DP_THREE);
  Polygon2D(Polygon2D const&) = default;
  Polygon2D(Polygon2D&&) = default;
  ~Polygon2D() = default;

  inline std::size_t Size() const { return VERTICES.size(); }
  inline Point2D const operator[](int i) const { return VERTICES[i]; }

  bool AlmostEquals(Polygon2D const& other, int decimal_precision = DP_THREE) const;
  // Point2D Centroid() const;
  // Polygon2D ToPolygon (int decimal_precision = DP_THREE) const; // useful for ToWkt() polygon
  // double SignedArea() const;  // if negative the order of points is clock-wise, otherwise it's counter-clockwise
  // double Area() const;
  // double Perimeter() const;
  // double DistanceTo(Point2D const& point, int decimal_precision = DP_THREE) const;
  // double Location(Point2D const& point, int decimal_precision = DP_THREE) const;
  // Point2D Interpolate(double pct) const;

  std::string ToWkt(int decimal_precision = DP_THREE) const;
  static Polygon2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path, int decimal_precision = DP_THREE) const;
  static Polygon2D FromFile(std::string const& path);

  Polygon2D& operator=(Polygon2D const& other);

#pragma region Geometrical Operations
  // bool Contains(Point2D const& point, int decimal_precision = DP_THREE) const;
  // using ReturnSet = std::optional<std::variant<Point2D>>;
  // bool Intersects(Line2D const& line, int decimal_precision = DP_THREE) const;
  // bool Intersects(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  // bool Intersects(LineSegment2D const& segment, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(Line2D const& line, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(Ray2D const& ray, int decimal_precision = DP_THREE) const;
  // ReturnSet Intersection(LineSegment2D const& other, int decimal_precision = DP_THREE) const;
#pragma endregion

 private:
  std::vector<Point2D> VERTICES;

  Polygon2D(std::vector<Point2D> const& points);
};

#pragma region Operator Overloading

bool operator==(Polygon2D const& lhs, Polygon2D const& rhs);

#pragma endregion

}  // namespace geompp