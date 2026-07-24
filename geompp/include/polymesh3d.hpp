#pragma once

#include "point3d.hpp"

#include <cstddef>
#include <vector>

namespace geompp {

class Polygon3D;

/// @brief A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer for efficiency
/// No adjacency structure is stored to find a face's neighbors.
class PolyMesh3D {
 public:
  static PolyMesh3D FromPolygons(std::vector<Polygon3D> const& polygons);

  PolyMesh3D(PolyMesh3D const&) = default;
  PolyMesh3D(PolyMesh3D&&) = default;
  ~PolyMesh3D() = default;

  std::size_t Size() const;  // returns the number of facets
  double Area() const;

  Polygon3D operator[](std::size_t i) const;

 private:
  std::vector<Point3D> VERTICES;              // all points (unique) of the mesh
  std::vector<std::size_t> FACE_INDICES;      // list of points per face, in index [0,1,2, 2,3,5,6, 0,1,5,7, ...]
  std::vector<std::size_t> FACE_IDX_BEGINS;   // list of index beginnings          [0,     3,       7,       ...]
  std::vector<std::size_t> FACE_IDX_OFFSETS;  // number of vertices per polygon    [3,     4,       4,       ...]
  double AREA;

  PolyMesh3D(std::vector<Point3D> unique_vertices, std::vector<std::size_t> face_indices,
             std::vector<std::size_t> face_idx_begins, std::vector<std::size_t> face_idx_offsets, double area);
};

#pragma region Inlined Functions

inline std::size_t PolyMesh3D::Size() const { return FACE_IDX_BEGINS.size(); }
inline double PolyMesh3D::Area() const { return AREA; }

#pragma endregion

}  // namespace geompp
