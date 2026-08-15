#pragma once

#include "../generic_concepts.hpp"
#include "../line_segment2d.hpp"
#include "../point2d.hpp"
#include "../view2d.hpp"

#include <utility>
#include <vector>

namespace geompp {

inline namespace geometry {

class Polygon2D;
class Line2D;

template <typename LineSegmentT>
struct PolygonTangents {
  LineSegmentT left;
  LineSegmentT right;
};

namespace detail {

/// @brief Winding-number contribution of a single ring (vertices) around point p.
/// @returns winding number increment/decrement for the ring
int winding_number(std::vector<Point2D> const& vertices, Point2D const& p);

namespace view {

/// @brief Point-on-edge perimeter test projected through a View2D.
/// Works for both 2D (View2D::XY()) and 3D (dominant-axis view) rings.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each vertex to 2D x/y coordinates.
/// @param px     Test point x in view space.
/// @param py     Test point y in view space.
template <PointContainer Points>
bool is_on_perimeter(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py);

extern template bool is_on_perimeter(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&,
                                     View2D const&, double, double);
extern template bool is_on_perimeter(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&,
                                     View2D const&, double, double);

/// @brief Winding-number point-in-polygon test projected through a View2D.
/// Works for both 2D (View2D::XY()) and 3D (dominant-axis view) rings.
/// Does NOT check the perimeter — callers handle that separately.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each vertex to 2D x/y coordinates.
/// @param px     Test point x in view space.
/// @param py     Test point y in view space.
template <PointContainer Points>
bool polygon_contains(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py);

extern template bool polygon_contains(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&,
                                      View2D const&, double, double);
extern template bool polygon_contains(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&,
                                      View2D const&, double, double);

/// @brief Distance between a polygon and an infinite line, both projected through @p view (the distance
/// is zero if they cross). Works entirely on view.x()/view.y() scalars — never materializes projected
/// Point2D copies of @p outer_loop, so a coplanar/parallel-to-plane 3D case (View2D::OnPlane(...)) costs
/// no more than the native 2D case (View2D::XY()).
/// @param outer_loop  Outer ring vertices (Point2D or Point3D).
/// @param is_convex   Caller's assertion that outer_loop is convex (enables the O(log n) fast path).
/// @param line_p0, line_p1  Two points defining the line, in the same coordinate space as outer_loop.
/// @param view        Projects each vertex (and the line) to 2D x/y coordinates.
template <PointContainer Points, Point P>
double distance_to(Points const& outer_loop, bool is_convex, P const& line_p0, P const& line_p1, View2D const& view);

extern template double distance_to(std::vector<Point2D> const&, bool, Point2D const&, Point2D const&, View2D const&);
extern template double distance_to(std::vector<Point3D> const&, bool, Point3D const&, Point3D const&, View2D const&);

/// @brief Left/Right tangent vertex indices from an external point p to a polygon loop, projected through @p view.
/// Uses Daniel Sunday's O(log n) binary search when @p is_convex; otherwise the loop is first reduced to its
/// convex hull (also projected through @p view — the tangent from an external point can only ever touch a
/// hull vertex) and the result is mapped back to an index into the original @p outer_loop.
/// @param outer_loop  Outer ring vertices (Point2D or Point3D).
/// @param is_convex   Caller's assertion that outer_loop is convex (enables the O(log n) fast path).
/// @param p           The external point, in the same coordinate space as outer_loop.
/// @param view        Projects each vertex (and p) to 2D x/y coordinates.
/// @pre p must be strictly outside outer_loop and not equal to any of its vertices.
template <PointContainer Points, Point P>
std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(Points const& outer_loop, bool is_convex, P const& p,
                                                             View2D const& view);

extern template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const&, bool,
                                                                             Point2D const&, View2D const&);
extern template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point3D> const&, bool,
                                                                             Point3D const&, View2D const&);

/// @brief RIGHT tangent index of loop1 paired with the LEFT tangent index of loop2 (the "RL" common outer
/// tangent), both projected through @p view. Neither loop needs to be convex — each is internally reduced to
/// its convex hull when its matching is_convex flag is false; returned indices are into the ORIGINAL loop1 /
/// loop2 (not the hull).
/// @return {loop1_index, loop2_index}. Swap the (loop, is_convex) argument pairs to get the LR tangent instead.
template <PointContainer Points>
std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(Points const& loop1, bool is_convex1, Points const& loop2,
                                                            bool is_convex2, View2D const& view);

extern template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const&, bool,
                                                                            std::vector<Point2D> const&, bool,
                                                                            View2D const&);
extern template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point3D> const&, bool,
                                                                            std::vector<Point3D> const&, bool,
                                                                            View2D const&);

/// @brief Computes the parametric intervals [t0, t1] on a line where it intersects a polygon.
/// @param outer_coplanar_ccw  Outer ring vertices in CCW winding order, all coplanar. Asserted in debug mode.
/// @param holes_coplanar_cw   Hole ring vertices in CW winding order, all coplanar. Asserted in debug mode.
/// @param is_convex_input     Caller's assertion that the polygon is convex (enables the fast convex path).
///                            Asserted in debug mode against the actual vertex data.
template <PointContainer Points, Point P>
std::vector<std::pair<double, double>> compute_parametric_intersection_intervals(
    Points const& outer_coplanar_ccw, std::vector<Points> const& holes_coplanar_cw, bool is_convex_input,
    P const& line_p0, P const& line_p1, View2D const& view);

/// @brief Concrete Point2D wrapper for compute_parametric_intersection_intervals.
/// The template definition lives in calc_utils/polygon_queries2d.cpp only; this non-template
/// declaration lets other TUs call it without triggering implicit instantiation
/// (which would fail because the template body is not in the header).
std::vector<std::pair<double, double>> compute_intersection_intervals_2d(
    std::vector<Point2D> const& outer_coplanar_ccw, std::vector<std::vector<Point2D>> const& holes_coplanar_cw,
    bool is_convex_input, Point2D const& line_p0, Point2D const& line_p1, View2D const& view);

/// @brief Concrete Point3D wrapper for compute_parametric_intersection_intervals — same reasoning as
/// compute_intersection_intervals_2d. Used for a line coplanar with the polygon (e.g. the shared line
/// between two non-coplanar polygons' planes, when computing where they strike through each other).
std::vector<std::pair<double, double>> compute_intersection_intervals_3d(
    std::vector<Point3D> const& outer_coplanar_ccw, std::vector<std::vector<Point3D>> const& holes_coplanar_cw,
    bool is_convex_input, Point3D const& line_p0, Point3D const& line_p1, View2D const& view);

}  // namespace view

double distance_to(std::vector<Point2D> const& outer_loop, bool is_convex, Point2D const& line_p0,
                   Point2D const& line_p1);

/// @brief finds the LEFT / RIGHT tangents of the point to the polygon (two different algorithms - depending on whether
/// the polygon is convex)
/// @return (left, right) indices of the outer_loop that form a Ray from the point to that polygon's index.
std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const& outer_loop, bool is_convex,
                                                             Point2D const& p);

/// @brief Returns the RL tangent of loop1 towards loop2. Neither loop needs to be convex — each is internally
/// reduced to its convex hull when the matching is_convex flag is false.
/// @return indices into the ORIGINAL loop1 / loop2 (not the hull) for the RL tangent between loop1 and loop2.
/// Swap the (loop, is_convex) argument pairs to get the LR tangent instead.
std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const& loop1, bool is_convex1,
                                                            std::vector<Point2D> const& loop2, bool is_convex2);

}  // namespace detail

/// @brief computes the distance between a polygon and a line (the distance is zero if they intersect)
double distance_to(Polygon2D const& polygon, Line2D const& line);

/// @brief Finds the left and right tangent vertices from a point to a convex polygon.
/// @pre The point 'p' MUST be strictly outside the polygon.
/// @pre The point 'p' MUST NOT be equal to any vertex of the polygon.
/// @note Complexity: O(log N)
/// @warning Violating preconditions results in undefined behavior (infinite loops or assertions).
PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Point2D const& p);

/// @brief finds the tangents from a polygon to another
PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Polygon2D const& other);

}  // namespace geometry

}  // namespace geompp
