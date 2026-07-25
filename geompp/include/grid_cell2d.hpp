#pragma once

#include "constants.hpp"

#include <array>
#include <tuple>
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

  /// @brief returns the vector of unique points registered
  std::vector<Point2D> GetUniques() const;

  std::vector<std::array<std::size_t, 3>> GetFaceIndices() const;

 private:
  std::vector<Point2D> UNIQUE_POINTS;
  std::vector<std::array<std::size_t, 3>> FACE_INDICES;

  GridCellMapForMesh2D(std::vector<Point2D> const& unique_points,
                       std::vector<std::array<std::size_t, 3>> const& face_indices);
};

inline GridCellMapForMesh2D::GridCellMapForMesh2D(std::vector<Point2D> const& unique_points,
                                                  std::vector<std::array<std::size_t, 3>> const& face_indices)
    : UNIQUE_POINTS(unique_points), FACE_INDICES(face_indices) {}

inline std::vector<Point2D> GridCellMapForMesh2D::GetUniques() const { return UNIQUE_POINTS; }

inline std::vector<std::array<std::size_t, 3>> GridCellMapForMesh2D::GetFaceIndices() const { return FACE_INDICES; }

}  // namespace detail

}  // namespace geompp
