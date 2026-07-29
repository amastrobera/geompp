#include "polymesh3d.hpp"

#include "grid_cell3d.hpp"
#include "polygon3d.hpp"

#include <stdexcept>

namespace geompp {

PolyMesh3D PolyMesh3D::FromPolygons(std::vector<Polygon3D> const& polygons) {
  // GridCellMapForPolyMesh3D::Make() throws std::invalid_argument if polygons is empty or holed.
  auto mesh_maker = detail::GridCellMapForPolyMesh3D::Make(polygons);

  // compute and save area
  double area = 0;
  for (auto const& p : polygons) {
    area += p.Area();
  }

  return {mesh_maker.GetUniques(), mesh_maker.GetFaceIndices(), mesh_maker.GetFaceIdxBegins(),
          mesh_maker.GetFaceIdxOffsets(), area};
}

Polygon3D PolyMesh3D::operator[](std::size_t i) const {
  if (i >= FACE_IDX_BEGINS->size()) {
    throw std::out_of_range("index out of FACE_IDX_BEGINS list");
  }
  std::size_t f_idx_begin = (*FACE_IDX_BEGINS)[i];    // where the polygon starts
  std::size_t f_idx_offset = (*FACE_IDX_OFFSETS)[i];  // how many points it has

  std::vector<Point3D> vertices;
  vertices.reserve(f_idx_offset);  // number of vertices per polygon
  for (std::size_t i = 0; i < f_idx_offset; ++i) {
    std::size_t v_idx = (*FACE_INDICES)[f_idx_begin + i];
    vertices.emplace_back((*VERTICES)[v_idx]);
  }

  return Polygon3D::Make(vertices);
}

}  // namespace geompp
