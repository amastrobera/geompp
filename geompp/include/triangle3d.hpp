#pragma once

#include "constants.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "vector3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <tuple>
#include <variant>

namespace geompp {

class Line3D;
class Ray3D;
// class Polyline3D;

class Triangle3D {
 public:
  static Triangle3D Make(Point3D const& p0, Point3D const& p1, Point3D const& p2);
  Triangle3D(Triangle3D const&) = default;
  Triangle3D(Triangle3D&&) = default;
  ~Triangle3D() = default;

  inline std::tuple<Point3D, Point3D, Point3D> const Vertices() const { return {P0, P1, P2}; }

  bool AlmostEquals(Triangle3D const& other, double epsilon = DOUBLE_EPSILON) const;
  Point3D Centroid() const;
  Polygon3D ToPolygon() const;  // useful for ToWkt() polygon
  Plane ToPlane() const;
  Vector3D AreaVector() const;
  Vector3D Normal() const;
  double SignedArea(Vector3D const& ref_normal)
      const;  // if negative the order of points is clock-wise, otherwise it's counter-clockwise
  double Area() const;
  double Perimeter() const;
  bool IsCCW(Vector3D const& ref_normal) const;

#pragma region line operations

  /// @brief Distance from a point to this triangle (interior or boundary).
  /// @param point The point to measure distance to.
  /// @return 0 if @p point is inside the triangle's closed region; otherwise the distance to the closest edge or vertex.
  double DistanceTo(Point3D const& point) const;

  /// @brief Local 2D basis spanning the triangle's plane.
  /// @return Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.
  std::tuple<Vector3D, Vector3D> ToAxis() const;

  /// @brief Interpolates a point in the triangle's plane from barycentric-like coordinates (s, t) along U and V.
  /// @param s Scalar along the U axis (= P1 - P0).
  /// @param t Scalar along the V axis (= P2 - P0).
  /// @return P0 + s·U + t·V if (s, t) describes a point inside the triangle (s, t, s+t in [0, 1]); otherwise std::nullopt.
  std::optional<Point3D> Interpolate(double s, double t) const;

  /// @brief Inverse of @ref Interpolate — locates a point in the (s, t) basis of @ref ToAxis.
  /// @param point The point to locate. Must lie in the triangle's plane.
  /// @return The (s, t) pair if @p point is inside the triangle; otherwise std::nullopt.
  std::optional<std::tuple<double, double>> Location(Point3D const& point) const;

#pragma endregion

  std::string ToWkt() const;
  static Triangle3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Triangle3D FromFile(std::string const& path);

  Triangle3D& operator=(Triangle3D const& other);

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside the triangle (interior, edge, or vertex).
  /// @param point The point to test. Must lie in the triangle's plane (within decimal precision).
  /// @return true if @p point is in the triangle's closed region.
  bool Contains(Point3D const& point) const;

  using ReturnSet = std::optional<std::variant<Point3D, LineSegment3D, Triangle3D, Polygon3D>>;

  /// @brief Tests whether this triangle intersects a line.
  /// @param line The line.
  /// @return true if the line crosses the triangle's closed region.
  bool Intersects(Line3D const& line) const;

  /// @brief Tests whether this triangle intersects a ray.
  /// @param ray The ray.
  /// @return true if the ray hits the triangle within its own domain (sc on the ray >= 0).
  bool Intersects(Ray3D const& ray) const;

  /// @brief Tests whether this triangle intersects a segment.
  /// @param segment The segment.
  /// @return true if any part of the segment lies inside the triangle.
  bool Intersects(LineSegment3D const& segment) const;

  /// @brief Tests whether this triangle intersects a plane.
  /// @param plane The plane.
  /// @return true if the plane cuts through the triangle or touches it.
  bool Intersects(Plane const& plane) const;

  /// @brief Tests whether this triangle intersects another triangle.
  /// @param other The other triangle.
  /// @return true if the two triangles share any point.
  bool Intersects(Triangle3D const& other) const;

  /// @brief Intersection of this triangle with a line.
  /// @param line The line.
  /// @return The crossing point as Point3D, or std::nullopt if the line misses the triangle.
  ReturnSet Intersection(Line3D const& line) const;

  /// @brief Intersection of this triangle with a ray.
  /// @param ray The ray.
  /// @return The crossing point if it lies within the ray's domain, or std::nullopt otherwise.
  ReturnSet Intersection(Ray3D const& ray) const;

  /// @brief Intersection of this triangle with a segment.
  /// @param segment The segment.
  /// @return The crossing point if it lies on the segment, or std::nullopt otherwise.
  ReturnSet Intersection(LineSegment3D const& segment) const;

  /// @brief Intersection of this triangle with a plane.
  /// @param plane The plane.
  /// @return A Point3D (touches at a vertex), a LineSegment3D (cuts through interior), or std::nullopt if disjoint.
  ReturnSet Intersection(Plane const& plane) const;

  /// @brief Intersection of two coplanar or skew triangles.
  /// @param other The other triangle.
  /// @return A Point3D, LineSegment3D, or std::nullopt depending on how the two triangles meet.
  ReturnSet Intersection(Triangle3D const& other) const;

#pragma endregion

 private:
  Point3D P0, P1, P2;

  Triangle3D(Point3D const& p0, Point3D const& p1, Point3D const& p2);
};

#pragma region Operator Overloading

bool operator==(Triangle3D const& lhs, Triangle3D const& rhs);

std::ostream& operator<<(std::ostream& os, Triangle3D const& g);

#pragma endregion

}  // namespace geompp
