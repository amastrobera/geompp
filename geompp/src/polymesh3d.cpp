#include "polymesh3d.hpp"

#include "grid_cell3d.hpp"
#include "polygon3d.hpp"

#include <deque>
#include <iterator>
#include <set>
#include <stdexcept>
#include <unordered_map>

namespace geompp {

PolyMesh3D::PolyMesh3D(std::vector<Point3D> unique_vertices, std::vector<std::size_t> face_indices,
                       std::vector<std::size_t> face_idx_begins, std::vector<std::size_t> face_idx_offsets, double area)
    : VERTICES(unique_vertices),
      FACE_INDICES(face_indices),
      FACE_IDX_BEGINS(face_idx_begins),
      FACE_IDX_OFFSETS(face_idx_offsets),
      AREA(area) {}

PolyMesh3D PolyMesh3D::FromPolygons(std::vector<Polygon3D> const& polygons) {
  std::size_t n_polygons = polygons.size();

  if (n_polygons == 0) {
    throw std::invalid_argument("provided zero polygons to initialize the mesh");
  }

  //  unique points (remove duplicates from polygons) --> use the GridCell and save into a hashmap
  //    \_ using deque to avoid constant dynamic re-allocations as the size doubles
  std::deque<Point3D> point_deque;

  // grid-cell to keep nearby vertices on the same hashkey
  std::unordered_map<GridCell3D, std::size_t, GridCell3DHash> grid_map;
  //   lambda that
  //    (1) finds a hash for a point
  //    (2) saves the point into point_deque
  //    (3) returns the index of that point_deque
  auto get_gridcell_hash_index = [&grid_map, &point_deque](Point3D const& p) -> std::size_t {
    auto gc = GridCell3D::FromPoint(p);
    if (auto search = grid_map.find(gc); search != grid_map.end()) {
      return search->second;
    }
    point_deque.push_back(p);
    std::size_t n = point_deque.size();
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
    std::size_t n_poly = poly.Size();

    face_idx_begins.push_back(face_indices_deque.size());  // beginning of the face indices
    face_idx_offsets.push_back(n_poly);                    // number of points of this face

    for (std::size_t i_p = 0; i_p < n_poly; ++i_p) {
      std::size_t i = get_gridcell_hash_index(poly[i_p]);
      face_indices_deque.push_back(i);
    }
  }

  // transform the deque in vector (1 allocation, using move)
  std::vector<Point3D> unique_vertices(std::make_move_iterator(point_deque.begin()),
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

Polygon3D PolyMesh3D::operator[](std::size_t i) const {
  if (i >= FACE_IDX_BEGINS.size()) {
    throw std::out_of_range("index out of FACE_IDX_BEGINS list");
  }
  std::size_t f_idx_begin = FACE_IDX_BEGINS[i];    // where the polygon starts
  std::size_t f_idx_offset = FACE_IDX_OFFSETS[i];  // how many points it has

  std::vector<Point3D> vertices;
  vertices.reserve(f_idx_offset);  // number of vertices per polygon
  for (std::size_t i = 0; i < f_idx_offset; ++i) {
    std::size_t v_idx = FACE_INDICES[f_idx_begin + i];
    vertices.emplace_back(VERTICES[v_idx]);
  }

  return Polygon3D::Make(vertices);
}

}  // namespace geompp
