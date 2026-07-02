#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Line3D;
class LineSegment3D;

class Ray3D {
 public:
  static Ray3D Make(Point3D const& orig, Vector3D const& dir);
  Ray3D(Ray3D const&) = default;
  Ray3D(Ray3D&&) = default;
  ~Ray3D() = default;

  Point3D const& Origin() const;
  Vector3D const& Direction() const;
  bool AlmostEquals(Ray3D const& other, double epsilon = DOUBLE_EPSILON) const;

  /// @brief Tests whether a point lies on the half-space ahead of (or at) the ray's origin along its direction.
  /// @param point The point to test.
  /// @return true if @p (point - origin) · direction >= 0.
  bool IsAhead(Point3D const& point) const;

  /// @brief Tests whether a point lies strictly behind the ray's origin along its direction.
  /// @param point The point to test.
  /// @return true if @p (point - origin) · direction < 0.
  bool IsBehind(Point3D const& point) const;

  /// @brief Promotes the ray to an infinite line through the same origin and direction.
  /// @return A Line3D that contains every point on this ray and extends backwards as well.
  Line3D ToLine() const;

#pragma region line operations

  /// @brief Orthogonal projection of a point onto this ray, clamped to the origin.
  /// @param point The point to project.
  /// @return Closest point on the ray to @p point. Returns the ray's origin if @p point projects behind it.
  Point3D ProjectOnto(Point3D const& point) const;

  /// @brief Distance from a point to this ray.
  /// @param point The point to measure distance to.
  /// @return Perpendicular distance to the ray's domain, or distance to the origin if @p point is behind the ray.
  double DistanceTo(Point3D const& point) const;

  /// @brief Directed line-segment connecting this ray's closest point to a line.
  /// @param other The line.
  /// @return Segment from the ray's closest point to the line's closest point, or std::nullopt if they intersect or overlap.
  std::optional<LineSegment3D> Distance(Line3D const& other) const;

  /// @brief Scalar distance between this ray and a line.
  /// @param other The line.
  /// @return 0 if they intersect or overlap; otherwise the minimum distance.
  double DistanceTo(Line3D const& other) const;

  /// @brief Directed line-segment connecting this ray's closest point to another ray.
  /// @param ray The other ray.
  /// @return Segment from this ray's closest point to @p ray's closest point, or std::nullopt if they intersect or share a region.
  std::optional<LineSegment3D> Distance(Ray3D const& ray) const;

  /// @brief Scalar distance between two rays.
  /// @param ray The other ray.
  /// @return 0 if they intersect or share a region; otherwise the minimum distance.
  double DistanceTo(Ray3D const& ray) const;

  /// @brief Directed line-segment connecting this ray's closest point to a segment.
  /// @param seg The segment.
  /// @return Segment from the ray's closest point to the segment's closest point, or std::nullopt if they intersect or share a region.
  std::optional<LineSegment3D> Distance(LineSegment3D const& seg) const;

  /// @brief Scalar distance between this ray and a segment.
  /// @param seg The segment.
  /// @return 0 if they intersect or share a region; otherwise the minimum distance.
  double DistanceTo(LineSegment3D const& seg) const;

#pragma endregion

  std::string ToWkt() const;
  static Ray3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Ray3D FromFile(std::string const& path);

  Ray3D& operator=(Ray3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on this ray.
  /// @param point The point to test.
  /// @return true if @p point is collinear with the ray's direction AND ahead of (or at) the origin.
  bool Contains(Point3D const& point) const;

  using ReturnSet = std::optional<std::variant<Point3D>>;

  /// @brief Tests whether this ray intersects a line.
  /// @param line The line.
  /// @return true if they meet at a point on the ray's domain (sc on the ray >= 0).
  bool Intersects(Line3D const& line) const;

  /// @brief Tests whether this ray intersects another ray.
  /// @param other The other ray.
  /// @return true if both rays' domains share the crossing point.
  bool Intersects(Ray3D const& other) const;

  /// @brief Tests whether this ray intersects a segment.
  /// @param segment The segment.
  /// @return true if the ray hits the segment within both domains.
  bool Intersects(LineSegment3D const& segment) const;

  /// @brief Intersection point of this ray with a line.
  /// @param line The line.
  /// @return The intersection point if it lies on the ray (sc >= 0), or std::nullopt otherwise.
  ReturnSet Intersection(Line3D const& line) const;

  /// @brief Intersection point of two rays.
  /// @param other The other ray.
  /// @return The intersection point if it lies on both rays' domains, or std::nullopt otherwise.
  ReturnSet Intersection(Ray3D const& other) const;

  /// @brief Intersection point of this ray with a segment.
  /// @param segment The segment.
  /// @return The intersection point if it lies on both the ray and the segment, or std::nullopt otherwise.
  ReturnSet Intersection(LineSegment3D const& segment) const;

#pragma endregion

 private:
  Point3D ORIGIN;
  Vector3D DIR;  // unit

  Ray3D(Point3D const& orig, Vector3D const& dir);
};

#pragma region Operator Overloading

bool operator==(Ray3D const& lhs, Ray3D const& rhs);

std::ostream& operator<<(std::ostream& os, Ray3D const& g);

#pragma endregion

#pragma region Inlined Functions

inline Point3D const& Ray3D::Origin() const { return ORIGIN; }
inline Vector3D const& Ray3D::Direction() const { return DIR; }
inline Ray3D::Ray3D(Point3D const& orig, Vector3D const& dir) : ORIGIN(orig), DIR(dir.Normalize()) {}

#pragma endregion

}  // namespace geompp
