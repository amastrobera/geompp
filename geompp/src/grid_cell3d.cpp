#include "grid_cell3d.hpp"

#include "point3d.hpp"
#include "polygon3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <deque>
#include <stdexcept>
#include <unordered_map>

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

/// @brief function that registers a new point and returns the hash (if old point, returns the known GridCell hash)
/// In particular, this polyvalent function:
///    (1) finds a hash for a point
///    (2) saves the point into POINT_DEQUE
///    (3) returns the index of that POINT_DEQUE
std::size_t add_point(Point3D const& p, std::deque<Point3D>& uniques,
                      std::unordered_map<GridCell3D, std::size_t, GridCell3DHash>& cell_map) {
  auto gc = GridCell3D::FromPoint(p);
  if (auto search = cell_map.find(gc); search != cell_map.end()) {
    return search->second;
  }
  uniques.push_back(p);
  std::size_t n = uniques.size() - 1;  // 0-based index of the element just pushed
  cell_map[gc] = n;
  return n;
}

void add_triangle(
    Triangle3D const& t, std::deque<Point3D>& point_deque,
    std::unordered_map<GridCell3D, std::size_t, GridCell3DHash>& grid_map,
    std::vector<std::array<std::size_t, 3>>& face_indices) {
  auto const& [p0, p1, p2] = t.Vertices();
  std::size_t i0 = add_point(p0, point_deque, grid_map);
  std::size_t i1 = add_point(p1, point_deque, grid_map);
  std::size_t i2 = add_point(p2, point_deque, grid_map);

  face_indices.push_back({i0, i1, i2});
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
    add_triangle(t, point_deque, grid_map, face_indices);
  }

  // transform the deque in vector (1 allocation, using move)
  std::vector<Point3D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));

  return {unique_vertices, face_indices};
}

}  // namespace detail

}  // namespace geompp
