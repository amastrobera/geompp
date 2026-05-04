#pragma once

#include "constants.hpp"
#include "plane.hpp"
#include "point3d.hpp"
#include "segment_iterator3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <variant>

namespace geompp {

class Line3D;
class Ray3D;
class LineSegment3D;
class Polyline3D;
class Triangle3D;

class Polygon3D {
 public:
  static Polygon3D Make(std::vector<Point3D> const& points);
  static Polygon3D Make(std::vector<Point3D> const& points, std::vector<std::vector<Point3D>> const& holes);
  Polygon3D(Polygon3D const&) = default;
  Polygon3D(Polygon3D&&) = default;
  ~Polygon3D() = default;

  inline std::size_t Size() const { return VERTICES.size(); }
  Point3D const& operator[](int i) const;
  inline Plane GetPlane() const { return PLANE; }

  bool AlmostEquals(Polygon3D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange3D ToSegments() const;
  Point3D Centroid() const;
  double Area() const;
  inline double Perimeter() const { return PERIMETER; }
  double DistanceTo(Point3D const& point) const;

  std::string ToWkt() const;
  static Polygon3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polygon3D FromFile(std::string const& path);

  Polygon3D& operator=(Polygon3D const& other);

#pragma region Geometrical Operations
  bool Contains(Point3D const& point) const;
  using ReturnSet = std::optional<std::variant<Point3D>>;
  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& ray) const;
  bool Intersects(LineSegment3D const& segment) const;
  ReturnSet Intersection(Line3D const& line) const;
  ReturnSet Intersection(Ray3D const& ray) const;
  ReturnSet Intersection(LineSegment3D const& other) const;
#pragma endregion

 private:
  std::vector<Point3D> VERTICES;
  std::vector<std::vector<Point3D>> HOLES;
  Plane PLANE;
  double PERIMETER;

  Polygon3D(std::vector<Point3D> const& points, Plane const& plane, double perimeter);
  Polygon3D(std::vector<Point3D> const& points, Plane const& plane, double perimeter,
            std::vector<std::vector<Point3D>> const& holes);
};

#pragma region Operator Overloading

bool operator==(Polygon3D const& lhs, Polygon3D const& rhs);

std::ostream& operator<<(std::ostream& os, Polygon3D const& g);

#pragma endregion

}  // namespace geompp
