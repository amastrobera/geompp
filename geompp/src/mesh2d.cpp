#include "mesh2d.hpp"

#include "grid_cell2d.hpp"
#include "triangle2d.hpp"

#include <stdexcept>

namespace geompp {

Mesh2D::Mesh2D(std::vector<Point2D> unique_vertices, std::vector<std::array<std::size_t, 3>> face_indices, double area)
    : VERTICES(unique_vertices), FACE_INDICES(face_indices), AREA(area) {}

Mesh2D Mesh2D::FromTriangles(std::vector<Triangle2D> const& triangles) {
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
  if (i >= FACE_INDICES.size()) {
    throw std::out_of_range("index out of FACE_INDICES list");
  }
  auto f_idx = FACE_INDICES[i];
  return Triangle2D::Make(VERTICES[f_idx[0]], VERTICES[f_idx[1]], VERTICES[f_idx[2]]);
}

}  // namespace geompp
