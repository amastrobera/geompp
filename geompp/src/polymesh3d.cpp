#include "polymesh3d.hpp"

#include "calc_utils3d.hpp"
#include "grid_cell3d.hpp"
#include "mesh3d.hpp"
#include "polygon3d.hpp"
#include "triangle3d.hpp"

#include <iterator>
#include <stdexcept>

namespace geompp {

PolyMesh3D PolyMesh3D::FromPolygons(std::vector<Polygon3D> const& polygons) {
  // Every edge must have at most 1 neighbor (no T-junction, no edge shared by 3+ facets) -- bad
  // adjacency is treated as invalid caller input here, never silently repaired.
  detail::assert_adjacency(validate_adjacency(polygons));

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

Mesh3D PolyMesh3D::Triangulate(TriangulationParams::Strategy strategy) const {
  // VERTICES is already a simple, CCW-wound, duplicate-free point set (guaranteed by how PolyMesh3D is
  // built from valid Polygon3D instances), so every one of triangulate_impl's input-quality checks can be
  // skipped.
  std::size_t n_faces = FACE_IDX_BEGINS->size();

  // Every simple facet triangulates into exactly (vertex_count - 2) triangles, known upfront from
  // FACE_IDX_OFFSETS — reserve once so the outer vector never reallocates/copies triangles already
  // appended by earlier facets as later ones are added.
  std::size_t total_triangles = 0;
  for (std::size_t i = 0; i < n_faces; ++i) {
    total_triangles += (*FACE_IDX_OFFSETS)[i] - 2;
  }

  std::vector<Triangle3D> triangles;
  triangles.reserve(total_triangles);

  for (std::size_t i = 0; i < n_faces; ++i) {
    std::size_t f_idx_begin = (*FACE_IDX_BEGINS)[i];    // where the polygon starts
    std::size_t f_idx_offset = (*FACE_IDX_OFFSETS)[i];  // how many points it has

    std::vector<Point3D> vertices;
    vertices.reserve(f_idx_offset);  // number of vertices per polygon
    for (std::size_t j = 0; j < f_idx_offset; ++j) {
      std::size_t v_idx = (*FACE_INDICES)[f_idx_begin + j];
      vertices.emplace_back((*VERTICES)[v_idx]);
    }

    auto tris = triangulate(vertices, TriangulationParams{strategy, TriangulationParams::Simplicity::Guaranteed,
                                                          TriangulationParams::Winding::Guaranteed,
                                                          TriangulationParams::Collinearity::Guaranteed});
    triangles.insert(triangles.end(), std::make_move_iterator(tris.begin()), std::make_move_iterator(tris.end()));
  }

  return Mesh3D::FromTriangles(triangles);
}

}  // namespace geompp
