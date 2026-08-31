#pragma once

#include "../constants.hpp"
#include "../generic_concepts.hpp"
#include "../point2d.hpp"
#include "../view2d.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace geompp {

inline namespace geometry {

class Polygon2D;
class Polygon3D;
class Triangle2D;
class Triangle3D;

namespace detail {

namespace view {

// all triangulation functions

/// @brief O(n^2)-worst-case ear-clipping triangulation of a single simple, CCW-wound ring, projected
/// through @p view. Repeatedly clips a convex "ear" vertex (one whose candidate triangle contains no
/// other, currently-reflex vertex) until 3 vertices remain, then emits that last triangle. Robust for
/// small-to-medium polygons; does not require the ring to be monotone or well-conditioned.
/// @param input ring vertices (Point2D or Point3D), simple, CCW, no consecutive duplicates or collinear
/// points — the caller (triangulate_impl) is responsible for enforcing this via TriangulationParams.
/// @param view projects each vertex to 2D x/y coordinates.
/// @returns one `{i, j, k}` index triplet per triangle, indices into @p input, n - 2 triangles total.
/// @warning A diagonal that happens to pass exactly through a non-adjacent vertex (a coincidental
/// collinearity across the ring, not just between consecutive points — TriangulationParams::Collinearity
/// only ever checks consecutive triplets) can misclassify an ear; a known limitation of the strict
/// point-in-triangle test shared by most textbook ear-clipping implementations.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<PointT> const& input,
                                                                   View2D const& view);

extern template std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<Point2D> const& input,
                                                                                   View2D const& view);
extern template std::vector<std::array<std::size_t, 3>> ear_clipping_triangulation(std::vector<Point3D> const& input,
                                                                                   View2D const& view);

/// @brief Like ear_clipping_triangulation, but each step does a full lap over the current ring to clip
/// the best-scoring valid ear (by shape quality) instead of the first one found — avoids gratuitously
/// thin slivers that plain EarClipping can produce purely from scan order. Never rejects a
/// geometrically valid ear outright (only reorders which one is preferred), so it keeps the same Two
/// Ears Theorem termination guarantee as ear_clipping_triangulation. Unconditionally ~O(n²): a full
/// O(current n) rescan runs on every single clip, where ear_clipping_triangulation's O(n²) is only a
/// worst case.
/// @param input ring vertices (Point2D or Point3D), simple, CCW, no consecutive duplicates or collinear
/// points — the caller (triangulate_impl) is responsible for enforcing this via TriangulationParams.
/// @param view projects each vertex to 2D x/y coordinates.
/// @returns one `{i, j, k}` index triplet per triangle, indices into @p input, n - 2 triangles total.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(std::vector<PointT> const& input,
                                                                            View2D const& view);

extern template std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(
    std::vector<Point2D> const& input, View2D const& view);
extern template std::vector<std::array<std::size_t, 3>> ear_clipping_best_fit_triangulation(
    std::vector<Point3D> const& input, View2D const& view);

/// @brief O(n log n)-worst-case triangulation of a monotone ring, projected through @p view.
/// @param input ring vertices (Point2D or Point3D), simple, CCW, monotone with respect to some direction.
/// @param view projects each vertex to 2D x/y coordinates.
/// @returns one `{i, j, k}` index triplet per triangle, indices into @p input.
/// @throws std::runtime_error not yet implemented.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(std::vector<PointT> const& input,
                                                                       View2D const& view);

extern template std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(
    std::vector<Point2D> const& input, View2D const& view);
extern template std::vector<std::array<std::size_t, 3>> monotone_polygon_triangulation(
    std::vector<Point3D> const& input, View2D const& view);

/// @brief O(n log n)-worst-case Delaunay triangulation of a point set's convex hull, projected through
/// @p view — maximizes the minimum angle across all triangles (avoids skinny slivers), unlike EarClipping
/// or MonotonePolygon which triangulate the given polygon's own boundary.
/// @param input point set (Point2D or Point3D).
/// @param view projects each vertex to 2D x/y coordinates.
/// @returns one `{i, j, k}` index triplet per triangle, indices into @p input.
/// @throws std::runtime_error not yet implemented.
template <typename PointT>
std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<PointT> const& input, View2D const& view);

extern template std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<Point2D> const& input,
                                                                               View2D const& view);
extern template std::vector<std::array<std::size_t, 3>> delaunay_triangulation(std::vector<Point3D> const& input,
                                                                               View2D const& view);

/// @brief Shared implementation behind every geompp::triangulate() overload: validates/fixes @p input
/// per @p settings (Collinearity, then Winding, then Simplicity — in that order, since Simplicity's
/// self-intersection sweep is the most expensive check and the other two can shrink/reorder the ring
/// first), then dispatches to the requested Strategy.
/// @param input polygon outer loop, no holes (assumed CCW unless settings.ccw_winding says otherwise).
/// @param view projects each vertex to 2D x/y coordinates (2D: View2D::XY(); 3D: a plane-derived view).
/// @param settings see TriangulationParams — strategy, and how to handle non-simple/non-CCW/collinear
/// input (Guaranteed: skip the check; Assert: throw if violated; Enforce: fix it in place).
/// @returns one `std::array<PointT, 3>` (actual point coordinates, not indices) per triangle.
/// @throws std::invalid_argument if @p input has fewer than 3 points, or (Assert mode) if a precondition
/// is violated, or if @p settings names an unknown enumerator.
template <typename PointT>
std::vector<std::array<PointT, 3>> triangulate_impl(std::vector<PointT> const& input, View2D const& view,
                                                    TriangulationParams const& settings = TriangulationParams{});

extern template std::vector<std::array<Point2D, 3>> triangulate_impl(std::vector<Point2D> const& input,
                                                                     View2D const& view,
                                                                     TriangulationParams const& settings);
extern template std::vector<std::array<Point3D, 3>> triangulate_impl(std::vector<Point3D> const& input,
                                                                     View2D const& view,
                                                                     TriangulationParams const& settings);

}  // namespace view

}  // namespace detail

/// @brief Breaks down a simple polygon into triangles
/// @param input polygon's outer loop of points (assumed CCW) and no holes allowed
/// @param settings options for functions inner workings
///                 (1) triangulation strategy options: user decides what algorithm to run
///                     - EarClipping clips the first valid ear it finds in scan order. Most robust and
///                                   general-purpose, and often close to O(n) in practice, but O(n²) worst-case -- and
///                                   doesn't optimize triangle shape, so it can produce a visually thin sliver purely
///                                   from scan order, even on ordinary input.
///                     - EarClippingBestFit clips the best-scoring (least sliver-prone) valid ear every step
///                                   instead of the first one. Same termination guarantee as EarClipping, but
///                                   unconditionally ~O(n²) -- a full rescan of the current ring on every single clip,
///                                   not just worst-case.
///                                   [Default: prefers shape quality over raw speed.]
///                     - MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition
///                                        into monotone pieces)
///                     - Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle
///                                 of all the angles of the triangles in the triangulation (avoiding skinny triangles)
///                 (2) simplicity: the input for the algo should be a simple polygon (no self-intersections)
///                     - Guaranteed: the input is assumed to be a good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's input will be simplified in O(n log n) time
///                 (3) winding: the input points should be in counter clockwise order (CCW)
///                     - Guaranteed: the input is assumed to be good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's input will be reversed in O(n) time
///                 (4) collinearity: the input points should not contain any collinear (including consecutive
///                      duplicates) points
///                     - Guaranteed: the input is assumed to be good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's bad vertices will be removed in O(n) time
/// @returns one Triangle2D per triangle; input.size() - 2 triangles for a simple polygon.
/// @throws std::invalid_argument if @p input has fewer than 3 points, or (Assert mode) if a precondition
/// named by @p settings is violated.
std::vector<Triangle2D> triangulate(std::vector<Point2D> const& input,
                                    TriangulationParams const& settings = TriangulationParams{});

/// @brief One "more than 1 neighbor" violation of the mesh-conformity rule found by validate_adjacency()
/// across a batch of facets — see AdjacencyConformity's own docs for what the rule means. Covers two
/// distinct cases, distinguishable by facet_indices.size() (1 vs. 3+, never 2) if a caller needs to:
/// a T-junction (facet_indices has exactly 1 entry, on_vertex is the foreign vertex lying on the edge),
/// or a non-manifold edge (facet_indices lists 3+ facets that all share this exact edge; on_vertex is
/// not meaningful there -- just a reused edge endpoint, not a real foreign vertex).
/// @tparam PointT Point2D or Point3D. Not View2D-projected: unlike triangulation/convexity/winding,
/// "does this vertex lie on this edge" is a well-defined, exact question in native space for either
/// dimension. For a general 3D mesh (facets in many different planes -- a building's walls and roof,
/// say), flattening through one shared 2D view would actually be WRONG here: it can manufacture a
/// spurious overlap between two edges that don't actually touch in 3D, or miss a genuine touch,
/// depending on the projection angle.
template <typename PointT>
struct AdjacencyViolation {
  PointT edge_p0, edge_p1;                 ///< the shared/coarse edge the violation is on.
  std::vector<std::size_t> facet_indices;  ///< For a T-junction: always exactly 1 entry, facet_indices[0],
                                           ///< the facet owning the coarse edge (edge_p0, edge_p1) -- the
                                           ///< one fix_adjacency() splices @ref on_vertex into. The foreign
                                           ///< facet @ref on_vertex itself belongs to is deliberately NOT
                                           ///< reported: when 3+ facets share that exact vertex,
                                           ///< validate_adjacency_impl's grid-cell dedup only keeps one
                                           ///< arbitrary representative, so naming "the" owner would be
                                           ///< misleading. For a non-manifold edge: every facet (3+, always)
                                           ///< that shares this exact edge, exhaustively.
  PointT on_vertex;                        ///< only meaningful for a T-junction (facet_indices.size() == 1):
                                           ///< the foreign vertex lying in the interior of (edge_p0, edge_p1).
};

/// @brief Checks a batch of facets for the mesh-conformity rule "every edge has at most 1 neighbor" --
/// equivalently, no facet vertex may lie in the interior of another facet's edge, only exactly at that
/// edge's own start/end vertex. The same rule Mesh2D/3D::FromTriangles/PolyMesh2D/3D::FromPolygons/
/// ConnectedMesh2D/3D::FromTriangles enforce (always, via Assert) at construction time.
/// @param facets each facet's outer ring (e.g. Polygon2D/3D::Perimeter(), or a Triangle2D/3D's 3 vertices).
/// @returns every violation found (both T-junctions and non-manifold edges); empty if conforming.
/// @note O(n²)-ish (pairwise edge/vertex comparisons) -- a validation/construction-time check, not
/// intended to run in a hot loop.
std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<Polygon2D> const& facets);
std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<Triangle2D> const& facets);
std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<Polygon3D> const& facets);
std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<Triangle3D> const& facets);

/// @brief validate_adjacency() overload for raw point rings (not yet wrapped in Polygon2D/3D) -- what
/// fix_adjacency() below returns, and what its own detection pass runs on internally.
std::vector<AdjacencyViolation<Point2D>> validate_adjacency(std::vector<std::vector<Point2D>> const& facet_rings);
std::vector<AdjacencyViolation<Point3D>> validate_adjacency(std::vector<std::vector<Point3D>> const& facet_rings);

/// @brief Repairs every T-junction validate_adjacency() would report. For each foreign vertex, splices
/// it into the coarse edge, then cuts a diagonal from that vertex to the nearest ring vertex that forms
/// a valid, non-crossing diagonal (checked via proper-segment-intersection + a point-in-polygon interior
/// test), splitting the facet into two pieces along it. A facet with several T-junctions on one edge
/// ends up split into several pieces, not just spliced once. Does NOT attempt to fix a non-manifold edge
/// (a full edge shared by 3+ facets); there's no principled automatic repair for that, so it throws
/// instead -- same as validate_adjacency() + Assert would.
///
/// Returns raw point rings, NOT reconstructed Polygon2D/3D objects: the split pieces have no guarantee
/// of matching a valid Polygon2D/3D winding/hole structure. Unlike the Triangle overload below, these
/// pieces have no leftover flat (180°) vertices, so -- unlike that overload -- they can be triangulated
/// afterward with the default Collinearity::Enforce; no special handling needed.
/// @throws std::invalid_argument if any non-manifold edge is found.
std::vector<std::vector<Point2D>> fix_adjacency(std::vector<Polygon2D> const& facets);
std::vector<std::vector<Point3D>> fix_adjacency(std::vector<Polygon3D> const& facets);

/// @brief Triangle overload of fix_adjacency(): unlike a Polygon2D/3D facet, a triangle can't just
/// absorb a spliced-in vertex and stay a triangle -- a repaired facet is re-triangulated (via the same
/// ear-clipping engine triangulate() uses, with Collinearity::Guaranteed for the same reason the
/// Polygon overload avoids Polygon2D/3D::Make()) into 2+ triangles covering the exact same area as the
/// original one. An unaffected facet passes through unchanged (still exactly 1 triangle).
/// @throws std::invalid_argument if any non-manifold edge is found.
std::vector<Triangle2D> fix_adjacency(std::vector<Triangle2D> const& facets);
std::vector<Triangle3D> fix_adjacency(std::vector<Triangle3D> const& facets);

/// @brief Batch-triangulates a set of polygon facets together. The free-function equivalent of
/// `PolyMesh2D::FromPolygons(polygons).Triangulate()` for callers who just want triangles without
/// constructing/keeping a full PolyMesh2D. Unlike PolyMesh2D::FromPolygons (whose own conformity
/// parameter defaults to Assert, since bad input there is a straightforward construction error), this
/// defaults to fixing what it can.
/// @param polygons each facet's outer ring (no holes).
/// @param settings per-facet TriangulationParams (Strategy/Simplicity/Winding/Collinearity), same as the
/// single-ring triangulate() overload above, plus `TriangulationParams::conformity` (see the standalone
/// @ref AdjacencyConformity): how to handle cross-facet adjacency violations (T-junctions /
/// non-manifold edges) before triangulating. Defaults to Enforce. Under Enforce, a facet that needed a
/// conformity splice is always triangulated with
/// Collinearity::Guaranteed regardless of the rest of @p settings -- otherwise the caller's own
/// Collinearity::Enforce (the TriangulationParams default) would strip the just-spliced vertex right back
/// out, silently undoing the repair and reintroducing the T-junction in the triangulated output.
/// @returns every triangle from every facet, combined into one flat list.
/// @throws std::invalid_argument on a non-manifold edge (any conformity mode other than Guaranteed), or
/// on any violation at all under Assert.
std::vector<Triangle2D> triangulate(std::vector<Polygon2D> const& polygons,
                                    TriangulationParams const& settings = TriangulationParams{});

namespace detail {

/// @brief Throws std::invalid_argument with a descriptive message (naming the offending edge and
/// facets) if @p violations is non-empty. Shared by Mesh2D/3D::FromTriangles, PolyMesh2D/3D::FromPolygons,
/// and ConnectedMesh2D/3D::FromTriangles, which all unconditionally Assert this rule at construction
/// time — a non-conforming mesh is treated as invalid caller input there, never silently repaired.
template <typename PointT>
void assert_adjacency(std::vector<AdjacencyViolation<PointT>> const& violations);

extern template void assert_adjacency(std::vector<AdjacencyViolation<Point2D>> const&);
extern template void assert_adjacency(std::vector<AdjacencyViolation<Point3D>> const&);

}  // namespace detail

}  // namespace geometry

}  // namespace geompp
