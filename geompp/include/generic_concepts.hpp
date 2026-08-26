#pragma once

#include "point2d.hpp"
#include "utils.hpp"

#include <concepts>
#include <optional>
#include <ranges>

namespace geompp {

inline namespace geometry {

// clang-format off

template <typename T>
concept Point = requires(T const& p) {
  { p.x() } -> std::convertible_to<double>;
  { p.y() } -> std::convertible_to<double>;
};

template <typename T>
concept PointContainer =
       std::ranges::random_access_range<T>    // Requires operator[](std::size_t) and iteration
    && std::ranges::sized_range<T>            // Requires size() / empty()
    && Point<std::ranges::range_value_t<T>>;  // Enforces that the element type is a Point

/// @brief A non-owning, cheaply-copyable view over one facet of a welded triangle mesh, exposing its
/// geometry and its precomputed edge-adjacency -- what calc_utils/polygonization2d.hpp's polygonize_impl
/// and its strategy helpers (partition_into_coplanar_clusters, hertel_mehlhorn_polygonization, ...) are
/// templated over. Modeled by detail::MeshFaceView2D/3D; ConnectedMesh2D/3D::Polygonize() and
/// Mesh2D/3D::Polygonize() each build a vector of these directly over their own (already-welded, already
/// index-adjacent) storage -- see calc_utils/polygonization2d.hpp's own docs for why no ConnectedMesh2D
/// object needs to be constructed just to call Polygonize() on a Mesh2D.
template <typename T>
concept TriangleFaceView = requires(T const& f, detail::TriangleCompactNeighborRef::TriangleEdge e) {
  { f.ID() } -> std::convertible_to<std::size_t>;
  f.Geometry();
  { f.Neighbor(e) } -> std::same_as<std::optional<T>>;
};

// clang-format on

}  // namespace geometry

}  // namespace geompp
