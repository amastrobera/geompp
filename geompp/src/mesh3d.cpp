#include "mesh3d.hpp"

#include "calc_utils/polygonization3d.hpp"
#include "calc_utils2d.hpp"
#include "connected_mesh3d.hpp"
#include "grid_cell3d.hpp"
#include "polygon3d.hpp"
#include "polymesh3d.hpp"
#include "triangle3d.hpp"

#include <stdexcept>

namespace geompp {

inline namespace geometry {

Mesh3D Mesh3D::FromTriangles(std::vector<Triangle3D> const& triangles) {
  // Every edge must have at most 1 neighbor (no T-junction, no edge shared by 3+ facets) -- bad
  // adjacency is treated as invalid caller input here, never silently repaired.
  detail::assert_adjacency(validate_adjacency(triangles));

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

ConnectedMesh3D Mesh3D::Connect() const {
  std::vector<Triangle3D> triangles;
  triangles.reserve(FACE_INDICES->size());
  for (std::size_t i = 0; i < FACE_INDICES->size(); ++i) {
    auto f_idx = (*FACE_INDICES)[i];
    triangles.emplace_back(Triangle3D::Make((*VERTICES)[f_idx[0]], (*VERTICES)[f_idx[1]], (*VERTICES)[f_idx[2]]));
  }
  return ConnectedMesh3D::FromTriangles(triangles);
}

PolyMesh3D Mesh3D::Polygonize(PolygonizationParams const& params) const {
  // FACE_INDICES's std::array<size_t,3> elements are laid out contiguously, so data()->data() is a valid
  // flat size_t[3*n] view with no copy -- see this method's own header doc comment / Mesh2D::Polygonize()'s
  // for why that (plus build_neighbor_refs' pure index-hashmap math) makes this cheaper than Connect().
  std::size_t n = FACE_INDICES->size();
  auto neighbor_refs = detail::build_neighbor_refs(FACE_INDICES->data()->data(), n);

  std::vector<detail::MeshFaceView3D> faces;
  faces.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    faces.emplace_back(VERTICES->data(), FACE_INDICES->data()->data(), neighbor_refs.data(), i);
  }

  auto pieces = detail::polygonize_impl(faces, params);
  auto polygons = detail::polygons_from_pieces(std::move(pieces));
  return PolyMesh3D::FromPolygons(polygons);
}

}  // namespace geometry

}  // namespace geompp
