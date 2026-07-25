#include "polymesh2d.hpp"

#include "grid_cell2d.hpp"
#include "polygon2d.hpp"

#include <stdexcept>
#include <utility>

namespace geompp {

PolyMesh2D::PolyMesh2D(std::vector<Point2D> unique_vertices, std::vector<std::size_t> face_indices,
                       std::vector<std::size_t> face_idx_begins, std::vector<std::size_t> face_idx_offsets, double area)
    : VERTICES(std::move(unique_vertices)),
      FACE_INDICES(std::move(face_indices)),
      FACE_IDX_BEGINS(std::move(face_idx_begins)),
      FACE_IDX_OFFSETS(std::move(face_idx_offsets)),
      AREA(area) {}

PolyMesh2D PolyMesh2D::FromPolygons(std::vector<Polygon2D> const& polygons) {
  // GridCellMapForPolyMesh2D::Make() throws std::invalid_argument if polygons is empty or holed.
  auto mesh_maker = detail::GridCellMapForPolyMesh2D::Make(polygons);

  // compute and save area
  double area = 0;
  for (auto const& p : polygons) {
    area += p.Area();
  }

  return {mesh_maker.GetUniques(), mesh_maker.GetFaceIndices(), mesh_maker.GetFaceIdxBegins(),
          mesh_maker.GetFaceIdxOffsets(), area};
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
