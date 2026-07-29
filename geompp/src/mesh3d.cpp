#include "mesh3d.hpp"

#include "grid_cell3d.hpp"
#include "triangle3d.hpp"

#include <stdexcept>

namespace geompp {

Mesh3D Mesh3D::FromTriangles(std::vector<Triangle3D> const& triangles) {
  // GridCellMapForMesh3D::Make() throws std::invalid_argument if triangles is empty.
  auto mesh_maker = detail::GridCellMapForMesh3D::Make(triangles);

  // compute and save area
  double area = 0;
  for (auto const& t : triangles) {
    area += t.Area();
  }

  return {mesh_maker.GetUniques(), mesh_maker.GetFaceIndices(), area};
}

Triangle3D Mesh3D::operator[](std::size_t i) const {
  if (i >= FACE_INDICES->size()) {
    throw std::out_of_range("index out of FACE_INDICES list");
  }
  auto f_idx = (*FACE_INDICES)[i];
  return Triangle3D::Make((*VERTICES)[f_idx[0]], (*VERTICES)[f_idx[1]], (*VERTICES)[f_idx[2]]);
}

}  // namespace geompp
