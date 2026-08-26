#pragma once

#include "../constants.hpp"
#include "../generic_concepts.hpp"
#include "../plane.hpp"
#include "../point2d.hpp"
#include "../point3d.hpp"
#include "../triangle2d.hpp"
#include "../triangle3d.hpp"
#include "../utils.hpp"
#include "../view2d.hpp"
#include "convex_hull2d.hpp"
#include "polygon_ops2d.hpp"

#include <array>
#include <cstddef>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace geompp {

inline namespace geometry {

class Polygon2D;

namespace detail {

/// @brief Non-owning view over one facet of a welded 2D triangle mesh, plus its precomputed edge-adjacency
/// (see build_neighbor_refs) -- models the TriangleFaceView concept (generic_concepts.hpp) that polygonize_impl
/// and its strategy helpers below are templated over.
///
/// Deliberately independent of ConnectedMesh2D/Mesh2D: both Mesh2D's per-face std::array<size_t,3> layout
/// and ConnectedMesh2D's flat, stride-3 std::vector<size_t> layout already guarantee 3 contiguous indices
/// per face, so this works uniformly from a raw size_t const* regardless of which owner it came from --
/// ConnectedMesh2D::Polygonize() wraps its own already-stored VERTICES/TRIANGLES/NEIGHBORS directly (zero
/// building), and Mesh2D::Polygonize() builds only a transient NEIGHBORS array via build_neighbor_refs()
/// on its own already-welded FACE_INDICES (no re-weld, unlike going through Mesh2D::Connect() would be --
/// see Mesh2D::Polygonize()'s own comment). Neither needs a friend grant to read mesh topology this way,
/// since both are themselves members of the mesh class they read from.
///
/// @warning Non-owning: the vertex/index/neighbor buffers it points into must outlive it -- same lifetime
/// contract as ConnectedMesh2D::FaceView2D / Mesh2D::Faces().
class MeshFaceView2D {
 public:
  /// @param vertices Pointer to the mesh's shared unique-vertex buffer.
  /// @param face_index_base Pointer to the first of 3*n_faces contiguous vertex indices (face f's are at
  /// face_index_base[3*f], [3*f+1], [3*f+2]).
  /// @param neighbor_base Pointer to the first of n_faces per-facet neighbor-ref arrays.
  /// @param face_id Which facet this view refers to.
  MeshFaceView2D(Point2D const* vertices, std::size_t const* face_index_base,
                 std::array<TriangleCompactNeighborRef, 3> const* neighbor_base, std::size_t face_id);

  /// @returns This facet's index into the mesh's face buffer.
  std::size_t ID() const;

  /// @brief The mesh-wide vertex index of this facet's @p local_i-th corner (0, 1, or 2).
  /// @param local_i Which corner (0, 1, or 2).
  /// @returns The index into the mesh's shared unique-vertex buffer.
  std::size_t VertexIndex(std::size_t local_i) const;

  /// @returns This facet's geometry, rebuilt from the mesh's welded vertices.
  Triangle2D Geometry() const;

  /// @param edge Which of this facet's 3 edges to cross.
  /// @returns The adjacent facet, or std::nullopt if @p edge is a boundary edge (no twin).
  std::optional<MeshFaceView2D> Neighbor(TriangleCompactNeighborRef::TriangleEdge edge) const;
  /// @param edge Which of this facet's 3 edges to cross.
  /// @returns The local edge id of the twin edge on the other side of @p edge, or TriangleEdge::INVALID
  /// if @p edge is a boundary edge (no twin).
  TriangleCompactNeighborRef::TriangleEdge NeighborEntryEdge(TriangleCompactNeighborRef::TriangleEdge edge) const;

 private:
  Point2D const* m_vertices;
  std::size_t const* m_face_index_base;
  std::array<TriangleCompactNeighborRef, 3> const* m_neighbor_base;
  std::size_t m_face_id;
};

inline MeshFaceView2D::MeshFaceView2D(Point2D const* vertices, std::size_t const* face_index_base,
                                      std::array<TriangleCompactNeighborRef, 3> const* neighbor_base,
                                      std::size_t face_id)
    : m_vertices(vertices), m_face_index_base(face_index_base), m_neighbor_base(neighbor_base), m_face_id(face_id) {}

inline std::size_t MeshFaceView2D::ID() const { return m_face_id; }

inline std::size_t MeshFaceView2D::VertexIndex(std::size_t local_i) const {
  return m_face_index_base[m_face_id * 3 + local_i];
}

/// @brief 3D counterpart of MeshFaceView2D -- same non-owning, raw-pointer-index view shape, over a welded
/// 3D triangle mesh (Mesh3D's std::array<size_t,3>-per-face layout or ConnectedMesh3D's flat, stride-3
/// std::vector<size_t> layout). Declared here, defined in polygonization3d.cpp -- mirrors this codebase's
/// existing triangulation2d.hpp/.cpp split, where the PointT-generic algorithm (ear_clipping_triangulation,
/// triangulate_impl, ...) and its extern template declarations for BOTH Point2D and Point3D live in the
/// "2d" file, and the "3d" file/pair is a thin wrapper providing the 3D-specific public API. Kept alongside
/// MeshFaceView2D (rather than only in polygonization3d.hpp) because FaceViewPointT/RingPiecesOf/every
/// strategy helper below need MeshFaceView3D's full declaration (specifically Geometry()'s return type) to
/// even name `RingPiecesOf<MeshFaceView3D>` in this header's own extern template declarations.
class MeshFaceView3D {
 public:
  /// @param vertices Pointer to the mesh's shared unique-vertex buffer.
  /// @param face_index_base Pointer to the first of 3*n_faces contiguous vertex indices (face f's are at
  /// face_index_base[3*f], [3*f+1], [3*f+2]).
  /// @param neighbor_base Pointer to the first of n_faces per-facet neighbor-ref arrays.
  /// @param face_id Which facet this view refers to.
  MeshFaceView3D(Point3D const* vertices, std::size_t const* face_index_base,
                 std::array<TriangleCompactNeighborRef, 3> const* neighbor_base, std::size_t face_id);

  /// @returns This facet's index into the mesh's face buffer.
  std::size_t ID() const;
  /// @brief The mesh-wide vertex index of this facet's @p local_i-th corner (0, 1, or 2).
  /// @param local_i Which corner (0, 1, or 2).
  /// @returns The index into the mesh's shared unique-vertex buffer.
  std::size_t VertexIndex(std::size_t local_i) const;

  /// @returns This facet's geometry, rebuilt from the mesh's welded vertices.
  Triangle3D Geometry() const;

  /// @param edge Which of this facet's 3 edges to cross.
  /// @returns The adjacent facet, or std::nullopt if @p edge is a boundary edge (no twin).
  std::optional<MeshFaceView3D> Neighbor(TriangleCompactNeighborRef::TriangleEdge edge) const;
  /// @param edge Which of this facet's 3 edges to cross.
  /// @returns The local edge id of the twin edge on the other side of @p edge, or TriangleEdge::INVALID
  /// if @p edge is a boundary edge (no twin).
  TriangleCompactNeighborRef::TriangleEdge NeighborEntryEdge(TriangleCompactNeighborRef::TriangleEdge edge) const;

 private:
  Point3D const* m_vertices;
  std::size_t const* m_face_index_base;
  std::array<TriangleCompactNeighborRef, 3> const* m_neighbor_base;
  std::size_t m_face_id;
};

inline MeshFaceView3D::MeshFaceView3D(Point3D const* vertices, std::size_t const* face_index_base,
                                      std::array<TriangleCompactNeighborRef, 3> const* neighbor_base,
                                      std::size_t face_id)
    : m_vertices(vertices), m_face_index_base(face_index_base), m_neighbor_base(neighbor_base), m_face_id(face_id) {}

inline std::size_t MeshFaceView3D::ID() const { return m_face_id; }

inline std::size_t MeshFaceView3D::VertexIndex(std::size_t local_i) const {
  return m_face_index_base[m_face_id * 3 + local_i];
}

/// @brief Extracts the point type (Point2D/Point3D) a TriangleFaceView-modeling type's Geometry() works in,
/// via its Triangle2D/3D's own Vertices() tuple -- used to keep polygonize_impl's strategy helpers generic
/// over both dimensions without hard-coding either Triangle type by name.
/// @note remove_const_t is required, not cosmetic: Triangle2D/3D::Vertices() returns
/// `std::tuple<PointT, PointT, PointT> const` (a top-level const on the tuple itself), which
/// std::tuple_element_t propagates onto the extracted element -- without stripping it here, every
/// std::vector<FaceViewPointT<FaceViewT>> below would be a std::vector<PointT const>, which
/// static_asserts at compile time ("the C++ Standard forbids containers of const elements").
template <TriangleFaceView FaceViewT>
using FaceViewPointT =
    std::remove_const_t<std::tuple_element_t<0, decltype(std::declval<FaceViewT const&>().Geometry().Vertices())>>;

/// @brief One disjoint output component of any polygonize_impl strategy: an outer ring plus its holes (if
///        any), in whichever point type (Point2D/Point3D)
/// @p FaceViewT's own Geometry() works in -- shorthand for the return type every strategy helper and polygonize_impl
///                itself share.
template <TriangleFaceView FaceViewT>
using RingPiecesOf =
    std::vector<std::pair<std::vector<FaceViewPointT<FaceViewT>>, std::vector<std::vector<FaceViewPointT<FaceViewT>>>>>;

/// @brief Partitions a welded triangle mesh's facets into coplanar, edge-connected clusters -- BFS over
/// each facet's Neighbor() links, only crossing into a neighbor if it's coplanar with the CLUSTER's own
/// reference plane (the seed facet's plane, fixed once per cluster and never updated as the walk grows --
/// comparing every candidate against a drifting "current facet" plane instead would let coplanarity error
/// accumulate transitively across a long chain of individually-within-epsilon steps; same non-transitive-
/// epsilon pitfall documented for SweepLine2D's tie-breaking in polygon_ops2d.cpp's BooleanOp_MissedCrossing
/// note). All 3 PolygonizationParams strategies need this first -- none may merge across non-coplanar facets.
///
/// For a 2D mesh (FaceViewPointT<FaceViewT> == Point2D) every facet is trivially coplanar, so the
/// coplanarity check is skipped entirely (resolved via `if constexpr` on the deduced point type, not a
/// runtime branch) and the whole mesh -- if edge-connected -- collapses to a single cluster.
///
/// @tparam FaceViewT MeshFaceView2D or MeshFaceView3D.
/// @param faces Every facet of the mesh, one MeshFaceView2D/3D per facet, ID() == index into this vector.
/// @returns One inner vector of facet IDs per coplanar cluster; every facet appears in exactly one cluster.
template <TriangleFaceView FaceViewT>
std::vector<std::vector<std::size_t>> partition_into_coplanar_clusters(std::vector<FaceViewT> const& faces);

extern template std::vector<std::vector<std::size_t>> partition_into_coplanar_clusters(
    std::vector<MeshFaceView2D> const& faces);
extern template std::vector<std::vector<std::size_t>> partition_into_coplanar_clusters(
    std::vector<MeshFaceView3D> const& faces);

/// @brief Removes edges that appear once in each direction (e.g. (A,B) and (B,A), within DECIMAL_PRECISION)
/// -- an internal seam shared by two adjacent facets/polygons, not a genuine boundary. Shared by
/// boundary_extraction_polygonization() below and merge()'s outer-ring cancellation step (both need "cancel
/// a shared reverse-direction edge pair" without boolean_op's winding-number classification machinery,
/// since their input is assumed already non-overlapping/edge-tiled rather than possibly-crossing).
/// @param edges Every directed edge to consider, each already oriented per its own owning ring's winding.
/// @returns Every edge that had no matching reverse-direction partner in @p edges, in input order. If a
/// key has more forward than reverse occurrences (or vice versa) only the excess survives -- e.g. 2
/// forward + 1 reverse leaves exactly 1 forward edge standing.
std::vector<LineSegment2D> cancel_reverse_pairs(std::vector<LineSegment2D> const& edges);

/// @brief Path-halving union-find lookup: follows @p parent[x] until a self-referencing (root) entry,
/// flattening every visited link to point at its grandparent along the way. Shared by
/// hertel_mehlhorn_polygonization() (per-cluster union-find over face ids) and both merge() overloads
/// (per-plane-group union-find over hole indices, for touching-hole detection) -- promoted to non-anonymous
/// detail:: scope (rather than staying a TU-local anonymous-namespace helper) specifically so
/// polygonization3d.cpp's merge(vector<Polygon3D>) can call the exact same implementation instead of
/// duplicating it.
/// @param parent Union-find parent array, mutated in place by path-halving.
/// @param x The element to find the current root of.
/// @returns The root of @p x's set.
std::size_t uf_find(std::vector<std::size_t>& parent, std::size_t x);

/// @brief PolygonizationParams::PlanarBoundaryExtraction strategy: per coplanar cluster (see
/// partition_into_coplanar_clusters), gathers every facet's 3 directed edges (projected through the
/// cluster's own plane -- View2D::XY() for a 2D mesh, an identity projection; View2D::OnPlane(cluster_plane)
/// for a 3D one), cancels the ones shared with a same-cluster neighbor (cancel_reverse_pairs -- an edge
/// shared with a neighbor in a DIFFERENT cluster, or with no neighbor at all, survives: it's a genuine
/// boundary of the merged region), traces the survivors into closed loops via the same directed-edge tracer
/// boolean_op() uses (trace_directed_boundary), and groups them into {outer, holes} pieces by containment
/// (package_result_rings) -- a cluster whose triangles surround an untriangulated hole produces a real hole
/// ring here, not just an outer one. For a 3D mesh, every output point is unprojected back via View2D::xyz()
/// before returning -- same round-trip Polygon3D::Union()/Simplify() already use.
/// @param faces Every facet of the mesh (see partition_into_coplanar_clusters).
/// @param clusters partition_into_coplanar_clusters(faces)'s own output.
/// @returns One {outer, holes} piece per disjoint boundary loop found (usually, but not always, one per
/// cluster -- a cluster can still decompose into >1 disjoint piece, e.g. two triangle fans touching only at
/// a single shared vertex).
template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> boundary_extraction_polygonization(std::vector<FaceViewT> const& faces,
                                                           std::vector<std::vector<std::size_t>> const& clusters);

extern template RingPiecesOf<MeshFaceView2D> boundary_extraction_polygonization(
    std::vector<MeshFaceView2D> const& faces, std::vector<std::vector<std::size_t>> const& clusters);
extern template RingPiecesOf<MeshFaceView3D> boundary_extraction_polygonization(
    std::vector<MeshFaceView3D> const& faces, std::vector<std::vector<std::size_t>> const& clusters);

/// @brief PolygonizationParams::HertelMehlhorn strategy: per coplanar cluster, greedily merges adjacent
/// facets across a shared edge whenever dissolving that edge into an internal diagonal keeps the merged
/// region convex, via union-find over face ids. Each internal edge is considered once (from its lower-id
/// side); an edge between two faces already in the same union-find group (e.g. an internal diagonal of a
/// triangulated hexagon's cyclic dual, reached a second time via a different path) is a no-op. A candidate
/// merge is tested by re-tracing the union of both groups' CURRENT members (trace_face_group_boundary) and
/// checking the result is a single, hole-free, convex ring (detail::is_convex) -- only then is the union
/// committed; otherwise the two groups stay separate and this diagonal remains un-dissolved. Once every
/// internal edge has been considered, each surviving union-find group is traced one final time into its
/// output piece.
/// @note Re-tracing a growing group on every candidate merge (rather than maintaining an O(1)-splice
/// half-edge boundary structure per group) means this is correct but not strictly O(N) worst-case --
/// O(N * average group size) -- same complexity tradeoff this codebase already accepts elsewhere for a
/// simpler, clearly-correct implementation (see EarClippingBestFit's own "unconditionally ~O(n^2)" note).
/// @param faces Every facet of the mesh (see partition_into_coplanar_clusters).
/// @param clusters partition_into_coplanar_clusters(faces)'s own output.
/// @returns One convex output piece per final union-find group (never has holes -- a convex region can't).
template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> hertel_mehlhorn_polygonization(std::vector<FaceViewT> const& faces,
                                                       std::vector<std::vector<std::size_t>> const& clusters);

extern template RingPiecesOf<MeshFaceView2D> hertel_mehlhorn_polygonization(
    std::vector<MeshFaceView2D> const& faces, std::vector<std::vector<std::size_t>> const& clusters);
extern template RingPiecesOf<MeshFaceView3D> hertel_mehlhorn_polygonization(
    std::vector<MeshFaceView3D> const& faces, std::vector<std::vector<std::size_t>> const& clusters);

/// @brief PolygonizationParams::PlanarQuads strategy: per coplanar cluster, greedily pairs each
/// still-unpaired facet with one still-unpaired same-cluster Neighbor() across a shared edge (first found,
/// in local-edge order) into a quad -- no convexity requirement, unlike HertelMehlhorn (see
/// PolygonizationParams::Strategy's own doc: "planar yet not necessarily convex"). A facet left over with
/// no unpaired same-cluster neighbor (odd cluster size, or all its neighbors already paired) is emitted as
/// a 3-point polygon as-is -- Polygon2D/3D::Make() already accepts a 3-point ring, so no special-casing is
/// needed downstream. Both the pairing and the leftover case reuse trace_face_group_boundary on a 2- or
/// 1-face group respectively, rather than hand-building the merged ring.
/// @param faces Every facet of the mesh (see partition_into_coplanar_clusters).
/// @param clusters partition_into_coplanar_clusters(faces)'s own output.
/// @returns One piece per pair (a quad) or leftover single facet (a triangle); never has holes.
template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> quad_only_polygonization(std::vector<FaceViewT> const& faces,
                                                 std::vector<std::vector<std::size_t>> const& clusters);

extern template RingPiecesOf<MeshFaceView2D> quad_only_polygonization(
    std::vector<MeshFaceView2D> const& faces, std::vector<std::vector<std::size_t>> const& clusters);
extern template RingPiecesOf<MeshFaceView3D> quad_only_polygonization(
    std::vector<MeshFaceView3D> const& faces, std::vector<std::vector<std::size_t>> const& clusters);

/// @brief Shared implementation behind every Polygonize()/polygonize() entry point: partitions @p faces
/// into coplanar clusters (partition_into_coplanar_clusters), then dispatches to the strategy named by
/// @p params.strategy.
/// @param faces Every facet of the mesh (see partition_into_coplanar_clusters).
/// @param params Which polygonization strategy to run.
/// @returns One {outer, holes} piece per disjoint output region -- see each strategy's own docs.
/// @throws std::invalid_argument if @p params names an unknown enumerator.
template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> polygonize_impl(std::vector<FaceViewT> const& faces, PolygonizationParams const& params);

extern template RingPiecesOf<MeshFaceView2D> polygonize_impl(std::vector<MeshFaceView2D> const& faces,
                                                             PolygonizationParams const& params);
extern template RingPiecesOf<MeshFaceView3D> polygonize_impl(std::vector<MeshFaceView3D> const& faces,
                                                             PolygonizationParams const& params);

/// @brief Packages polygonize_impl()/package_result_rings()'s {outer, holes} pieces into Polygon2D,
/// deliberately WITHOUT running remove_collinear() on them: every strategy already only reaches this
/// point after trace_face_group_boundary()/package_result_rings() classified each ring as outer (CCW) or
/// hole (CW) by actually testing are_ccw()/are_cw() on it, so that fact is a proven invariant here, not a
/// guess -- Polygon2D::FromUniqueCCWPoints() trusts it instead of re-deriving it. Skipping
/// remove_collinear() specifically (not just the winding re-check) is a correctness requirement, not an
/// optimization: two independently-produced pieces can share a boundary vertex that's collinear on ONE
/// piece's ring but a genuine corner on its NEIGHBOR's ring (e.g. HertelMehlhorn merging an L-shaped
/// 6-triangle region into a 2x1 rectangle plus a 1x1 square -- the square's corner sits exactly at the
/// rectangle's top edge's midpoint). Running Polygon2D::Make()'s usual remove_collinear() pass on each
/// piece independently (with no awareness of what's on the other side of a given edge) would silently
/// drop that shared vertex from the rectangle's ring alone, leaving the square's corner touching the
/// middle of the rectangle's edge -- a T-junction that PolyMesh2D::FromPolygons()'s own adjacency
/// validation (assert_adjacency/validate_adjacency) then rejects.
///
/// This function itself still never runs remove_collinear() -- by the time pieces gets here, each ring
/// has ALREADY been through trace_face_group_boundary()'s own collapse_redundant_seams() pass
/// (polygonization2d.cpp), which is neighbor-aware: it only drops a collinear vertex once the output-group
/// identity on both flanking edges' far side matches (both the mesh's own outer boundary, or both the
/// exact same neighboring piece), so a genuinely load-bearing shared corner like the one above survives
/// while a truly redundant one (e.g. the seam between the rectangle's own 2 source triangle-squares, on
/// its OTHER, non-shared edge) collapses away. Running remove_collinear() again here would be redundant at
/// best for a merge()-sourced piece (see merge()'s own doc comment: it has no such per-edge neighbor
/// information to draw on, so it always keeps every traced vertex).
/// @param pieces Every {outer, holes} piece polygonize_impl() or merge()'s own packaging produced.
/// @returns One Polygon2D per input piece, same order.
/// @throws Whatever Polygon2D::FromUniqueCCWPoints() itself throws (e.g. a hole self-intersection) --
/// everything it still checks (holes are pairwise non-crossing, don't cross the outer ring, are actually
/// contained by it) is a genuine geometric relationship this function cannot vouch for on the caller's
/// behalf, unlike winding and collinearity.
std::vector<Polygon2D> polygons_from_pieces(RingPiecesOf<MeshFaceView2D> pieces);

}  // namespace detail

/// @brief Merges a set of (not necessarily adjacency-ordered) triangles into polygons, per
/// @p params.strategy -- see PolygonizationParams for what each strategy guarantees. The free-function
/// equivalent of `Mesh2D::FromTriangles(triangles).Polygonize(params)` for callers who just want polygons
/// without constructing/keeping a full Mesh2D -- welds vertices (detail::GridCellMapForMesh2D, same
/// welding Mesh2D::FromTriangles itself uses) and validates adjacency once (every edge has at most 1
/// neighbor -- untrusted raw input gets this checked here, same as every other *::FromTriangles/batch-
/// triangulate() entry point), then builds adjacency (detail::build_neighbor_refs) and dispatches through
/// detail::polygonize_impl.
/// @param triangles The triangles to polygonize. Order is not required to reflect adjacency.
/// @param params Which polygonization strategy to run -- see PolygonizationParams::Strategy.
/// @returns One Polygon2D per output piece (see each Strategy's own doc for how many, and whether holes
/// are possible).
/// @throws std::invalid_argument if @p triangles is empty, if any edge is shared by more than 2 triangles,
/// or if @p params names an unknown strategy enumerator.
std::vector<Polygon2D> polygonize(std::vector<Triangle2D> const& triangles,
                                  PolygonizationParams const& params = PolygonizationParams{});

/// @brief Welds a set of (not necessarily adjacent) polygons that tile a plane without overlapping into
/// fewer, bigger polygons, by cancelling every edge shared between two of them and tracing what's left.
/// A 2D-native operation (2D has one implicit "plane") -- see the Polygon3D overload for the 3D version,
/// which additionally groups input by plane first.
///
/// Two input polygons touching along a shared OUTER-ring edge merge into one bigger outer boundary.
/// Holes are handled the same way, one level down: every input hole ring is checked against every other
/// for touching (does any of one ring's points lie on the other's perimeter -- an O(h^2) pass, h = total
/// hole count, matching validate_adjacency's own "not a hot loop" complexity acceptance elsewhere in this
/// codebase), and touching holes are unioned into one bigger hole via reverse-winding + Polygon2D::Union()
/// + reverse-winding-back (reusing Union()'s already-correct, already-tested set-union logic rather than
/// reimplementing a second cancel-and-trace pass for holes specifically -- Union() also transparently
/// handles the case of 3+ mutually-touching holes chaining into one, or holes that overlap rather than
/// just touch). A hole untouched by any other hole passes through unchanged. The resulting outer
/// boundaries and (merged or untouched) holes are then grouped into {outer, holes} polygons by
/// containment, exactly like boolean_op()'s own result packaging.
/// @param polygons The polygons to merge. Order doesn't matter; polygons that don't touch anything simply
/// pass through as their own separate output piece.
/// @returns One Polygon2D per disjoint merged region.
/// @throws std::invalid_argument if a group of touching holes doesn't merge into a single Polygon2D::Union
/// result (e.g. 3 holes touching in a way that leaves more than one piece).
std::vector<Polygon2D> merge(std::vector<Polygon2D> const& polygons);

}  // namespace geometry

}  // namespace geompp
