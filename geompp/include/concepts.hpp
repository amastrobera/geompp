#pragma once

#include "point2d.hpp"

#include <concepts>
#include <cstddef>
#include <ranges>
#include <string>

namespace geompp {

// clang-format off

// any container that exposes a count and indexed access to LineSegment2D-like elements
template <typename Segments>
concept SegmentList = requires(Segments const& s, std::size_t i) {
  { s.size() } -> std::convertible_to<std::size_t>;
  { s[i].First() } -> std::convertible_to<Point2D>;
  { s[i].Last() } -> std::convertible_to<Point2D>;
};

template <typename T>
concept WktSerializable = requires(const T& obj, const std::string& wkt) {
  { obj.ToWkt() } -> std::convertible_to<std::string>;
  { T::FromWkt(wkt) } -> std::same_as<T>;
};

// A vector type must be able to dot with another vector of the same type, yielding a scalar.
template <typename T>
concept VectorType = requires(T v) {
  { v.Dot(v) } -> std::convertible_to<double>;
};

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



// Uses composition (&&) to inherit all requirements from the Point concept automatically.
template <typename P, typename V>
concept ProjectablePointWith = Point<P> && requires(P const& p, V const& v) {
    { p.ToVector().Dot(v) } -> std::convertible_to<double>;
};

// 3. Integrated Container Concept (Bonus)
// If you need a container where elements are guaranteed to be projectable with V
template <typename T, typename V>
concept ProjectablePointContainerWith =
       std::ranges::random_access_range<T>
    && std::ranges::sized_range<T>
    && ProjectablePointWith<std::ranges::range_value_t<T>, V>;

// clang-format on

}  // namespace geompp
