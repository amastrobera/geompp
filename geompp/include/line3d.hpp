#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

class Ray3D;
class LineSegment3D;
class Triangle3D;

/// @brief Infinite 3D line, internally stored as an origin point and a unit direction vector.
///
/// Construct with one of the @ref Make factories. Lines compare by collinearity, not by
/// the specific endpoint pair used to construct them.
class Line3D {
 public:
  /// @brief Construct a line through two distinct points.
  /// @param p0 First point on the line; becomes @ref First and @ref Origin.
  /// @param p1 Second point on the line; becomes @ref Last.
  /// @return A new Line3D whose direction is the normalized vector from p0 to p1.
  /// @throws std::runtime_error if @p p0 and @p p1 coincide at the current decimal precision.
  static Line3D Make(Point3D const& p0, Point3D const& p1);

  /// @brief Construct a line through a point along a direction vector.
  /// @param orig Origin point on the line.
  /// @param dir Non-zero direction vector. Need not be unit; will be normalized.
  /// @return A new Line3D rooted at @p orig with normalized direction @p dir.
  /// @throws std::runtime_error if @p dir is (near-)zero at the current decimal precision.
  static Line3D Make(Point3D const& orig, Vector3D const& dir);

  Line3D(Line3D const&) = default;
  Line3D(Line3D&&) = default;
  ~Line3D() = default;

  /// @brief First point used to construct the line.
  /// @return Same point passed as @p p0 to Make.
  inline Point3D const& First() const { return P0; }

  /// @brief Second point used to construct the line.
  /// @return Same point passed as @p p1 to Make, or @p orig + @p dir for the (origin, direction) form.
  inline Point3D const& Last() const { return P1; }

  /// @brief Origin point of the line. Equivalent to @ref First.
  /// @return Origin of the line as a Point3D.
  inline Point3D const& Origin() const { return P0; }

  /// @brief Unit direction vector of the line.
  /// @return Normalized vector from @ref First to @ref Last.
  inline Vector3D const& Direction() const { return DIR; }

  /// @brief Tests whether two lines are the same infinite line (collinear, same/opposite direction).
  /// @param other Line to compare to.
  /// @param epsilon Tolerance for parallelism and collinearity checks.
  /// @return true if @p other is collinear with this line (heading does not matter).
  bool AlmostEquals(Line3D const& other, double epsilon = DOUBLE_EPSILON) const;

#pragma region line operations

  /// @brief Perpendicular distance from a point to this infinite line.
  /// @param point The point to measure distance to.
  /// @return Length of the perpendicular from @p point onto this line.
  double DistanceTo(Point3D const& point) const;

  /// @brief Orthogonal projection of a point onto this infinite line.
  /// @param point The point to project.
  /// @return Closest point on this line to @p point.
  Point3D ProjectOnto(Point3D const& point) const;

  /// @brief Directed line-segment whose endpoints are the pair of closest points between the two lines.
  /// @param other The other line.
  /// @return A LineSegment3D from this line's closest point to @p other's closest point, or std::nullopt if the lines intersect or are collinear (in which case the distance is 0).
  std::optional<LineSegment3D> Distance(Line3D const& other) const;

  /// @brief Scalar distance between two lines.
  /// @param other The other line.
  /// @return 0 if the lines intersect or are collinear; otherwise the perpendicular distance between them.
  double DistanceTo(Line3D const& other) const;

  /// @brief Directed line-segment whose endpoints are the closest pair between this line and a ray.
  /// @param ray The ray.
  /// @return A LineSegment3D from this line's closest point to the ray's closest point, or std::nullopt if they intersect or overlap.
  std::optional<LineSegment3D> Distance(Ray3D const& ray) const;

  /// @brief Scalar distance between this line and a ray.
  /// @param ray The ray.
  /// @return 0 if the line intersects the ray or contains it; otherwise the minimum distance.
  double DistanceTo(Ray3D const& ray) const;

  /// @brief Directed line-segment whose endpoints are the closest pair between this line and a segment.
  /// @param seg The segment.
  /// @return A LineSegment3D from this line's closest point to the segment's closest point, or std::nullopt if they intersect or overlap.
  std::optional<LineSegment3D> Distance(LineSegment3D const& seg) const;

  /// @brief Scalar distance between this line and a segment.
  /// @param seg The segment.
  /// @return 0 if the line intersects or contains the segment; otherwise the minimum distance.
  double DistanceTo(LineSegment3D const& seg) const;

#pragma endregion

  /// @brief WKT (Well-Known Text) representation of the line, formatted as "LINE (x0 y0 z0, x1 y1 z1)".
  /// @return WKT string at the current decimal precision.
  std::string ToWkt() const;

  /// @brief Parse a line from a WKT string.
  /// @param wkt Well-Known Text string of the form "LINE (x0 y0 z0, x1 y1 z1)".
  /// @return The parsed Line3D.
  /// @throws std::runtime_error if the WKT is malformed or describes a different geometry.
  static Line3D FromWkt(std::string const& wkt);

  /// @brief Write the line to a file as WKT.
  /// @param path Filesystem path to write to. Existing content is overwritten.
  void ToFile(std::string const& path) const;

  /// @brief Read a line from a WKT file.
  /// @param path Filesystem path containing a WKT line.
  /// @return The parsed Line3D.
  /// @throws std::runtime_error if the file cannot be read or the contents are malformed.
  static Line3D FromFile(std::string const& path);

  Line3D& operator=(Line3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies on this line.
  /// @param point The point to test.
  /// @return true if @p point is collinear with this line within decimal precision.
  bool Contains(Point3D const& point) const;

  using ReturnSet = std::optional<std::variant<Point3D>>;

  /// @brief Tests whether this line intersects another line.
  /// @param other The other line.
  /// @return true if they meet at a single point (skew or parallel lines return false; collinear lines return true).
  bool Intersects(Line3D const& other) const;

  /// @brief Tests whether this line intersects a ray.
  /// @param ray The ray.
  /// @return true if the line meets the ray's domain (sc on the ray must be >= 0).
  bool Intersects(Ray3D const& ray) const;

  /// @brief Tests whether this line intersects a segment.
  /// @param segment The segment.
  /// @return true if the line meets the segment's domain (sc on the segment must be in [0, 1]).
  bool Intersects(LineSegment3D const& segment) const;

  /// @brief Tests whether this line intersects a triangle (interior or edge).
  /// @param t The triangle.
  /// @return true if the line passes through the triangle's closed region.
  bool Intersects(Triangle3D const& t) const;

  /// @brief Intersection point of two lines.
  /// @param other The other line.
  /// @return The intersection point wrapped in the variant, or std::nullopt for parallel/skew lines.
  ReturnSet Intersection(Line3D const& other) const;

  /// @brief Intersection point of this line with a ray.
  /// @param ray The ray.
  /// @return The intersection point if it lies on the ray (sc >= 0), or std::nullopt otherwise.
  ReturnSet Intersection(Ray3D const& ray) const;

  /// @brief Intersection point of this line with a segment.
  /// @param segment The segment.
  /// @return The intersection point if it lies on the segment (sc in [0, 1]), or std::nullopt otherwise.
  ReturnSet Intersection(LineSegment3D const& segment) const;
  // TODO make ReturnSet public, and write Intersection(triangle)

#pragma endregion

 private:
  Point3D P0, P1;
  Vector3D DIR;  // unit

  Line3D(Point3D const& p0, Point3D const& p1);
  Line3D(Point3D const& orig, Vector3D const& dir);
};

#pragma region Operator Overloading

bool operator==(Line3D const& lhs, Line3D const& rhs);

std::ostream& operator<<(std::ostream& os, Line3D const& g);

#pragma endregion

}  // namespace geompp
