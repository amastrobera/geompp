#pragma once

#include "constants.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"

#include <cstddef>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

class GeometryCollection2D;
class Mesh2D;

/// @brief A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer for efficiency
/// No adjacency structure is stored to find a face's neighbors.
/// Each facet has no holes.
class PolyMesh2D {
 public:
  /// @brief Builds a mesh from a set of (hole-free) polygons, welding vertices that land in the same
  /// spatial grid cell (see @ref GridCell2D) into a single shared vertex.
  /// @param polygons The polygon faces to weld into a mesh. Order is not required to reflect adjacency.
  /// @return A mesh whose vertex count is ≤ the sum of every polygon's `Size()` (fewer once shared
  /// vertices are welded).
  /// @throws std::invalid_argument if @p polygons is empty, or if any polygon has one or more holes.
  /// @note Vertex welding uses `DOUBLE_EPSILON` (which tracks the same thread-local `DECIMAL_PRECISION`
  /// as `AlmostEquals`) but compares points via grid-cell floor-bucketing rather than a direct pairwise
  /// distance check: two points that fall in the same cell are welded even if they're up to
  /// `sqrt(2) * epsilon` apart, and two points within `epsilon` of each other but on opposite sides of a
  /// cell boundary are kept distinct. This trades exactness for O(1) average welding per vertex instead
  /// of an O(n) `AlmostEquals` scan against every prior unique vertex.
  static PolyMesh2D FromPolygons(std::vector<Polygon2D> const& polygons);

  PolyMesh2D(PolyMesh2D const&) = default;
  PolyMesh2D(PolyMesh2D&&) = default;
  ~PolyMesh2D() = default;

  // Declaring the move constructor above suppresses the implicitly-declared copy assignment operator too
  // (not just move assignment) -- without these, `mesh = transform(mesh, m)` would not compile. Both are
  // correct as plain member-wise defaults: all four buffers are shared_ptr, so copy-assignment is just
  // refcount bumps, not a deep copy.
  PolyMesh2D& operator=(PolyMesh2D const&) = default;
  PolyMesh2D& operator=(PolyMesh2D&&) = default;

  std::size_t Size() const;  // returns the number of facets
  double Area() const;       // sum of each input polygon's own Area(), independent of welding

#pragma region Iterators and Accessors

  /// @brief The @p i-th facet, rebuilt as a Polygon2D from the mesh's welded vertices.
  /// @throws std::out_of_range if @p i >= Size().
  Polygon2D operator[](std::size_t i) const;

  /// @brief A lazy view of every facet, each rebuilt as a Polygon2D on demand.
  /// @return A `std::ranges::view` of `Polygon2D`, one per facet, in the same order as the input.
  /// @warning The view borrows `this` — it must not outlive the PolyMesh2D it was obtained from.
  auto Faces() const;  // practically read-only faces (built just before returning)

#pragma endregion

  /// @brief Returns a mesh of triangles instead of n-gons — every facet is triangulated independently
  /// (its own vertices are already simple, CCW-wound, and duplicate-free by construction, since every
  /// PolyMesh2D facet came from a validated Polygon2D, so every TriangulationParams check is skipped)
  /// and the results are combined into one Mesh2D.
  /// @param strategy The triangulation strategy to use. Options:
  ///        - EarClipping: O(n^2) worst case, but simple and robust for small polygons
  ///        - MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition
  ///                           into monotone pieces)
  ///        - Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum
  ///                    angle of all the angles of the triangles in the triangulation (avoiding skinny triangles)
  /// @returns A Mesh2D with sum(facet_vertex_count - 2) triangles across every facet.
  /// @throws whatever the chosen @p strategy itself throws (e.g. std::runtime_error for a
  /// not-yet-implemented strategy).
  Mesh2D Triangulate(
      TriangulationParams::Strategy strategy = TriangulationParams::Strategy::EarClippingBestFit) const;

  /// @brief Every facet, as its own standalone Polygon2D, packaged into one GeometryCollection2D.
  /// @returns A GeometryCollection2D with Size() entries, all Polygon2D, same order as Faces().
  GeometryCollection2D ToGeometryCollection() const;

  /// @brief WKT-like serialization, specific to this library: "POLYMESH ((x0 y0, ..., x0 y0), ...)" --
  /// one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed by
  /// repeating its first point, comma-separated, wrapped once more in "POLYMESH ( ... )". Not a standard
  /// OGC WKT geometry type.
  /// @returns The serialized mesh, one facet ring per facet, in Faces() order.
  std::string ToWkt() const;
  /// @brief Parses a mesh written by ToWkt() (or matching its "POLYMESH (((...)), ...)" grammar) back
  /// into a PolyMesh2D. Every facet must have at least 3 vertices.
  /// @throws std::runtime_error if @p wkt doesn't parse, or any facet has fewer than 3 vertices.
  static PolyMesh2D FromWkt(std::string const& wkt);
  /// @brief Writes ToWkt()'s output to @p path (plain text, truncates any existing content).
  void ToFile(std::string const& path) const;
  /// @brief Reads a file written by ToFile() and parses it via FromWkt().
  /// @throws std::runtime_error if @p path can't be opened or its content doesn't parse.
  static PolyMesh2D FromFile(std::string const& path);

 private:
  // shared_ptr, not plain vector: copying a PolyMesh2D (or handing its vertex buffer to a future
  // Polygonize()/Triangulate() conversion) becomes an O(1) refcount bump instead of an O(n) deep
  // copy. Safe without copy-on-write because PolyMesh2D never exposes a mutable reference to any of
  // these buffers after construction.
  std::shared_ptr<std::vector<Point2D>> VERTICES;              // all points (unique) of the mesh
  std::shared_ptr<std::vector<std::size_t>> FACE_INDICES;      // index list       [0,1,2, 2,3,5,6, 0,1,5,7, ...]
  std::shared_ptr<std::vector<std::size_t>> FACE_IDX_BEGINS;   // beginnings list  [0,     3,       7,       ...]
  std::shared_ptr<std::vector<std::size_t>> FACE_IDX_OFFSETS;  // index count      [3,     4,       4,       ...]
  double AREA;

  // Takes shared_ptr by const&, not by value+move: copying a shared_ptr is just an atomic refcount
  // bump (no vector copy), so there's no expensive-copy case left to avoid with a move overload.
  PolyMesh2D(std::shared_ptr<std::vector<Point2D>> const& unique_vertices,
             std::shared_ptr<std::vector<std::size_t>> const& face_indices,
             std::shared_ptr<std::vector<std::size_t>> const& face_idx_begins,
             std::shared_ptr<std::vector<std::size_t>> const& face_idx_offsets, double area);
};

#pragma region Inlined Functions

inline PolyMesh2D::PolyMesh2D(std::shared_ptr<std::vector<Point2D>> const& unique_vertices,
                              std::shared_ptr<std::vector<std::size_t>> const& face_indices,
                              std::shared_ptr<std::vector<std::size_t>> const& face_idx_begins,
                              std::shared_ptr<std::vector<std::size_t>> const& face_idx_offsets, double area)
    : VERTICES(unique_vertices),
      FACE_INDICES(face_indices),
      FACE_IDX_BEGINS(face_idx_begins),
      FACE_IDX_OFFSETS(face_idx_offsets),
      AREA(area) {}

inline std::size_t PolyMesh2D::Size() const { return FACE_IDX_BEGINS->size(); }
inline double PolyMesh2D::Area() const { return AREA; }

inline auto PolyMesh2D::Faces() const {
  // Generate indices [0, 1, 2, ..., num_faces - 1]
  return std::views::iota(std::size_t{0}, FACE_IDX_BEGINS->size()) | std::views::transform([this](std::size_t i) {
           std::size_t start = (*FACE_IDX_BEGINS)[i];
           std::size_t count = (*FACE_IDX_OFFSETS)[i];

           // 1. Pre-allocate buffer for exact vertex count (zero reallocations)
           std::vector<Point2D> pts;
           pts.reserve(count);

           // 2. Look up point coordinates from the flattened indices
           for (std::size_t k = 0; k < count; ++k) {
             pts.push_back((*VERTICES)[(*FACE_INDICES)[start + k]]);
           }

           // 3. Construct and return the Polygon
           return Polygon2D::Make(pts);
         });
}

#pragma endregion

}  // namespace geometry

}  // namespace geompp
