#include "polymesh2d.hpp"

#include "calc_utils2d.hpp"
#include "grid_cell2d.hpp"
#include "mesh2d.hpp"
#include "polygon2d.hpp"
#include "triangle2d.hpp"

#include <iterator>
#include <stdexcept>

namespace geompp {

inline namespace geometry {

PolyMesh2D PolyMesh2D::FromPolygons(std::vector<Polygon2D> const& polygons) {
  // Every edge must have at most 1 neighbor (no T-junction, no edge shared by 3+ facets) -- bad
  // adjacency is treated as invalid caller input here, never silently repaired.
  detail::assert_adjacency(validate_adjacency(polygons));

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
  if (i >= FACE_IDX_BEGINS->size()) {
    throw std::out_of_range("index out of FACE_IDX_BEGINS list");
  }
  std::size_t f_idx_begin = (*FACE_IDX_BEGINS)[i];    // where the polygon starts
  std::size_t f_idx_offset = (*FACE_IDX_OFFSETS)[i];  // how many points it has

  std::vector<Point2D> vertices;
  vertices.reserve(f_idx_offset);  // number of vertices per polygon
  for (std::size_t i = 0; i < f_idx_offset; ++i) {
    std::size_t v_idx = (*FACE_INDICES)[f_idx_begin + i];
    vertices.emplace_back((*VERTICES)[v_idx]);
  }

  // NOT Make(): vertices came straight from FromPolygons()'s own validated, welded storage, and may
  // deliberately still contain a collinear vertex that's load-bearing for a neighboring facet (see
  // Polygon2D's PolyMesh2D friend-grant doc comment). Make()'s remove_collinear() would silently strip it
  // back out on every read, reintroducing the exact T-junction FromPolygons() already proved doesn't exist.
  return Polygon2D::FromUniquePoints(std::move(vertices));
}

Mesh2D PolyMesh2D::Triangulate(TriangulationParams::Strategy strategy) const {
  // VERTICES is already a simple, CCW-wound, duplicate-free point set (guaranteed by how PolyMesh2D is
  // built from valid Polygon2D instances), so every one of triangulate_impl's input-quality checks can be
  // skipped.
  std::size_t n_faces = FACE_IDX_BEGINS->size();

  // Every simple facet triangulates into exactly (vertex_count - 2) triangles, known upfront from
  // FACE_IDX_OFFSETS — reserve once so the outer vector never reallocates/copies triangles already
  // appended by earlier facets as later ones are added.
  std::size_t total_triangles = 0;
  for (std::size_t i = 0; i < n_faces; ++i) {
    total_triangles += (*FACE_IDX_OFFSETS)[i] - 2;
  }

  std::vector<Triangle2D> triangles;
  triangles.reserve(total_triangles);

  for (std::size_t i = 0; i < n_faces; ++i) {
    std::size_t f_idx_begin = (*FACE_IDX_BEGINS)[i];    // where the polygon starts
    std::size_t f_idx_offset = (*FACE_IDX_OFFSETS)[i];  // how many points it has

    std::vector<Point2D> vertices;
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

  return Mesh2D::FromTriangles(triangles);
}

}  // namespace geometry

}  // namespace geompp
