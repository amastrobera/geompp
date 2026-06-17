#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "segment_iterator3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Line3D;
class Ray3D;
class LineSegment3D;

class Polyline3D {
 public:
  // TODO: it would be nice to use variadic templates,
  //       BUT they don't support the optional parameter decimal_precision,
  //       AND must be definied in the header!
  static Polyline3D Make(std::vector<Point3D> const& points);
  Polyline3D(Polyline3D const&) = default;
  Polyline3D(Polyline3D&&) = default;
  ~Polyline3D() = default;

  inline int Size() const { return KNOTS.size(); }
  Point3D const& operator[](std::size_t i) const;

  bool AlmostEquals(Polyline3D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange3D ToSegments() const;
  inline double Length() const { return LENGTH; }

#pragma region line operations

  /// @brief Closest point on the polyline (any of its segments) to a given point.
  /// @param point The point to project.
  /// @return Point on the polyline at minimum distance from @p point; falls on one of the segments' interiors or knots.
  Point3D ProjectOnto(Point3D const& point) const;

  /// @brief Distance from a point to the polyline (minimum over all of its segments).
  /// @param point The point to measure distance to.
  /// @return Length of the perpendicular from @p point to the closest segment.
  double DistanceTo(Point3D const& point) const;

  /// @brief Position of a point along the polyline, normalized to [0, 1] by arc length.
  /// @param point The point to locate. Must lie on the polyline.
  /// @return 0 at the first knot, 1 at the last, fractional values in between, or +infinity if @p point is off the polyline.
  double Location(Point3D const& point) const;

  /// @brief Linear interpolation along the polyline by arc-length fraction.
  /// @param pct Normalized position in [0, 1]. Values outside the range are clamped to the polyline's endpoints.
  /// @return Point at the given arc-length fraction along the polyline.
  Point3D Interpolate(double pct) const;

#pragma endregion

  std::string ToWkt() const;
  static Polyline3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polyline3D FromFile(std::string const& path);

  Polyline3D& operator=(Polyline3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on the polyline.
  /// @param point The point to test.
  /// @return true if @p point is on any of the polyline's segments (interior or knot).
  bool Contains(Point3D const& point) const;

  using MultiPoint = std::vector<Point3D>;
  using ReturnSet = std::optional<std::variant<Point3D, MultiPoint>>;

  /// @brief Tests whether this polyline intersects a line.
  /// @param line The line.
  /// @return true if the line crosses any of the polyline's segments.
  bool Intersects(Line3D const& line) const;

  /// @brief Tests whether this polyline intersects a ray.
  /// @param ray The ray.
  /// @return true if the ray crosses any of the polyline's segments within its domain.
  bool Intersects(Ray3D const& ray) const;

  /// @brief Tests whether this polyline intersects a segment.
  /// @param segment The segment.
  /// @return true if the segment crosses any of the polyline's segments.
  bool Intersects(LineSegment3D const& segment) const;

  /// @brief Tests whether two polylines intersect.
  /// @param other The other polyline.
  /// @return true if any segment of either polyline crosses any segment of the other.
  bool Intersects(Polyline3D const& other) const;

  /// @brief Intersection of this polyline with a line.
  /// @param line The line.
  /// @return A single Point3D when there's one crossing, a list when there are several, or std::nullopt if disjoint.
  ReturnSet Intersection(Line3D const& line) const;

  /// @brief Intersection of this polyline with a ray.
  /// @param ray The ray.
  /// @return A single Point3D or a list of crossings, or std::nullopt if disjoint.
  ReturnSet Intersection(Ray3D const& ray) const;

  /// @brief Intersection of this polyline with a segment.
  /// @param segment The segment.
  /// @return A single Point3D or a list of crossings, or std::nullopt if disjoint.
  ReturnSet Intersection(LineSegment3D const& segment) const;

  /// @brief Intersection of two polylines.
  /// @param other The other polyline.
  /// @return A single Point3D or a list of crossings, or std::nullopt if disjoint.
  ReturnSet Intersection(Polyline3D const& other) const;

#pragma endregion

 private:
  std::vector<Point3D> KNOTS;
  double LENGTH;

  Polyline3D(std::vector<Point3D>&& points, double length);
};

#pragma region Operator Overloading

bool operator==(Polyline3D const& lhs, Polyline3D const& rhs);

std::ostream& operator<<(std::ostream& os, Polyline3D const& g);

#pragma endregion

}  // namespace geompp
