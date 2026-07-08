#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <string>
#include <variant>

namespace geompp {

class Line3D;
class Polyline3D;
class Ray3D;

class LineSegment3D {
 public:
  static LineSegment3D Make(Point3D const& p0, Point3D const& p1);
  LineSegment3D(LineSegment3D const&) = default;
  LineSegment3D(LineSegment3D&&) = default;
  ~LineSegment3D() = default;

  Point3D const& First() const;
  Point3D const& Last() const;

  bool AlmostEquals(LineSegment3D const& other, double epsilon = DOUBLE_EPSILON) const;
  Line3D ToLine() const;
  double Length() const;
  LineSegment3D Reversed() const;

#pragma region line operations

  /// @brief Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.
  /// @param point The point to project.
  /// @return Closest point on the segment to @p point (one of the endpoints if @p point projects outside the segment).
  Point3D ProjectOnto(Point3D const& point) const;

  /// @brief Distance from a point to this segment.
  /// @param point The point to measure distance to.
  /// @return Length of the perpendicular from @p point to the segment, or distance to the nearest endpoint if @p point
  /// projects outside the segment.
  double DistanceTo(Point3D const& point) const;

  /// @brief Position of a point along the segment, normalized to [0, 1].
  /// @param point The point to locate. Must lie on the segment's underlying line.
  /// @return 0 at @ref First, 1 at @ref Last, fractional values in between, or +infinity if @p point is not on the
  /// segment's line.
  double Location(Point3D const& point) const;

  /// @brief Linear interpolation along the segment.
  /// @param pct Normalized position in [0, 1]. Values outside that range are clamped to the segment's endpoints.
  /// @return Point at the given fraction along the segment.
  Point3D Interpolate(double pct) const;

  /// @brief Directed line-segment connecting this segment's closest point to another line.
  /// @param other The other line.
  /// @return Segment from this segment's closest point to @p other's closest point, or std::nullopt if they intersect
  /// or are collinear.
  std::optional<LineSegment3D> Distance(Line3D const& other) const;

  /// @brief Scalar distance between this segment and a line.
  /// @param other The other line.
  /// @return 0 if the line crosses or contains the segment; otherwise the minimum distance.
  double DistanceTo(Line3D const& other) const;

  /// @brief Directed line-segment connecting this segment's closest point to a ray.
  /// @param ray The ray.
  /// @return Segment from this segment's closest point to the ray's closest point, or std::nullopt if they intersect or
  /// overlap.
  std::optional<LineSegment3D> Distance(Ray3D const& ray) const;

  /// @brief Scalar distance between this segment and a ray.
  /// @param ray The ray.
  /// @return 0 if they intersect or overlap; otherwise the minimum distance.
  double DistanceTo(Ray3D const& ray) const;

  /// @brief Directed line-segment connecting this segment's closest point to another segment.
  /// @param seg The other segment.
  /// @return Segment from this segment's closest point to @p seg's closest point, or std::nullopt if they intersect or
  /// share a region.
  std::optional<LineSegment3D> Distance(LineSegment3D const& seg) const;

  /// @brief Scalar distance between two segments.
  /// @param seg The other segment.
  /// @return 0 if they intersect or share a region; otherwise the minimum distance.
  double DistanceTo(LineSegment3D const& seg) const;

#pragma endregion

  std::string ToWkt() const;
  static LineSegment3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static LineSegment3D FromFile(std::string const& path);

  LineSegment3D& operator=(LineSegment3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on this segment (between or at the endpoints).
  /// @param point The point to test.
  /// @return true if @p point is collinear with the segment and falls within [First, Last].
  bool Contains(Point3D const& point) const;

  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& ray) const;
  bool Intersects(LineSegment3D const& segment) const;
  bool Intersects(Polyline3D const& polyline) const;

  std::optional<Point3D> Intersection(Line3D const& line) const;
  std::optional<Point3D> Intersection(Ray3D const& ray) const;
  std::optional<Point3D> Intersection(LineSegment3D const& other) const;
  std::optional<std::variant<Point3D, std::vector<Point3D>>> Intersection(Polyline3D const& polyline) const;

  bool Overlaps(Line3D const& line) const;
  bool Overlaps(Ray3D const& ray) const;
  bool Overlaps(LineSegment3D const& seg) const;
  bool Overlaps(Polyline3D const& polyline) const;

  std::optional<LineSegment3D> Overlap(Line3D const& line) const;
  std::optional<LineSegment3D> Overlap(Ray3D const& ray) const;
  std::optional<LineSegment3D> Overlap(LineSegment3D const& seg) const;
  std::optional<std::vector<LineSegment3D>> Overlap(Polyline3D const& polyline) const;

  bool Touches(Line3D const& line) const;
  bool Touches(Ray3D const& ray) const;
  bool Touches(LineSegment3D const& seg) const;
  bool Touches(Polyline3D const& polyline) const;

  std::optional<Point3D> Touch(Line3D const& line) const;
  std::optional<Point3D> Touch(Ray3D const& ray) const;
  std::optional<Point3D> Touch(LineSegment3D const& seg) const;
  std::optional<std::vector<Point3D>> Touch(Polyline3D const& polyline) const;

#pragma endregion

 private:
  Point3D P0, P1;

  LineSegment3D(Point3D const& p0, Point3D const& p1);
};

#pragma region Operator Overloading

bool operator==(LineSegment3D const& lhs, LineSegment3D const& rhs);

std::ostream& operator<<(std::ostream& os, LineSegment3D const& g);

#pragma endregion

#pragma region Inlined Functions

inline Point3D const& LineSegment3D::First() const { return P0; }
inline Point3D const& LineSegment3D::Last() const { return P1; }
inline LineSegment3D::LineSegment3D(Point3D const& p0, Point3D const& p1) : P0(p0), P1(p1) {}

#pragma endregion

}  // namespace geompp
