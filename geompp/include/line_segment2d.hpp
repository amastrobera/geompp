#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Line2D;
class Polyline2D;
class Ray2D;

class LineSegment2D {
 public:
  static LineSegment2D Make(Point2D const& p0, Point2D const& p1);
  LineSegment2D(LineSegment2D const&) = default;
  LineSegment2D(LineSegment2D&&) = default;
  ~LineSegment2D() = default;

  Point2D const& First() const;
  Point2D const& Last() const;

  bool AlmostEquals(LineSegment2D const& other, double epsilon = DOUBLE_EPSILON) const;
  Line2D ToLine() const;
  double Length() const;
  LineSegment2D Reversed() const;

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

  bool Intersects(Line2D const& line) const;
  bool Intersects(Ray2D const& ray) const;
  bool Intersects(LineSegment2D const& segment) const;
  bool Intersects(Polyline2D const& polyline) const;

  std::optional<Point2D> Intersection(Line2D const& line) const;
  std::optional<Point2D> Intersection(Ray2D const& ray) const;
  std::optional<Point2D> Intersection(LineSegment2D const& other) const;
  std::optional<std::vector<Point2D>> Intersection(Polyline2D const& polyline) const;

  bool Overlaps(Line2D const& line) const;
  bool Overlaps(Ray2D const& ray) const;
  bool Overlaps(LineSegment2D const& seg) const;
  bool Overlaps(Polyline2D const& polyline) const;

  std::optional<LineSegment2D> Overlap(Line2D const& line) const;
  std::optional<LineSegment2D> Overlap(Ray2D const& ray) const;
  std::optional<LineSegment2D> Overlap(LineSegment2D const& seg) const;
  std::optional<std::vector<LineSegment2D>> Overlap(Polyline2D const& polyline) const;

  bool Touches(Line2D const& line) const;
  bool Touches(Ray2D const& ray) const;
  bool Touches(LineSegment2D const& seg) const;
  bool Touches(Polyline2D const& polyline) const;

  std::optional<Point2D> Touch(Line2D const& line) const;
  std::optional<Point2D> Touch(Ray2D const& ray) const;
  std::optional<Point2D> Touch(LineSegment2D const& seg) const;
  std::optional<std::vector<Point2D>> Touch(Polyline2D const& polyline) const;

#pragma endregion

 private:
  // Trusted to construct LineSegment2D from consecutive points without the AlmostEquals degeneracy check that
  // Make() does: both validate their whole backing point list once, up front, in their own constructor —
  // repeating that check on every operator[]/operator*() access (as they would through Make()) would be
  // redundant, since that's exactly what a sweep line does hundreds of times over the same points.
  friend class SegmentIterator2D;
  friend class SegmentRange2D;

  Point2D P0, P1;

  LineSegment2D(Point2D const& p0, Point2D const& p1);
};

#pragma region Collections Operations

bool intersect(LineSegment2D const& seg1,
               LineSegment2D const& seg2);  // true if the segments intersect (including at endpoints)

/// @brief Shamos-Hoey: true if any two segments in the set intersect
bool has_intersections(std::vector<LineSegment2D> const& segments);

/// @brief Bentley-Ottmann: all intersection points among the segments, sorted bottom-left to top-right
std::vector<Point2D> find_intersections(std::vector<LineSegment2D> const& segments);

#pragma endregion

#pragma region Operator Overloading

bool operator==(LineSegment2D const& lhs, LineSegment2D const& rhs);

std::ostream& operator<<(std::ostream& os, LineSegment2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline Point2D const& LineSegment2D::First() const { return P0; }
inline Point2D const& LineSegment2D::Last() const { return P1; }
inline LineSegment2D::LineSegment2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1) {}

#pragma endregion

}  // namespace geompp
