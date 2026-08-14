#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <variant>

namespace geompp {

inline namespace geometry {

class Line2D;
class Ray2D;
class LineSegment2D;
class Polyline2D;
class Polygon2D;

class Triangle2D {
 public:
  static Triangle2D Make(Point2D const& p0, Point2D const& p1, Point2D const& p2);
  Triangle2D(Triangle2D const&) = default;
  Triangle2D(Triangle2D&&) = default;
  ~Triangle2D() = default;

  std::tuple<Point2D, Point2D, Point2D> const Vertices() const;

  bool AlmostEquals(Triangle2D const& other, double epsilon = DOUBLE_EPSILON) const;
  Point2D Centroid() const;
  Polygon2D ToPolygon() const;  // useful for ToWkt() polygon
  double SignedArea() const;    // if negative the order of points is clock-wise, otherwise it's counter-clockwise
  double Area() const;
  double Perimeter() const;
  bool IsCCW() const;

#pragma region line operations

  /// @brief Distance from a point to this triangle (interior or boundary).
  /// @param point The point to measure distance to.
  /// @return 0 if @p point is inside the triangle; otherwise the distance to the closest edge or vertex.
  double DistanceTo(Point2D const& point) const;

  /// @brief Local 2D basis spanning the triangle.
  /// @return Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.
  std::tuple<Vector2D, Vector2D> ToAxis() const;

  /// @brief Interpolates a point in the triangle from barycentric-like coordinates (s, t) along U and V.
  /// @param s Scalar along the U axis (= P1 - P0).
  /// @param t Scalar along the V axis (= P2 - P0).
  /// @return P0 + s·U + t·V if (s, t, s+t) all lie in [0, 1]; otherwise std::nullopt.
  std::optional<Point2D> Interpolate(double s, double t) const;

  /// @brief Inverse of @ref Interpolate — locates a point in the (s, t) basis of @ref ToAxis.
  /// @param point The point to locate.
  /// @return The (s, t) pair if @p point is inside the triangle; otherwise std::nullopt.
  std::optional<std::tuple<double, double>> Location(Point2D const& point) const;

#pragma endregion

  std::string ToWkt() const;
  static Triangle2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Triangle2D FromFile(std::string const& path);

  Triangle2D& operator=(Triangle2D const& other);
  Triangle2D& operator=(Triangle2D&&) = default;

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside the triangle (interior, edge, or vertex).
  /// @param point The point to test.
  /// @return true if @p point is in the triangle's closed region.
  bool Contains(Point2D const& point) const;

  using ReturnSet = std::optional<std::variant<Point2D, LineSegment2D, Triangle2D, Polygon2D>>;

  /// @brief Tests whether this triangle intersects a line.
  /// @param line The line.
  /// @return true if the line crosses the triangle's closed region.
  bool Intersects(Line2D const& line) const;

  /// @brief Tests whether this triangle intersects a ray.
  /// @param ray The ray.
  /// @return true if the ray hits the triangle within its own domain.
  bool Intersects(Ray2D const& ray) const;

  /// @brief Tests whether this triangle intersects a segment.
  /// @param segment The segment.
  /// @return true if any part of the segment lies inside the triangle.
  bool Intersects(LineSegment2D const& segment) const;

  /// @brief Tests whether this triangle intersects another triangle.
  /// @param other The other triangle.
  /// @return true if the two share any point.
  bool Intersects(Triangle2D const& other) const;

  /// @brief Intersection of this triangle with a line.
  /// @param line The line.
  /// @return A Point2D (line touches a vertex), a LineSegment2D (line cuts through interior), or std::nullopt if
  /// disjoint.
  ReturnSet Intersection(Line2D const& line) const;

  /// @brief Intersection of this triangle with a ray.
  /// @param ray The ray.
  /// @return Point2D / LineSegment2D depending on geometry, or std::nullopt if disjoint.
  ReturnSet Intersection(Ray2D const& ray) const;

  /// @brief Intersection of this triangle with a segment.
  /// @param segment The segment.
  /// @return Point2D / LineSegment2D depending on geometry, or std::nullopt if disjoint.
  ReturnSet Intersection(LineSegment2D const& segment) const;

  /// @brief Intersection of two triangles.
  /// @param other The other triangle.
  /// @return A Point2D, LineSegment2D, Triangle2D, or Polygon2D depending on overlap, or std::nullopt if disjoint.
  ReturnSet Intersection(Triangle2D const& other) const;

#pragma endregion

 private:
  Point2D P0, P1, P2;

  Triangle2D(Point2D const& p0, Point2D const& p1, Point2D const& p2);
};

#pragma region Operator Overloading

bool operator==(Triangle2D const& lhs, Triangle2D const& rhs);

std::ostream& operator<<(std::ostream& os, Triangle2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline std::tuple<Point2D, Point2D, Point2D> const Triangle2D::Vertices() const { return {P0, P1, P2}; }
inline Triangle2D::Triangle2D(Point2D const& p0, Point2D const& p1, Point2D const& p2) : P0(p0), P1(p1), P2(p2) {}

#pragma endregion

}  // namespace geometry

}  // namespace geompp
