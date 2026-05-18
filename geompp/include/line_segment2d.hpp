#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <string>
#include <variant>

namespace geompp {

class Line2D;
class Ray2D;

class LineSegment2D {
 public:
  static LineSegment2D Make(Point2D const& p0, Point2D const& p1);
  LineSegment2D(LineSegment2D const&) = default;
  LineSegment2D(LineSegment2D&&) = default;
  ~LineSegment2D() = default;

  inline Point2D const& First() const { return P0; }
  inline Point2D const& Last() const { return P1; }

  bool AlmostEquals(LineSegment2D const& other, double epsilon = DOUBLE_EPSILON) const;
  Line2D ToLine() const;
  double Length() const;

#pragma region line operations

  /// @brief Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.
  /// @param point The point to project.
  /// @return Closest point on the segment to @p point (one of the endpoints if @p point projects outside).
  Point2D ProjectOnto(Point2D const& point) const;

  /// @brief Distance from a point to this segment.
  /// @param point The point to measure distance to.
  /// @return Perpendicular distance to the segment, or distance to the nearest endpoint if @p point projects outside.
  double DistanceTo(Point2D const& point) const;

  /// @brief Position of a point along the segment, normalized to [0, 1].
  /// @param point The point to locate. Must lie on the segment's underlying line.
  /// @return 0 at @ref First, 1 at @ref Last, fractional values in between, or +infinity if not on the line.
  double Location(Point2D const& point) const;

  /// @brief Linear interpolation along the segment.
  /// @param pct Normalized position in [0, 1]. Values outside are clamped to the segment's endpoints.
  /// @return Point at the given fraction along the segment.
  Point2D Interpolate(double pct) const;

  /// @brief Tests whether a point is on the left of the segment's direction (looking from First toward Last).
  /// @param p The point to test.
  /// @return true if the 2D cross product @p (P1 - P0) × (p - P0) is positive.
  bool IsLeft(Point2D const& p) const;

#pragma endregion

  std::string ToWkt() const;
  static LineSegment2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static LineSegment2D FromFile(std::string const& path);

  LineSegment2D& operator=(LineSegment2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on this segment.
  /// @param point The point to test.
  /// @return true if @p point is collinear with the segment and falls within [First, Last].
  bool Contains(Point2D const& point) const;

  using ReturnSet = std::optional<std::variant<Point2D>>;

  /// @brief Tests whether this segment intersects a line.
  /// @param line The line.
  /// @return true if the line crosses or touches the segment.
  bool Intersects(Line2D const& line) const;

  /// @brief Tests whether this segment intersects a ray.
  /// @param ray The ray.
  /// @return true if the segment is met within the ray's domain.
  bool Intersects(Ray2D const& ray) const;

  /// @brief Tests whether this segment intersects another segment.
  /// @param segment The other segment.
  /// @return true if both segments' domains share the crossing point.
  bool Intersects(LineSegment2D const& segment) const;

  /// @brief Intersection point of this segment with a line.
  /// @param line The line.
  /// @return The intersection point if it lies on the segment, or std::nullopt otherwise.
  ReturnSet Intersection(Line2D const& line) const;

  /// @brief Intersection point of this segment with a ray.
  /// @param ray The ray.
  /// @return The intersection point if it lies on both, or std::nullopt otherwise.
  ReturnSet Intersection(Ray2D const& ray) const;

  /// @brief Intersection point of two segments.
  /// @param other The other segment.
  /// @return The intersection point if it lies on both segments, or std::nullopt otherwise.
  ReturnSet Intersection(LineSegment2D const& other) const;

#pragma endregion

 private:
  Point2D P0, P1;

  LineSegment2D(Point2D const& p0, Point2D const& p1);
};

#pragma region Operator Overloading

bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs);

std::ostream& operator<<(std::ostream& os, LineSegment2D const& g);

#pragma endregion

}  // namespace geompp
