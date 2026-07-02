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
concept PointContainer = 
    std::ranges::random_access_range<T> && 
    std::ranges::sized_range<T>; // contains somethig like size() or empty() -
                                 //   but using preferably std::ranges::empty(c)
                                 //   and std::ranges::size(c) instead

// clang-format on

}  // namespace geompp
