#pragma once

#include "constants.hpp"
#include "point3d.hpp"
#include "triangle3d.hpp"

#include <array>
#include <cstddef>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class ConnectedMesh3D;
class GeometryCollection3D;
class PolyMesh3D;

/// @brief A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.
/// No adjacency structure is stored to find a face's neighbors.
/// Each facet has no holes (a triangle cannot have one).
class Mesh3D {
 public:
  /// @brief Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid
  /// cell (see @ref GridCell3D) into a single shared vertex.
  /// @param triangles The triangles to weld into a mesh. Order is not required to reflect adjacency.
  /// @return A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).
  /// @throws std::invalid_argument if @p triangles is empty.
  /// @note Vertex welding uses `DOUBLE_EPSILON` (which tracks the same thread-local `DECIMAL_PRECISION`
  /// as `AlmostEquals`) but compares points via grid-cell floor-bucketing rather than a direct pairwise
  /// distance check: two points that fall in the same cell are welded even if they're up to
  /// `sqrt(3) * epsilon` apart, and two points within `epsilon` of each other but on opposite sides of a
  /// cell boundary are kept distinct. This trades exactness for O(1) average welding per vertex instead
  /// of an O(n) `AlmostEquals` scan against every prior unique vertex.
  static Mesh3D FromTriangles(std::vector<Triangle3D> const& triangles);

  Mesh3D(Mesh3D const&) = default;
  Mesh3D(Mesh3D&&) = default;
  ~Mesh3D() = default;

  // Declaring the move constructor above suppresses the implicitly-declared copy assignment operator too
  // (not just move assignment) -- without these, `mesh = transform(mesh, m)` would not compile. Both are
  // correct as plain member-wise defaults: VERTICES/FACE_INDICES are shared_ptr, so copy-assignment is just
  // two refcount bumps, not a deep copy.
  Mesh3D& operator=(Mesh3D const&) = default;
  Mesh3D& operator=(Mesh3D&&) = default;

  std::size_t Size() const;  // returns the number of facets
  double Area() const;       // sum of each input triangle's own Area(), independent of welding

#pragma region Iterators and Accessors

  /// @brief The @p i-th facet, rebuilt as a Triangle3D from the mesh's welded vertices.
  /// @throws std::out_of_range if @p i >= Size().
  Triangle3D operator[](std::size_t i) const;

  /// @brief A lazy view of every facet, each rebuilt as a Triangle3D on demand.
  /// @return A `std::ranges::view` of `Triangle3D`, one per facet, in the same order as the input.
  /// @warning The view borrows `this` — it must not outlive the Mesh3D it was obtained from.
  auto Faces() const;  // practically read-only faces (built just before returning)

#pragma endregion

  ConnectedMesh3D Connect() const;

  /// @brief Merges coplanar, edge-adjacent facets into polygons, per @p params.strategy -- see
  /// PolygonizationParams for what each strategy guarantees.
  /// @note Deliberately does NOT go through Connect() -- see Mesh2D::Polygonize()'s own comment (same
  /// reasoning applies here verbatim). Builds only a transient per-facet NEIGHBORS array via
  /// detail::build_neighbor_refs() on FACE_INDICES directly, and wraps it together with the existing
  /// VERTICES/FACE_INDICES buffers, unchanged, into the same detail::MeshFaceView3D view
  /// ConnectedMesh3D::Polygonize() uses.
  /// @param params Which polygonization strategy to run -- see PolygonizationParams::Strategy.
  /// @returns A PolyMesh3D of the merged polygon facets.
  /// @throws std::invalid_argument if @p params names an unknown strategy enumerator.
  PolyMesh3D Polygonize(PolygonizationParams const& params = PolygonizationParams{}) const;

  /// @brief Every facet, as its own standalone Triangle3D, packaged into one GeometryCollection3D.
  /// @returns A GeometryCollection3D with Size() entries, all Triangle3D, same order as Faces().
  GeometryCollection3D ToGeometryCollection() const;

  /// @brief WKT-like serialization, specific to this library: "MESH ((x0 y0 z0, ..., x0 y0 z0), ...)" --
  /// one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed by
  /// repeating its first point, comma-separated, wrapped once more in "MESH ( ... )". Not a standard OGC
  /// WKT geometry type.
  /// @returns The serialized mesh, one triangle ring per facet, in Faces() order.
  std::string ToWkt() const;
  /// @brief Parses a mesh written by ToWkt() (or matching its "MESH (((...)), ...)" grammar) back into a
  /// Mesh3D. Every facet must have exactly 3 vertices (a mesh facet is always a triangle).
  /// @throws std::runtime_error if @p wkt doesn't parse, or any facet doesn't have exactly 3 vertices.
  static Mesh3D FromWkt(std::string const& wkt);
  /// @brief Writes ToWkt()'s output to @p path (plain text, truncates any existing content).
  void ToFile(std::string const& path) const;
  /// @brief Reads a file written by ToFile() and parses it via FromWkt().
  /// @throws std::runtime_error if @p path can't be opened or its content doesn't parse.
  static Mesh3D FromFile(std::string const& path);

 private:
  // shared_ptr, not plain vector: copying a Mesh3D (or handing its vertex buffer to a future
  // Polygonize()/Triangulate() conversion) becomes an O(1) refcount bump instead of an O(n) deep
  // copy. Safe without copy-on-write because Mesh3D never exposes a mutable reference to either
  // buffer after construction.
  std::shared_ptr<std::vector<Point3D>> VERTICES;
  std::shared_ptr<std::vector<std::array<std::size_t, 3>>> FACE_INDICES;
  double AREA;

  // Takes shared_ptr by const&, not by value+move: copying a shared_ptr is just an atomic refcount
  // bump (no vector copy), so there's no expensive-copy case left to avoid with a move overload.
  Mesh3D(std::shared_ptr<std::vector<Point3D>> const& unique_vertices,
         std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices, double area);

  // Same reason as Mesh2D::ConnectedMesh2D's own friend grant -- see its doc comment.
  friend class ConnectedMesh3D;
};

#pragma region Inlined Functions

inline Mesh3D::Mesh3D(std::shared_ptr<std::vector<Point3D>> const& unique_vertices,
                      std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices, double area)
    : VERTICES(unique_vertices), FACE_INDICES(face_indices), AREA(area) {}

inline std::size_t Mesh3D::Size() const { return FACE_INDICES->size(); }
inline double Mesh3D::Area() const { return AREA; }

inline auto Mesh3D::Faces() const {
  return *FACE_INDICES | std::views::transform([this](const auto& idx) {
    return Triangle3D::Make((*VERTICES)[idx[0]], (*VERTICES)[idx[1]], (*VERTICES)[idx[2]]);
  });
}

#pragma endregion

}  // namespace geometry

}  // namespace geompp
