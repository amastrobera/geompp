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

template <typename T>
concept Point = requires(std::ranges::range_value_t<T> const& p) {
  { p.x() } -> std::convertible_to<double>;  // element must expose x() and y()
  { p.y() } -> std::convertible_to<double>;
};

template <typename T>
concept PointContainer =
       std::ranges::random_access_range<T>  // operator[](size_t) and iteration
    && std::ranges::sized_range<T>          // size() / empty()
    && requires(std::ranges::range_value_t<T> const& p) {
  { p.x() } -> std::convertible_to<double>;  // element must expose x() and y()
  { p.y() } -> std::convertible_to<double>;
};

// clang-format on

}  // namespace geompp
