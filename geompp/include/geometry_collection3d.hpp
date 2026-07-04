#pragma once

#include "constants.hpp"
#include "shape3d.hpp"

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Point3D;
class Line3D;
class Ray3D;
class LineSegment3D;
class Polyline3D;
class Triangle3D;
class Polygon3D;
class GeometryCollection3D;

class GeometryCollection3D {
 public:
  bool AlmostEquals(GeometryCollection3D const& other, double epsilon = DOUBLE_EPSILON) const;

  std::string ToWkt() const;
  static GeometryCollection3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static GeometryCollection3D FromFile(std::string const& path);

  GeometryCollection3D& operator=(GeometryCollection3D const& other);

#pragma region Getters And Setters

  std::size_t Size() const;

  void Add(Point3D const& point);
  void Add(Line3D const& line);
  void Add(LineSegment3D const& line_segment);
  void Add(Ray3D const& ray);
  void Add(Polyline3D const& polyline);
  void Add(Triangle3D const& triangle);
  void Add(Polygon3D const& polygon);
  void Add(GeometryCollection3D const& polygon);

  Shape3D Get(std::size_t index) const;

#pragma endregion

 private:
  struct Entry {
    GeometryType Type;
    std::size_t Index;
  };

  std::vector<Entry> GEOMETRIES;

  std::vector<Point3D> POINTS;
  std::vector<Line3D> LINES;
  std::vector<LineSegment3D> LINE_SEGMENTS;
  std::vector<Ray3D> RAYS;
  std::vector<Polyline3D> POLYLINES;
  std::vector<Triangle3D> TRIANGLES;
  std::vector<Polygon3D> POLYGONS;
  std::vector<GeometryCollection3D> GEOMETRY_COLLECTIONS;
};

#pragma region Operator Overloading

bool operator==(GeometryCollection3D const& lhs, GeometryCollection3D const& rhs);

std::ostream& operator<<(std::ostream& os, GeometryCollection3D const& g);

#pragma endregion

#pragma region Inlined Functions

inline std::size_t GeometryCollection3D::Size() const { return GEOMETRIES.size(); }

#pragma endregion

}  // namespace geompp
