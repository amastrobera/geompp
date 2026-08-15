#include "calc_utils/convex_hull3d.hpp"

#include "line3d.hpp"
#include "polygon3d.hpp"
#include "vector3d.hpp"
#include "view2d.hpp"

#include <cmath>

namespace geompp {

inline namespace geometry {
namespace detail {

std::vector<std::size_t> convex_hull_indices(std::vector<Point3D> const& points, Vector3D normal) {
  if (points.size() < 3) {
    throw std::invalid_argument("less than 3 points");
  }

  auto dax = normal.DominantAxis();

  switch (dax) {
    case Axis::X:
      return view::convex_hull_monotone_chain(points, View2D::YZ());
    case Axis::Y:
      return view::convex_hull_monotone_chain(points, View2D::ZX());
    case Axis::Z:
      return view::convex_hull_monotone_chain(points, View2D::XY());
    default:
      throw std::logic_error("unexpected dominant axis");
  }
}

std::vector<std::size_t> convex_hull_indices(std::vector<Point3D> const& points) {
  if (points.size() < 3) {
    throw std::invalid_argument("less than 3 points");
  }

  auto frame = principal_axes(points);
  Vector3D normal = frame.Z;

  return convex_hull_indices(points, normal);
}

}  // namespace detail

namespace {

// Jacobi eigendecomposition for a symmetric 3x3 matrix (Jacobi 1846).
// Repeatedly applies Givens (plane) rotations J in the (p,q) plane to zero out
// the largest off-diagonal element, driving A toward diagonal form.
// After convergence: A's diagonal holds the eigenvalues, V's columns hold the
// corresponding unit eigenvectors (V = J0 * J1 * ... * Jk).
// A is modified in place; V must be passed as zeroed storage (we initialise it here).
static void jacobi3(double A[3][3], double V[3][3]) {
  // V starts as the identity — each rotation will be accumulated into it
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      V[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }

  // iterate until all off-diagonal entries are negligible (at most 50 sweeps)
  for (int iter = 0; iter < 50; ++iter) {
    // find the largest off-diagonal element A[p][q] — this is the pivot to annihilate
    // (only the 3 upper-triangle pairs need checking: (0,1), (0,2), (1,2))
    int p = 0, q = 1;
    double maxval = std::abs(A[0][1]);
    if (std::abs(A[0][2]) > maxval) {
      p = 0;
      q = 2;
      maxval = std::abs(A[0][2]);
    }
    if (std::abs(A[1][2]) > maxval) {
      p = 1;
      q = 2;
      maxval = std::abs(A[1][2]);
    }
    // converged: all off-diagonal entries are below floating-point noise
    if (maxval < 1e-12) {
      break;
    }

    // compute the Givens rotation angle φ that zeros A[p][q]
    // \_ θ = cot(2φ) = (A[q][q] - A[p][p]) / (2 * A[p][q])
    // \_ t = tan(φ), chosen with sign(θ) so |φ| ≤ π/4 — keeps the rotation small,
    //        numerically equivalent to the standard formula but avoids catastrophic cancellation
    // \_ c = cos(φ) = 1/√(1+t²),  s = sin(φ) = t·c
    double theta = 0.5 * (A[q][q] - A[p][p]) / A[p][q];
    double t = (theta >= 0 ? 1.0 : -1.0) / (std::abs(theta) + std::sqrt(1.0 + theta * theta));
    double c = 1.0 / std::sqrt(1.0 + t * t);
    double s = t * c;

    // apply the similarity transform A ← J^T A J in the (p,q) plane
    // \_ update the two diagonal entries (quadratic terms of the rotation)
    double App = A[p][p], Aqq = A[q][q], Apq = A[p][q];
    A[p][p] = c * c * App - 2 * s * c * Apq + s * s * Aqq;
    A[q][q] = s * s * App + 2 * s * c * Apq + c * c * Aqq;
    // \_ zero out the pivot (exact by construction)
    A[p][q] = A[q][p] = 0.0;
    // \_ rotate the remaining off-diagonal rows/columns (r ≠ p,q)
    for (int r = 0; r < 3; ++r) {
      if (r == p || r == q) {
        continue;
      }
      double Arp = A[r][p], Arq = A[r][q];
      A[r][p] = A[p][r] = c * Arp - s * Arq;
      A[r][q] = A[q][r] = s * Arp + c * Arq;
    }
    // accumulate the rotation into V so that V's columns converge to the eigenvectors
    // \_ each column of V is rotated in the same (p,q) plane
    for (int r = 0; r < 3; ++r) {
      double Vrp = V[r][p], Vrq = V[r][q];
      V[r][p] = c * Vrp - s * Vrq;
      V[r][q] = s * Vrp + c * Vrq;
    }
  }
}

}  // anonymous namespace

CoordinateFrame principal_axes(std::vector<Point3D> const& points) {
  if (points.size() < 3) {
    throw std::runtime_error("principal_axes: need at least 3 points");
  }

  // compute mean point
  double cx = 0, cy = 0, cz = 0;
  for (auto const& p : points) {
    cx += p.x();
    cy += p.y();
    cz += p.z();
  }
  double inv_n = 1.0 / static_cast<double>(points.size());
  cx *= inv_n;
  cy *= inv_n;
  cz *= inv_n;

  // compute 3x3 covariance matrix (upper triangle, symmetric)
  double C[3][3] = {};
  for (auto const& p : points) {
    double dx = p.x() - cx;
    double dy = p.y() - cy;
    double dz = p.z() - cz;
    C[0][0] += dx * dx;
    C[0][1] += dx * dy;
    C[0][2] += dx * dz;
    C[1][1] += dy * dy;
    C[1][2] += dy * dz;
    C[2][2] += dz * dz;
  }
  C[1][0] = C[0][1];
  C[2][0] = C[0][2];
  C[2][1] = C[1][2];
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      C[i][j] *= inv_n;
    }
  }

  // Jacobi eigendecomposition — eigenvalues on C diagonal, eigenvectors in columns of V
  double V[3][3] = {};
  jacobi3(C, V);

  // eigenvalues are C[0][0], C[1][1], C[2][2]; eigenvector i is column i of V
  // sort by eigenvalue descending (largest → X, smallest → Z)
  int idx[3] = {0, 1, 2};
  // simple 3-element sort (insertion sort)
  for (int i = 1; i < 3; ++i) {
    for (int j = i; j > 0 && C[idx[j]][idx[j]] > C[idx[j - 1]][idx[j - 1]]; --j) {
      int tmp = idx[j];
      idx[j] = idx[j - 1];
      idx[j - 1] = tmp;
    }
  }

  auto get_axis = [&](int col) -> Vector3D {
    Vector3D v(V[0][col], V[1][col], V[2][col]);
    return v.Normalize();
  };

  return CoordinateFrame{get_axis(idx[0]), get_axis(idx[1]), get_axis(idx[2])};
}

Vector3D principal_normal(std::vector<Point3D> const& points) { return principal_axes(points).Z; }

Vector3D principal_direction(std::vector<Point3D> const& points) { return principal_axes(points).X; }

bool is_convex(std::vector<Point3D> const& vertices, std::vector<std::vector<Point3D>> const& holes,
               Vector3D const& normal) {
  if (!holes.empty() || vertices.size() < 3) {
    return false;
  }
  Axis dax = normal.DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
  return detail::view::is_convex(vertices, view);
}

ExtremePoints<Point3D> find_extreme_points(Polygon3D const& polygon, Line3D const& line) {
  auto [min_i, max_i] = detail::extreme_points(polygon.Perimeter(), polygon.IsConvex(), line.Direction());
  return {polygon[min_i], polygon[max_i]};
}

}  // namespace geometry

}  // namespace geompp
