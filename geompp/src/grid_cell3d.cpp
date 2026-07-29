#include "grid_cell3d.hpp"

#include "point3d.hpp"
#include "polygon3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <cassert>
#include <deque>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace geompp {

GridCell3D GridCell3D::FromPoint(Point3D const& p, double epsilon) {
  return {static_cast<std::int64_t>(std::floor(p.x() / epsilon)),
          static_cast<std::int64_t>(std::floor(p.y() / epsilon)),
          static_cast<std::int64_t>(std::floor(p.z() / epsilon))};
}

bool GridCell3D::operator==(GridCell3D const& other) const { return x == other.x && y == other.y && z == other.z; }

namespace detail {

std::size_t GridCell3DHash::operator()(GridCell3D const& cell) const noexcept {
  // Standard hash combination (e.g., boost::hash_combine style)
  std::size_t h1 = std::hash<std::int64_t>{}(cell.x);
  std::size_t h2 = std::hash<std::int64_t>{}(cell.y);
  std::size_t h3 = std::hash<std::int64_t>{}(cell.z);
  return h1 ^ (h2 << 1) ^ (h3 << 2);
}

namespace {

// function that registers a new point and returns the hash (if old point, returns the known GridCell hash)
// In particular, this polyvalent function:
//    (1) finds a hash for a point
//    (2) saves the point into POINT_DEQUE
//    (3) returns the index of that POINT_DEQUE
std::size_t add_point(Point3D const& p, std::deque<Point3D>& point_deque,
                      std::unordered_map<GridCell3D, std::size_t, GridCell3DHash>& grid_map) {
  auto gc = GridCell3D::FromPoint(p);
  if (auto search = grid_map.find(gc); search != grid_map.end()) {
    return search->second;
  }
  point_deque.push_back(p);
  std::size_t n = point_deque.size() - 1;  // 0-based index of the element just pushed
  grid_map[gc] = n;
  return n;
}
}  // namespace

GridCellMapForMesh3D GridCellMapForMesh3D::Make(std::vector<Triangle3D> const& triangles) {
  std::size_t n_triangles = triangles.size();

  if (n_triangles == 0) {
    throw std::invalid_argument("provided zero triangles to initialize the mesh");
  }

  //  unique points (remove duplicates from triangles) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point3D> point_deque;

  // grid-cell to keep nearby vertices on the same hashkey
  std::unordered_map<GridCell3D, std::size_t, GridCell3DHash> grid_map;

  // arrange vertices into face indices (get indices from the hashmap above)
  std::vector<std::array<std::size_t, 3>> face_indices;
  face_indices.reserve(n_triangles);

  for (auto const& t : triangles) {
    auto const& [p0, p1, p2] = t.Vertices();
    std::size_t i0 = add_point(p0, point_deque, grid_map);
    std::size_t i1 = add_point(p1, point_deque, grid_map);
    std::size_t i2 = add_point(p2, point_deque, grid_map);

    face_indices.push_back({i0, i1, i2});
  }

  // transform the deque in vector (1 allocation, using move)
  std::vector<Point3D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));

  return {std::make_shared<std::vector<Point3D>>(std::move(unique_vertices)),
          std::make_shared<std::vector<std::array<std::size_t, 3>>>(std::move(face_indices))};
}

GridCellMapForPolyMesh3D GridCellMapForPolyMesh3D::Make(std::vector<Polygon3D> const& polygons) {
  std::size_t n_polygons = polygons.size();

  if (n_polygons == 0) {
    throw std::invalid_argument("provided zero polygons to initialize the mesh");
  }

  for (auto const& poly : polygons) {
    if (poly.HasHoles()) {
      throw std::invalid_argument("PolyMesh3D faces cannot have holes");
    }
  }

  //  unique points (remove duplicates from polygons) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point3D> point_deque;

  // grid-cell to keep nearby vertices on the same hashkey
  std::unordered_map<GridCell3D, std::size_t, GridCell3DHash> grid_map;

  // arrange vertices into face indices (get indices from the hashmap above)
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<std::size_t> face_indices_deque;

  std::vector<std::size_t> face_idx_begins;
  face_idx_begins.reserve(n_polygons);
  std::vector<std::size_t> face_idx_offsets;
  face_idx_offsets.reserve(n_polygons);

  for (auto const& poly : polygons) {
    face_idx_begins.push_back(face_indices_deque.size());  // beginning of the face indices
    face_idx_offsets.push_back(poly.Size());               // number of points of this face

    for (auto const& p : poly) {
      std::size_t i = add_point(p, point_deque, grid_map);
      face_indices_deque.push_back(i);
    }
  }

  // transform the deques into vectors (1 allocation each, using move)
  std::vector<Point3D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));
  std::vector<std::size_t> face_indices(std::make_move_iterator(face_indices_deque.begin()),
                                        std::make_move_iterator(face_indices_deque.end()));

  return {std::make_shared<std::vector<Point3D>>(std::move(unique_vertices)),
          std::make_shared<std::vector<std::size_t>>(std::move(face_indices)),
          std::make_shared<std::vector<std::size_t>>(std::move(face_idx_begins)),
          std::make_shared<std::vector<std::size_t>>(std::move(face_idx_offsets))};
}

GridCellMapForConnectedMesh3D GridCellMapForConnectedMesh3D::Make(std::vector<Triangle3D> const& triangles) {
  std::size_t n_triangles = triangles.size();

  if (n_triangles == 0) {
    throw std::invalid_argument("provided zero triangles to initialize the mesh");
  }

  //  unique points (remove duplicates from triangles) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point3D> point_deque;
  std::unordered_map<GridCell3D, std::size_t, GridCell3DHash> grid_map;

  // arrange triangle indices 3 at a time in the same vector of indices
  std::vector<std::size_t> triangle_indices;
  triangle_indices.reserve(n_triangles * 3);

  for (auto const& t : triangles) {
    auto const& [p0, p1, p2] = t.Vertices();
    std::size_t i0 = add_point(p0, point_deque, grid_map);
    std::size_t i1 = add_point(p1, point_deque, grid_map);
    std::size_t i2 = add_point(p2, point_deque, grid_map);

    // fill up the points
    triangle_indices.push_back(i0);
    triangle_indices.push_back(i1);
    triangle_indices.push_back(i2);
  }

  // refs containg the neighbour edge (or twin edge, or other half-edge) for each of the 3 edges of each triangle
  std::vector<std::array<detail::TriangleCompactNeighborRef, 3>> neighbour_refs;
  neighbour_refs.reserve(n_triangles);
  //  \_ safery on numerical limits of this computer
  if (triangle_indices.size() > std::numeric_limits<uint32_t>::max()) {
    throw std::overflow_error("Mesh vertex count exceeds 32-bit limit (4.29B vertices).");
  }
  if (n_triangles >= (1ULL << 30)) {
    throw std::overflow_error("Mesh triangle count exceeds TriangleCompactNeighborRef limit (1.07B triangles).");
  }
  //  \_ init to nothing
  for (auto const& t : triangles) {
    neighbour_refs.push_back(
        {detail::TriangleCompactNeighborRef(-1, detail::TriangleCompactNeighborRef::TriangleEdge::FIRST),
         detail::TriangleCompactNeighborRef(-1, detail::TriangleCompactNeighborRef::TriangleEdge::SECOND),
         detail::TriangleCompactNeighborRef(-1, detail::TriangleCompactNeighborRef::TriangleEdge::THIRD)});
  }
  // \_ get a hashmap to save edges and their twins (based on triangle indices)
  std::unordered_map<std::uint64_t, detail::TriangleCompactNeighborRef> edge_map;
  edge_map.reserve(n_triangles * 3);
  auto make_edge_key = [](std::size_t u, std::size_t v) -> std::uint64_t {
    // Fail fast in Debug mode if vertex indices overflow 32-bit limits
    assert(u <= std::numeric_limits<std::uint32_t>::max() && "Vertex index 'u' exceeds 32-bit limits!");
    assert(v <= std::numeric_limits<std::uint32_t>::max() && "Vertex index 'v' exceeds 32-bit limits!");

    return (static_cast<uint64_t>(static_cast<std::uint32_t>(u)) << 32) | static_cast<std::uint32_t>(v);
  };

  for (std::size_t t = 0; t < n_triangles; ++t) {  // used to pick the specific triangle in the neighbour_refs array

    std::size_t tri_id = t * 3;  // used to pick triangle points, in the triangle_indices array

    for (std::size_t edge_id = 0; edge_id < 3; ++edge_id) {
      std::size_t p_idx1 = tri_id + edge_id;
      std::size_t p_idx2 = tri_id + (edge_id + 1) % 3;

      std::uint32_t u = triangle_indices[p_idx1];
      std::uint32_t v = triangle_indices[p_idx2];

      // TriangleEdge is 1-indexed (FIRST=1, SECOND=2, THIRD=3) so that 0 stays free as an "invalid
      // edge" sentinel distinct from a real edge; neighbour_refs stays 0-indexed to match std::array.
      auto local_edge = static_cast<detail::TriangleCompactNeighborRef::TriangleEdge>(edge_id + 1);

      // Look for the opposite twin edge (v -> u)
      std::uint64_t twin_key = make_edge_key(v, u);
      if (auto search = edge_map.find(twin_key); search != edge_map.end()) {
        //  \_ see if the twice edge exists, and if so set the neighbouring data for both the edge and its twin

        detail::TriangleCompactNeighborRef neighbor_ref = search->second;

        // Wire up both sides of the adjacency link
        neighbour_refs[t][edge_id] = neighbor_ref;
        neighbour_refs[neighbor_ref.triangle_id()][static_cast<size_t>(neighbor_ref.edge_id()) - 1] =
            detail::TriangleCompactNeighborRef(t, local_edge);

      } else {
        //  \_ save the edge, if it doesn't exist
        std::uint64_t my_key = make_edge_key(u, v);
        edge_map[my_key] = detail::TriangleCompactNeighborRef(t, local_edge);
      }
    }
  }

  // transform the deque in vector (1 allocation, using move)
  std::vector<Point3D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));

  return {std::make_shared<std::vector<Point3D>>(std::move(unique_vertices)),
          std::make_shared<std::vector<std::size_t>>(std::move(triangle_indices)),
          std::make_shared<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>>(neighbour_refs)};
}

}  // namespace detail

}  // namespace geompp
