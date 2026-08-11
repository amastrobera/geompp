#include "connected_mesh3d.hpp"

#include "calc_utils2d.hpp"
#include "grid_cell3d.hpp"
#include "triangle3d.hpp"

#include <stdexcept>
#include <utility>

namespace geompp {

ConnectedMesh3D ConnectedMesh3D::FromTriangles(std::vector<Triangle3D> const& triangles) {
  // Every edge must have at most 1 neighbor (no T-junction, no edge shared by 3+ facets) -- bad
  // adjacency is treated as invalid caller input here, never silently repaired.
  detail::assert_adjacency(validate_adjacency(triangles));

  // GridCellMapForConnectedMesh3D::Make() throws std::invalid_argument if triangles is empty.
  auto mesh_maker = detail::GridCellMapForConnectedMesh3D::Make(triangles);

  // compute and save area
  double area = 0;
  for (auto const& t : triangles) {
    area += t.Area();
  }

  return {mesh_maker.GetUniques(), mesh_maker.GetTriangles(), mesh_maker.GetNeighborRefs(), area};
}

ConnectedMesh3D::FaceView3D ConnectedMesh3D::operator[](std::size_t i) const {
  if (i >= TRIANGLES->size() / 3) {
    throw std::out_of_range("index out of TRIANGLES list");
  }
  return ConnectedMesh3D::FaceView3D(this, i);
}

Triangle3D ConnectedMesh3D::FaceView3D::Geometry() const {
  // m_face_id is valid by construction, and all arrays of ConnectedMesh3D are immutable, so there's
  // no need to check for an index-out-of-range problem here.
  std::size_t i0 = (*m_mesh->TRIANGLES)[m_face_id * 3];
  std::size_t i1 = (*m_mesh->TRIANGLES)[m_face_id * 3 + 1];
  std::size_t i2 = (*m_mesh->TRIANGLES)[m_face_id * 3 + 2];
  return Triangle3D::Make((*m_mesh->VERTICES)[i0], (*m_mesh->VERTICES)[i1], (*m_mesh->VERTICES)[i2]);
}

std::optional<ConnectedMesh3D::FaceView3D> ConnectedMesh3D::FaceView3D::Neighbor(
    detail::TriangleCompactNeighborRef::TriangleEdge edge_id) const {
  // TriangleEdge is 0-indexed (FIRST=0, SECOND=1, THIRD=2), matching NEIGHBORS' std::array<T, 3>.
  auto neighbor_ref = (*m_mesh->NEIGHBORS)[m_face_id][static_cast<std::uint32_t>(edge_id)];

  if (neighbor_ref.is_boundary()) {
    return std::nullopt;
  }

  return FaceView3D(m_mesh, neighbor_ref.triangle_id());
}

detail::TriangleCompactNeighborRef::TriangleEdge ConnectedMesh3D::FaceView3D::NeighborEntryEdge(
    detail::TriangleCompactNeighborRef::TriangleEdge edge_id) const {
  // TriangleEdge is 0-indexed (FIRST=0, SECOND=1, THIRD=2), matching NEIGHBORS' std::array<T, 3>.
  auto neighbor_ref = (*m_mesh->NEIGHBORS)[m_face_id][static_cast<std::uint32_t>(edge_id)];

  if (neighbor_ref.is_boundary()) {
    return detail::TriangleCompactNeighborRef::TriangleEdge::INVALID;
  }

  return neighbor_ref.edge_id();
}

}  // namespace geompp
