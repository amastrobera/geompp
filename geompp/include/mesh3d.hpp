#pragma once

#include "point3d.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace geompp {

class Triangle3D;

/// @brief A mesh made of adjacent triangles.
/// No adjacency structure is stored to find a face's neighbors.
class Mesh3D {
 public:
  static Mesh3D FromTriangles(std::vector<Triangle3D> const& triangles);

  Mesh3D(Mesh3D const&) = default;
  Mesh3D(Mesh3D&&) = default;
  ~Mesh3D() = default;

  std::size_t Size() const;  // returns the number of facets
  double Area() const;

  Triangle3D operator[](std::size_t i) const;

 private:
  std::vector<Point3D> VERTICES;
  std::vector<std::array<std::size_t, 3>> FACE_INDICES;
  double AREA;

  Mesh3D(std::vector<Point3D> unique_vertices, std::vector<std::array<std::size_t, 3>> face_indices, double area);
};

#pragma region Inlined Functions

inline std::size_t Mesh3D::Size() const { return FACE_INDICES.size(); }
inline double Mesh3D::Area() const { return AREA; }

#pragma endregion

}  // namespace geompp
