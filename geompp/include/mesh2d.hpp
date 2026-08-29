#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "triangle2d.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class ConnectedMesh2D;
class GeometryCollection2D;
class PolyMesh2D;

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

  // Declaring the move constructor above suppresses the implicitly-declared copy assignment operator too
  // (not just move assignment) -- without these, `mesh = transform(mesh, m)` would not compile. Both are
  // correct as plain member-wise defaults: VERTICES/FACE_INDICES are shared_ptr, so copy-assignment is just
  // two refcount bumps, not a deep copy.
  Mesh2D& operator=(Mesh2D const&) = default;
  Mesh2D& operator=(Mesh2D&&) = default;

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

  ConnectedMesh2D Connect() const;

  /// @brief Merges coplanar, edge-adjacent facets into polygons, per @p params.strategy -- see
  /// PolygonizationParams for what each strategy guarantees (planar boundary extraction, quads, or
  /// Hertel-Mehlhorn convex merging).
  /// @note Deliberately does NOT go through Connect(): Connect() rebuilds every facet into a fresh
  /// Triangle2D and reruns ConnectedMesh2D::FromTriangles' entire pipeline, including a second geometric
  /// grid-cell re-weld of vertices this Mesh2D already welded once at FromTriangles() time, plus an
  /// O(n^2) validate_adjacency pass this Mesh2D never needed (its own FromTriangles already proved the
  /// adjacency invariant, and Mesh2D is immutable, so it can't have become invalid since). Polygonize()
  /// instead builds only the missing piece -- a transient per-facet NEIGHBORS array, via
  /// detail::build_neighbor_refs() on FACE_INDICES directly (already-welded indices in, pure index-hashmap
  /// math, O(n), no geometry) -- and wraps it together with the existing VERTICES/FACE_INDICES buffers,
  /// unchanged, into the same detail::MeshTriangleFaceView2D view ConnectedMesh2D::Polygonize() uses.
  /// @param params Which polygonization strategy to run -- see PolygonizationParams::Strategy.
  /// @returns A PolyMesh2D of the merged polygon facets.
  /// @throws std::invalid_argument if @p params names an unknown strategy enumerator.
  PolyMesh2D Polygonize(PolygonizationParams const& params = PolygonizationParams{}) const;

  /// @brief Every facet, as its own standalone Triangle2D, packaged into one GeometryCollection2D.
  /// @returns A GeometryCollection2D with Size() entries, all Triangle2D, same order as Faces().
  GeometryCollection2D ToGeometryCollection() const;

  /// @brief WKT-like serialization, specific to this library: "MESH ((x0 y0, x1 y1, x2 y2, x0 y0), ...)"
  /// -- one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed
  /// by repeating its first point, comma-separated, wrapped once more in "MESH ( ... )". Not a standard
  /// OGC WKT geometry type.
  /// @returns The serialized mesh, one triangle ring per facet, in Faces() order.
  std::string ToWkt() const;
  /// @brief Parses a mesh written by ToWkt() (or matching its "MESH (((...)), ...)" grammar) back into a
  /// Mesh2D. Every facet must have exactly 3 vertices (a mesh facet is always a triangle).
  /// @throws std::runtime_error if @p wkt doesn't parse, or any facet doesn't have exactly 3 vertices.
  static Mesh2D FromWkt(std::string const& wkt);
  /// @brief Writes ToWkt()'s output to @p path (plain text, truncates any existing content).
  void ToFile(std::string const& path) const;
  /// @brief Reads a file written by ToFile() and parses it via FromWkt().
  /// @throws std::runtime_error if @p path can't be opened or its content doesn't parse.
  static Mesh2D FromFile(std::string const& path);

 private:
  // shared_ptr, not plain vector: copying a Mesh2D (or handing its vertex buffer to a future
  // Polygonize()/Triangulate() conversion) becomes an O(1) refcount bump instead of an O(n) deep
  // copy. Safe without copy-on-write because Mesh2D never exposes a mutable reference to either
  // buffer after construction.
  std::shared_ptr<std::vector<Point2D>> VERTICES;
  std::shared_ptr<std::vector<std::array<std::size_t, 3>>> FACE_INDICES;
  double AREA;

  // Takes shared_ptr by const&, not by value+move: copying a shared_ptr is just an atomic refcount
  // bump (no vector copy), so there's no expensive-copy case left to avoid with a move overload.
  Mesh2D(std::shared_ptr<std::vector<Point2D>> const& unique_vertices,
         std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices, double area);

  // ConnectedMesh2D::Disconnect() reaches this private ctor to build its result directly: VERTICES is
  // reused as-is (O(1) refcount bump, no re-weld -- ConnectedMesh2D already welded it), and AREA carries
  // over unchanged (dropping NEIGHBORS doesn't change any geometry), so the only real work Disconnect()
  // does is repacking TRIANGLES' flat std::vector<size_t> into FACE_INDICES' std::vector<array<size_t,3>>.
  friend class ConnectedMesh2D;
};

#pragma region Inlined Functions

inline Mesh2D::Mesh2D(std::shared_ptr<std::vector<Point2D>> const& unique_vertices,
                      std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices, double area)
    : VERTICES(unique_vertices), FACE_INDICES(face_indices), AREA(area) {}

inline std::size_t Mesh2D::Size() const { return FACE_INDICES->size(); }
inline double Mesh2D::Area() const { return AREA; }

inline auto Mesh2D::Faces() const {
  return *FACE_INDICES | std::views::transform([this](const auto& idx) {
    return Triangle2D::Make((*VERTICES)[idx[0]], (*VERTICES)[idx[1]], (*VERTICES)[idx[2]]);
  });
}

#pragma endregion

}  // namespace geometry

}  // namespace geompp
