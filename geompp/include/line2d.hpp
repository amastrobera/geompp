#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

inline namespace geometry {

class Polyline2D;
class Ray2D;
class LineSegment2D;
class Triangle2D;

class Line2D {
 public:
  static Line2D Make(Point2D const& p0, Point2D const& p1);
  static Line2D Make(Point2D const& orig, Vector2D const& dir);
  Line2D(Line2D const&) = default;
  Line2D(Line2D&&) = default;
  ~Line2D() = default;

  Point2D const& First() const;
  Point2D const& Last() const;
  Point2D const& Origin() const;
  Vector2D const& Direction() const;

  bool AlmostEquals(Line2D const& other, double epsilon = DOUBLE_EPSILON) const;

#pragma region line operations

  /// @brief Perpendicular distance from a point to this infinite line.
  /// @param point The point to measure distance to.
  /// @return Length of the perpendicular from @p point onto this line.
  double DistanceTo(Point2D const& point) const;

  /// @brief Orthogonal projection of a point onto this infinite line.
  /// @param point The point to project.
  /// @return Closest point on this line to @p point.
  Point2D ProjectOnto(Point2D const& point) const;

#pragma endregion

  std::string ToWkt() const;
  static Line2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Line2D FromFile(std::string const& path);

  Line2D& operator=(Line2D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on this line.
  /// @param point The point to test.
  /// @return true if @p point is collinear with the line within decimal precision.
  bool Contains(Point2D const& point) const;

  bool Intersects(Line2D const& other) const;
  bool Intersects(Ray2D const& ray) const;
  bool Intersects(LineSegment2D const& segment) const;
  bool Intersects(Triangle2D const& t) const;
  bool Intersects(Polyline2D const& polyline) const;

  std::optional<Point2D> Intersection(Line2D const& other) const;
  std::optional<Point2D> Intersection(Ray2D const& ray) const;
  std::optional<Point2D> Intersection(LineSegment2D const& segment) const;
  std::optional<std::vector<Point2D>> Intersection(Polyline2D const& polyline) const;

  bool Overlaps(Line2D const& line) const;
  bool Overlaps(Ray2D const& ray) const;
  bool Overlaps(LineSegment2D const& seg) const;
  bool Overlaps(Polyline2D const& polyline) const;

  std::optional<Line2D> Overlap(Line2D const& line) const;
  std::optional<Ray2D> Overlap(Ray2D const& ray) const;
  std::optional<LineSegment2D> Overlap(LineSegment2D const& seg) const;
  std::optional<std::vector<LineSegment2D>> Overlap(Polyline2D const& polyline) const;

  bool Touches(Ray2D const& ray) const;
  bool Touches(LineSegment2D const& seg) const;
  bool Touches(Polyline2D const& polyline) const;

  std::optional<Point2D> Touch(Ray2D const& ray) const;
  std::optional<Point2D> Touch(LineSegment2D const& seg) const;
  std::optional<std::vector<Point2D>> Touch(Polyline2D const& polyline) const;

#pragma endregion

 private:
  Point2D P0, P1;
  Vector2D DIR;  // unit

  Line2D(Point2D const& p0, Point2D const& p1);
  Line2D(Point2D const& orig, Vector2D const& dir);
};

#pragma region Operator Overloading

bool operator==(Line2D const& lhs, Line2D const& rhs);

std::ostream& operator<<(std::ostream& os, Line2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline Point2D const& Line2D::First() const { return P0; }
inline Point2D const& Line2D::Last() const { return P1; }
inline Point2D const& Line2D::Origin() const { return P0; }
inline Vector2D const& Line2D::Direction() const { return DIR; }
inline Line2D::Line2D(Point2D const& p0, Point2D const& p1) : P0(p0), P1(p1), DIR((p1 - p0).Normalize()) {}
inline Line2D::Line2D(Point2D const& orig, Vector2D const& dir) : P0(orig), DIR(dir.Normalize()), P1(orig + dir) {}

#pragma endregion

}  // namespace geometry

}  // namespace geompp
