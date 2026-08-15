#include "calc_utils/self_intersections3d.hpp"

#include "calc_utils/convex_hull2d.hpp"
#include "calc_utils/convex_hull3d.hpp"
#include "geompp_log.hpp"
#include "line_segment3d.hpp"
#include "segment_iterator3d.hpp"
#include "vector3d.hpp"
#include "view2d.hpp"

#include <limits>

namespace geompp {

inline namespace geometry {
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

}  // namespace detail

std::vector<LineSegment3D> to_segments(std::vector<Point3D> const& points) {
  SegmentRange3D range(points, true);
  std::vector<LineSegment3D> segments;
  segments.reserve(range.size());
  for (auto const& seg : range) {
    segments.push_back(seg);
  }
  return segments;
}

bool is_simple(std::vector<Point3D> const& points, Vector3D const& normal) {
  Axis dax = normal.DominantAxis();
  View2D view = (dax == Axis::X) ? View2D::YZ() : (dax == Axis::Y) ? View2D::ZX() : View2D::XY();
  return detail::view::is_simple(points, view);
}

bool is_simple(std::vector<Point3D> const& points) { return is_simple(points, principal_normal(points)); }

}  // namespace geometry

}  // namespace geompp
