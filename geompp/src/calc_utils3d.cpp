#include "calc_utils3d.hpp"

#include "calc_utils2d.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "polygon3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"
#include "view2d.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace geompp {

namespace detail {

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

std::optional<Point3D> line_intersection(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0,
                                         Point3D const& L2_P1, double& sc, double& tc) {
  try {
    // input parameters: this line as P0 + s*DIR, other as Q0 + t*DIR
    //
    //  3D Line-Line Intersection
    //      - minimize the (perpendicular) distance between lines
    //      - and later verify that this distance is nearly zero
    //        (intersection) or not (skew lines)
    //
    // Let L1 be defined by P0 + s*U, and L2 by Q0 + t*V, where U = P1 - P0, V = Q1 - Q0
    //  L1: P0 + s*U
    //  L2: Q0 + t*V
    //    Let w0 = P0 - Q0
    //  W(s,t) = L1(s) - L2(t) = w0 + s*U - t*V
    //  ... we are looking for the (s,t) that minimize the distance, that is the lenght of W(s,t)
    //
    //  Geometrically, the minimum distance occurs when W(s,t) is perpendicular to both lines, that is:
    //  W(s,t) . U = 0
    //  W(s,t) . V = 0
    //
    //  when we expand this out, we get the following system of equations:
    //  (u.u)*s - (v.u)*t = -v.w0
    //  (u.v)*s - (v.v)*t = -v.w0
    //
    //  which we can re-write as a system of equaltion
    //
    //  | u*u -u*v | | s |  =  | -u*w0 |
    //  | u*v -v*v | | t |     | -v*w0 |
    //
    // and then solve the system for s,t
    //
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

namespace {

// Minimum distance from a fixed 3D line to a closed ring of points (one polygon boundary loop —
// outer ring or a hole). Point-to-line distance has nested convex (ellipsoidal) level sets centered
// on the line, so its minimum over any closed planar region is always attained on the region's
// boundary, regardless of whether that region is convex — so an exact per-edge scan suffices for
// both convex and non-convex rings alike; no separate convex fast path is needed here.
// Per edge P0->P1 (E = P1-P0, A = P0-line_origin, D = line direction, unit), the squared distance
// from the edge point P0+t*E to the line is the quadratic c0 + c1*t + c2*t^2 derived from
// |A+tE|^2 - ((A+tE).D)^2; minimizing over t in [0,1] is a standard clamped-vertex search.
double ring_distance_to_line(std::vector<Point3D> const& ring, Point3D const& line_origin,
                             Vector3D const& line_dir) {
  std::size_t n = ring.size();
  double min_d2 = -1;
  for (std::size_t i = 0; i < n; ++i) {
    Point3D const& p0 = ring[i];
    Point3D const& p1 = ring[(i + 1) % n];
    Vector3D E = p1 - p0;
    Vector3D A = p0 - line_origin;

    double AD = A.Dot(line_dir);
    double ED = E.Dot(line_dir);
    double c0 = A.Dot(A) - AD * AD;
    double c1 = 2.0 * (A.Dot(E) - AD * ED);
    double c2 = E.Dot(E) - ED * ED;

    double t = 0.0;
    if (compare(c2, 0.0) != 0) {
      t = -c1 / (2.0 * c2);
    } else if (c1 < 0) {
      t = 1.0;
    }
    t = std::clamp(t, 0.0, 1.0);

    double d2 = c0 + c1 * t + c2 * t * t;
    if (compare(min_d2, 0) < 0 || d2 < min_d2) {
      min_d2 = d2;
    }
  }
  return std::sqrt(std::max(min_d2, 0.0));
}

}  // namespace

/// @brief computes the distance between a polygon and a line (the distance is zero if they intersect)
double distance_to(Polygon3D const& polygon, Line3D const& line) {
  auto poly_plane = polygon.GetPlane();
  bool plane_is_parallel_to_line = poly_plane.IsParallel(line);

  // cases that can be reduced to 2D: a line parallel to the plane has a constant perpendicular
  // offset `h` to it, so the true 3D distance is sqrt(h^2 + d2d^2), where d2d is the in-plane
  // distance between the polygon and the line's projection onto the plane. Coplanarity is just the
  // special case h == 0, which this formula already handles without a separate branch.
  if (plane_is_parallel_to_line) {
    double h = poly_plane.DistanceTo(line.Origin());

    double d2d;
    // these world planes will make the maths very quick downstream - if they can be used
    if (poly_plane == Plane::XY()) {
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::XY());
    } else if (poly_plane == Plane::YZ()) {
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::YZ());
    } else if (poly_plane == Plane::ZX()) {
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::ZX());
    } else {
      // otherwise we will have to go slower and use the plane's own (Custom) basis
      d2d = detail::view::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.Origin(),
                                      line.Origin() + line.Direction(), View2D::OnPlane(poly_plane));
    }

    return std::sqrt(h * h + d2d * d2d);
  }

  // cases that are purely 3D
  // the line is skewed, therefore it must intersect the plane in one point
  auto plane_p = poly_plane.Intersection(line);
  if (!plane_p.has_value() || !std::holds_alternative<Point3D>(*plane_p)) {
    throw std::logic_error("skew line does not intersect plane as expected");
  }
  auto const& plane_pp = std::get<Point3D>(*plane_p);

  // if the point is inside the polygon the distance is zero
  if (polygon.Contains(plane_pp)) {
    return 0.0;
  }

  // otherwise, the true minimum is on the polygon's boundary (see ring_distance_to_line above) —
  // scan the outer ring and every hole, in native 3D, no projection needed.
  double min_d = ring_distance_to_line(polygon.Perimeter(), line.Origin(), line.Direction());
  for (auto const& hole : polygon.Holes()) {
    min_d = std::min(min_d, ring_distance_to_line(hole, line.Origin(), line.Direction()));
  }
  return min_d;
}

namespace {

// Cheapest View2D that can represent `plane`: one of the fast world-plane views when it matches exactly,
// otherwise the plane's own (Custom) basis. Mirrors the selection distance_to(Polygon3D, Line3D) makes above.
View2D view_for_plane(Plane const& plane) {
  if (plane == Plane::XY()) {
    return View2D::XY();
  }
  if (plane == Plane::YZ()) {
    return View2D::YZ();
  }
  if (plane == Plane::ZX()) {
    return View2D::ZX();
  }
  return View2D::OnPlane(plane);
}

}  // namespace

PolygonTangents<LineSegment3D> tangents_to(Polygon3D const& polygon, Point3D const& p) {
  auto poly_plane = polygon.GetPlane();
  if (!poly_plane.Contains(p)) {
    throw std::logic_error("tangents_to(Polygon3D, Point3D) — point is not coplanar with the polygon");
  }

  auto [left_i, right_i] =
      detail::view::point_poly_tangent_lr_to(polygon.Perimeter(), polygon.IsConvex(), p, view_for_plane(poly_plane));
  return {LineSegment3D::Make(p, polygon[left_i]), LineSegment3D::Make(p, polygon[right_i])};
}

PolygonTangents<LineSegment3D> tangents_to(Polygon3D const& polygon, Polygon3D const& other) {
  auto poly_plane = polygon.GetPlane();
  if (!(poly_plane == other.GetPlane())) {
    throw std::logic_error("tangents_to(Polygon3D, Polygon3D) — polygons are not coplanar");
  }
  View2D view = view_for_plane(poly_plane);

  auto [RL_poly_i, RL_other_i] = detail::view::poly_poly_RL_tangent_to(polygon.Perimeter(), polygon.IsConvex(),
                                                                        other.Perimeter(), other.IsConvex(), view);
  auto [LR_other_i, LR_poly_i] = detail::view::poly_poly_RL_tangent_to(other.Perimeter(), other.IsConvex(),
                                                                        polygon.Perimeter(), polygon.IsConvex(), view);
  return {LineSegment3D::Make(polygon[RL_poly_i], other[RL_other_i]),
          LineSegment3D::Make(polygon[LR_poly_i], other[LR_other_i])};
}

}  // namespace geompp
