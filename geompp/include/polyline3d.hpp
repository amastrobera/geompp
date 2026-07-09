#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "segment_iterator3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

namespace geompp {

class Line3D;
class Ray3D;
class LineSegment3D;
class Polygon3D;

class Polyline3D {
 public:
  // TODO: it would be nice to use variadic templates,
  //       BUT they don't support the optional parameter decimal_precision,
  //       AND must be definied in the header!
  static Polyline3D Make(std::vector<Point3D> const& points);
  Polyline3D(Polyline3D const&) = default;
  Polyline3D(Polyline3D&&) = default;
  ~Polyline3D() = default;

  int Size() const;
  Point3D const& operator[](std::size_t i) const;

  bool AlmostEquals(Polyline3D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange3D ToSegments() const;
  double Length() const;

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
  /// @return 0 at the first knot, 1 at the last, fractional values in between, or +infinity if @p point is off the
  /// polyline.
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

  bool Intersects(Line3D const& line) const;
  bool Intersects(Ray3D const& ray) const;
  bool Intersects(LineSegment3D const& segment) const;
  bool Intersects(Polyline3D const& other) const;

  std::optional<std::vector<Point3D>> Intersection(Line3D const& line) const;
  std::optional<std::vector<Point3D>> Intersection(Ray3D const& ray) const;
  std::optional<std::vector<Point3D>> Intersection(LineSegment3D const& segment) const;
  std::optional<std::vector<Point3D>> Intersection(Polyline3D const& other) const;

  bool Overlaps(Line3D const& line) const;
  bool Overlaps(Ray3D const& ray) const;
  bool Overlaps(LineSegment3D const& seg) const;
  bool Overlaps(Polyline3D const& other) const;

  std::optional<std::vector<LineSegment3D>> Overlap(Line3D const& line) const;
  std::optional<std::vector<LineSegment3D>> Overlap(Ray3D const& ray) const;
  std::optional<std::vector<LineSegment3D>> Overlap(LineSegment3D const& seg) const;
  std::optional<std::vector<LineSegment3D>> Overlap(Polyline3D const& other) const;

  bool Touches(Line3D const& line) const;
  bool Touches(Ray3D const& ray) const;
  bool Touches(LineSegment3D const& seg) const;
  bool Touches(Polyline3D const& other) const;

  std::optional<std::vector<Point3D>> Touch(Line3D const& line) const;
  std::optional<std::vector<Point3D>> Touch(Ray3D const& ray) const;
  std::optional<std::vector<Point3D>> Touch(LineSegment3D const& seg) const;
  std::optional<std::vector<Point3D>> Touch(Polyline3D const& other) const;

  /// @brief Tests whether all knots of the polyline are coplanar.
  /// @return true if all knots lie in a common plane.
  bool IsPlanar() const;

  /// @brief Tests whether the polyline has no self-intersections (when projected onto its best-fit plane).
  /// @return true if the polyline does not self-intersect.
  /// @throws std::runtime_error if the polyline is not planar.
  bool IsSimple() const;

  /// @brief Tests whether the polyline is a convex polygon boundary.
  /// @return true if the polyline is planar, simple, and all turns go in the same direction.
  /// @throws std::runtime_error if the polyline is not planar.
  bool IsConvex() const;

  /// @brief Computes the convex hull of the polyline's knots.
  /// @return A new Polyline3D containing the convex hull vertices in CCW order.
  /// @throws std::runtime_error if fewer than 3 non-collinear points.
  Polyline3D ConvexHull() const;

  /// @brief Reduces the polyline to one with less vertices
  /// @param strategy decimation strategy
  ///         - RadialDistance (brute force) -> O(N). Linear vertex reduction (the distance between each vertex smaller
  ///                                                 than threshold)
  ///         - RamerDouglasPeucker -> O(N.LogN) to O(N^2). Given ponts P1,P2,P3, the distance of P2 from P1->P3 segment
  ///                                                       should be smaller than a threshold.
  ///         - VisvalingamWhyatt -> O(N.LogN) to O(N^2). Given ponts P1,P2,P3, the area of the triangle P1-P2-P3
  ///                                                     should be smaller than a threshold.
  /// @param threshold
  /// @return
  Polyline3D Reduce(PolylineDecimationStrategy strategy = PolylineDecimationStrategy::RamerDouglasPeucker,
                    double threshold = 0.5) const;

  /// @brief Converts this polyline to a Polygon3D.
  /// @return A Polygon3D with the same vertices.
  /// @throws std::runtime_error if the polyline is not planar or has fewer than 3 vertices.
  Polygon3D ToPolygon() const;

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

#pragma region Inlined Functions

inline int Polyline3D::Size() const { return KNOTS.size(); }
inline SegmentRange3D Polyline3D::ToSegments() const { return SegmentRange3D(KNOTS); }
inline double Polyline3D::Length() const { return LENGTH; }
inline Polyline3D::Polyline3D(std::vector<Point3D>&& points, double length)
    : KNOTS{std::move(points)}, LENGTH(length) {}

#pragma endregion

}  // namespace geompp
