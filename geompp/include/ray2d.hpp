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
class Polyline2D;

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

  bool Intersects(Line2D const& line) const;
  bool Intersects(Ray2D const& other) const;
  bool Intersects(LineSegment2D const& segment) const;
  bool Intersects(Polyline2D const& polyline) const;

  std::optional<Point2D> Intersection(Line2D const& line) const;
  std::optional<Point2D> Intersection(Ray2D const& other) const;
  std::optional<Point2D> Intersection(LineSegment2D const& segment) const;
  std::optional<std::vector<Point2D>> Intersection(Polyline2D const& polyline) const;

  bool Overlaps(Line2D const& line) const;
  bool Overlaps(Ray2D const& ray) const;
  bool Overlaps(LineSegment2D const& seg) const;
  bool Overlaps(Polyline2D const& polyline) const;

  std::optional<Ray2D> Overlap(Line2D const& line) const;
  std::optional<std::variant<Ray2D, LineSegment2D>> Overlap(Ray2D const& ray) const;
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
