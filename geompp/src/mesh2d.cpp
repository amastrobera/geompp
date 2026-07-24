#include "mesh2d.hpp"

#include "grid_cell2d.hpp"
#include "triangle2d.hpp"

#include <deque>
#include <iterator>
#include <set>
#include <stdexcept>
#include <unordered_map>

namespace geompp {

Mesh2D::Mesh2D(std::vector<Point2D> unique_vertices, std::vector<std::array<std::size_t, 3>> face_indices, double area)
    : VERTICES(unique_vertices), FACE_INDICES(face_indices), AREA(area) {}

Mesh2D Mesh2D::FromTriangles(std::vector<Triangle2D> const& triangles) {
  std::size_t n_triangles = triangles.size();

  if (n_triangles == 0) {
    throw std::invalid_argument("provided zero triangles to initialize the mesh");
  }

  //  unique points (remove duplicates from triangles) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point2D> point_deque;

  // grid-cell to keep nearby vertices on the same hashkey
  std::unordered_map<GridCell2D, std::size_t, GridCell2DHash> grid_map;
  //   lambda that
  //    (1) finds a hash for a point
  //    (2) saves the point into point_deque
  //    (3) returns the index of that point_deque
  auto get_gridcell_hash_index = [&grid_map, &point_deque](Point2D const& p) -> std::size_t {
    auto gc = GridCell2D::FromPoint(p);
    if (auto search = grid_map.find(gc); search != grid_map.end()) {
      return search->second;
    }
    point_deque.push_back(p);
    std::size_t n = point_deque.size();
    grid_map[gc] = n;
    return n;
  };

  // arrange vertices into face indices (get indices from the hashmap above)
  std::vector<std::array<std::size_t, 3>> face_indices;
  face_indices.reserve(n_triangles);

  for (auto const& t : triangles) {
    auto const& [p0, p1, p2] = t.Vertices();
    std::size_t i0 = get_gridcell_hash_index(p0);
    std::size_t i1 = get_gridcell_hash_index(p1);
    std::size_t i2 = get_gridcell_hash_index(p2);

    face_indices.push_back({i0, i1, i2});
  }

  // transform the deque in vector (1 allocation, using move)
  std::vector<Point2D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));

  // compute and save area
  double area = 0;
  for (auto const& t : triangles) {
    area += t.Area();
  }

  return {std::move(unique_vertices), std::move(face_indices), area};
}

Triangle2D Mesh2D::operator[](std::size_t i) const {
  if (i >= FACE_INDICES.size()) {
    throw std::out_of_range("index out of FACE_INDICES list");
  }
  auto f_idx = FACE_INDICES[i];
  return Triangle2D::Make(VERTICES[f_idx[0]], VERTICES[f_idx[1]], VERTICES[f_idx[2]]);
}

}  // namespace geompp
