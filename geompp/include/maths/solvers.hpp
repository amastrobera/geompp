#pragma once

#include "detail/linear_algebra_detail.hpp"
#include "matrix.hpp"
#include "maths_concepts.hpp"
#include "vector.hpp"

#include <cstddef>
#include <vector>

namespace geompp::maths {

/// @brief Solves the square linear system A x = b via Gauss-Jordan elimination with partial pivoting
/// (detail::gauss_jordan_eliminate on the augmented system [A | b]).
/// @param a the system's N x N coefficient matrix.
/// @param b the N known terms.
/// @returns the N unknowns x such that a * x == b.
/// @throws std::invalid_argument if `a` is singular (no unique solution).
template <typename T, std::size_t N>
  requires Numeric<T>
Vector<T, N> solve_gauss(Matrix<T, N, N> const& a, Vector<T, N> const& b) {
  std::vector<T> a_buf(a.data(), a.data() + N * N);
  std::vector<T> b_buf(b.data(), b.data() + N);
  detail::gauss_jordan_eliminate(a_buf, N, b_buf, 1);
  Vector<T, N> x;
  for (std::size_t i = 0; i < N; ++i) {
    x[i] = b_buf[i];
  }
  return x;
}

/// @brief Solves the square linear system A x = b via Cramer's rule: x_i = det(A with column i replaced
/// by b) / det(A). O(N) determinant evaluations of an N x N matrix (each itself O(N!) via
/// detail::determinant_generic) -- the classic textbook method, considerably more expensive than
/// solve_gauss() for anything past N == 3 or 4, but a direct, non-iterative closed form some callers
/// specifically want (e.g. to inspect an individual unknown's determinant ratio).
/// @param a the system's N x N coefficient matrix.
/// @param b the N known terms.
/// @returns the N unknowns x such that a * x == b.
/// @throws std::invalid_argument if det(a) is zero (no unique solution).
template <typename T, std::size_t N>
  requires Numeric<T>
Vector<T, N> solve_cramer(Matrix<T, N, N> const& a, Vector<T, N> const& b) {
  T det_a = a.Determinant();
  if (det_a == T{0}) {
    throw std::invalid_argument("solve_cramer: matrix is singular (determinant is zero)");
  }

  Vector<T, N> x;
  for (std::size_t col = 0; col < N; ++col) {
    Matrix<T, N, N> a_col = a;
    for (std::size_t r = 0; r < N; ++r) {
      a_col(r, col) = b[r];
    }
    x[col] = a_col.Determinant() / det_a;
  }
  return x;
}

}  // namespace geompp::maths
