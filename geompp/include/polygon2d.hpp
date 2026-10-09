#pragma once

#include "constants.hpp"
#include "line_segment2d.hpp"
#include "maths.hpp"
#include "point2d.hpp"
#include "segment_iterator2d.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <variant>

namespace geompp {

inline namespace geometry {

class Line2D;
class Ray2D;
class Polyline2D;
class Triangle2D;
class Polygon2D;
class PolyMesh2D;

}  // namespace geometry

namespace transformations {
// Forward-declared so Polygon2D can friend this exact overload below -- transform() is the only caller
// allowed to reach the private FromUniqueCCWPoints() fast constructor (see its own doc comment for why).
geometry::Polygon2D transform(geometry::Polygon2D const& poly, maths::Matrix3 const& m);
}  // namespace transformations

inline namespace geometry {
namespace detail {
// Forward-declared (concrete pair/vector spelling, not the RingPiecesOf<MeshTriangleFaceView2D> alias -- that
// alias needs MeshTriangleFaceView2D's full definition to resolve, which would pull calc_utils/polygonization2d.hpp
// in here and cycle straight back to this header) so Polygon2D can friend this exact overload below.
// Packages polygonize()/merge()/Mesh2D::Polygonize()/ConnectedMesh2D::Polygonize()'s traced {outer, holes}
// pieces into Polygon2D, WITHOUT running remove_collinear() on them first -- see this function's own doc
// comment (calc_utils/polygonization2d.cpp) for why that's a correctness requirement here, not just an
// optimization: a collinear point dropped from one piece's boundary can still be a genuine, load-bearing
// corner of a NEIGHBORING piece, and silently dropping it produces a T-junction between the two once
// they're welded into a PolyMesh2D.
std::vector<Polygon2D> polygons_from_pieces(
    std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>> pieces);
}  // namespace detail
}  // namespace geometry

inline namespace geometry {

class Polygon2D {
 public:
  static Polygon2D Make(std::vector<Point2D> const& points);
  static Polygon2D Make(std::vector<Point2D> const& points, std::vector<std::vector<Point2D>> const& holes);
  /// @brief Same as the const& overload, but consumes @p points instead of copying it — every
  /// point-cleanup step (collinear removal, etc.) reuses @p points' own storage instead of allocating
  /// a fresh vector.
  static Polygon2D Make(std::vector<Point2D>&& points);
  /// @brief Same as the const& overload, but consumes both @p points and @p holes instead of copying them.
  static Polygon2D Make(std::vector<Point2D>&& points, std::vector<std::vector<Point2D>>&& holes);
  Polygon2D(Polygon2D const&) = default;
  Polygon2D(Polygon2D&&) = default;
  ~Polygon2D() = default;

  std::size_t Size() const;
  Point2D const& operator[](std::size_t i) const;

  bool AlmostEquals(Polygon2D const& other, double epsilon = DOUBLE_EPSILON) const;
  SegmentRange2D ToSegments() const;
  Point2D Centroid() const;

  /// @brief Outer ring area minus holes. Holes are always simple (Make() rejects a self-intersecting hole
  /// outright), so their contribution is always a direct O(1)-per-hole shoelace sum. If the outer ring is
  /// also simple, the whole thing is O(n). If the outer ring self-intersects (e.g. a bowtie), it's
  /// decomposed at O(n log n) into its real bounded faces (discarding the unbounded "outside" face the
  /// decomposition also produces) and their areas summed — the total COVERED area, matching what
  /// Intersection()/Difference()/etc. operate against (winding-number membership counts every lobe as
  /// "inside" regardless of local winding sign), not a net/signed sum where opposite-winding lobes would
  /// otherwise partially cancel.
  double Area() const;
  double PerimeterSize() const;
  bool IsSimple() const;  // no self-intersections, but holes are allowed
  bool IsConvex() const;  // no holes and all turns in the same direction — cached at construction
  Polygon2D ConvexHull();
  /// @brief Decomposes a self-intersecting polygon into one or more simple polygons.
  /// @return {*this} if already simple; otherwise the set of simple polygons covering the same area.
  std::vector<Polygon2D> Simplify() const;
  std::vector<Point2D> const& Perimeter() const;
  /// @brief Whether this polygon has one or more holes.
  bool HasHoles() const;
  /// @brief The polygon's holes, each an ordered (CW) ring of vertices. Empty when the polygon has no holes.
  std::vector<std::vector<Point2D>> const& Holes() const;

  /// @brief Breaks down the polygon (outer ring ONLY, holes are ignored) into a set of triangles.
  /// Make() already guarantees the outer ring is simple, CCW-wound, and free of collinear/duplicate
  /// points, so this always calls the free triangulate() with every TriangulationParams check set to
  /// Guaranteed — no re-validation cost.
  /// @param strategy which triangulation algorithm to run (see TriangulationParams::Strategy).
  /// @returns one Triangle2D per triangle; Size() - 2 triangles.
  /// @throws whatever the chosen @p strategy itself throws (e.g. std::runtime_error for a
  /// not-yet-implemented strategy).
  std::vector<Triangle2D> Triangulate(TriangulationParams::Strategy strategy) const;

  /// @brief Distance from a point to this polygon's closed region.
  /// @param point The point to measure distance to.
  /// @return 0 if @p point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.
  double DistanceTo(Point2D const& point) const;

  std::string ToWkt() const;
  static Polygon2D FromWkt(std::string const& wkt);
  void ToFile(std::string const& path) const;
  static Polygon2D FromFile(std::string const& path);

  Polygon2D& operator=(Polygon2D const& other);
  Polygon2D& operator=(Polygon2D&&) = default;

#pragma region Geometrical Operations

  /// @brief Tests whether a point lies inside the polygon (winding-number check).
  /// @param point The point to test.
  /// @return true if @p point is in the polygon's closed region.
  bool Contains(Point2D const& point) const;

  /// @brief Tests whether a point lies on the polygon's boundary.
  /// @param point The point to test.
  /// @return true if @p point is on any of the polygon's edges or vertices.
  bool IsOnPerimeter(Point2D const& point) const;

  /// @brief Tests whether this polygon intersects a line.
  /// @param line The line.
  /// @return true if the line crosses the polygon's closed region.
  bool Intersects(Line2D const& line) const;

  /// @brief Tests whether this polygon intersects a ray.
  /// @param ray The ray.
  /// @return true if the ray hits the polygon within its domain.
  bool Intersects(Ray2D const& ray) const;

  /// @brief Tests whether this polygon intersects a segment.
  /// @param segment The segment.
  /// @return true if any part of the segment is inside the polygon or crosses its boundary.
  bool Intersects(LineSegment2D const& segment) const;

  /// @brief Tests whether this polygon shares any area (or boundary) with another.
  /// @param other The other polygon.
  /// @return true if the two polygons overlap, touch, or either fully contains the other.
  bool Intersects(Polygon2D const& other) const;

  /// @brief Intersection of this polygon with a line.
  /// @param line The line.
  /// @return The crossing point, or std::nullopt if the line misses the polygon.
  std::optional<std::vector<LineSegment2D>> Intersection(Line2D const& line) const;

  /// @brief Intersection of this polygon with a ray.
  /// @param ray The ray.
  /// @return The crossing point if within the ray's domain, or std::nullopt otherwise.
  std::optional<std::vector<LineSegment2D>> Intersection(Ray2D const& ray) const;

  /// @brief Intersection of this polygon with a segment.
  /// @param other The segment.
  /// @return The crossing point if it lies on the segment, or std::nullopt otherwise.
  std::optional<std::vector<LineSegment2D>> Intersection(LineSegment2D const& other) const;

#pragma endregion

#pragma region Boolean Operations

  /// @brief Set union of this polygon and other. Handles holes and self-intersecting operands; a
  /// disjoint pair of polygons yields more than one result polygon.
  /// @return Zero or more result polygons (zero is impossible for Union unless both operands are empty).
  std::vector<Polygon2D> Union(Polygon2D const& other) const;

  /// @brief Set intersection of this polygon and other (overloads Intersection() by argument type).
  /// @return Zero or more result polygons — empty if the two polygons don't overlap.
  std::vector<Polygon2D> Intersection(Polygon2D const& other) const;

  /// @brief Set difference (this minus other). Handles the case where other lies entirely inside this
  /// polygon with no shared boundary, correctly producing a hole.
  /// @return Zero or more result polygons — empty if other fully covers this polygon.
  std::vector<Polygon2D> Difference(Polygon2D const& other) const;

  /// @brief Symmetric difference (the area covered by exactly one of the two polygons).
  /// @return Zero or more result polygons.
  std::vector<Polygon2D> Xor(Polygon2D const& other) const;

#pragma endregion

#pragma region Iterators

  // Only expose const_iterator
  using const_iterator = std::vector<Point2D>::const_iterator;

  // Both const and non-const begin/end return const_iterator!
  const_iterator begin() const;
  const_iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;

#pragma endregion

 private:
  std::vector<Point2D> VERTICES;
  std::vector<std::vector<Point2D>> HOLES;
  double PERIMETER;
  bool IS_CONVEX;

  // Shared by every Make() overload (const&/&& on points, with/without holes): validates an
  // already-collinear-filtered outer ring (and, for the second overload, raw holes still needing their
  // own per-hole cleanup) and wraps it. Taking everything by value lets each Make() overload hand off
  // its data with a single move regardless of whether it started from a const& or && parameter.
  static Polygon2D FromUniquePoints(std::vector<Point2D> unique_points);
  static Polygon2D FromUniquePoints(std::vector<Point2D> unique_points, std::vector<std::vector<Point2D>> holes);

  // Same contract as FromUniquePoints, but ALSO trusts the caller that unique_points is already correctly
  // CCW (skips the are_ccw() check) and that is_convex is already known -- both true by construction for
  // transform()'s own use: it reverses the ring itself first whenever the transform's determinant is
  // negative (see transformations2d.cpp), so winding is already right by the time this runs, and
  // convexity is affine-invariant so the source polygon's own IsConvex() carries over unchanged. Private
  // and friended to transform() and polygons_from_pieces() specifically -- NOT exposed publicly, since
  // calling this with points that aren't actually CCW, or a wrong is_convex, would silently corrupt the
  // winding/convexity invariants every other method on this class relies on.
  static Polygon2D FromUniqueCCWPoints(std::vector<Point2D> unique_points, bool is_convex);
  static Polygon2D FromUniqueCCWPoints(std::vector<Point2D> unique_points, std::vector<std::vector<Point2D>> holes,
                                       bool is_convex);

  friend Polygon2D geompp::transformations::transform(Polygon2D const& poly, maths::Matrix3 const& m);

  // Same trust as transform()'s own friend grant above: polygons_from_pieces() only ever calls this with
  // pieces already proven CCW-outer/CW-hole (package_result_rings/the boundary trace both classify a ring
  // into outer-vs-hole BY testing are_ccw/are_cw, so the fact is already computed before this ctor would
  // redo it) and an explicitly-computed is_convex -- see polygons_from_pieces' own doc comment.
  friend std::vector<Polygon2D> geompp::geometry::detail::polygons_from_pieces(
      std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>> pieces);

  // PolyMesh2D::operator[]() reconstructs a Polygon2D from its own stored VERTICES/FACE_INDICES buffers --
  // vertices FromPolygons() already welded and validated (assert_adjacency) at construction time, in the
  // exact order they were given. It must use FromUniquePoints() here, NOT the public Make(), for the same
  // load-bearing-collinear-vertex reason as polygons_from_pieces() above: Make()'s remove_collinear() pass
  // has no visibility into a neighboring facet's needs, so it would happily strip a vertex that's collinear
  // on THIS facet's own ring but is another facet's genuine corner touching this edge -- silently
  // reintroducing, on every read, the exact T-junction FromPolygons() just finished proving doesn't exist.
  friend class PolyMesh2D;

  Polygon2D(std::vector<Point2D> const& points, double perimeter, bool is_convex);
  Polygon2D(std::vector<Point2D> const& points, double perimeter, std::vector<std::vector<Point2D>> const& holes,
            bool is_convex);
  Polygon2D(std::vector<Point2D>&& points, double perimeter, bool is_convex);
  Polygon2D(std::vector<Point2D>&& points, double perimeter, std::vector<std::vector<Point2D>>&& holes, bool is_convex);
};

#pragma region Operator Overloading

bool operator==(Polygon2D const& lhs, Polygon2D const& rhs);

std::ostream& operator<<(std::ostream& os, Polygon2D const& g);

#pragma endregion

#pragma region Inlined Functions

inline std::size_t Polygon2D::Size() const { return VERTICES.size(); }
inline bool Polygon2D::IsConvex() const { return IS_CONVEX; }
inline bool Polygon2D::HasHoles() const { return !HOLES.empty(); }
inline std::vector<std::vector<Point2D>> const& Polygon2D::Holes() const { return HOLES; }
inline Polygon2D::Polygon2D(std::vector<Point2D> const& points, double perimeter, bool is_convex)
    : VERTICES(points), HOLES{}, PERIMETER(perimeter), IS_CONVEX(is_convex) {}
inline Polygon2D::Polygon2D(std::vector<Point2D> const& points, double perimeter,
                            std::vector<std::vector<Point2D>> const& holes, bool is_convex)
    : VERTICES(points), HOLES(holes), PERIMETER(perimeter), IS_CONVEX(is_convex) {}
inline Polygon2D::Polygon2D(std::vector<Point2D>&& points, double perimeter, bool is_convex)
    : VERTICES(std::move(points)), HOLES{}, PERIMETER(perimeter), IS_CONVEX(is_convex) {}
inline Polygon2D::Polygon2D(std::vector<Point2D>&& points, double perimeter, std::vector<std::vector<Point2D>>&& holes,
                            bool is_convex)
    : VERTICES(std::move(points)), HOLES(std::move(holes)), PERIMETER(perimeter), IS_CONVEX(is_convex) {}

// Both const and non-const begin/end return const_iterator!
inline Polygon2D::const_iterator Polygon2D::begin() const { return VERTICES.cbegin(); }
inline Polygon2D::const_iterator Polygon2D::end() const { return VERTICES.cend(); }

inline Polygon2D::const_iterator Polygon2D::cbegin() const { return VERTICES.cbegin(); }
inline Polygon2D::const_iterator Polygon2D::cend() const { return VERTICES.cend(); }

#pragma endregion

}  // namespace geometry

}  // namespace geompp
