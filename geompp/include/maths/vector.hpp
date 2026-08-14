#pragma once

#include "maths_concepts.hpp"

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>

namespace geompp::maths {

// Forward-declared so Vector::Transpose() can return a row-vector Matrix<T, 1, N> -- matrix.hpp includes
// this header (it needs Vector for Matrix*Vector and the homogeneous-transform factories), so the actual
// Transpose() body is defined out-of-line in matrix.hpp, once Matrix is a complete type.
template <typename T, std::size_t Rows, std::size_t Cols>
  requires Numeric<T> && (Rows >= 1) && (Cols >= 1)
class Matrix;

/// @brief Fixed-size, compile-time-dimensioned numeric vector (N known at compile time, same spirit as
/// Matrix<T, Rows, Cols>). Deliberately distinct from geompp::geometry::Vector2D/Vector3D: those model a
/// *geometric* displacement (Dot/Cross tied to Point2D/3D, WKT, DECIMAL_PRECISION-aware AlmostEquals);
/// this models a plain linear-algebra column vector for geompp::maths/geompp::transformations to build
/// matrix machinery on top of, with no dependency on the geometry types at all.
///
/// Size/type compatibility between operands (Vector+Vector, Matrix*Vector) is enforced at compile time via
/// template parameters rather than a runtime check: since N is part of the type, an incompatible pairing
/// simply doesn't compile -- a stronger guarantee than a runtime throw, and it's caught at the call site
/// instead of at test time. Runtime std::invalid_argument is reserved for failures that are genuinely only
/// knowable at runtime (a zero-length vector in Normalized(), a singular matrix in Matrix::Inverse() /
/// solve_gauss() / solve_cramer()).
template <typename T, std::size_t N>
  requires Numeric<T> && (N >= 1)
class Vector {
 protected:
  std::array<T, N> m_data{};

 public:
  constexpr Vector() = default;

  /// @brief Component-wise constructor: Vector2(x, y), Vector3(x, y, z), Vector4(x, y, z, w). Requires
  /// exactly N arguments so it can never silently accept a partial or over-long init list.
  template <typename... Args>
    requires(sizeof...(Args) == N) && (std::convertible_to<Args, T> && ...) && (N > 1)
  constexpr Vector(Args... args) noexcept : m_data{static_cast<T>(args)...} {}

  /// @brief Single-component constructor (N == 1 only) -- split from the variadic one above so a lone
  /// scalar argument isn't ambiguous with the (deleted-by-absence) copy-from-T conversion.
  constexpr explicit Vector(T value) noexcept
    requires(N == 1)
      : m_data{value} {}

  constexpr T& operator[](std::size_t i) noexcept { return m_data[i]; }
  constexpr T const& operator[](std::size_t i) const noexcept { return m_data[i]; }

  /// @brief Bounds-checked element access -- operator[] stays noexcept/unchecked (hot path, matches the
  /// Matrix::operator() convention this module follows); use At() when the index isn't already known to
  /// be in range.
  constexpr T& At(std::size_t i) {
    if (i >= N) {
      throw std::out_of_range("Vector::At: index out of range");
    }
    return m_data[i];
  }
  constexpr T const& At(std::size_t i) const {
    if (i >= N) {
      throw std::out_of_range("Vector::At: index out of range");
    }
    return m_data[i];
  }

  constexpr T* data() noexcept { return m_data.data(); }
  constexpr T const* data() const noexcept { return m_data.data(); }

  static constexpr std::size_t Size() noexcept { return N; }

  static constexpr Vector Zero() noexcept { return Vector{}; }

  // Named accessors -- gated by N via `requires` on the member function itself, so e.g. Vector<T,2>::z()
  // simply doesn't exist (a compile error at the call site) rather than an out-of-range runtime access.
  constexpr T& x() noexcept
    requires(N >= 1)
  {
    return m_data[0];
  }
  constexpr T const& x() const noexcept
    requires(N >= 1)
  {
    return m_data[0];
  }
  constexpr T& y() noexcept
    requires(N >= 2)
  {
    return m_data[1];
  }
  constexpr T const& y() const noexcept
    requires(N >= 2)
  {
    return m_data[1];
  }
  constexpr T& z() noexcept
    requires(N >= 3)
  {
    return m_data[2];
  }
  constexpr T const& z() const noexcept
    requires(N >= 3)
  {
    return m_data[2];
  }
  constexpr T& w() noexcept
    requires(N >= 4)
  {
    return m_data[3];
  }
  constexpr T const& w() const noexcept
    requires(N >= 4)
  {
    return m_data[3];
  }

  constexpr Vector operator+(Vector const& o) const noexcept {
    Vector r;
    for (std::size_t i = 0; i < N; ++i) {
      r.m_data[i] = m_data[i] + o.m_data[i];
    }
    return r;
  }
  constexpr Vector operator-(Vector const& o) const noexcept {
    Vector r;
    for (std::size_t i = 0; i < N; ++i) {
      r.m_data[i] = m_data[i] - o.m_data[i];
    }
    return r;
  }
  constexpr Vector operator-() const noexcept {
    Vector r;
    for (std::size_t i = 0; i < N; ++i) {
      r.m_data[i] = -m_data[i];
    }
    return r;
  }
  constexpr Vector operator*(T scalar) const noexcept {
    Vector r;
    for (std::size_t i = 0; i < N; ++i) {
      r.m_data[i] = m_data[i] * scalar;
    }
    return r;
  }
  /// @throws std::invalid_argument if scalar is zero.
  constexpr Vector operator/(T scalar) const {
    if (scalar == T{0}) {
      throw std::invalid_argument("Vector::operator/: division by zero scalar");
    }
    Vector r;
    for (std::size_t i = 0; i < N; ++i) {
      r.m_data[i] = m_data[i] / scalar;
    }
    return r;
  }

  constexpr Vector& operator+=(Vector const& o) noexcept { return *this = *this + o; }
  constexpr Vector& operator-=(Vector const& o) noexcept { return *this = *this - o; }
  constexpr Vector& operator*=(T scalar) noexcept { return *this = *this * scalar; }
  constexpr Vector& operator/=(T scalar) { return *this = *this / scalar; }

  constexpr bool operator==(Vector const& o) const noexcept {
    for (std::size_t i = 0; i < N; ++i) {
      if (m_data[i] != o.m_data[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr T Dot(Vector const& o) const noexcept {
    T sum{};
    for (std::size_t i = 0; i < N; ++i) {
      sum += m_data[i] * o.m_data[i];
    }
    return sum;
  }

  /// @brief 3D cross product -- only defined for N == 3 (a compile error otherwise, not a runtime one).
  constexpr Vector Cross(Vector const& o) const noexcept
    requires(N == 3)
  {
    return Vector(m_data[1] * o.m_data[2] - m_data[2] * o.m_data[1],
                  m_data[2] * o.m_data[0] - m_data[0] * o.m_data[2],
                  m_data[0] * o.m_data[1] - m_data[1] * o.m_data[0]);
  }

  constexpr T LengthSquared() const noexcept { return Dot(*this); }
  T Length() const { return std::sqrt(static_cast<double>(LengthSquared())); }

  /// @throws std::invalid_argument if this vector's length is zero (no direction to normalize to).
  Vector Normalized() const {
    T len = static_cast<T>(Length());
    if (len == T{0}) {
      throw std::invalid_argument("Vector::Normalized: zero-length vector has no direction");
    }
    return *this / len;
  }

  /// @brief Row-vector view of this (conceptually column) vector, as a 1xN Matrix -- defined out-of-line
  /// in matrix.hpp, once Matrix<T, 1, N> is a complete type.
  constexpr Matrix<T, 1, N> Transpose() const noexcept;

  std::string ToString() const {
    std::ostringstream out;
    out << "(";
    for (std::size_t i = 0; i < N; ++i) {
      if (i > 0) {
        out << ", ";
      }
      out << m_data[i];
    }
    out << ")";
    return out.str();
  }
};

template <typename T, std::size_t N>
constexpr Vector<T, N> operator*(T scalar, Vector<T, N> const& v) noexcept {
  return v * scalar;
}

template <typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, Vector<T, N> const& v) {
  return os << v.ToString();
}

using Vector2 = Vector<double, 2>;
using Vector3 = Vector<double, 3>;
using Vector4 = Vector<double, 4>;

}  // namespace geompp::maths
