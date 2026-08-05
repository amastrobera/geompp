#include "mesh2d.hpp"

#include "calc_utils2d.hpp"
#include "connected_mesh2d.hpp"
#include "grid_cell2d.hpp"
#include "triangle2d.hpp"

#include <stdexcept>

namespace geompp {

Mesh2D Mesh2D::FromTriangles(std::vector<Triangle2D> const& triangles) {
  // Every edge must have at most 1 neighbor (no T-junction, no edge shared by 3+ facets) -- bad
  // adjacency is treated as invalid caller input here, never silently repaired.
  detail::assert_adjacency(validate_adjacency(triangles));

  // GridCellMapForMesh2D::Make() throws std::invalid_argument if triangles is empty.
  auto mesh_maker = detail::GridCellMapForMesh2D::Make(triangles);

  // compute and save area
  double area = 0;
  for (auto const& t : triangles) {
    area += t.Area();
  }

  return {mesh_maker.GetUniques(), mesh_maker.GetFaceIndices(), area};
}

Triangle2D Mesh2D::operator[](std::size_t i) const {
  if (i >= FACE_INDICES->size()) {
    throw std::out_of_range("index out of FACE_INDICES list");
  }
  auto f_idx = (*FACE_INDICES)[i];
  return Triangle2D::Make((*VERTICES)[f_idx[0]], (*VERTICES)[f_idx[1]], (*VERTICES)[f_idx[2]]);
}

ConnectedMesh2D Mesh2D::Connect() const {
  std::vector<Triangle2D> triangles;
  triangles.reserve(FACE_INDICES->size());
  for (std::size_t i = 0; i < FACE_INDICES->size(); ++i) {
    auto f_idx = (*FACE_INDICES)[i];
    triangles.emplace_back(Triangle2D::Make((*VERTICES)[f_idx[0]], (*VERTICES)[f_idx[1]], (*VERTICES)[f_idx[2]]));
  }
  return ConnectedMesh2D::FromTriangles(triangles);
}

}  // namespace geompp
