#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "segment_iterator2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

inline namespace geometry {

class Line2D;
class Ray2D;
class LineSegment2D;
class Polygon2D;

class Polyline2D {
 public:
  // TODO: it would be nice to use variadic templates,
  //       BUT they don't support the optional parameter decimal_precision,
  //       AND must be definied in the header!
  static Polyline2D Make(std::vector<Point2D> const& points);
  Polyline2D(Polyline2D const&) = default;
  Polyline2D(Polyline2D&&) = default;
  ~Polyline2D() = default;

  int Size() const;
  Point2D const& operator[](std::size_t i) const;

  bool AlmostEquals(Polyline2D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange2D ToSegments() const;
  double Length() const;
  bool IsSimple() const;

  /// @brief Convex Hull via the Melkman's algorithm. It requires IsSimple() to be true in order to make sense
  /// @return The convex hull polygon of the polyline, even if the polyline is not simple (it will be a wrong hull in
  /// that case). So please check IsSimple() before running this.
  Polygon2D ConvexHull();

  /// @brief Reduces the polyline to one with fewer vertices.
  /// @param settings bundles the decimation strategy and its threshold — see PolylineDecimationParams and
  /// PolylineDecimationParams::Strategy for the per-strategy behavior and Big-O. Defaults to
  /// `{RamerDouglasPeucker, 0.5}`, so `Reduce()` with no arguments keeps working.
  /// @return A copy of this polyline with fewer vertices.
  Polyline2D Reduce(PolylineDecimationParams const& settings = PolylineDecimationParams{}) const;

  /// @brief Rounds every inner corner of the polyline with a quadratic Bezier arc — the inverse
  /// direction of Reduce(): this adds vertices rather than removing them. Each corner is delegated to
  /// bezier_smoothing_2 independently (see PolylineExpansionParams for the per-corner controls); the
  /// true first/last knots are never smoothed. Corners whose sampled arc collapses to a single point
  /// (see PolylineExpansionParams::min_segment_length) are deduplicated rather than emitted as
  /// repeated/zero-length segments.
  /// @param settings bundles smoothness, sampling density, and the tiny-corner skip threshold.
  /// Defaults to `{0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON}`, so `Expand()` with no arguments works.
  /// @return A copy of this polyline with rounded corners. Unchanged if it has fewer than 3 knots
  /// (no inner corner exists to round).
  /// @throws std::invalid_argument if settings.mode == FixedSegments and settings.segments_per_corner < 1,
  /// or if settings.mode == MinDistance and settings.min_distance <= 0 (propagated from bezier_smoothing_2).
  Polyline2D Expand(PolylineExpansionParams const& settings = PolylineExpansionParams{}) const;

#pragma region line operations

  /// @brief Closest point on the polyline (any of its segments) to a given point.
  /// @param point The point to project.
  /// @return Point on the polyline at minimum distance from @p point.
  Point2D ProjectOnto(Point2D const& point) const;

  /// @brief Distance from a point to the polyline (minimum over all of its segments).
  /// @param point The point to measure distance to.
  /// @return Length of the perpendicular from @p point to the closest segment.
  double DistanceTo(Point2D const& point) const;

  /// @brief Position of a point along the polyline, normalized to [0, 1] by arc length.
  /// @param point The point to locate. Must lie on the polyline.
  /// @return 0 at the first knot, 1 at the last, or +infinity if @p point is off the polyline.
  double Location(Point2D const& point) const;

  /// @brief Linear interpolation along the polyline by arc-length fraction.
  /// @param pct Normalized position in [0, 1]. Values outside the range are clamped to the polyline's endpoints.
  /// @return Point at the given arc-length fraction along the polyline.
  Point2D Interpolate(double pct) const;

#pragma endregion

  std::string ToWkt() const;
  static Polyline2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polyline2D FromFile(std::string const& path);

  Polyline2D& operator=(Polyline2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on the polyline.
  /// @param point The point to test.
  /// @return true if @p point is on any of the polyline's segments.
  bool Contains(Point2D const& point) const;

  bool Intersects(Line2D const& line) const;
  bool Intersects(Ray2D const& ray) const;
  bool Intersects(LineSegment2D const& segment) const;
  bool Intersects(Polyline2D const& other) const;

  std::optional<std::vector<Point2D>> Intersection(Line2D const& line) const;
  std::optional<std::vector<Point2D>> Intersection(Ray2D const& ray) const;
  std::optional<std::vector<Point2D>> Intersection(LineSegment2D const& segment) const;
  std::optional<std::vector<Point2D>> Intersection(Polyline2D const& other) const;

  bool Overlaps(Line2D const& line) const;
  bool Overlaps(Ray2D const& ray) const;
  bool Overlaps(LineSegment2D const& seg) const;
  bool Overlaps(Polyline2D const& other) const;

  std::optional<std::vector<LineSegment2D>> Overlap(Line2D const& line) const;
  std::optional<std::vector<LineSegment2D>> Overlap(Ray2D const& ray) const;
  std::optional<std::vector<LineSegment2D>> Overlap(LineSegment2D const& seg) const;
  std::optional<std::vector<LineSegment2D>> Overlap(Polyline2D const& other) const;

  bool Touches(Line2D const& line) const;
  bool Touches(Ray2D const& ray) const;
  bool Touches(LineSegment2D const& seg) const;
  bool Touches(Polyline2D const& other) const;

  std::optional<std::vector<Point2D>> Touch(Line2D const& line) const;
  std::optional<std::vector<Point2D>> Touch(Ray2D const& ray) const;
  std::optional<std::vector<Point2D>> Touch(LineSegment2D const& seg) const;
  std::optional<std::vector<Point2D>> Touch(Polyline2D const& other) const;

#pragma endregion

 private:
  std::vector<Point2D> KNOTS;
  double LENGTH;

  Polyline2D(std::vector<Point2D>&& points, double length);
};

#pragma region Operator Overloading

bool operator==(Polyline2D const& lhs, Polyline2D const& rhs);

std::ostream& operator<<(std::ostream& os, Polyline2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline int Polyline2D::Size() const { return KNOTS.size(); }
inline SegmentRange2D Polyline2D::ToSegments() const { return SegmentRange2D(KNOTS); }
inline double Polyline2D::Length() const { return LENGTH; }
inline Polyline2D::Polyline2D(std::vector<Point2D>&& points, double length)
    : KNOTS{std::move(points)}, LENGTH(length) {}

#pragma endregion

}  // namespace geometry

}  // namespace geompp
