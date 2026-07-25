#pragma once

#include "constants.hpp"

#include <array>
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

  /// @brief returns the vector of unique points registered
  std::vector<Point3D> GetUniques() const;

  std::vector<std::array<std::size_t, 3>> GetFaceIndices() const;

 private:
  std::vector<Point3D> UNIQUE_POINTS;
  std::vector<std::array<std::size_t, 3>> FACE_INDICES;

  GridCellMapForMesh3D(std::vector<Point3D> const& unique_points,
                       std::vector<std::array<std::size_t, 3>> const& face_indices);
};

inline GridCellMapForMesh3D::GridCellMapForMesh3D(std::vector<Point3D> const& unique_points,
                                                  std::vector<std::array<std::size_t, 3>> const& face_indices)
    : UNIQUE_POINTS(unique_points), FACE_INDICES(face_indices) {}

inline std::vector<Point3D> GridCellMapForMesh3D::GetUniques() const { return UNIQUE_POINTS; }

inline std::vector<std::array<std::size_t, 3>> GridCellMapForMesh3D::GetFaceIndices() const { return FACE_INDICES; }

}  // namespace detail

}  // namespace geompp
