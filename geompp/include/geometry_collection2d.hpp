#pragma once

#include "constants.hpp"
#include "shape2d.hpp"

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Point2D;
class Line2D;
class Ray2D;
class LineSegment2D;
class Polyline2D;
class Triangle2D;
class Polygon2D;
class GeometryCollection2D;

class GeometryCollection2D {
 public:
  bool AlmostEquals(GeometryCollection2D const& other, double epsilon = DOUBLE_EPSILON) const;

  std::string ToWkt() const;
  static GeometryCollection2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static GeometryCollection2D FromFile(std::string const& path);

  GeometryCollection2D& operator=(GeometryCollection2D const& other);

#pragma region Getters And Setters

  inline std::size_t Size() const { return GEOMETRIES.size(); }

  void Add(Point2D const& point);
  void Add(Line2D const& line);
  void Add(LineSegment2D const& line_segment);
  void Add(Ray2D const& ray);
  void Add(Polyline2D const& polyline);
  void Add(Triangle2D const& triangle);
  void Add(Polygon2D const& polygon);
  void Add(GeometryCollection2D const& polygon);

  Shape2D Get(std::size_t index) const;

#pragma endregion

 private:
  struct Entry {
    GeometryType Type;
    std::size_t Index;
  };

  std::vector<Entry> GEOMETRIES;

  std::vector<Point2D> POINTS;
  std::vector<Line2D> LINES;
  std::vector<LineSegment2D> LINE_SEGMENTS;
  std::vector<Ray2D> RAYS;
  std::vector<Polyline2D> POLYLINES;
  std::vector<Triangle2D> TRIANGLES;
  std::vector<Polygon2D> POLYGONS;
  std::vector<GeometryCollection2D> GEOMETRY_COLLECTIONS;
};

#pragma region Operator Overloading

bool operator==(GeometryCollection2D const& lhs, GeometryCollection2D const& rhs);

std::ostream& operator<<(std::ostream& os, GeometryCollection2D const& g);

#pragma endregion

}  // namespace geompp
