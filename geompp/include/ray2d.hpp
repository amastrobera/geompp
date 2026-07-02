#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Line2D;
class LineSegment2D;

class Ray2D {
 public:
  static Ray2D Make(Point2D const& orig, Vector2D const& dir);
  Ray2D(Ray2D const&) = default;
  Ray2D(Ray2D&&) = default;
  ~Ray2D() = default;

  Point2D const& Origin() const;
  Vector2D const& Direction() const;
  bool AlmostEquals(Ray2D const& other, double epsilon = DOUBLE_EPSILON) const;

  /// @brief Tests whether a point lies on the half-space ahead of (or at) the ray's origin along its direction.
  /// @param point The point to test.
  /// @return true if @p (point - origin) · direction >= 0.
  bool IsAhead(Point2D const& point) const;

  /// @brief Tests whether a point lies strictly behind the ray's origin along its direction.
  /// @param point The point to test.
  /// @return true if @p (point - origin) · direction < 0.
  bool IsBehind(Point2D const& point) const;

  /// @brief Promotes the ray to an infinite line through the same origin and direction.
  /// @return A Line2D that contains every point on this ray and extends backwards as well.
  Line2D ToLine() const;

#pragma region line operations

  /// @brief Orthogonal projection of a point onto this ray, clamped to the origin.
  /// @param point The point to project.
  /// @return Closest point on the ray to @p point. Returns the ray's origin if @p point projects behind it.
  Point2D ProjectOnto(Point2D const& point) const;

  /// @brief Distance from a point to this ray.
  /// @param point The point to measure distance to.
  /// @return Perpendicular distance to the ray's domain, or distance to the origin if @p point is behind the ray.
  double DistanceTo(Point2D const& point) const;

#pragma endregion

  std::string ToWkt() const;
  static Ray2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Ray2D FromFile(std::string const& path);

  Ray2D& operator=(Ray2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on this ray.
  /// @param point The point to test.
  /// @return true if @p point is collinear with the ray's direction AND ahead of (or at) the origin.
  bool Contains(Point2D const& point) const;

  using ReturnSet = std::optional<std::variant<Point2D>>;

  /// @brief Tests whether this ray intersects a line.
  /// @param line The line.
  /// @return true if they meet at a point on the ray's domain.
  bool Intersects(Line2D const& line) const;

  /// @brief Tests whether this ray intersects another ray.
  /// @param other The other ray.
  /// @return true if both rays' domains share the crossing point.
  bool Intersects(Ray2D const& other) const;

  /// @brief Tests whether this ray intersects a segment.
  /// @param segment The segment.
  /// @return true if the ray hits the segment within both domains.
  bool Intersects(LineSegment2D const& segment) const;

  /// @brief Intersection point of this ray with a line.
  /// @param line The line.
  /// @return The intersection point if it lies on the ray, or std::nullopt otherwise.
  ReturnSet Intersection(Line2D const& line) const;

  /// @brief Intersection point of two rays.
  /// @param other The other ray.
  /// @return The intersection point if it lies on both rays' domains, or std::nullopt otherwise.
  ReturnSet Intersection(Ray2D const& other) const;

  /// @brief Intersection point of this ray with a segment.
  /// @param segment The segment.
  /// @return The intersection point if it lies on both the ray and the segment, or std::nullopt otherwise.
  ReturnSet Intersection(LineSegment2D const& segment) const;

#pragma endregion

 private:
  Point2D ORIGIN;
  Vector2D DIR;  // unit

  Ray2D(Point2D const& orig, Vector2D const& dir);
};

#pragma region Operator Overloading

bool operator==(Ray2D const& lhs, Ray2D const& rhs);

std::ostream& operator<<(std::ostream& os, Ray2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline Point2D const& Ray2D::Origin() const { return ORIGIN; }
inline Vector2D const& Ray2D::Direction() const { return DIR; }
inline Ray2D::Ray2D(Point2D const& orig, Vector2D const& dir) : ORIGIN(orig), DIR(dir.Normalize()) {}

#pragma endregion

}  // namespace geompp
