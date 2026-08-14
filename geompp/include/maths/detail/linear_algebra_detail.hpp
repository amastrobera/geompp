#pragma once

#include "../maths_concepts.hpp"

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace geompp::maths::detail {

/// @brief Determinant of an n x n matrix stored row-major in a flat buffer, via recursive Laplace
/// (cofactor) expansion along row 0. O(n!) -- fine for the n in {1,2,3,4} this module actually
/// instantiates (Matrix2/3/4::Determinant()), not intended for large n. Operates on a raw buffer (not
/// Matrix<T,N,N>) so both matrix.hpp (Matrix::Determinant()) and solvers.hpp (solve_cramer, which needs a
/// column-replaced copy per unknown) can share one implementation without a matrix.hpp <-> solvers.hpp
/// include cycle.
/// @param a   row-major n*n buffer.
/// @param n   matrix dimension.
template <typename T>
  requires Numeric<T>
T determinant_generic(std::vector<T> const& a, std::size_t n) {
  if (n == 1) {
    return a[0];
  }
  if (n == 2) {
    return a[0] * a[3] - a[1] * a[2];
  }

  T det{};
  std::vector<T> minor(( n - 1) * (n - 1));
  for (std::size_t col = 0; col < n; ++col) {
    // Build the minor obtained by deleting row 0 and column `col`.
    std::size_t k = 0;
    for (std::size_t r = 1; r < n; ++r) {
      for (std::size_t c = 0; c < n; ++c) {
        if (c == col) {
          continue;
        }
        minor[k++] = a[r * n + c];
      }
    }
    T cofactor = determinant_generic(minor, n - 1);
    det += ((col % 2 == 0) ? T{1} : T{-1}) * a[col] * cofactor;
  }
  return det;
}

/// @brief Gauss-Jordan elimination with partial pivoting on the augmented system [A | B], where A is n x n
/// and B is n x rhs_cols (both row-major flat buffers). Reduces A to the identity in place and carries B
/// along, so B ends up holding the solution: rhs_cols == 1 solves Ax = b (used by solve_gauss()); rhs_cols
/// == n with B seeded as the identity computes A^-1 (used by Matrix::Inverse()) -- one implementation
/// shared by both call sites instead of two near-identical elimination loops.
/// @throws std::invalid_argument if A is singular (no pivot found within DOUBLE_EPSILON-independent
/// numeric tolerance -- this module has no geometry-precision dependency, so it uses a fixed small
/// tolerance scaled to T's own epsilon rather than geompp::DECIMAL_PRECISION).
template <typename T>
  requires Numeric<T>
void gauss_jordan_eliminate(std::vector<T>& a, std::size_t n, std::vector<T>& b, std::size_t rhs_cols) {
  auto const tol = static_cast<T>(1e-12);

  for (std::size_t pivot_row = 0; pivot_row < n; ++pivot_row) {
    // Partial pivoting: swap in the row with the largest magnitude in this column, for numerical
    // stability (and to find a usable pivot at all when a(pivot_row, pivot_row) is exactly zero).
    std::size_t best_row = pivot_row;
    T best_val = a[pivot_row * n + pivot_row] < T{0} ? -a[pivot_row * n + pivot_row] : a[pivot_row * n + pivot_row];
    for (std::size_t r = pivot_row + 1; r < n; ++r) {
      T val = a[r * n + pivot_row] < T{0} ? -a[r * n + pivot_row] : a[r * n + pivot_row];
      if (val > best_val) {
        best_val = val;
        best_row = r;
      }
    }
    if (best_val <= tol) {
      throw std::invalid_argument("gauss_jordan_eliminate: matrix is singular (no usable pivot)");
    }
    if (best_row != pivot_row) {
      for (std::size_t c = 0; c < n; ++c) {
        std::swap(a[pivot_row * n + c], a[best_row * n + c]);
      }
      for (std::size_t c = 0; c < rhs_cols; ++c) {
        std::swap(b[pivot_row * rhs_cols + c], b[best_row * rhs_cols + c]);
      }
    }

    T pivot = a[pivot_row * n + pivot_row];
    for (std::size_t c = 0; c < n; ++c) {
      a[pivot_row * n + c] /= pivot;
    }
    for (std::size_t c = 0; c < rhs_cols; ++c) {
      b[pivot_row * rhs_cols + c] /= pivot;
    }

    for (std::size_t r = 0; r < n; ++r) {
      if (r == pivot_row) {
        continue;
      }
      T factor = a[r * n + pivot_row];
      if (factor == T{0}) {
        continue;
      }
      for (std::size_t c = 0; c < n; ++c) {
        a[r * n + c] -= factor * a[pivot_row * n + c];
      }
      for (std::size_t c = 0; c < rhs_cols; ++c) {
        b[r * rhs_cols + c] -= factor * b[pivot_row * rhs_cols + c];
      }
    }
  }
}

}  // namespace geompp::maths::detail
