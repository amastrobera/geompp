#pragma once

#include "constants.hpp"
#include "utils.hpp"

#include <array>
#include <memory>
#include <tuple>
#include <vector>

namespace geompp {

class Point3D;
class Triangle3D;
class Polygon3D;

/// @brief Axis-aligned spatial hash bucket: the integer grid cell a Point3D falls into at a given
/// resolution. Two points map to the same GridCell3D iff `floor(x / epsilon)`, `floor(y / epsilon)`,
/// and `floor(z / epsilon)` all match — an O(1) approximate-equality test used to weld near-duplicate
/// mesh vertices. `epsilon` defaults to `DOUBLE_EPSILON`, which tracks the same thread-local
/// `DECIMAL_PRECISION` as `AlmostEquals`, but grid-cell bucketing is a different comparison *algorithm*
/// than a direct pairwise distance check (see @ref Mesh3D::FromTriangles for the boundary-case tradeoff
/// this implies).
struct GridCell3D {
  std::int64_t x, y, z;

  /// @brief Buckets a point into its grid cell at the given resolution.
  /// @param epsilon The cell side length. Points within `epsilon` of each other are not guaranteed to
  /// land in the same cell if they straddle a cell boundary; points up to `sqrt(3) * epsilon` apart can
  /// land in the same cell if they don't.
  static GridCell3D FromPoint(Point3D const& p, double epsilon = DOUBLE_EPSILON);

  bool operator==(GridCell3D const& other) const;
};

/// @brief Internal helpers backing Mesh3D/PolyMesh3D construction — not part of the public API.
namespace detail {

/// @brief `std::unordered_map` hash functor for @ref GridCell3D.
struct GridCell3DHash {
  std::size_t operator()(GridCell3D const& cell) const noexcept;
};

/// @brief Welds a set of triangles' vertices via @ref GridCell3D bucketing into a unique-vertex list
/// plus one index triple per input triangle. Backs @ref Mesh3D::FromTriangles.
class GridCellMapForMesh3D {
 public:
  /// @throws std::invalid_argument if @p triangles is empty.
  static GridCellMapForMesh3D Make(std::vector<Triangle3D> const& triangles);
  ~GridCellMapForMesh3D() = default;

  /// @brief The vector of unique points registered (shared, not copied).
  std::shared_ptr<std::vector<Point3D>> GetUniques() const;

  /// @brief One vertex-index triple per input triangle, indexing into @ref GetUniques (shared, not copied).
  std::shared_ptr<std::vector<std::array<std::size_t, 3>>> GetFaceIndices() const;

 private:
  std::shared_ptr<std::vector<Point3D>> UNIQUE_POINTS;
  std::shared_ptr<std::vector<std::array<std::size_t, 3>>> FACE_INDICES;

  GridCellMapForMesh3D(std::shared_ptr<std::vector<Point3D>> const& unique_points,
                       std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices);
};

inline GridCellMapForMesh3D::GridCellMapForMesh3D(
    std::shared_ptr<std::vector<Point3D>> const& unique_points,
    std::shared_ptr<std::vector<std::array<std::size_t, 3>>> const& face_indices)
    : UNIQUE_POINTS(unique_points), FACE_INDICES(face_indices) {}

inline std::shared_ptr<std::vector<Point3D>> GridCellMapForMesh3D::GetUniques() const { return UNIQUE_POINTS; }

inline std::shared_ptr<std::vector<std::array<std::size_t, 3>>> GridCellMapForMesh3D::GetFaceIndices() const {
  return FACE_INDICES;
}

/// @brief Welds a set of polygons' vertices via @ref GridCell3D bucketing into a unique-vertex list
/// plus a flat variable-length face-index buffer (one run of indices per input polygon). Backs
/// @ref PolyMesh3D::FromPolygons.
class GridCellMapForPolyMesh3D {
 public:
  /// @throws std::invalid_argument if @p polygons is empty, or if any polygon has one or more holes.
  static GridCellMapForPolyMesh3D Make(std::vector<Polygon3D> const& polygons);
  ~GridCellMapForPolyMesh3D() = default;

  /// @brief The vector of unique points registered (shared, not copied).
  std::shared_ptr<std::vector<Point3D>> GetUniques() const;

  /// @brief The flat index buffer: every registered polygon's vertex indices, concatenated (shared, not copied).
  std::shared_ptr<std::vector<std::size_t>> GetFaceIndices() const;

  /// @brief Each polygon's starting offset into @ref GetFaceIndices (shared, not copied).
  std::shared_ptr<std::vector<std::size_t>> GetFaceIdxBegins() const;

  /// @brief Each polygon's vertex count, i.e. run length into @ref GetFaceIndices (shared, not copied).
  std::shared_ptr<std::vector<std::size_t>> GetFaceIdxOffsets() const;

 private:
  std::shared_ptr<std::vector<Point3D>> UNIQUE_POINTS;
  std::shared_ptr<std::vector<std::size_t>> FACE_INDICES;
  std::shared_ptr<std::vector<std::size_t>> FACE_IDX_BEGINS;
  std::shared_ptr<std::vector<std::size_t>> FACE_IDX_OFFSETS;

  GridCellMapForPolyMesh3D(std::shared_ptr<std::vector<Point3D>> const& unique_points,
                           std::shared_ptr<std::vector<std::size_t>> const& face_indices,
                           std::shared_ptr<std::vector<std::size_t>> const& face_idx_begins,
                           std::shared_ptr<std::vector<std::size_t>> const& face_idx_offsets);
};

inline GridCellMapForPolyMesh3D::GridCellMapForPolyMesh3D(
    std::shared_ptr<std::vector<Point3D>> const& unique_points,
    std::shared_ptr<std::vector<std::size_t>> const& face_indices,
    std::shared_ptr<std::vector<std::size_t>> const& face_idx_begins,
    std::shared_ptr<std::vector<std::size_t>> const& face_idx_offsets)
    : UNIQUE_POINTS(unique_points),
      FACE_INDICES(face_indices),
      FACE_IDX_BEGINS(face_idx_begins),
      FACE_IDX_OFFSETS(face_idx_offsets) {}

inline std::shared_ptr<std::vector<Point3D>> GridCellMapForPolyMesh3D::GetUniques() const { return UNIQUE_POINTS; }

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForPolyMesh3D::GetFaceIndices() const {
  return FACE_INDICES;
}

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForPolyMesh3D::GetFaceIdxBegins() const {
  return FACE_IDX_BEGINS;
}

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForPolyMesh3D::GetFaceIdxOffsets() const {
  return FACE_IDX_OFFSETS;
}

class GridCellMapForConnectedMesh3D {
 public:
  /// @throws std::invalid_argument if @p triangles is empty.
  static GridCellMapForConnectedMesh3D Make(std::vector<Triangle3D> const& triangles);
  ~GridCellMapForConnectedMesh3D() = default;

  /// @brief Moves out the vector of unique points registered.
  std::shared_ptr<std::vector<Point3D>> GetUniques();

  std::shared_ptr<std::vector<std::size_t>> GetTriangles();

  std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> GetNeighborRefs();

 private:
  std::shared_ptr<std::vector<Point3D>> UNIQUE_POINTS;
  std::shared_ptr<std::vector<std::size_t>> TRIANGLES;
  std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> NEIGHBORS;

  GridCellMapForConnectedMesh3D(
      std::shared_ptr<std::vector<Point3D>> unique_points, std::shared_ptr<std::vector<std::size_t>> triangles,
      std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> neighbors);
};

inline GridCellMapForConnectedMesh3D::GridCellMapForConnectedMesh3D(
    std::shared_ptr<std::vector<Point3D>> unique_points, std::shared_ptr<std::vector<std::size_t>> triangles,
    std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>> neighbors)
    : UNIQUE_POINTS(unique_points), TRIANGLES(triangles), NEIGHBORS(neighbors) {}

inline std::shared_ptr<std::vector<Point3D>> GridCellMapForConnectedMesh3D::GetUniques() { return UNIQUE_POINTS; }

inline std::shared_ptr<std::vector<std::size_t>> GridCellMapForConnectedMesh3D::GetTriangles() { return TRIANGLES; }

inline std::shared_ptr<std::vector<std::array<detail::TriangleCompactNeighborRef, 3>>>
GridCellMapForConnectedMesh3D::GetNeighborRefs() {
  return NEIGHBORS;
}

}  // namespace detail

}  // namespace geompp
