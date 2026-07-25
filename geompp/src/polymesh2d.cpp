#include "polymesh2d.hpp"

#include "grid_cell2d.hpp"
#include "polygon2d.hpp"

#include <deque>
#include <iterator>
#include <set>
#include <stdexcept>
#include <unordered_map>

namespace geompp {

PolyMesh2D::PolyMesh2D(std::vector<Point2D> unique_vertices, std::vector<std::size_t> face_indices,
                       std::vector<std::size_t> face_idx_begins, std::vector<std::size_t> face_idx_offsets, double area)
    : VERTICES(unique_vertices),
      FACE_INDICES(face_indices),
      FACE_IDX_BEGINS(face_idx_begins),
      FACE_IDX_OFFSETS(face_idx_offsets),
      AREA(area) {}

PolyMesh2D PolyMesh2D::FromPolygons(std::vector<Polygon2D> const& polygons) {
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
  std::unordered_map<GridCell2D, std::size_t, detail::GridCell2DHash> grid_map;
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
    std::size_t n = point_deque.size() - 1;  // 0-based index of the element just pushed
    grid_map[gc] = n;
    return n;
  };

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
      std::size_t i = get_gridcell_hash_index(p);
      face_indices_deque.push_back(i);
    }
  }

  // transform the deque in vector (1 allocation, using move)
  std::vector<Point2D> unique_vertices(std::make_move_iterator(point_deque.begin()),
                                       std::make_move_iterator(point_deque.end()));

  std::vector<std::size_t> face_indices(std::make_move_iterator(face_indices_deque.begin()),
                                        std::make_move_iterator(face_indices_deque.end()));

  // compute and save area
  double area = 0;
  for (auto const& t : polygons) {
    area += t.Area();
  }

  return {std::move(unique_vertices), std::move(face_indices), std::move(face_idx_begins), std::move(face_idx_offsets),
          area};
}

Polygon2D PolyMesh2D::operator[](std::size_t i) const {
  if (i >= FACE_IDX_BEGINS.size()) {
    throw std::out_of_range("index out of FACE_IDX_BEGINS list");
  }
  std::size_t f_idx_begin = FACE_IDX_BEGINS[i];    // where the polygon starts
  std::size_t f_idx_offset = FACE_IDX_OFFSETS[i];  // how many points it has

  std::vector<Point2D> vertices;
  vertices.reserve(f_idx_offset);  // number of vertices per polygon
  for (std::size_t i = 0; i < f_idx_offset; ++i) {
    std::size_t v_idx = FACE_INDICES[f_idx_begin + i];
    vertices.emplace_back(VERTICES[v_idx]);
  }

  return Polygon2D::Make(vertices);
}

}  // namespace geompp
