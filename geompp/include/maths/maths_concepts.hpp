#pragma once

#include <type_traits>

namespace geompp::maths {

/// @brief Any built-in arithmetic type (int, long, float, double, unsigned variants, ...) -- the element
/// type Vector<T, N> and Matrix<T, Rows, Cols> are constrained to.
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

}  // namespace geompp::maths
