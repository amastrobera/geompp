#pragma once

#include "point2d.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace geompp {

class Triangle2D;

/// @brief A mesh made of adjacent triangles.
/// No adjacency structure is stored to find a face's neighbors.
class Mesh2D {
 public:
  static Mesh2D FromTriangles(std::vector<Triangle2D> const& triangles);

  Mesh2D(Mesh2D const&) = default;
  Mesh2D(Mesh2D&&) = default;
  ~Mesh2D() = default;

  std::size_t Size() const;  // returns the number of facets
  double Area() const;

  Triangle2D operator[](std::size_t i) const;

 private:
  std::vector<Point2D> VERTICES;
  std::vector<std::array<std::size_t, 3>> FACE_INDICES;
  double AREA;

  Mesh2D(std::vector<Point2D> unique_vertices, std::vector<std::array<std::size_t, 3>> face_indices, double area);
};

#pragma region Inlined Functions

inline std::size_t Mesh2D::Size() const { return FACE_INDICES.size(); }
inline double Mesh2D::Area() const { return AREA; }

#pragma endregion

}  // namespace geompp
