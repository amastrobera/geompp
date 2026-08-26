#include "calc_utils/polygonization2d.hpp"

#include "calc_utils/polygon_queries2d.hpp"
#include "calc_utils/triangulation2d.hpp"
#include "grid_cell2d.hpp"
#include "line_segment2d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <stdexcept>
#include <type_traits>

namespace geompp {

inline namespace geometry {

namespace detail {

Triangle2D MeshFaceView2D::Geometry() const {
  return Triangle2D::Make(m_vertices[VertexIndex(0)], m_vertices[VertexIndex(1)], m_vertices[VertexIndex(2)]);
}

std::optional<MeshFaceView2D> MeshFaceView2D::Neighbor(TriangleCompactNeighborRef::TriangleEdge edge) const {
  auto const& ref = m_neighbor_base[m_face_id][static_cast<std::uint32_t>(edge)];
  if (ref.is_boundary()) {
    return std::nullopt;
  }
  return MeshFaceView2D(m_vertices, m_face_index_base, m_neighbor_base, ref.triangle_id());
}

TriangleCompactNeighborRef::TriangleEdge MeshFaceView2D::NeighborEntryEdge(
    TriangleCompactNeighborRef::TriangleEdge edge) const {
  auto const& ref = m_neighbor_base[m_face_id][static_cast<std::uint32_t>(edge)];
  if (ref.is_boundary()) {
    return TriangleCompactNeighborRef::TriangleEdge::INVALID;
  }
  return ref.edge_id();
}

namespace {

// The 3 local edges of a triangle, in a fixed order -- shared by every walk below.
constexpr std::array<TriangleCompactNeighborRef::TriangleEdge, 3> kLocalEdges = {
    TriangleCompactNeighborRef::TriangleEdge::FIRST, TriangleCompactNeighborRef::TriangleEdge::SECOND,
    TriangleCompactNeighborRef::TriangleEdge::THIRD};

// Rounded-to-DECIMAL_PRECISION directed-edge key, shared by cancel_reverse_pairs() and the seam-collapse
// pass below (collapse_redundant_seams/far_side_group_of_boundary) so both index the exact same key space
// over the exact same edges.
using EdgeVertexKey = std::pair<long long, long long>;
using EdgeKey = std::pair<EdgeVertexKey, EdgeVertexKey>;

EdgeVertexKey edge_vertex_key(Point2D const& p) {
  double scale = std::pow(10.0, static_cast<double>(DECIMAL_PRECISION));
  return {llround(p.x() * scale), llround(p.y() * scale)};
}

EdgeKey edge_key(Point2D const& a, Point2D const& b) { return {edge_vertex_key(a), edge_vertex_key(b)}; }

// Sentinel far-side identity for a boundary edge with no neighbor triangle at all (a genuine mesh-boundary
// edge) -- see far_side_group_of_boundary()'s own doc comment.
constexpr std::size_t kNoNeighborGroup = static_cast<std::size_t>(-1);

// The projection every strategy helper below traces a face group's edges through: identity (View2D::XY())
// for a 2D mesh, or the plane fitted from face_ids[0] for a 3D one. Internal-linkage helper, not part of
// the public detail:: API -- every strategy needs it, but callers outside this file only ever go through
// polygonize_impl.
template <TriangleFaceView FaceViewT>
View2D cluster_view(std::vector<FaceViewT> const& faces, std::vector<std::size_t> const& face_ids) {
  using PointT = FaceViewPointT<FaceViewT>;
  if constexpr (std::is_same_v<PointT, Point3D>) {
    auto const& [p0, p1, p2] = faces[face_ids[0]].Geometry().Vertices();
    return View2D::OnPlane(Plane::From3Points(p0, p1, p2));
  } else {
    return View2D::XY();
  }
}

// For every one of @p face_ids' raw triangle edges (projected through @p view), the OUTPUT-GROUP identity
// of whatever lies immediately across it: kNoNeighborGroup for a true mesh-boundary edge (Neighbor()
// returns nullopt), or @p face_group_id[neighbor face id] otherwise. @p face_group_id must already hold
// each face's FINAL output-group id (uf_find(parent, f) once HertelMehlhorn's merging is fully settled;
// the owning cluster/pair index for the other two strategies, which don't grow groups incrementally) --
// see collapse_redundant_seams()'s own doc for why this can't be a candidate/in-progress id.
//
// An edge shared with a SAME-group neighbor (the kind cancel_reverse_pairs() cancels outright) never needs
// a tag, since it never survives into the traced boundary -- this only needs to be exact for edges that
// DO survive, so a stale/irrelevant entry for a cancelled edge's key is harmless.
template <TriangleFaceView FaceViewT>
std::map<EdgeKey, std::size_t> far_side_group_of_boundary(std::vector<FaceViewT> const& faces, View2D const& view,
                                                           std::vector<std::size_t> const& face_ids,
                                                           std::vector<std::size_t> const& face_group_id) {
  std::map<EdgeKey, std::size_t> far_side;
  for (std::size_t face_id : face_ids) {
    auto const& [p0, p1, p2] = faces[face_id].Geometry().Vertices();
    Point2D a(view.x(p0), view.y(p0));
    Point2D b(view.x(p1), view.y(p1));
    Point2D c(view.x(p2), view.y(p2));
    std::array<std::pair<Point2D, Point2D>, 3> local_edges = {{{a, b}, {b, c}, {c, a}}};
    for (std::size_t i = 0; i < 3; ++i) {
      auto neighbor = faces[face_id].Neighbor(kLocalEdges[i]);
      std::size_t far = neighbor ? face_group_id[neighbor->ID()] : kNoNeighborGroup;
      far_side[edge_key(local_edges[i].first, local_edges[i].second)] = far;
    }
  }
  return far_side;
}

// Drops a boundary vertex between two collinear, SAME-far-side-identity survivor edges (both facing the
// mesh's own outer boundary, or both facing the exact same neighboring output group). A blind
// remove_collinear() pass can't make this distinction -- collinear-on-this-ring-alone doesn't mean
// unneeded: the HertelMehlhorn L-shape example in this file's module doc (and polygonization2d.hpp's
// polygons_from_pieces() doc) has a vertex collinear on the rectangle piece's own ring that's still the
// square piece's one load-bearing shared corner, because the far side changes there (mesh boundary on one
// side, the square's group on the other) even though the two flanking edges are perfectly straight. This
// only collapses the OTHER case: a vertex where nothing's identity actually changes, like the leftover
// seam between two triangle-squares HertelMehlhorn merged into one rectangle -- both flanking edges face
// the mesh's own outer boundary there, so nothing downstream can ever need that vertex.
void collapse_redundant_seams(std::vector<Point2D>& ring, std::map<EdgeKey, std::size_t> const& far_side) {
  std::size_t n = ring.size();
  if (n < 4) {
    return;  // a triangle has no redundant vertex to drop
  }

  auto far_side_of = [&](Point2D const& a, Point2D const& b) -> std::size_t {
    auto it = far_side.find(edge_key(a, b));
    return it != far_side.end() ? it->second : kNoNeighborGroup;
  };

  std::vector<Point2D> kept;
  kept.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    Point2D const& prev = ring[(i + n - 1) % n];
    Point2D const& cur = ring[i];
    Point2D const& next = ring[(i + 1) % n];
    if (are_collinear(prev, cur, next) && far_side_of(prev, cur) == far_side_of(cur, next)) {
      continue;  // redundant: nothing on either side needs a corner here
    }
    kept.push_back(cur);
  }
  if (kept.size() >= 3) {
    ring = std::move(kept);
  }
}

// Gathers every face_id's 3 directed edges (projected through @p view), cancels the ones shared between
// two faces in @p face_ids (cancel_reverse_pairs -- an edge shared with a face NOT in @p face_ids, or with
// no neighbor at all, survives as a genuine boundary), traces the survivors into closed loops
// (trace_directed_boundary), and groups them into {outer, holes} pieces by containment
// (package_result_rings). Stays in the 2D-projected form -- trace_face_group_boundary unprojects it for
// callers that want real output points; is_group_boundary_convex tests it directly, since
// convexity/turn-direction is preserved under View2D's affine-in-plane projection.
//
// @p face_ids need not be @p faces' full coplanar cluster -- boundary_extraction_polygonization calls this
// (via trace_face_group_boundary) with a whole cluster, quad_only_polygonization with just a pair (or a
// single leftover facet), hertel_mehlhorn_polygonization with a candidate (and, once committed, final)
// union-find group. All 3 need exactly this same "cancel this sub-group's internal edges, trace what's
// left" operation, just over a different-sized face_ids list, so it's shared rather than reimplemented.
template <TriangleFaceView TriView>
RingPieces trace_face_group_boundary_2d(std::vector<TriView> const& faces, View2D const& view,
                                        std::vector<std::size_t> const& face_ids) {
  std::vector<LineSegment2D> edges;
  edges.reserve(face_ids.size() * 3);
  for (std::size_t face_id : face_ids) {
    auto const& [p0, p1, p2] = faces[face_id].Geometry().Vertices();
    Point2D a(view.x(p0), view.y(p0));
    Point2D b(view.x(p1), view.y(p1));
    Point2D c(view.x(p2), view.y(p2));
    edges.push_back(LineSegment2D::Make(a, b));
    edges.push_back(LineSegment2D::Make(b, c));
    edges.push_back(LineSegment2D::Make(c, a));
  }

  auto survivors = cancel_reverse_pairs(edges);
  auto raw_rings = trace_directed_boundary(survivors);
  return package_result_rings(raw_rings);
}

// trace_face_group_boundary_2d(), unprojected back to @p faces' own point type (a no-op for a 2D mesh; for
// a 3D mesh, View2D::xyz() per point -- same round-trip Polygon3D::Union()/Simplify() already use).
//
// @p face_group_id, when supplied, must hold every face's FINAL output-group id (see
// far_side_group_of_boundary()'s own doc) -- this runs collapse_redundant_seams() on every ring while
// still in the 2D-projected form, before unprojecting. Left null for a CANDIDATE trace (is_group_boundary_
// convex, mid-merge in hertel_mehlhorn_polygonization) where no group id is settled yet and only raw
// convexity is being tested -- collapsing collinear vertices wouldn't change that answer anyway.
template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> trace_face_group_boundary(std::vector<FaceViewT> const& faces, View2D const& view,
                                                   std::vector<std::size_t> const& face_ids,
                                                   std::vector<std::size_t> const* face_group_id = nullptr) {
  using PointT = FaceViewPointT<FaceViewT>;
  auto pieces_2d = trace_face_group_boundary_2d(faces, view, face_ids);

  if (face_group_id) {
    auto far_side = far_side_group_of_boundary(faces, view, face_ids, *face_group_id);
    for (auto& [outer2d, holes2d] : pieces_2d) {
      collapse_redundant_seams(outer2d, far_side);
      for (auto& hole2d : holes2d) {
        collapse_redundant_seams(hole2d, far_side);
      }
    }
  }

  RingPiecesOf<FaceViewT> result;
  result.reserve(pieces_2d.size());
  for (auto& [outer2d, holes2d] : pieces_2d) {
    std::vector<PointT> outer;
    outer.reserve(outer2d.size());
    for (auto const& p2d : outer2d) {
      if constexpr (std::is_same_v<PointT, Point3D>) {
        outer.push_back(view.xyz(p2d));
      } else {
        outer.push_back(p2d);
      }
    }

    std::vector<std::vector<PointT>> holes;
    holes.reserve(holes2d.size());
    for (auto const& hole2d : holes2d) {
      std::vector<PointT> hole;
      hole.reserve(hole2d.size());
      for (auto const& p2d : hole2d) {
        if constexpr (std::is_same_v<PointT, Point3D>) {
          hole.push_back(view.xyz(p2d));
        } else {
          hole.push_back(p2d);
        }
      }
      holes.push_back(std::move(hole));
    }

    result.push_back({std::move(outer), std::move(holes)});
  }
  return result;
}

// Whether trace_face_group_boundary_2d(faces, view, face_ids) is a single, hole-free, convex region.
// Reuses detail::is_convex (calc_utils/convex_hull2d.hpp) rather than reimplementing the turn-direction
// sweep.
template <TriangleFaceView FaceViewT>
bool is_group_boundary_convex(std::vector<FaceViewT> const& faces, View2D const& view,
                              std::vector<std::size_t> const& face_ids) {
  auto pieces_2d = trace_face_group_boundary_2d(faces, view, face_ids);
  if (pieces_2d.size() != 1 || !pieces_2d[0].second.empty()) {
    return false;  // not a single, hole-free region -- can't be convex
  }
  return is_convex(pieces_2d[0].first, {});
}

}  // namespace

template <TriangleFaceView FaceViewT>
std::vector<std::vector<std::size_t>> partition_into_coplanar_clusters(std::vector<FaceViewT> const& faces) {
  using PointT = FaceViewPointT<FaceViewT>;

  std::size_t n = faces.size();
  std::vector<bool> visited(n, false);
  std::vector<std::vector<std::size_t>> clusters;

  for (std::size_t start = 0; start < n; ++start) {
    if (visited[start]) {
      continue;
    }

    // Reference plane for this cluster (3D only), fixed once from the seed facet and never updated as the
    // walk grows -- see this function's own header doc comment for why that avoids transitive epsilon
    // drift across a long chain of individually-within-epsilon steps.
    std::optional<Plane> cluster_plane;
    if constexpr (std::is_same_v<PointT, Point3D>) {
      auto const& [p0, p1, p2] = faces[start].Geometry().Vertices();
      cluster_plane = Plane::From3Points(p0, p1, p2);
    }

    std::vector<std::size_t> cluster;
    std::vector<std::size_t> queue{start};
    visited[start] = true;

    while (!queue.empty()) {
      std::size_t cur_id = queue.back();
      queue.pop_back();
      cluster.push_back(cur_id);

      FaceViewT const& cur = faces[cur_id];
      for (auto edge : kLocalEdges) {
        auto neighbor = cur.Neighbor(edge);
        if (!neighbor) {
          continue;
        }
        std::size_t nb_id = neighbor->ID();
        if (visited[nb_id]) {
          continue;
        }

        bool coplanar = true;
        if constexpr (std::is_same_v<PointT, Point3D>) {
          auto const& [q0, q1, q2] = neighbor->Geometry().Vertices();
          coplanar = cluster_plane->AlmostEquals(Plane::From3Points(q0, q1, q2));
        }

        if (coplanar) {
          visited[nb_id] = true;
          queue.push_back(nb_id);
        }
      }
    }
    clusters.push_back(std::move(cluster));
  }
  return clusters;
}

template std::vector<std::vector<std::size_t>> partition_into_coplanar_clusters(std::vector<MeshFaceView2D> const&);
template std::vector<std::vector<std::size_t>> partition_into_coplanar_clusters(std::vector<MeshFaceView3D> const&);

std::vector<LineSegment2D> cancel_reverse_pairs(std::vector<LineSegment2D> const& edges) {
  // How many times each directed edge occurs.
  std::map<EdgeKey, int> count;
  for (auto const& e : edges) {
    count[edge_key(e.First(), e.Last())]++;
  }

  // How many (forward, reverse) pairs to cancel per key, computed once from the FIXED counts above (not
  // updated as edges are consumed below) so that e.g. both edges of a simple (A,B)/(B,A) pair correctly
  // see "1 pair to cancel" from their own independent map entry, rather than one edge's processing zeroing
  // out the count the other edge still needs to check.
  std::map<EdgeKey, int> to_cancel;
  for (auto const& [key, c] : count) {
    EdgeKey rev_key = {key.second, key.first};
    auto rev_it = count.find(rev_key);
    if (rev_it != count.end()) {
      to_cancel[key] = std::min(c, rev_it->second);
    }
  }

  std::vector<LineSegment2D> survivors;
  survivors.reserve(edges.size());
  for (auto const& e : edges) {
    auto key = edge_key(e.First(), e.Last());
    auto it = to_cancel.find(key);
    if (it != to_cancel.end() && it->second > 0) {
      --it->second;
      continue;
    }
    survivors.push_back(e);
  }
  return survivors;
}

std::size_t uf_find(std::vector<std::size_t>& parent, std::size_t x) {
  while (parent[x] != x) {
    parent[x] = parent[parent[x]];
    x = parent[x];
  }
  return x;
}

template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> boundary_extraction_polygonization(std::vector<FaceViewT> const& faces,
                                                            std::vector<std::vector<std::size_t>> const& clusters) {
  // A whole cluster IS this strategy's output group, so a face's cluster index already is its final
  // output-group id -- feeds collapse_redundant_seams() via trace_face_group_boundary() below.
  std::vector<std::size_t> face_group_id(faces.size(), kNoNeighborGroup);
  for (std::size_t ci = 0; ci < clusters.size(); ++ci) {
    for (std::size_t fid : clusters[ci]) {
      face_group_id[fid] = ci;
    }
  }

  RingPiecesOf<FaceViewT> result;
  for (auto const& cluster : clusters) {
    if (cluster.empty()) {
      continue;
    }
    auto view = cluster_view(faces, cluster);
    auto pieces = trace_face_group_boundary(faces, view, cluster, &face_group_id);
    result.insert(result.end(), std::make_move_iterator(pieces.begin()), std::make_move_iterator(pieces.end()));
  }
  return result;
}

template RingPiecesOf<MeshFaceView2D> boundary_extraction_polygonization(
    std::vector<MeshFaceView2D> const&, std::vector<std::vector<std::size_t>> const&);
template RingPiecesOf<MeshFaceView3D> boundary_extraction_polygonization(
    std::vector<MeshFaceView3D> const&, std::vector<std::vector<std::size_t>> const&);

template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> hertel_mehlhorn_polygonization(std::vector<FaceViewT> const& faces,
                                                       std::vector<std::vector<std::size_t>> const& clusters) {
  std::size_t n = faces.size();

  std::vector<std::size_t> cluster_id(n, static_cast<std::size_t>(-1));
  for (std::size_t ci = 0; ci < clusters.size(); ++ci) {
    for (std::size_t fid : clusters[ci]) {
      cluster_id[fid] = ci;
    }
  }

  std::vector<std::size_t> parent(n);
  std::iota(parent.begin(), parent.end(), std::size_t{0});

  // Phase 1: greedily dissolve every internal edge that keeps the merged region convex, one cluster at a
  // time. Merges never cross a cluster boundary (guarded below), so every cluster's final union-find state
  // is independent of every other's -- settling ALL of them here, before any tracing starts, is what lets
  // face_group_id below hold each face's TRUE final group even when the face sits in a cluster this loop
  // hasn't reached yet. Interleaving merge-then-trace per cluster (as this used to) would instead have to
  // tag a not-yet-merged neighbor cluster's faces with their premature, still-unmerged ids, which is still
  // SAFE for collapse_redundant_seams() below (a stale tag can only miss a valid collapse, never cause a
  // wrong one -- see that function's own doc) but needlessly conservative.
  for (auto const& cluster : clusters) {
    if (cluster.empty()) {
      continue;
    }
    auto view = cluster_view(faces, cluster);
    for (std::size_t face_id : cluster) {
      for (auto edge : kLocalEdges) {
        auto neighbor = faces[face_id].Neighbor(edge);
        if (!neighbor) {
          continue;
        }
        std::size_t nb_id = neighbor->ID();
        if (nb_id < face_id) {
          continue;  // consider each internal edge once, from its lower-id side
        }
        if (cluster_id[nb_id] != cluster_id[face_id]) {
          continue;  // different (or no) coplanar cluster -- not a candidate for this strategy
        }

        std::size_t ra = uf_find(parent, face_id);
        std::size_t rb = uf_find(parent, nb_id);
        if (ra == rb) {
          continue;  // already merged, e.g. an internal diagonal of a triangulated cycle reached again
        }

        std::vector<std::size_t> candidate;
        candidate.reserve(cluster.size());
        for (std::size_t fid : cluster) {
          std::size_t r = uf_find(parent, fid);
          if (r == ra || r == rb) {
            candidate.push_back(fid);
          }
        }

        if (is_group_boundary_convex(faces, view, candidate)) {
          parent[ra] = rb;
        }
      }
    }
  }

  // Every face's FINAL output-group id, now that every cluster's merges are settled -- feeds
  // collapse_redundant_seams() via trace_face_group_boundary() below.
  std::vector<std::size_t> face_group_id(n);
  for (std::size_t f = 0; f < n; ++f) {
    face_group_id[f] = uf_find(parent, f);
  }

  // Phase 2: package final groups -- one output piece per surviving union-find root per cluster.
  RingPiecesOf<FaceViewT> result;
  for (auto const& cluster : clusters) {
    if (cluster.empty()) {
      continue;
    }
    auto view = cluster_view(faces, cluster);

    std::map<std::size_t, std::vector<std::size_t>> groups;
    for (std::size_t fid : cluster) {
      groups[face_group_id[fid]].push_back(fid);
    }
    for (auto const& [root, members] : groups) {
      auto pieces = trace_face_group_boundary(faces, view, members, &face_group_id);
      result.insert(result.end(), std::make_move_iterator(pieces.begin()), std::make_move_iterator(pieces.end()));
    }
  }

  return result;
}

template RingPiecesOf<MeshFaceView2D> hertel_mehlhorn_polygonization(
    std::vector<MeshFaceView2D> const&, std::vector<std::vector<std::size_t>> const&);
template RingPiecesOf<MeshFaceView3D> hertel_mehlhorn_polygonization(
    std::vector<MeshFaceView3D> const&, std::vector<std::vector<std::size_t>> const&);

template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> quad_only_polygonization(std::vector<FaceViewT> const& faces,
                                                 std::vector<std::vector<std::size_t>> const& clusters) {
  std::size_t n = faces.size();

  std::vector<std::size_t> cluster_id(n, static_cast<std::size_t>(-1));
  for (std::size_t ci = 0; ci < clusters.size(); ++ci) {
    for (std::size_t fid : clusters[ci]) {
      cluster_id[fid] = ci;
    }
  }

  std::vector<bool> paired(n, false);

  // Phase 1: decide every pairing (and leftover) first, settling each face's FINAL output-group id before
  // any tracing starts -- same reason hertel_mehlhorn_polygonization above splits into 2 phases: tracing a
  // group as soon as it's decided would tag a not-yet-visited face's neighbor with a premature id.
  std::vector<std::vector<std::size_t>> groups;
  std::vector<std::size_t> face_group_id(n, kNoNeighborGroup);
  for (auto const& cluster : clusters) {
    if (cluster.empty()) {
      continue;
    }
    for (std::size_t face_id : cluster) {
      if (paired[face_id]) {
        continue;
      }

      std::optional<std::size_t> partner;
      for (auto edge : kLocalEdges) {
        auto neighbor = faces[face_id].Neighbor(edge);
        if (!neighbor) {
          continue;
        }
        std::size_t nb_id = neighbor->ID();
        if (cluster_id[nb_id] != cluster_id[face_id] || paired[nb_id]) {
          continue;
        }
        partner = nb_id;
        break;
      }

      std::size_t group_id = groups.size();
      if (partner) {
        paired[face_id] = true;
        paired[*partner] = true;
        face_group_id[face_id] = group_id;
        face_group_id[*partner] = group_id;
        groups.push_back({face_id, *partner});
      } else {
        paired[face_id] = true;
        face_group_id[face_id] = group_id;
        groups.push_back({face_id});
      }
    }
  }

  // Phase 2: trace every decided group.
  RingPiecesOf<FaceViewT> result;
  for (auto const& group : groups) {
    auto view = cluster_view(faces, group);
    auto pieces = trace_face_group_boundary(faces, view, group, &face_group_id);
    result.insert(result.end(), std::make_move_iterator(pieces.begin()), std::make_move_iterator(pieces.end()));
  }

  return result;
}

template RingPiecesOf<MeshFaceView2D> quad_only_polygonization(std::vector<MeshFaceView2D> const&,
                                                                std::vector<std::vector<std::size_t>> const&);
template RingPiecesOf<MeshFaceView3D> quad_only_polygonization(std::vector<MeshFaceView3D> const&,
                                                                std::vector<std::vector<std::size_t>> const&);

template <TriangleFaceView FaceViewT>
RingPiecesOf<FaceViewT> polygonize_impl(std::vector<FaceViewT> const& faces, PolygonizationParams const& params) {
  auto clusters = partition_into_coplanar_clusters(faces);

  switch (params.strategy) {
    case PolygonizationParams::Strategy::PlanarBoundaryExtraction:
      return boundary_extraction_polygonization(faces, clusters);
    case PolygonizationParams::Strategy::HertelMehlhorn:
      return hertel_mehlhorn_polygonization(faces, clusters);
    case PolygonizationParams::Strategy::PlanarQuads:
      return quad_only_polygonization(faces, clusters);
  }
  throw std::invalid_argument("polygonize: unknown PolygonizationParams::Strategy");
}

template RingPiecesOf<MeshFaceView2D> polygonize_impl(std::vector<MeshFaceView2D> const&,
                                                       PolygonizationParams const&);
template RingPiecesOf<MeshFaceView3D> polygonize_impl(std::vector<MeshFaceView3D> const&,
                                                       PolygonizationParams const&);

std::vector<Polygon2D> polygons_from_pieces(RingPiecesOf<MeshFaceView2D> pieces) {
  std::vector<Polygon2D> result;
  result.reserve(pieces.size());
  for (auto& [outer, holes] : pieces) {
    bool convex = is_convex(outer, holes);
    result.push_back(holes.empty() ? Polygon2D::FromUniqueCCWPoints(std::move(outer), convex)
                                    : Polygon2D::FromUniqueCCWPoints(std::move(outer), std::move(holes), convex));
  }
  return result;
}

}  // namespace detail

std::vector<Polygon2D> polygonize(std::vector<Triangle2D> const& triangles, PolygonizationParams const& params) {
  // Every edge must have at most 1 neighbor -- same adjacency invariant Mesh2D/ConnectedMesh2D::FromTriangles
  // both Assert on untrusted raw input.
  detail::assert_adjacency(validate_adjacency(triangles));

  // GridCellMapForMesh2D::Make() throws std::invalid_argument if triangles is empty. Weld only (no
  // ConnectedMesh2D scaffolding) -- same welding Mesh2D::FromTriangles itself uses.
  auto mesh_maker = detail::GridCellMapForMesh2D::Make(triangles);
  auto vertices = mesh_maker.GetUniques();
  auto face_indices = mesh_maker.GetFaceIndices();

  std::size_t n = face_indices->size();
  auto neighbor_refs = detail::build_neighbor_refs(face_indices->data()->data(), n);

  std::vector<detail::MeshFaceView2D> faces;
  faces.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    faces.emplace_back(vertices->data(), face_indices->data()->data(), neighbor_refs.data(), i);
  }

  auto pieces = detail::polygonize_impl(faces, params);
  return detail::polygons_from_pieces(std::move(pieces));
}

namespace {

// Does any point of @p a lie on @p b's perimeter, or vice versa? Symmetric on purpose -- two rings can
// touch via just one of A's vertices landing mid-edge on B (a T-junction-shaped touch), which only shows
// up when testing that direction.
bool rings_touch(std::vector<Point2D> const& a, std::vector<Point2D> const& b) {
  for (auto const& p : a) {
    if (detail::view::is_on_perimeter(b, std::vector<std::vector<Point2D>>{}, View2D::XY(), p.x(), p.y())) {
      return true;
    }
  }
  for (auto const& p : b) {
    if (detail::view::is_on_perimeter(a, std::vector<std::vector<Point2D>>{}, View2D::XY(), p.x(), p.y())) {
      return true;
    }
  }
  return false;
}

}  // namespace

std::vector<Polygon2D> merge(std::vector<Polygon2D> const& polygons) {
  if (polygons.empty()) {
    return {};
  }

  // 2D has one implicit plane -- no plane-grouping step needed here (contrast the Polygon3D overload).

  // Cancel every OUTER-ring edge shared between two input polygons, trace what's left.
  std::vector<LineSegment2D> outer_edges;
  for (auto const& poly : polygons) {
    auto const& outer = poly.Perimeter();
    std::size_t n = outer.size();
    for (std::size_t i = 0; i < n; ++i) {
      outer_edges.push_back(LineSegment2D::Make(outer[i], outer[(i + 1) % n]));
    }
  }
  auto outer_survivors = detail::cancel_reverse_pairs(outer_edges);
  auto outer_rings = detail::trace_directed_boundary(outer_survivors);

  // Detect touching holes (union-find over an O(h^2) pairwise touch test -- see this function's own
  // header doc comment for why that complexity is acceptable here), then union each touching group.
  std::vector<std::vector<Point2D>> all_holes;
  for (auto const& poly : polygons) {
    for (auto const& hole : poly.Holes()) {
      all_holes.push_back(hole);
    }
  }

  std::size_t nh = all_holes.size();
  std::vector<std::size_t> hole_parent(nh);
  std::iota(hole_parent.begin(), hole_parent.end(), std::size_t{0});

  for (std::size_t i = 0; i < nh; ++i) {
    for (std::size_t j = i + 1; j < nh; ++j) {
      if (!rings_touch(all_holes[i], all_holes[j])) {
        continue;
      }
      std::size_t ri = detail::uf_find(hole_parent, i);
      std::size_t rj = detail::uf_find(hole_parent, j);
      if (ri != rj) {
        hole_parent[ri] = rj;
      }
    }
  }

  std::map<std::size_t, std::vector<std::size_t>> hole_groups;
  for (std::size_t i = 0; i < nh; ++i) {
    hole_groups[detail::uf_find(hole_parent, i)].push_back(i);
  }

  std::vector<std::vector<Point2D>> merged_holes;
  merged_holes.reserve(hole_groups.size());
  for (auto const& [root, members] : hole_groups) {
    if (members.size() == 1) {
      merged_holes.push_back(all_holes[members[0]]);
      continue;
    }

    // Fold the touching group together: reverse each hole's winding (CW -> CCW, treating it as a
    // "positive" region), Union() them pairwise (reused, already-tested set-union logic -- correct for
    // touching or overlapping regions, so this doesn't need its own crossing-detection logic), then
    // reverse the result's winding back (CCW -> CW) -- that's the merged hole.
    std::vector<Point2D> first = all_holes[members[0]];
    std::reverse(first.begin(), first.end());
    Polygon2D acc = Polygon2D::Make(std::move(first));

    for (std::size_t k = 1; k < members.size(); ++k) {
      std::vector<Point2D> next = all_holes[members[k]];
      std::reverse(next.begin(), next.end());
      auto unioned = acc.Union(Polygon2D::Make(std::move(next)));
      if (unioned.size() != 1) {
        throw std::invalid_argument("merge: a group of touching holes did not union into a single region");
      }
      acc = std::move(unioned[0]);
    }

    std::vector<Point2D> merged = acc.Perimeter();
    std::reverse(merged.begin(), merged.end());
    merged_holes.push_back(std::move(merged));
  }

  // Package the traced outer boundaries + (merged or untouched) holes into {outer, holes} polygons by
  // containment -- same job package_result_rings already does for boolean_op()'s own result.
  std::vector<std::vector<Point2D>> all_rings = std::move(outer_rings);
  all_rings.insert(all_rings.end(), std::make_move_iterator(merged_holes.begin()),
                   std::make_move_iterator(merged_holes.end()));
  auto pieces = detail::package_result_rings(all_rings);
  return detail::polygons_from_pieces(std::move(pieces));
}

}  // namespace geometry

}  // namespace geompp
