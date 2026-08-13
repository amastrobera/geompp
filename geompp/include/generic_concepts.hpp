#pragma once

#include "point2d.hpp"

#include <concepts>
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

// clang-format on

}  // namespace geometry

}  // namespace geompp
