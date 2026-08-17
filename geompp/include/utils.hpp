#pragma once

#include "constants.hpp"

#include <array>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

namespace geompp {

inline namespace geometry {

double round(double x, int decimal_precision = DECIMAL_PRECISION);

std::partial_ordering compare(double a, double b, double epsilon = DOUBLE_EPSILON);

bool is_in_range(double value, double min, double max, double epsilon = DOUBLE_EPSILON);

bool is_greater_or_equal(double value, double threshold, double epsilon = DOUBLE_EPSILON);

int sign(double x);

std::string trim(std::string s);

std::string to_upper(std::string s);

std::vector<double> tokenize_to_doubles(std::string const& str, char delimiter = ' ');

std::vector<std::string> tokenize_string(std::string const& str, char delimiter = ',');

int count_decimal_places(double number);

/// @brief removes duplicates from a sorted vector (duplicates are consecutive)
///        input {0,0,0,1,2,3,4,4,5} --> output: {0,1,2,3,4,5}
/// @param sorted_vec vector of sorted elements
void remove_duplicates(std::vector<double>& sorted_vec, double epsilon = DOUBLE_EPSILON);

/// @brief removes ALL elements that have (consecutve) duplicates and the elements themselves.
///        input {0,0,0,1,2,3,4,4,5} --> output: {1,2,3,5}
/// @param sorted_vec vector of sorted elements
void remove_all_duplicated_elements(std::vector<double>& sorted_vec, double epsilon = DOUBLE_EPSILON);

namespace detail {

// Adjacency table: for each of the 3 local edges of every triangle.
struct TriangleCompactNeighborRef {
  // 0-indexed to prevent bit collision with INVALID (0xFFFFFFFF)
  enum class TriangleEdge : std::uint32_t { FIRST = 0, SECOND = 1, THIRD = 2, INVALID = 3 };

  static constexpr std::uint32_t INVALID = 0xFFFFFFFF;  // Boundary sentinel

  std::uint32_t data = INVALID;  // it contains at once, in only 4 bytes:
  // (1) triangle_id -> ID of adjacent triangle (-1 if boundary edge)
  // (2) edge_id -> Which local edge (0, 1, or 2) in the adjacent triangle

  TriangleCompactNeighborRef() = default;

  // Encode: triangle_id in top 30 bits, edge_id in bottom 2 bits
  TriangleCompactNeighborRef(std::uint32_t tri_id, TriangleEdge local_edge_id);

  /// @brief if true, the class is invalid: it means this is no neighbor, it is a boundary of the triangle mesh
  [[nodiscard]] bool is_boundary() const;

  /// @brief allows to find the triangle in its container (often index of a triangle array)
  [[nodiscard]] std::uint32_t triangle_id() const;

  /// @brief the entering edge of the neighbor triangle (0,1,2 - unless invalid)
  [[nodiscard]] TriangleEdge edge_id() const;
};

/// @brief Builds the per-facet edge-adjacency table for a set of triangles whose vertices are ALREADY
/// welded into shared indices (i.e. two triangles sharing an edge in space also share the same 2 vertex
/// indices) -- purely index math, no geometry, no re-welding. Used by GridCellMapForConnectedMesh2D/3D::Make
/// (which welds raw Triangle2D/3D first, then calls this) and directly by Mesh2D/3D::Polygonize() (whose
/// FACE_INDICES are already welded at FromTriangles() time, so it skips the weld and calls this alone --
/// see Mesh2D::Polygonize()'s own comment for why re-welding via Connect() would be wasteful).
/// @param triangle_indices Flattened, 3-per-triangle vertex indices (triangle t's vertices are
/// triangle_indices[3*t], [3*t+1], [3*t+2]), already welded (shared vertices use the same index). A raw
/// pointer + count, not a std::vector<size_t> const&, specifically so a caller whose own storage is
/// already laid out as 3-contiguous-size_t-per-triangle under a DIFFERENT container type (e.g. Mesh2D/3D's
/// std::vector<std::array<std::size_t,3>> FACE_INDICES -- guaranteed layout-compatible with a flat
/// size_t[3*n] by std::array's own contiguity guarantee) can pass it directly, with zero copy/flatten.
/// @param n_triangles Number of triangles @p triangle_indices describes (i.e. 3*n_triangles indices).
/// @returns One std::array<TriangleCompactNeighborRef, 3> per triangle, FIRST/SECOND/THIRD matching the
/// input's local edge order; a boundary edge (no twin) is left at its default-constructed INVALID state.
/// @throws std::overflow_error if 3*n_triangles exceeds the 32-bit vertex limit, or @p n_triangles exceeds
/// TriangleCompactNeighborRef's 1.07B-triangle limit.
std::vector<std::array<TriangleCompactNeighborRef, 3>> build_neighbor_refs(std::size_t const* triangle_indices,
                                                                           std::size_t n_triangles);

}  // namespace detail

#pragma region Template Implementation

template <typename T>
std::string string_join(std::vector<T> const& items, std::string const& delim = " ") {
  std::ostringstream buf;
  for (int i = 0; i < items.size(); ++i) {
    buf << items[i];
    if (i < items.size() - 1) {
      buf << delim;
    }
  }
  return buf.str();
}

template <typename T>
  requires requires(T t) {
    { t.ToWkt() } -> std::convertible_to<std::string>;
  }
std::string ToWkt(const std::vector<T>& items) {
  std::string out = "GEOMETRYCOLLECTION(";
  for (std::size_t i = 0; i < items.size(); ++i) {
    if (i > 0) {
      out += ", ";
    }
    out += items[i].ToWkt();
  }
  return out + ")";
}

#pragma endregion

#pragma region Inlined functions

namespace detail {
// Encode: triangle_id in top 30 bits, edge_id in bottom 2 bits
inline TriangleCompactNeighborRef::TriangleCompactNeighborRef(std::uint32_t tri_id, TriangleEdge local_edge_id) {
  if (tri_id == INVALID) {
    data = INVALID;
  } else {
    data = (tri_id << 2) | (static_cast<std::uint32_t>(local_edge_id) & 0x3);
  }
}

[[nodiscard]] inline bool TriangleCompactNeighborRef::is_boundary() const { return data == INVALID; }

[[nodiscard]] inline std::uint32_t TriangleCompactNeighborRef::triangle_id() const { return data >> 2; }

[[nodiscard]] inline TriangleCompactNeighborRef::TriangleEdge TriangleCompactNeighborRef::edge_id() const {
  return static_cast<TriangleEdge>(data & 0x3);
}
}  // namespace detail

#pragma endregion

}  // namespace geometry

}  // namespace geompp
