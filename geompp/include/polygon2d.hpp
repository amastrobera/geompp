#pragma once

#include "constants.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "segment_iterator2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Line2D;
class Ray2D;
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
  Point2D const& operator[](std::size_t i) const;

  bool AlmostEquals(Polygon2D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange2D ToSegments() const;
  Point2D Centroid() const;
  double Area() const;
  double PerimeterSize() const;
  bool IsSimple() const;  // no self-intersections, but holes are allowed
  bool IsConvex() const;  // no holes and all turns in the same direction — cached at construction
  Polygon2D ConvexHull();
  /// @brief Decomposes a self-intersecting polygon into one or more simple polygons.
  /// @return {*this} if already simple; otherwise the set of simple polygons covering the same area.
  std::vector<Polygon2D> Simplify() const;
  std::vector<Point2D> const& Perimeter() const;
  /// @brief Whether this polygon has one or more holes.
  bool HasHoles() const;
  /// @brief The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.
  std::vector<std::vector<Point2D>> const& Holes() const;

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
  bool IsOnPerimeter(Point2D const& point) const;

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

  /// @brief Tests whether this polygon shares any area (or boundary) with another.
  /// @param other The other polygon.
  /// @return true if the two polygons overlap, touch, or either fully contains the other.
  bool Intersects(Polygon2D const& other) const;

  /// @brief Intersection of this polygon with a line.
  /// @param line The line.
  /// @return The crossing point, or std::nullopt if the line misses the polygon.
  std::optional<std::vector<LineSegment2D>> Intersection(Line2D const& line) const;

  /// @brief Intersection of this polygon with a ray.
  /// @param ray The ray.
  /// @return The crossing point if within the ray's domain, or std::nullopt otherwise.
  std::optional<std::vector<LineSegment2D>> Intersection(Ray2D const& ray) const;

  /// @brief Intersection of this polygon with a segment.
  /// @param other The segment.
  /// @return The crossing point if it lies on the segment, or std::nullopt otherwise.
  std::optional<std::vector<LineSegment2D>> Intersection(LineSegment2D const& other) const;

#pragma endregion

#pragma region Boolean Operations

  /// @brief Set union of this polygon and other. Handles holes and self-intersecting operands; a
  /// disjoint pair of polygons yields more than one result polygon.
  /// @return Zero or more result polygons (zero is impossible for Union unless both operands are empty).
  std::vector<Polygon2D> Union(Polygon2D const& other) const;

  /// @brief Set intersection of this polygon and other (overloads Intersection() by argument type).
  /// @return Zero or more result polygons — empty if the two polygons don't overlap.
  std::vector<Polygon2D> Intersection(Polygon2D const& other) const;

  /// @brief Set difference (this minus other). Handles the case where other lies entirely inside this
  /// polygon with no shared boundary, correctly producing a hole.
  /// @return Zero or more result polygons — empty if other fully covers this polygon.
  std::vector<Polygon2D> Difference(Polygon2D const& other) const;

  /// @brief Symmetric difference (the area covered by exactly one of the two polygons).
  /// @return Zero or more result polygons.
  std::vector<Polygon2D> Xor(Polygon2D const& other) const;

#pragma endregion

 private:
  std::vector<Point2D> VERTICES;
  std::vector<std::vector<Point2D>> HOLES;
  double PERIMETER;
  bool IS_CONVEX;

  Polygon2D(std::vector<Point2D> const& points, double perimeter, bool is_convex);
  Polygon2D(std::vector<Point2D> const& points, double perimeter, std::vector<std::vector<Point2D>> const& holes,
            bool is_convex);
};

#pragma region Operator Overloading

bool operator==(Polygon2D const& lhs, Polygon2D const& rhs);

std::ostream& operator<<(std::ostream& os, Polygon2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline std::size_t Polygon2D::Size() const { return VERTICES.size(); }
inline bool Polygon2D::IsConvex() const { return IS_CONVEX; }
inline bool Polygon2D::HasHoles() const { return !HOLES.empty(); }
inline std::vector<std::vector<Point2D>> const& Polygon2D::Holes() const { return HOLES; }
inline Polygon2D::Polygon2D(std::vector<Point2D> const& points, double perimeter, bool is_convex)
    : VERTICES(points), HOLES{}, PERIMETER(perimeter), IS_CONVEX(is_convex) {}
inline Polygon2D::Polygon2D(std::vector<Point2D> const& points, double perimeter,
                            std::vector<std::vector<Point2D>> const& holes, bool is_convex)
    : VERTICES(points), HOLES(holes), PERIMETER(perimeter), IS_CONVEX(is_convex) {}

#pragma endregion

}  // namespace geompp
