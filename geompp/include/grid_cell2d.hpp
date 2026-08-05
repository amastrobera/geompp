#pragma once

#include "constants.hpp"
#include "utils.hpp"

#include <array>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

namespace geompp {

class Point2D;
class Triangle2D;
class Polygon2D;

/// @brief Axis-aligned spatial hash bucket: the integer grid cell a Point2D falls into at a given
/// resolution. Two points map to the same GridCell2D iff `floor(x / epsilon)` and `floor(y / epsilon)`
/// both match — an O(1) approximate-equality test used to weld near-duplicate mesh vertices. `epsilon`
/// defaults to `DOUBLE_EPSILON`, which tracks the same thread-local `DECIMAL_PRECISION` as
/// `AlmostEquals`, but grid-cell bucketing is a different comparison *algorithm* than a direct pairwise
/// distance check (see @ref Mesh2D::FromTriangles for the boundary-case tradeoff this implies).
struct GridCell2D {
  std::int64_t x, y;

  /// @brief Buckets a point into its grid cell at the given resolution.
  /// @param epsilon The cell side length. Points within `epsilon` of each other are not guaranteed to
  /// land in the same cell if they straddle a cell boundary; points up to `sqrt(2) * epsilon` apart can
  /// land in the same cell if they don't.
  static GridCell2D FromPoint(Point2D const& p, double epsilon = DOUBLE_EPSILON);

  bool operator==(GridCell2D const& other) const;
};

/// @brief Internal helpers backing Mesh2D/PolyMesh2D construction — not part of the public API.
namespace detail {

/// @brief `std::unordered_map` hash functor for @ref GridCell2D.
struct GridCell2DHash {
  std::size_t operator()(GridCell2D const& cell) const noexcept;
};

/// @brief Welds a set of triangles' vertices via @ref GridCell2D bucketing into a unique-vertex list
/// plus one index triple per input triangle. Backs @ref Mesh2D::FromTriangles.
class GridCellMapForMesh2D {
 public:
  /// @throws std::invalid_argument if @p triangles is empty.
  static GridCellMapForMesh2D Make(std::vector<Triangle2D> const& triangles);
  ~GridCellMapForMesh2D() = default;

  /// @brief The vector of unique points registered (shared, not copied).
  std::shared_ptr<std::vector<Point2D>> GetUniques() const;

  /// @brief One vertex-index triple per input triangle, indexing into @ref GetUniques (shared, not copied).
  std::shared_ptr<std::vector<std::array<std::size_t, 3>>> GetFaceIndices() const;

 private:
  std::shared_ptr<std::vector<Point2D>> UNIQUE_POINTS;
  std::shared_ptr<std::vector<std::array<std::size_t, 3>>> FACE_INDICES;

  GridCellMapForMesh2D(std::shared_ptr<std::vector<Point2D>> const& unique_points,
                       std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices);
};

inline GridCellMapForMesh2D::GridCellMapForMesh2D(
    std::shared_ptr<std::vector<Point2D>> const& unique_points,
    std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices)
    : UNIQUE_POINTS(unique_points), FACE_INDICES(face_indices) {}

inline std::shared_ptr<std::vector<Point2D>> GridCellMapForMesh2D::GetUniques() const { return UNIQUE_POINTS; }

inline std::shared_ptr<std::vector<std::array<std::size_t, 3>>> GridCellMapForMesh2D::GetFaceIndices() const {
  return FACE_INDICES;
}

/// @brief Welds a set of polygons' vertices via @ref GridCell2D bucketing into a unique-vertex list
/// plus a flat variable-length face-index buffer (one run of indices per input polygon). Backs
/// @ref PolyMesh2D::FromPolygons.
class GridCellMapForPolyMesh2D {
 public:
  /// @throws std::invalid_argument if @p polygons is empty, or if any polygon has one or more holes.
  static GridCellMapForPolyMesh2D Make(std::vector<Polygon2D> const& polygons);
  ~GridCellMapForPolyMesh2D() = default;

  /// @brief The vector of unique points registered (shared, not copied).
  std::shared_ptr<std::vector<Point2D>> GetUniques() const;

  /// @brief The flat index buffer: every registered polygon's vertex indices, concatenated (shared, not copied).
  std::shared_ptr<std::vector<std::size_t>> GetFaceIndices() const;

  /// @brief Each polygon's starting offset into @ref GetFaceIndices (shared, not copied).
  std::shared_ptr<std::vector<std::size_t>> GetFaceIdxBegins() const;

  /// @brief Each polygon's vertex count, i.e. run length into @ref GetFaceIndices (shared, not copied).
  std::shared_ptr<std::vector<std::size_t>> GetFaceIdxOffsets() const;

 private:
  std::shared_ptr<std::vector<Point2D>> UNIQUE_POINTS;
  std::shared_ptr<std::vector<std::size_t>> FACE_INDICES;
  std::shared_ptr<std::vector<std::size_t>> FACE_IDX_BEGINS;
  std::shared_ptr<std::vector<std::size_t>> FACE_IDX_OFFSETS;

  GridCellMapForPolyMesh2D(std::shared_ptr<std::vector<Point2D>> const& unique_points,
                           std::shared_ptr<std::vector<std::size_t>> const& face_indices,
                           std::shared_ptr<std::vector<std::size_t>> const& face_idx_begins,
                           std::shared_ptr<std::vector<std::size_t>> const& face_idx_offsets);
};

inline GridCellMapForPolyMesh2D::GridCellMapForPolyMesh2D(
    std::shared_ptr<std::vector<Point2D>> const& unique_points,
    std::shared_ptr<std::vector<std::size_t>> const& face_indices,
    std::shared_ptr<std::vector<std::size_t>> const& face_idx_begins,
    std::shared_ptr<std::vector<std::size_t>> const& face_idx_offsets)
    : UNIQUE_POINTS(unique_points),
      FACE_INDICES(face_indices),
      FACE_IDX_BEGINS(face_idx_begins),
      FACE_IDX_OFFSETS(face_idx_offsets) {}

inline std::shared_ptr<std::vector<Point2D>> GridCellMapForPolyMesh2D::GetUniques() const { return UNIQUE_POINTS; }

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForPolyMesh2D::GetFaceIndices() const {
  return FACE_INDICES;
}

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForPolyMesh2D::GetFaceIdxBegins() const {
  return FACE_IDX_BEGINS;
}

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForPolyMesh2D::GetFaceIdxOffsets() const {
  return FACE_IDX_OFFSETS;
}

class GridCellMapForConnectedMesh2D {
 public:
  /// @throws std::invalid_argument if @p triangles is empty.
  static GridCellMapForConnectedMesh2D Make(std::vector<Triangle2D> const& triangles);
  ~GridCellMapForConnectedMesh2D() = default;

  /// @brief Moves out the vector of unique points registered.
  std::shared_ptr<std::vector<Point2D>> GetUniques();

  std::shared_ptr<std::vector<std::size_t>> GetTriangles();

  std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> GetNeighborRefs();

 private:
  std::shared_ptr<std::vector<Point2D>> UNIQUE_POINTS;
  std::shared_ptr<std::vector<std::size_t>> TRIANGLES;
  std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> NEIGHBORS;

  GridCellMapForConnectedMesh2D(
      std::shared_ptr<std::vector<Point2D>> unique_points, std::shared_ptr<std::vector<std::size_t>> triangles,
      std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> neighbors);
};

inline GridCellMapForConnectedMesh2D::GridCellMapForConnectedMesh2D(
    std::shared_ptr<std::vector<Point2D>> unique_points, std::shared_ptr<std::vector<std::size_t>> triangles,
    std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> neighbors)
    : UNIQUE_POINTS(unique_points), TRIANGLES(triangles), NEIGHBORS(neighbors) {}

inline std::shared_ptr<std::vector<Point2D>> GridCellMapForConnectedMesh2D::GetUniques() { return UNIQUE_POINTS; }

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForConnectedMesh2D::GetTriangles() { return TRIANGLES; }

inline std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>>
GridCellMapForConnectedMesh2D::GetNeighborRefs() {
  return NEIGHBORS;
}

}  // namespace detail

}  // namespace geompp
