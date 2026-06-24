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

  std::size_t Size() const;
  Point2D const& operator[](int i) const;

  bool AlmostEquals(Polygon2D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange2D ToSegments() const;
  Point2D Centroid() const;
  double Area() const;
  double Perimeter() const;
  bool IsSimple() const;   // no self-intersections, but holes are allowed
  bool IsConvex() const;   // no holes and all turns in the same direction
  Polygon2D ConvexHull();
  std::vector<Point2D> ToPoints();

  /// @brief Distance from a point to this polygon's closed region.
  /// @param point The point to measure distance to.
  /// @return 0 if @p point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.
  double DistanceTo(Point2D const& point) const;

  std::string ToWkt() const;
  static Polygon2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polygon2D FromFile(std::string const& path);

  Polygon2D& operator=(Polygon2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside the polygon (winding-number check).
  /// @param point The point to test.
  /// @return true if @p point is in the polygon's closed region.
  bool Contains(Point2D const& point) const;

  /// @brief Tests whether a point lies on the polygon's boundary.
  /// @param point The point to test.
  /// @return true if @p point is on any of the polygon's edges or vertices.
  bool IsOnBoundary(Point2D const& point) const;

  using ReturnSet = std::optional<std::variant<Point2D>>;

  /// @brief Tests whether this polygon intersects a line.
  /// @param line The line.
  /// @return true if the line crosses the polygon's closed region.
  bool Intersects(Line2D const& line) const;

  /// @brief Tests whether this polygon intersects a ray.
  /// @param ray The ray.
  /// @return true if the ray hits the polygon within its domain.
  bool Intersects(Ray2D const& ray) const;

  /// @brief Tests whether this polygon intersects a segment.
  /// @param segment The segment.
  /// @return true if any part of the segment is inside the polygon or crosses its boundary.
  bool Intersects(LineSegment2D const& segment) const;

  /// @brief Intersection of this polygon with a line.
  /// @param line The line.
  /// @return The crossing point, or std::nullopt if the line misses the polygon.
  ReturnSet Intersection(Line2D const& line) const;

  /// @brief Intersection of this polygon with a ray.
  /// @param ray The ray.
  /// @return The crossing point if within the ray's domain, or std::nullopt otherwise.
  ReturnSet Intersection(Ray2D const& ray) const;

  /// @brief Intersection of this polygon with a segment.
  /// @param other The segment.
  /// @return The crossing point if it lies on the segment, or std::nullopt otherwise.
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
