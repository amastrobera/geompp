#include "grid_cell2d.hpp"

#include "point2d.hpp"
#include "polygon2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include <cassert>
#include <cstdint>
#include <deque>
#include <limits>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace geompp {

inline namespace geometry {

GridCell2D GridCell2D::FromPoint(Point2D const& p, double epsilon) {
  return {static_cast<std::int64_t>(std::floor(p.x() / epsilon)),
          static_cast<std::int64_t>(std::floor(p.y() / epsilon))};
}

bool GridCell2D::operator==(GridCell2D const& other) const { return x == other.x && y == other.y; }

namespace detail {

std::size_t GridCell2DHash::operator()(GridCell2D const& cell) const noexcept {
  // Standard hash combination (e.g., boost::hash_combine style)
  std::size_t h1 = std::hash<std::int64_t>{}(cell.x);
  std::size_t h2 = std::hash<std::int64_t>{}(cell.y);
  return h1 ^ (h2 << 1);
}

namespace {

// function that registers a new point and returns the hash (if old point, returns the known GridCell hash)
// In particular, this polyvalent function:
//    (1) finds a hash for a point
//    (2) saves the point into POINT_DEQUE
//    (3) returns the index of that POINT_DEQUE
std::size_t add_point(Point2D const& p, std::deque<Point2D>& point_deque,
                      std::unordered_map<GridCell2D, std::size_t, GridCell2DHash>& grid_map) {
  auto gc = GridCell2D::FromPoint(p);
  if (auto search = grid_map.find(gc); search != grid_map.end()) {
    return search->second;
  }
  point_deque.push_back(p);
  std::size_t n = point_deque.size() - 1;  // 0-based index of the element just pushed
  grid_map[gc] = n;
  return n;
}
}  // namespace

GridCellMapForMesh2D GridCellMapForMesh2D::Make(std::vector<Triangle2D> const& triangles) {
  std::size_t n_triangles = triangles.size();

  if (n_triangles == 0) {
    throw std::invalid_argument("provided zero triangles to initialize the mesh");
  }

  //  unique points (remove duplicates from triangles) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point2D> point_deque;

  // grid-cell to keep nearby vertices on the same hashkey
  std::unordered_map<GridCell2D, std::size_t, GridCell2DHash> grid_map;

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
  std::vector<Point2D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));

  return {std::make_shared<std::vector<Point2D>>(std::move(unique_vertices)),
          std::make_shared<std::vector<std::array<std::size_t, 3>>>(std::move(face_indices))};
}

GridCellMapForPolyMesh2D GridCellMapForPolyMesh2D::Make(std::vector<Polygon2D> const& polygons) {
  std::size_t n_polygons = polygons.size();

  if (n_polygons == 0) {
    throw std::invalid_argument("provided zero polygons to initialize the mesh");
  }

  for (auto const& poly : polygons) {
    if (poly.HasHoles()) {
      throw std::invalid_argument("PolyMesh2D faces cannot have holes");
    }
  }

  //  unique points (remove duplicates from polygons) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point2D> point_deque;

  // grid-cell to keep nearby vertices on the same hashkey
  std::unordered_map<GridCell2D, std::size_t, GridCell2DHash> grid_map;

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
  std::vector<Point2D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));
  std::vector<std::size_t> face_indices(std::make_move_iterator(face_indices_deque.begin()),
                                        std::make_move_iterator(face_indices_deque.end()));

  return {std::make_shared<std::vector<Point2D>>(std::move(unique_vertices)),
          std::make_shared<std::vector<std::size_t>>(std::move(face_indices)),
          std::make_shared<std::vector<std::size_t>>(std::move(face_idx_begins)),
          std::make_shared<std::vector<std::size_t>>(std::move(face_idx_offsets))};
}

GridCellMapForConnectedMesh2D GridCellMapForConnectedMesh2D::Make(std::vector<Triangle2D> const& triangles) {
  std::size_t n_triangles = triangles.size();

  if (n_triangles == 0) {
    throw std::invalid_argument("provided zero triangles to initialize the mesh");
  }

  //  unique points (remove duplicates from triangles) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point2D> point_deque;
  std::unordered_map<GridCell2D, std::size_t, GridCell2DHash> grid_map;

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

  // Adjacency: per-facet neighbor refs, purely from the already-welded indices above (no re-welding).
  std::vector<std::array<detail::TriangleCompactNeighborRef, 3>> neighbour_refs =
      detail::build_neighbor_refs(triangle_indices.data(), n_triangles);

  // transform the deque in vector (1 allocation, using move)
  std::vector<Point2D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));

  return {std::make_shared<std::vector<Point2D>>(std::move(unique_vertices)),
          std::make_shared<std::vector<std::size_t>>(std::move(triangle_indices)),
          std::make_shared<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>>(neighbour_refs)};
}

}  // namespace detail

}  // namespace geometry

}  // namespace geompp
