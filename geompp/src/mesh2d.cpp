#include "mesh2d.hpp"

#include "calc_utils2d.hpp"
#include "connected_mesh2d.hpp"
#include "grid_cell2d.hpp"
#include "polygon2d.hpp"
#include "polymesh2d.hpp"
#include "triangle2d.hpp"

#include <stdexcept>

namespace geompp {

inline namespace geometry {

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

PolyMesh2D Mesh2D::Polygonize(PolygonizationParams const& params) const {
  // FACE_INDICES's std::array<size_t,3> elements are laid out contiguously (std::array's own layout
  // guarantee), so data()->data() is a valid flat size_t[3*n] view with no copy -- see this method's own
  // header doc comment for why that (plus build_neighbor_refs' pure index-hashmap math) makes this cheaper
  // than going through Connect().
  std::size_t n = FACE_INDICES->size();
  auto neighbor_refs = detail::build_neighbor_refs(FACE_INDICES->data()->data(), n);

  std::vector<detail::MeshFaceView2D> faces;
  faces.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    faces.emplace_back(VERTICES->data(), FACE_INDICES->data()->data(), neighbor_refs.data(), i);
  }

  auto pieces = detail::polygonize_impl(faces, params);
  auto polygons = detail::polygons_from_pieces(std::move(pieces));
  return PolyMesh2D::FromPolygons(polygons);
}

}  // namespace geometry

}  // namespace geompp
