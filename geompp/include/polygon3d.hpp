#pragma once

#include "constants.hpp"
#include "maths.hpp"
#include "plane.hpp"
#include "point3d.hpp"
#include "segment_iterator3d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

inline namespace geometry {

class Line3D;
class Ray3D;
class LineSegment3D;
class Polyline3D;
class Triangle3D;
class Polygon3D;

}  // namespace geometry

namespace transformations {
// Forward-declared so Polygon3D can friend this exact overload below -- transform() is the only caller
// allowed to reach the private FromUniqueCoplanarCCWPoints() fast constructor (see its own doc comment).
geometry::Polygon3D transform(geometry::Polygon3D const& poly, maths::Matrix4 const& m);
}  // namespace transformations

inline namespace geometry {

class Polygon3D {
 public:
  static Polygon3D Make(std::vector<Point3D> const& points);
  static Polygon3D Make(std::vector<Point3D> const& points, std::vector<std::vector<Point3D>> const& holes);
  /// @brief Same as the const& overload, but consumes @p points instead of copying it — every
  /// point-cleanup step (collinear removal, etc.) reuses @p points' own storage instead of allocating
  /// a fresh vector.
  static Polygon3D Make(std::vector<Point3D>&& points);
  /// @brief Same as the const& overload, but consumes both @p points and @p holes instead of copying them.
  static Polygon3D Make(std::vector<Point3D>&& points, std::vector<std::vector<Point3D>>&& holes);
  Polygon3D(Polygon3D const&) = default;
  Polygon3D(Polygon3D&&) = default;
  ~Polygon3D() = default;

  std::size_t Size() const;
  Point3D const& operator[](std::size_t i) const;
  Plane GetPlane() const;

  bool AlmostEquals(Polygon3D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange3D ToSegments() const;
  Point3D Centroid() const;

  /// @brief Same convention as Polygon3D::Area(): holes are always simple (Make() rejects a
  /// self-intersecting hole outright) so their contribution is a direct O(1)-per-hole shoelace sum; the
  /// outer ring, if also simple, makes the whole thing O(n), otherwise it's decomposed at O(n log n) into
  /// its real bounded faces and their (plane-aware, not merely 2D-projected) areas summed.
  double Area() const;
  double PerimeterSize() const;
  bool IsSimple() const;
  bool IsConvex() const;  // no holes and all turns in the same direction
  Polygon3D ConvexHull();
  std::vector<Point3D> const& Perimeter() const;
  /// @brief Whether this polygon has one or more holes.
  bool HasHoles() const;
  /// @brief The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.
  std::vector<std::vector<Point3D>> const& Holes() const;

  /// @brief Decomposes a self-intersecting polygon into one or more simple polygons.
  /// @return {*this} if already simple; otherwise the set of simple polygons covering the same area.
  std::vector<Polygon3D> Simplify() const;

  /// @brief Breaks down the polygon (outer ring ONLY, holes are ignored) into a set of triangles, in the
  /// polygon's own plane (GetPlane().normal() supplies the projection — no PCA re-fit needed since it's
  /// already known). Make() already guarantees the outer ring is simple, CCW-wound, and free of
  /// collinear/duplicate points, so this always calls the free triangulate() with every
  /// TriangulationParams check set to Guaranteed — no re-validation cost.
  /// @param strategy which triangulation algorithm to run (see TriangulationParams::Strategy).
  /// @returns one Triangle3D per triangle; Size() - 2 triangles.
  /// @throws whatever the chosen @p strategy itself throws (e.g. std::runtime_error for a
  /// not-yet-implemented strategy).
  std::vector<Triangle3D> Triangulate(TriangulationParams::Strategy strategy) const;

  /// @brief Distance from a point to this polygon's closed region.
  /// @param point The point to measure distance to.
  /// @return 0 if @p point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.
  double DistanceTo(Point3D const& point) const;

  std::string ToWkt() const;
  static Polygon3D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polygon3D FromFile(std::string const& path);

  Polygon3D& operator=(Polygon3D const& other);
  Polygon3D& operator=(Polygon3D&&) = default;

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside the polygon (winding-number check).
  /// @param point The point to test. Must lie in the polygon's plane.
  /// @return true if @p point is in the polygon's closed region (interior or boundary).
  bool Contains(Point3D const& point) const;

  /// @brief Tests whether a point lies on the polygon's boundary (any of its edges).
  /// @param point The point to test.
  /// @return true if @p point is on any of the polygon's edges or vertices.
  bool IsOnPerimeter(Point3D const& point) const;

  /// @brief Tests whether this polygon intersects a line. This is a pure strike-through. Co-planar lines intersecting
  /// on a 2D planes are called Overlap.
  /// @param line The line.
  /// @return true if the line crosses the polygon's closed region.
  bool Intersects(Line3D const& line) const;

  /// @brief Tests whether this polygon intersects a ray. This is a pure strike-through. Co-planar lines intersecting on
  /// a 2D planes are called Overlap, so only one Point is returned.
  /// @param ray The ray.
  /// @return true if the ray hits the polygon within its domain.
  bool Intersects(Ray3D const& ray) const;

  /// @brief Tests whether this polygon intersects a segment. This is a pure strike-through. Co-planar lines
  /// intersecting on a 2D planes are called Overlap.
  /// @param segment The segment.
  /// @return true if any part of the segment is inside the polygon or crosses its boundary.
  bool Intersects(LineSegment3D const& segment) const;

  /// @brief Intersection of this polygon with a line. This is a pure strike-through. Co-planar lines intersecting on
  /// a 2D planes are called Overlap, so only one Point is returned.
  /// @param line The line.
  /// @return The crossing point as Point3D, or std::nullopt if the line misses the polygon.
  std::optional<Point3D> Intersection(Line3D const& line) const;

  /// @brief Intersection of this polygon with a ray. This is a pure strike-through. Co-planar lines intersecting on
  /// a 2D planes are called Overlap, so only one Point is returned.
  /// @param ray The ray.
  /// @return The crossing point if within the ray's domain, or std::nullopt otherwise.
  std::optional<Point3D> Intersection(Ray3D const& ray) const;

  /// @brief Intersection of this polygon with a segment. This is a pure strike-through. Co-planar lines intersecting on
  /// a 2D planes are called Overlap, so only one Point is returned.
  /// @param segment The segment.
  /// @return The crossing point if it lies on the segment, or std::nullopt otherwise.
  std::optional<Point3D> Intersection(LineSegment3D const& segment) const;

  /// @brief Tests whether this polygon shares any point with another — area overlap when coplanar, or a
  /// genuine strike-through (a shared segment on the two planes' common line) when not.
  bool Intersects(Polygon3D const& other) const;

  /// @brief Intersection of this polygon with another.
  /// - Coplanar (same plane): the set intersection of the two areas, same semantics as
  ///   Polygon3D::Intersection(Polygon3D) — zero or more result polygons.
  /// - Not coplanar, planes crossing: the two flat regions can only share points along the planes'
  ///   common line, so the result is the chain of segments where both polygons' bounded regions cover
  ///   that line (empty chain omitted — reported as std::nullopt, not an empty vector).
  /// - Not coplanar, planes parallel (and distinct, since coplanar was already ruled out): std::nullopt —
  ///   parallel distinct planes never share a point.
  /// @return std::nullopt if the two polygons share no point; otherwise whichever variant alternative
  /// matches the coplanar/non-coplanar case above.
  std::optional<std::variant<std::vector<Polygon3D>, std::vector<LineSegment3D>>> Intersection(
      Polygon3D const& other) const;

#pragma endregion

#pragma region Boolean Operations

  /// @brief Set union of this polygon and other.
  /// @pre Both polygons must be coplanar (GetPlane().AlmostEquals(other.GetPlane())) — a union of two
  /// polygons on different planes isn't representable as a single flat Polygon3D.
  /// @throws std::logic_error if the two polygons are not coplanar.
  std::vector<Polygon3D> Union(Polygon3D const& other) const;

  /// @brief Set difference (this minus other).
  /// @pre Same coplanarity requirement as Union() — see its docs for why.
  /// @throws std::logic_error if the two polygons are not coplanar.
  std::vector<Polygon3D> Difference(Polygon3D const& other) const;

  /// @brief Symmetric difference (the area covered by exactly one of the two polygons).
  /// @pre Same coplanarity requirement as Union() — see its docs for why.
  /// @throws std::logic_error if the two polygons are not coplanar.
  std::vector<Polygon3D> Xor(Polygon3D const& other) const;

#pragma endregion

#pragma region Iterators

  // Only expose const_iterator
  using const_iterator = std::vector<Point3D>::const_iterator;

  // Both const and non-const begin/end return const_iterator!
  const_iterator begin() const;
  const_iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;

#pragma endregion

 private:
  std::vector<Point3D> VERTICES;
  std::vector<std::vector<Point3D>> HOLES;
  Plane PLANE;
  double PERIMETER;
  bool IS_CONVEX;

  // Shared by every Make() overload (const&/&& on points, with/without holes): validates an
  // already-collinear-filtered outer ring (and, for the second overload, raw holes still needing their
  // own per-hole cleanup) and wraps it. Taking everything by value lets each Make() overload hand off
  // its data with a single move regardless of whether it started from a const& or && parameter.
  static Polygon3D FromUniquePoints(std::vector<Point3D> unique_points);
  static Polygon3D FromUniquePoints(std::vector<Point3D> unique_points, std::vector<std::vector<Point3D>> holes);

  // Same contract as FromUniquePoints, but ALSO trusts the caller that unique_points is already coplanar
  // (skips are_coplanar()) and that is_convex is already known -- both true by construction for
  // transform()'s own use: any affine map takes a plane to a plane (even a degenerate one, so this is
  // always safe for a non-degenerate transform), and convexity is likewise affine-invariant, so the
  // source polygon's own IsConvex() carries over unchanged. The outer_plane itself still has to be
  // recomputed fresh via newell_normal() -- that's not redundant, it's the thing that makes the result
  // correct regardless of the transform's determinant sign (see transformations3d.cpp). Private and
  // friended to transform() specifically -- NOT exposed publicly, for the same reason as
  // Polygon2D::FromUniqueCCWPoints (see its doc comment).
  static Polygon3D FromUniqueCoplanarCCWPoints(std::vector<Point3D> unique_points, bool is_convex);
  static Polygon3D FromUniqueCoplanarCCWPoints(std::vector<Point3D> unique_points,
                                               std::vector<std::vector<Point3D>> holes, bool is_convex);

  friend Polygon3D geompp::transformations::transform(Polygon3D const& poly, maths::Matrix4 const& m);

  Polygon3D(std::vector<Point3D> const& points, Plane const& plane, double perimeter, bool is_convex);
  Polygon3D(std::vector<Point3D> const& points, Plane const& plane, double perimeter,
            std::vector<std::vector<Point3D>> const& holes, bool is_convex);
  Polygon3D(std::vector<Point3D>&& points, Plane const& plane, double perimeter, bool is_convex);
  Polygon3D(std::vector<Point3D>&& points, Plane const& plane, double perimeter,
            std::vector<std::vector<Point3D>>&& holes, bool is_convex);
};

#pragma region Operator Overloading

bool operator==(Polygon3D const& lhs, Polygon3D const& rhs);

std::ostream& operator<<(std::ostream& os, Polygon3D const& g);

#pragma endregion

#pragma region Inlined Functions

inline std::size_t Polygon3D::Size() const { return VERTICES.size(); }
inline Plane Polygon3D::GetPlane() const { return PLANE; }
inline bool Polygon3D::IsConvex() const { return IS_CONVEX; }
inline bool Polygon3D::HasHoles() const { return !HOLES.empty(); }
inline std::vector<std::vector<Point3D>> const& Polygon3D::Holes() const { return HOLES; }
inline Polygon3D::Polygon3D(std::vector<Point3D> const& points, Plane const& plane, double perimeter, bool is_convex)
    : VERTICES(points), HOLES{}, PLANE(plane), PERIMETER(perimeter), IS_CONVEX(is_convex) {}
inline Polygon3D::Polygon3D(std::vector<Point3D> const& points, Plane const& plane, double perimeter,
                            std::vector<std::vector<Point3D>> const& holes, bool is_convex)
    : VERTICES(points), HOLES(holes), PLANE(plane), PERIMETER(perimeter), IS_CONVEX(is_convex) {}
inline Polygon3D::Polygon3D(std::vector<Point3D>&& points, Plane const& plane, double perimeter, bool is_convex)
    : VERTICES(std::move(points)), HOLES{}, PLANE(plane), PERIMETER(perimeter), IS_CONVEX(is_convex) {}
inline Polygon3D::Polygon3D(std::vector<Point3D>&& points, Plane const& plane, double perimeter,
                            std::vector<std::vector<Point3D>>&& holes, bool is_convex)
    : VERTICES(std::move(points)), HOLES(std::move(holes)), PLANE(plane), PERIMETER(perimeter), IS_CONVEX(is_convex) {}

// Both const and non-const begin/end return const_iterator!
inline Polygon3D::const_iterator Polygon3D::begin() const { return VERTICES.cbegin(); }
inline Polygon3D::const_iterator Polygon3D::end() const { return VERTICES.cend(); }

inline Polygon3D::const_iterator Polygon3D::cbegin() const { return VERTICES.cbegin(); }
inline Polygon3D::const_iterator Polygon3D::cend() const { return VERTICES.cend(); }

#pragma endregion

}  // namespace geometry

}  // namespace geompp
