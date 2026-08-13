#pragma once

#include "point3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <ranges>
#include <vector>

namespace geompp {

inline namespace geometry {

/// @brief A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.
/// Per-facet edge adjacency is precomputed internally (see `detail::TriangleCompactNeighborRef`) and
/// exposed via `FaceView3D::Neighbor()`/`NeighborEntryEdge()`.
/// Each facet has no holes (a triangle cannot have one).
class ConnectedMesh3D {
 public:
  /// @brief Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid
  /// cell (see @ref GridCell3D) into a single shared vertex, and precomputing per-facet edge adjacency.
  /// @param triangles The triangles to weld into a mesh. Order is not required to reflect adjacency.
  /// @return A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).
  /// @throws std::invalid_argument if @p triangles is empty.
  /// @note Vertex welding uses `DOUBLE_EPSILON` (which tracks the same thread-local `DECIMAL_PRECISION`
  /// as `AlmostEquals`) but compares points via grid-cell floor-bucketing rather than a direct pairwise
  /// distance check: two points that fall in the same cell are welded even if they're up to
  /// `sqrt(2) * epsilon` apart, and two points within `epsilon` of each other but on opposite sides of a
  /// cell boundary are kept distinct. This trades exactness for O(1) average welding per vertex instead
  /// of an O(n) `AlmostEquals` scan against every prior unique vertex.
  static ConnectedMesh3D FromTriangles(std::vector<Triangle3D> const& triangles);

  ConnectedMesh3D(ConnectedMesh3D const&) = default;
  ConnectedMesh3D(ConnectedMesh3D&&) = default;
  ~ConnectedMesh3D() = default;

  // Declaring the move constructor above suppresses the implicitly-declared copy assignment operator too
  // (not just move assignment) -- without these, `mesh = transform(mesh, m)` would not compile. Both are
  // correct as plain member-wise defaults: VERTICES/TRIANGLES/NEIGHBORS are shared_ptr, so copy-assignment
  // is just refcount bumps, not a deep copy.
  ConnectedMesh3D& operator=(ConnectedMesh3D const&) = default;
  ConnectedMesh3D& operator=(ConnectedMesh3D&&) = default;

  std::size_t Size() const;  // returns the number of facets
  double Area() const;       // sum of each input triangle's own Area(), independent of welding

#pragma region inner classes

  class FaceView3D {
   public:
    FaceView3D(const ConnectedMesh3D* mesh, std::size_t id) : m_face_id(id), m_mesh(mesh) {}

    /// @brief ID of the facet, corresponding to the location in the array TRIANGLES of the ConnectedMesh
    std::size_t ID() const;

    /// @brief facet geometry, rebuilt as a Triangle3D from the mesh's welded vertices.
    Triangle3D Geometry() const;

    /// @brief Topology navigation across one of this facet's 3 edges, returning another FaceView3D so
    /// calls chain as `face.Neighbor(edge)->Geometry()`.
    /// @param edge_id FIRST/SECOND/THIRD: which of the current face's 3 edges to cross.
    /// @return The adjacent facet, or `std::nullopt` if @p edge_id is a boundary edge (no twin).
    std::optional<FaceView3D> Neighbor(detail::TriangleCompactNeighborRef::TriangleEdge edge_id) const;

    /// @brief Which edge (FIRST/SECOND/THIRD) of the `Neighbor()` facet we entered into, i.e. the local
    /// edge id of the twin edge on the other side of @p edge_id.
    /// @param edge_id FIRST/SECOND/THIRD: which of the current face's 3 edges to cross.
    /// @return `TriangleEdge::INVALID` if @p edge_id is a boundary edge (no twin).
    detail::TriangleCompactNeighborRef::TriangleEdge NeighborEntryEdge(
        detail::TriangleCompactNeighborRef::TriangleEdge edge_id) const;

   private:
    std::size_t m_face_id{detail::TriangleCompactNeighborRef::INVALID};
    const ConnectedMesh3D* m_mesh{nullptr};
  };

#pragma endregion

#pragma region Iterators and Accessors

  /// @brief The @p i-th facet, as a FaceView3D into this mesh.
  /// @throws std::out_of_range if @p i >= Size().
  FaceView3D operator[](std::size_t i) const;

  /// @brief A lazy view of every facet, each rebuilt as a Triangle3D on demand.
  /// @return A `std::ranges::view` of `Triangle3D`, one per facet, in the same order as the input.
  /// @warning The view borrows `this` — it must not outlive the ConnectedMesh3D it was obtained from.
  auto Faces() const;  // practically read-only faces (built just before returning)

#pragma endregion

 private:
  std::shared_ptr<std::vector<Point3D>> VERTICES;

  // Flattened array of triangle vertex indices (3 per triangle)
  // Triangle i has vertices: Triangles[3*i], Triangles[3*i + 1], Triangles[3*i + 2]
  std::shared_ptr<std::vector<std::size_t>> TRIANGLES;

  // One entry per facet; each entry holds the adjacency ref for the facet's 3 local edges.
  std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> NEIGHBORS;

  double AREA;

  ConnectedMesh3D(std::shared_ptr<std::vector<Point3D>> unique_vertices,
                  std::shared_ptr<std::vector<std::size_t>> triangles,
                  std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> neighbors,
                  double area);
};

#pragma region Inlined Functions

inline ConnectedMesh3D::ConnectedMesh3D(
    std::shared_ptr<std::vector<Point3D>> unique_vertices, std::shared_ptr<std::vector<std::size_t>> triangles,
    std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> neighbors, double area)
    : VERTICES(unique_vertices), TRIANGLES(triangles), NEIGHBORS(neighbors), AREA(area) {}

inline std::size_t ConnectedMesh3D::Size() const { return TRIANGLES->size() / 3; }
inline double ConnectedMesh3D::Area() const { return AREA; }

inline auto ConnectedMesh3D::Faces() const {
  std::size_t n = TRIANGLES->size() / 3;
  return std::views::iota(std::size_t{0}, n) |
         std::views::transform([this](std::size_t i) { return ConnectedMesh3D::FaceView3D(this, i); });
}

#pragma region inner classes

inline std::size_t ConnectedMesh3D::FaceView3D::ID() const { return m_face_id; }

// Geometry(), Neighbor() and NeighborEntryEdge() rebuild a triangle / branch on boundary state —
// same weight class as ConnectedMesh3D::operator[], so like it they live in connected_mesh3d.cpp.

#pragma endregion

#pragma endregion

}  // namespace geometry

}  // namespace geompp
