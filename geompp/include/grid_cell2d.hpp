#pragma once

#include "constants.hpp"

#include <array>
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
///
/// @note `GetUniques()`/`GetFaceIndices()` move their result out on every call — call each exactly
/// once. This class only ever exists as a same-statement temporary in `Mesh2D::FromTriangles`, so
/// there's no second caller to hand a moved-from vector to.
class GridCellMapForMesh2D {
 public:
  /// @throws std::invalid_argument if @p triangles is empty.
  static GridCellMapForMesh2D Make(std::vector<Triangle2D> const& triangles);
  ~GridCellMapForMesh2D() = default;

  /// @brief Moves out the vector of unique points registered.
  std::vector<Point2D> GetUniques();

  /// @brief Moves out one vertex-index triple per input triangle, indexing into @ref GetUniques.
  std::vector<std::array<std::size_t, 3>> GetFaceIndices();

 private:
  std::vector<Point2D> UNIQUE_POINTS;
  std::vector<std::array<std::size_t, 3>> FACE_INDICES;

  GridCellMapForMesh2D(std::vector<Point2D> unique_points, std::vector<std::array<std::size_t, 3>> face_indices);
};

inline GridCellMapForMesh2D::GridCellMapForMesh2D(std::vector<Point2D> unique_points,
                                                  std::vector<std::array<std::size_t, 3>> face_indices)
    : UNIQUE_POINTS(std::move(unique_points)), FACE_INDICES(std::move(face_indices)) {}

inline std::vector<Point2D> GridCellMapForMesh2D::GetUniques() { return std::move(UNIQUE_POINTS); }

inline std::vector<std::array<std::size_t, 3>> GridCellMapForMesh2D::GetFaceIndices() { return std::move(FACE_INDICES); }

/// @brief Welds a set of polygons' vertices via @ref GridCell2D bucketing into a unique-vertex list
/// plus a flat variable-length face-index buffer (one run of indices per input polygon). Backs
/// @ref PolyMesh2D::FromPolygons.
///
/// @note Accessors move their result out on every call — see @ref GridCellMapForMesh2D's note; the
/// same single-use-temporary contract applies here.
class GridCellMapForPolyMesh2D {
 public:
  /// @throws std::invalid_argument if @p polygons is empty, or if any polygon has one or more holes.
  static GridCellMapForPolyMesh2D Make(std::vector<Polygon2D> const& polygons);
  ~GridCellMapForPolyMesh2D() = default;

  /// @brief Moves out the vector of unique points registered.
  std::vector<Point2D> GetUniques();

  /// @brief Moves out the flat index buffer: every registered polygon's vertex indices, concatenated.
  std::vector<std::size_t> GetFaceIndices();

  /// @brief Moves out each polygon's starting offset into @ref GetFaceIndices.
  std::vector<std::size_t> GetFaceIdxBegins();

  /// @brief Moves out each polygon's vertex count (run length into @ref GetFaceIndices).
  std::vector<std::size_t> GetFaceIdxOffsets();

 private:
  std::vector<Point2D> UNIQUE_POINTS;
  std::vector<std::size_t> FACE_INDICES;
  std::vector<std::size_t> FACE_IDX_BEGINS;
  std::vector<std::size_t> FACE_IDX_OFFSETS;

  GridCellMapForPolyMesh2D(std::vector<Point2D> unique_points, std::vector<std::size_t> face_indices,
                           std::vector<std::size_t> face_idx_begins, std::vector<std::size_t> face_idx_offsets);
};

inline GridCellMapForPolyMesh2D::GridCellMapForPolyMesh2D(std::vector<Point2D> unique_points,
                                                          std::vector<std::size_t> face_indices,
                                                          std::vector<std::size_t> face_idx_begins,
                                                          std::vector<std::size_t> face_idx_offsets)
    : UNIQUE_POINTS(std::move(unique_points)),
      FACE_INDICES(std::move(face_indices)),
      FACE_IDX_BEGINS(std::move(face_idx_begins)),
      FACE_IDX_OFFSETS(std::move(face_idx_offsets)) {}

inline std::vector<Point2D> GridCellMapForPolyMesh2D::GetUniques() { return std::move(UNIQUE_POINTS); }

inline std::vector<std::size_t> GridCellMapForPolyMesh2D::GetFaceIndices() { return std::move(FACE_INDICES); }

inline std::vector<std::size_t> GridCellMapForPolyMesh2D::GetFaceIdxBegins() { return std::move(FACE_IDX_BEGINS); }

inline std::vector<std::size_t> GridCellMapForPolyMesh2D::GetFaceIdxOffsets() { return std::move(FACE_IDX_OFFSETS); }

}  // namespace detail

}  // namespace geompp
