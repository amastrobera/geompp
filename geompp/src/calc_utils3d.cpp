#include "calc_utils3d.hpp"

#include "calc_utils2d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <cmath>
#include <stdexcept>

namespace geompp {

void distance_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0, Point3D const& L2_P1,
                           double& sc, double& tc) {
  // defined L1: P0 + s*(P1 - P0) = P0 + s*U
  //         L2: Q0 + t*(Q1 - Q0) = Q0 + t*V
  // a vector W(c) = L1(c) - L2(c) = (P0 - Q0) + s*U - t*V = w0 + s*U - t*V
  //          is simulateneously perpendicular to both L1 and L2
  // and the conditions
  //          W(c) . U = 0
  //          W(c) . V = 0
  // make up the system of equation (expressable in 2D matrix for, 2 EQ 2 UKN) solvable with Cramer's rule
  // if the lines are parallel, there is no unique solution, and we can return nullopt

  auto U = L1_P1 - L1_P0;
  if (compare(U.Length(), 0) == 0) {
    throw std::runtime_error("invalid input: L1_P0, L1_P1 makes a zero length line");
  }
  auto V = L2_P1 - L2_P0;
  if (compare(V.Length(), 0) == 0) {
    throw std::runtime_error("invalid input: L2_P0, L2_P1 makes a zero length line");
  }
  auto W0 = L1_P0 - L2_P0;

  double a = U.Dot(U);
  double b = U.Dot(V);
  double c = V.Dot(V);
  double d = U.Dot(W0);
  double e = V.Dot(W0);
  double D = a * c - b * b;  // Determinant

  if (compare(D, 0) == 0) {
    // this will magically return zero if the lines overlap, and a positive number if they are parallel but distinct
    sc = 0;
    tc = (b > c ? d / b : e / c);  // use the largest denominator

  } else {
    // Cramer's rule
    sc = (b * e - c * d) / D;
    tc = (a * e - b * d) / D;
  }
}

std::optional<Point3D> intersection_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0,
                                                 Point3D const& L2_P1, double& sc, double& tc) {
  try {
    // input parameters: this line as P0 + s*DIR, other as Q0 + t*DIR
    //
    //  3D Line-Line Intersection
    //      - minimize the (perpendicular) distance between lines
    //      - and later verify that this distance is nearly zero
    //        (intersection) or not (skew lines)
    //
    //  solving system   | u*u u*v | | s |  =  | u*w0 |
    //                   | u*v v*v | | t |     | v*w0 |
    Point3D P0 = L1_P0;
    Point3D P1 = L1_P1;
    Point3D Q0 = L2_P0;
    Point3D Q1 = L2_P1;
    Vector3D u = P1 - P0;
    Vector3D v = Q1 - Q0;
    Vector3D w0 = P0 - Q0;

    // variables
    //  solving system   | a b | | s |  =  | d |
    //                   | b c | | t |     | e |
    double a = u.Dot(u);
    double b = u.Dot(v);
    double c = v.Dot(v);
    double d = u.Dot(w0);
    double e = v.Dot(w0);
    double D = a * c - b * b;  // Determinant

    // Check if lines are parallel
    if (compare(D, 0) == 0) {
      return std::nullopt;  // Lines are parallel, no intersection
    }

    // Cramer's rule
    sc = (b * e - c * d) / D;
    tc = (a * e - b * d) / D;

    // The point on the Line closest to the Ray
    Point3D Pc = P0 + (u * sc);
    // The point on the Ray closest to the Line
    Point3D Qc = Q0 + (v * tc);

    // Check if they actually intersect (distance is near zero)
    if (compare(Pc.DistanceTo(Qc), 0.0) != 0) {
      return std::nullopt;  // No intersection, the closest points are not the same
    }

    return Pc;  // They intersect!

  } catch (...) {
    GEOMPP_LOG(WARNING) << "unexpected error while computing line intersection";
  }

  sc = tc = std::numeric_limits<double>::quiet_NaN();
  return std::nullopt;
}

std::vector<std::size_t> convex_hull_indices(std::vector<Point3D> const& points, std::optional<Vector3D> normal) {
  if (points.size() < 3) {
    throw std::invalid_argument("less than 3 points");
  }

  // TODO: can this block become a function that makes sense to use ?
  //       Vector3D make_normal_or_throw(std::vector<Point3D> const& cloud_of_coplanar_points);
  if (!normal) {
    auto no_col_pts = remove_collinear(points);
    if (no_col_pts.size() < 3) {
      throw std::invalid_argument("less than 3 non collinear points");
    }

    Vector3D calc_normal = (no_col_pts[1] - no_col_pts[0]).Cross(no_col_pts[2] - no_col_pts[0]);

    std::size_t n = no_col_pts.size();
    if (n > 3) {
      for (std::size_t i = 3; i < n + 1; ++i) {
        auto p0 = no_col_pts[(i - 2) % n];
        auto p1 = no_col_pts[(i - 1) % n];
        auto p2 = no_col_pts[i % n];

        // performance note: here we try to assess if all points are on the same plane - that is - all normals are equal
        //  I would have used .Normalize() and compare pairs of normals but that would require 3 sqrt() calls each time
        //  It is faster to use a cross product of the two calculated normals and check if they cross to a null vector
        //  (if they don't they aren't on the same plane)
        auto temp_norm = (p1 - p0).Cross(p2 - p0);
        if (!calc_normal.Cross(temp_norm).AlmostEquals(Vector3D{0, 0, 0})) {
          throw std::invalid_argument("points are not co-planar");
        }
      }
    }

    if (calc_normal.AlmostEquals(Vector3D{0, 0, 0})) {
      throw std::logic_error("failed to calculate points normal");
    }

    normal = calc_normal;
  }

  auto dax = normal->DominantAxis();

  switch (dax) {
    case Axis::X:
      // clang-format off
      return convex_hull_generic_impl_2D(
          points.size(),
          [&points](size_t i) { return points[i].y(); },
          [&points](size_t i) { return points[i].z(); },
          [&points](size_t o, size_t a, size_t b) {
              return compare((points[a].y() - points[o].y()) * (points[b].z() - points[o].z()) -
                             (points[a].z() - points[o].z()) * (points[b].y() - points[o].y()), 0) > 0;
          }
      );
      // clang-format on
    case Axis::Y:
      // clang-format off
      return convex_hull_generic_impl_2D(
          points.size(),
          [&points](size_t i) { return points[i].z(); },
          [&points](size_t i) { return points[i].x(); },
          [&points](size_t o, size_t a, size_t b) {
              return compare((points[a].z() - points[o].z()) * (points[b].x() - points[o].x()) -
                             (points[a].x() - points[o].x()) * (points[b].z() - points[o].z()), 0) > 0;
          }
      );
      // clang-format on

    case Axis::Z:
      // clang-format off
      return convex_hull_generic_impl_2D(
          points.size(),
          [&points](size_t i) { return points[i].x(); },
          [&points](size_t i) { return points[i].y(); },
          [&points](size_t o, size_t a, size_t b) {
              return compare((points[a].x() - points[o].x()) * (points[b].y() - points[o].y()) -
                             (points[a].y() - points[o].y()) * (points[b].x() - points[o].x()), 0) > 0;
          }
      );
      // clang-format on

    default:
      throw std::logic_error("unexpected dominant axis");
  }
}

namespace {

// Jacobi eigendecomposition for symmetric 3x3 matrix.
// A is modified in place (diagonal becomes eigenvalues), V accumulates eigenvectors (initialized to identity).
static void jacobi3(double A[3][3], double V[3][3]) {
  // Initialize V to identity
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      V[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }

  for (int iter = 0; iter < 50; ++iter) {
    // Find off-diagonal element with maximum absolute value
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
    if (maxval < 1e-12) {
      break;
    }

    double theta = 0.5 * (A[q][q] - A[p][p]) / A[p][q];
    double t = (theta >= 0 ? 1.0 : -1.0) / (std::abs(theta) + std::sqrt(1.0 + theta * theta));
    double c = 1.0 / std::sqrt(1.0 + t * t);
    double s = t * c;

    // Apply Givens rotation to annihilate A[p][q]
    double App = A[p][p], Aqq = A[q][q], Apq = A[p][q];
    A[p][p] = c * c * App - 2 * s * c * Apq + s * s * Aqq;
    A[q][q] = s * s * App + 2 * s * c * Apq + c * c * Aqq;
    A[p][q] = A[q][p] = 0.0;
    for (int r = 0; r < 3; ++r) {
      if (r == p || r == q) {
        continue;
      }
      double Arp = A[r][p], Arq = A[r][q];
      A[r][p] = A[p][r] = c * Arp - s * Arq;
      A[r][q] = A[q][r] = s * Arp + c * Arq;
    }
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

  // compute centroid
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

Vector3D principal_normal(std::vector<Point3D> const& points) {
  return principal_axes(points).Z;
}

Vector3D principal_direction(std::vector<Point3D> const& points) {
  return principal_axes(points).X;
}

}  // namespace geompp
