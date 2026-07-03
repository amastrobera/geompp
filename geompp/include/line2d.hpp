#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

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

  /// @brief Tests whether this line intersects another line.
  /// @param other The other line.
  /// @return true if they meet at a point; false for parallel non-collinear lines.
  bool Intersects(Line2D const& other) const;

  /// @brief Tests whether this line intersects a ray.
  /// @param ray The ray.
  /// @return true if they meet within the ray's domain.
  bool Intersects(Ray2D const& ray) const;

  /// @brief Tests whether this line intersects a segment.
  /// @param segment The segment.
  /// @return true if the line crosses or touches the segment.
  bool Intersects(LineSegment2D const& segment) const;

  /// @brief Tests whether this line passes through a triangle.
  /// @param t The triangle.
  /// @return true if the line crosses the triangle's interior or an edge.
  bool Intersects(Triangle2D const& t) const;

  /// @brief Intersection point of two lines, also returning the parametric values along each line.
  /// @param other The other line.
  /// @param sc Output: parameter along this line at the intersection.
  /// @param tc Output: parameter along @p other at the intersection.
  /// @return The intersection point, or std::nullopt for parallel lines.
  std::optional<Point2D> Intersection(Line2D const& other, double& sc, double& tc) const;

  /// @brief Intersection point of two lines.
  /// @param other The other line.
  /// @return The intersection point, or std::nullopt for parallel lines.
  std::optional<Point2D> Intersection(Line2D const& other) const;

  /// @brief Intersection point of this line with a ray.
  /// @param ray The ray.
  /// @return The intersection point if it lies on the ray, or std::nullopt otherwise.
  std::optional<Point2D> Intersection(Ray2D const& ray) const;

  /// @brief Intersection point of this line with a segment.
  /// @param segment The segment.
  /// @return The intersection point if it lies on the segment, or std::nullopt otherwise.
  std::optional<Point2D> Intersection(LineSegment2D const& segment) const;
  // TODO write Intersection(Triangle2D)

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

}  // namespace geompp
