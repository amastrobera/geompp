#pragma once

#include "point2d.hpp"
#include "triangle2d.hpp"

#include <array>
#include <cstddef>
#include <ranges>
#include <vector>

namespace geompp {

/// @brief A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.
/// No adjacency structure is stored to find a face's neighbors.
/// Each facet has no holes (a triangle cannot have one).
class Mesh2D {
 public:
  /// @brief Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid
  /// cell (see @ref GridCell2D) into a single shared vertex.
  /// @param triangles The triangles to weld into a mesh. Order is not required to reflect adjacency.
  /// @return A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).
  /// @throws std::invalid_argument if @p triangles is empty.
  /// @note Vertex welding uses `DOUBLE_EPSILON` (which tracks the same thread-local `DECIMAL_PRECISION`
  /// as `AlmostEquals`) but compares points via grid-cell floor-bucketing rather than a direct pairwise
  /// distance check: two points that fall in the same cell are welded even if they're up to
  /// `sqrt(2) * epsilon` apart, and two points within `epsilon` of each other but on opposite sides of a
  /// cell boundary are kept distinct. This trades exactness for O(1) average welding per vertex instead
  /// of an O(n) `AlmostEquals` scan against every prior unique vertex.
  static Mesh2D FromTriangles(std::vector<Triangle2D> const& triangles);

  Mesh2D(Mesh2D const&) = default;
  Mesh2D(Mesh2D&&) = default;
  ~Mesh2D() = default;

  std::size_t Size() const;  // returns the number of facets
  double Area() const;       // sum of each input triangle's own Area(), independent of welding

#pragma region Iterators and Accessors

  /// @brief The @p i-th facet, rebuilt as a Triangle2D from the mesh's welded vertices.
  /// @throws std::out_of_range if @p i >= Size().
  Triangle2D operator[](std::size_t i) const;

  /// @brief A lazy view of every facet, each rebuilt as a Triangle2D on demand.
  /// @return A `std::ranges::view` of `Triangle2D`, one per facet, in the same order as the input.
  /// @warning The view borrows `this` — it must not outlive the Mesh2D it was obtained from.
  auto Faces() const;  // practically read-only faces (built just before returning)

#pragma endregion

 private:
  std::vector<Point2D> VERTICES;
  std::vector<std::array<std::size_t, 3>> FACE_INDICES;
  double AREA;

  Mesh2D(std::vector<Point2D> unique_vertices, std::vector<std::array<std::size_t, 3>> face_indices, double area);
};

#pragma region Inlined Functions

inline std::size_t Mesh2D::Size() const { return FACE_INDICES.size(); }
inline double Mesh2D::Area() const { return AREA; }

inline auto Mesh2D::Faces() const {
  return FACE_INDICES | std::views::transform([this](const auto& idx) {
           return Triangle2D::Make(VERTICES[idx[0]], VERTICES[idx[1]], VERTICES[idx[2]]);
         });
}

#pragma endregion

}  // namespace geompp
