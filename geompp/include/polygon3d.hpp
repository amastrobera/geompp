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

  std::size_t Size() const;
  Point3D const& operator[](int i) const;
  inline Plane GetPlane() const { return PLANE; }

  bool AlmostEquals(Polygon3D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange3D ToSegments() const;
  Point3D Centroid() const;
  double Area() const;
  double Perimeter() const;
  bool IsSimple() const;
  Polygon3D ConvexHull();
  std::vector<Point3D> ToPoints();

  /// @brief Distance from a point to this polygon's closed region.
  /// @param point The point to measure distance to.
  /// @return 0 if @p point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.
  double DistanceTo(Point3D const& point) const;

  std::string ToWkt() const;
  static Polygon3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polygon3D FromFile(std::string const& path);

  Polygon3D& operator=(Polygon3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside the polygon (winding-number check).
  /// @param point The point to test. Must lie in the polygon's plane.
  /// @return true if @p point is in the polygon's closed region (interior or boundary).
  bool Contains(Point3D const& point) const;

  /// @brief Tests whether a point lies on the polygon's boundary (any of its edges).
  /// @param point The point to test.
  /// @return true if @p point is on any of the polygon's edges or vertices.
  bool IsOnBoundary(Point3D const& point) const;

  using ReturnSet = std::optional<std::variant<Point3D>>;

  /// @brief Tests whether this polygon intersects a line.
  /// @param line The line.
  /// @return true if the line crosses the polygon's closed region.
  bool Intersects(Line3D const& line) const;

  /// @brief Tests whether this polygon intersects a ray.
  /// @param ray The ray.
  /// @return true if the ray hits the polygon within its domain.
  bool Intersects(Ray3D const& ray) const;

  /// @brief Tests whether this polygon intersects a segment.
  /// @param segment The segment.
  /// @return true if any part of the segment is inside the polygon or crosses its boundary.
  bool Intersects(LineSegment3D const& segment) const;

  /// @brief Intersection of this polygon with a line.
  /// @param line The line.
  /// @return The crossing point as Point3D, or std::nullopt if the line misses the polygon.
  ReturnSet Intersection(Line3D const& line) const;

  /// @brief Intersection of this polygon with a ray.
  /// @param ray The ray.
  /// @return The crossing point if within the ray's domain, or std::nullopt otherwise.
  ReturnSet Intersection(Ray3D const& ray) const;

  /// @brief Intersection of this polygon with a segment.
  /// @param other The segment.
  /// @return The crossing point if it lies on the segment, or std::nullopt otherwise.
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
