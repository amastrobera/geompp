#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "segment_iterator2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Line2D;
class Ray2D;
class LineSegment2D;

class Polyline2D {
 public:
  // TODO: it would be nice to use variadic templates,
  //       BUT they don't support the optional parameter decimal_precision,
  //       AND must be definied in the header!
  static Polyline2D Make(std::vector<Point2D> const& points);
  Polyline2D(Polyline2D const&) = default;
  Polyline2D(Polyline2D&&) = default;
  ~Polyline2D() = default;

  inline int Size() const { return KNOTS.size(); }
  Point2D const& operator[](size_t i) const;

  bool AlmostEquals(Polyline2D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange2D ToSegments() const;
  inline double Length() const { return LENGTH; }

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

  using MultiPoint = std::vector<Point2D>;
  using ReturnSet = std::optional<std::variant<Point2D, MultiPoint>>;

  /// @brief Tests whether this polyline intersects a line.
  /// @param line The line.
  /// @return true if the line crosses any of the polyline's segments.
  bool Intersects(Line2D const& line) const;

  /// @brief Tests whether this polyline intersects a ray.
  /// @param ray The ray.
  /// @return true if the ray crosses any of the polyline's segments.
  bool Intersects(Ray2D const& ray) const;

  /// @brief Tests whether this polyline intersects a segment.
  /// @param segment The segment.
  /// @return true if the segment crosses any of the polyline's segments.
  bool Intersects(LineSegment2D const& segment) const;

  /// @brief Tests whether two polylines intersect.
  /// @param other The other polyline.
  /// @return true if any segment of either polyline crosses any segment of the other.
  bool Intersects(Polyline2D const& other) const;

  /// @brief Intersection of this polyline with a line.
  /// @param line The line.
  /// @return A single Point2D when there's one crossing, a list when there are several, or std::nullopt if disjoint.
  ReturnSet Intersection(Line2D const& line) const;

  /// @brief Intersection of this polyline with a ray.
  /// @param ray The ray.
  /// @return A single Point2D or a list of crossings, or std::nullopt if disjoint.
  ReturnSet Intersection(Ray2D const& ray) const;

  /// @brief Intersection of this polyline with a segment.
  /// @param segment The segment.
  /// @return A single Point2D or a list of crossings, or std::nullopt if disjoint.
  ReturnSet Intersection(LineSegment2D const& segment) const;

  /// @brief Intersection of two polylines.
  /// @param other The other polyline.
  /// @return A single Point2D or a list of crossings, or std::nullopt if disjoint.
  ReturnSet Intersection(Polyline2D const& other) const;

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

}  // namespace geompp
