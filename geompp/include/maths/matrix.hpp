#pragma once

#include "detail/linear_algebra_detail.hpp"
#include "maths_concepts.hpp"
#include "vector.hpp"

#include <array>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <numbers>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace geompp::maths {

/// @brief Fixed-size, compile-time-dimensioned (Rows x Cols known at compile time) numeric matrix, stored
/// row-major in a flat buffer: `m_data[r * Cols + c]`. See Vector<T,N>'s own docs for why size/type
/// compatibility between operands is enforced via template parameters (a compile error on mismatch)
/// rather than a runtime throw -- Matrix*Matrix/Matrix*Vector below follow the same reasoning.
template <typename T, std::size_t Rows, std::size_t Cols>
  requires Numeric<T> && (Rows >= 1) && (Cols >= 1)
class Matrix {
 protected:
  std::array<T, Rows * Cols> m_data{};

 public:
  constexpr Matrix() = default;

  /// @brief Full element-list constructor, row-major (matches the storage order): Matrix3(a,b,c, d,e,f,
  /// g,h,i) fills row 0 = {a,b,c}, row 1 = {d,e,f}, row 2 = {g,h,i}.
  template <typename... Args>
    requires(sizeof...(Args) == Rows * Cols) && (std::convertible_to<Args, T> && ...)
  constexpr Matrix(Args... args) noexcept : m_data{static_cast<T>(args)...} {}

  constexpr T& operator()(std::size_t r, std::size_t c) noexcept { return m_data[r * Cols + c]; }
  constexpr T const& operator()(std::size_t r, std::size_t c) const noexcept { return m_data[r * Cols + c]; }

  /// @brief Bounds-checked element access -- operator() stays noexcept/unchecked (hot path, matches
  /// Vector::operator[]'s convention); use At() when (r, c) isn't already known to be in range.
  constexpr T& At(std::size_t r, std::size_t c) {
    if (r >= Rows || c >= Cols) {
      throw std::out_of_range("Matrix::At: (row, col) out of range");
    }
    return m_data[r * Cols + c];
  }
  constexpr T const& At(std::size_t r, std::size_t c) const {
    if (r >= Rows || c >= Cols) {
      throw std::out_of_range("Matrix::At: (row, col) out of range");
    }
    return m_data[r * Cols + c];
  }

  constexpr T* data() noexcept { return m_data.data(); }
  constexpr T const* data() const noexcept { return m_data.data(); }

  static constexpr std::size_t RowCount() noexcept { return Rows; }
  static constexpr std::size_t ColCount() noexcept { return Cols; }

  static constexpr Matrix Zero() noexcept { return Matrix{}; }

  static constexpr Matrix Identity() noexcept
    requires(Rows == Cols)
  {
    Matrix m;
    for (std::size_t i = 0; i < Rows; ++i) {
      m(i, i) = T{1};
    }
    return m;
  }

  constexpr Matrix operator+(Matrix const& o) const noexcept {
    Matrix r;
    for (std::size_t i = 0; i < Rows * Cols; ++i) {
      r.m_data[i] = m_data[i] + o.m_data[i];
    }
    return r;
  }
  constexpr Matrix operator-(Matrix const& o) const noexcept {
    Matrix r;
    for (std::size_t i = 0; i < Rows * Cols; ++i) {
      r.m_data[i] = m_data[i] - o.m_data[i];
    }
    return r;
  }
  constexpr Matrix operator-() const noexcept {
    Matrix r;
    for (std::size_t i = 0; i < Rows * Cols; ++i) {
      r.m_data[i] = -m_data[i];
    }
    return r;
  }
  constexpr Matrix operator*(T scalar) const noexcept {
    Matrix r;
    for (std::size_t i = 0; i < Rows * Cols; ++i) {
      r.m_data[i] = m_data[i] * scalar;
    }
    return r;
  }
  /// @throws std::invalid_argument if scalar is zero.
  constexpr Matrix operator/(T scalar) const {
    if (scalar == T{0}) {
      throw std::invalid_argument("Matrix::operator/: division by zero scalar");
    }
    Matrix r;
    for (std::size_t i = 0; i < Rows * Cols; ++i) {
      r.m_data[i] = m_data[i] / scalar;
    }
    return r;
  }

  constexpr Matrix& operator+=(Matrix const& o) noexcept { return *this = *this + o; }
  constexpr Matrix& operator-=(Matrix const& o) noexcept { return *this = *this - o; }
  constexpr Matrix& operator*=(T scalar) noexcept { return *this = *this * scalar; }
  constexpr Matrix& operator/=(T scalar) { return *this = *this / scalar; }

  constexpr bool operator==(Matrix const& o) const noexcept {
    for (std::size_t i = 0; i < Rows * Cols; ++i) {
      if (m_data[i] != o.m_data[i]) {
        return false;
      }
    }
    return true;
  }

  /// @brief Matrix product: (Rows x Cols) * (Cols x OtherCols) -> (Rows x OtherCols). `this.Cols` must
  /// match `other`'s row count -- enforced by `other`'s own type (Matrix<T, Cols, OtherCols>), so a
  /// mismatched pairing is a compile error, not a runtime throw (see class docs).
  template <std::size_t OtherCols>
  constexpr Matrix<T, Rows, OtherCols> operator*(Matrix<T, Cols, OtherCols> const& other) const noexcept {
    Matrix<T, Rows, OtherCols> result;
    for (std::size_t r = 0; r < Rows; ++r) {
      for (std::size_t oc = 0; oc < OtherCols; ++oc) {
        T sum{};
        for (std::size_t k = 0; k < Cols; ++k) {
          sum += (*this)(r, k) * other(k, oc);
        }
        result(r, oc) = sum;
      }
    }
    return result;
  }

  /// @brief Matrix-vector product, treating `v` as a column vector: (Rows x Cols) * (Cols) -> (Rows).
  /// `this.Cols` must match `v`'s size -- enforced by `v`'s own type (Vector<T, Cols>), so a mismatched
  /// pairing is a compile error, not a runtime throw (see class docs).
  constexpr Vector<T, Rows> operator*(Vector<T, Cols> const& v) const noexcept {
    Vector<T, Rows> result;
    for (std::size_t r = 0; r < Rows; ++r) {
      T sum{};
      for (std::size_t c = 0; c < Cols; ++c) {
        sum += (*this)(r, c) * v[c];
      }
      result[r] = sum;
    }
    return result;
  }

  constexpr Matrix<T, Cols, Rows> Transpose() const noexcept {
    Matrix<T, Cols, Rows> result;
    for (std::size_t r = 0; r < Rows; ++r) {
      for (std::size_t c = 0; c < Cols; ++c) {
        result(c, r) = (*this)(r, c);
      }
    }
    return result;
  }

  /// @brief Determinant via recursive cofactor expansion (detail::determinant_generic) -- square only.
  T Determinant() const
    requires(Rows == Cols)
  {
    std::vector<T> buf(m_data.begin(), m_data.end());
    return detail::determinant_generic(buf, Rows);
  }

  /// @brief Matrix inverse via Gauss-Jordan elimination on [A | I] (detail::gauss_jordan_eliminate) --
  /// square only.
  /// @throws std::invalid_argument if the matrix is singular.
  Matrix Inverse() const
    requires(Rows == Cols)
  {
    std::vector<T> a(m_data.begin(), m_data.end());
    std::vector<T> identity(Rows * Rows, T{});
    for (std::size_t i = 0; i < Rows; ++i) {
      identity[i * Rows + i] = T{1};
    }
    detail::gauss_jordan_eliminate(a, Rows, identity, Rows);
    Matrix result;
    for (std::size_t i = 0; i < Rows * Rows; ++i) {
      result.m_data[i] = identity[i];
    }
    return result;
  }

  // -- Homogeneous 4x4 affine-transform factories (Matrix4 only -- `requires` gates them out of
  // existence, a compile error, for any other Matrix<T, Rows, Cols> instantiation). Kept here rather than
  // in geompp::transformations because TransformBuilder (transformations) is built by *composing* these
  // via plain Matrix4 multiplication -- they're primitive matrix constructors, not transform operations
  // in their own right. --

  static constexpr Matrix Translation(Vector<T, 3> const& offset) noexcept
    requires(Rows == 4 && Cols == 4)
  {
    Matrix m = Identity();
    m(0, 3) = offset.x();
    m(1, 3) = offset.y();
    m(2, 3) = offset.z();
    return m;
  }

  /// @brief Axis-angle rotation (Rodrigues' rotation formula) about `axis` through the origin, by
  /// `angle_rad` radians (right-hand rule).
  /// @throws std::invalid_argument if `axis` is zero-length.
  static Matrix Rotation(T angle_rad, Vector<T, 3> const& axis)
    requires(Rows == 4 && Cols == 4)
  {
    Vector<T, 3> u = axis.Normalized();  // throws on zero-length axis
    T c = static_cast<T>(std::cos(angle_rad));
    T s = static_cast<T>(std::sin(angle_rad));
    T one_minus_c = T{1} - c;

    Matrix m = Identity();
    m(0, 0) = c + u.x() * u.x() * one_minus_c;
    m(0, 1) = u.x() * u.y() * one_minus_c - u.z() * s;
    m(0, 2) = u.x() * u.z() * one_minus_c + u.y() * s;

    m(1, 0) = u.y() * u.x() * one_minus_c + u.z() * s;
    m(1, 1) = c + u.y() * u.y() * one_minus_c;
    m(1, 2) = u.y() * u.z() * one_minus_c - u.x() * s;

    m(2, 0) = u.z() * u.x() * one_minus_c - u.y() * s;
    m(2, 1) = u.z() * u.y() * one_minus_c + u.x() * s;
    m(2, 2) = c + u.z() * u.z() * one_minus_c;
    return m;
  }

  static constexpr Matrix Scale(T factor) noexcept
    requires(Rows == 4 && Cols == 4)
  {
    return Scale(factor, factor, factor);
  }

  static constexpr Matrix Scale(T sx, T sy, T sz) noexcept
    requires(Rows == 4 && Cols == 4)
  {
    Matrix m = Identity();
    m(0, 0) = sx;
    m(1, 1) = sy;
    m(2, 2) = sz;
    return m;
  }

  // -- Homogeneous 3x3 affine-transform factories (Matrix3 only) -- the 2D counterpart of the 4x4
  // set above, for geompp::transformations' 2D primitives (Point2D, ..., PolyMesh2D), which use a 3x3
  // homogeneous matrix ([x, y, 1]) the same way the 3D primitives use a 4x4 one ([x, y, z, 1]). --

  static constexpr Matrix Translation(Vector<T, 2> const& offset) noexcept
    requires(Rows == 3 && Cols == 3)
  {
    Matrix m = Identity();
    m(0, 2) = offset.x();
    m(1, 2) = offset.y();
    return m;
  }

  static Matrix Rotation(T angle_rad) noexcept
    requires(Rows == 3 && Cols == 3)
  {
    T c = static_cast<T>(std::cos(angle_rad));
    T s = static_cast<T>(std::sin(angle_rad));
    Matrix m = Identity();
    m(0, 0) = c;
    m(0, 1) = -s;
    m(1, 0) = s;
    m(1, 1) = c;
    return m;
  }

  static constexpr Matrix Scale(T factor) noexcept
    requires(Rows == 3 && Cols == 3)
  {
    return Scale(factor, factor);
  }

  static constexpr Matrix Scale(T sx, T sy) noexcept
    requires(Rows == 3 && Cols == 3)
  {
    Matrix m = Identity();
    m(0, 0) = sx;
    m(1, 1) = sy;
    return m;
  }

  std::string ToString() const {
    std::ostringstream out;
    for (std::size_t r = 0; r < Rows; ++r) {
      out << (r == 0 ? "[" : " [");
      for (std::size_t c = 0; c < Cols; ++c) {
        if (c > 0) {
          out << ", ";
        }
        out << (*this)(r, c);
      }
      out << (r + 1 == Rows ? "]" : "]\n");
    }
    return out.str();
  }
};

template <typename T, std::size_t Rows, std::size_t Cols>
constexpr Matrix<T, Rows, Cols> operator*(T scalar, Matrix<T, Rows, Cols> const& m) noexcept {
  return m * scalar;
}

template <typename T, std::size_t Rows, std::size_t Cols>
std::ostream& operator<<(std::ostream& os, Matrix<T, Rows, Cols> const& m) {
  return os << m.ToString();
}

using Matrix2 = Matrix<double, 2, 2>;
using Matrix3 = Matrix<double, 3, 3>;
using Matrix4 = Matrix<double, 4, 4>;

// Out-of-line now that Matrix is a complete type -- see the forward declaration + docs in vector.hpp.
template <typename T, std::size_t N>
  requires Numeric<T> && (N >= 1)
constexpr Matrix<T, 1, N> Vector<T, N>::Transpose() const noexcept {
  Matrix<T, 1, N> row;
  for (std::size_t i = 0; i < N; ++i) {
    row(0, i) = m_data[i];
  }
  return row;
}

}  // namespace geompp::maths
