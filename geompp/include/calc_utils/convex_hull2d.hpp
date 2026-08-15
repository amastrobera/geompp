#pragma once

#include "../generic_concepts.hpp"
#include "../point2d.hpp"
#include "../view2d.hpp"

#include <cstddef>
#include <ranges>
#include <utility>
#include <vector>

namespace geompp {

inline namespace geometry {

class Polygon2D;
class Line2D;

/// @brief The pair of points on a shape that are extreme (least / greatest) along a given direction.
/// @tparam PointT Point2D or Point3D.
template <typename PointT>
struct ExtremePoints {
  PointT min_point;
  PointT max_point;
};

namespace detail {

// The three concepts below have exactly one consumer — extreme_points() right after them — so they live
// here instead of in a shared concepts file.

// A vector type must be able to dot with another vector of the same type, yielding a scalar.
template <typename T>
concept VectorType = requires(T v) {
  { v.Dot(v) } -> std::convertible_to<double>;
};

// Uses composition (&&) to inherit all requirements from the Point concept automatically.
template <typename P, typename V>
concept ProjectablePointWith = Point<P> && requires(P const& p, V const& v) {
  { p.ToVector().Dot(v) } -> std::convertible_to<double>;
};

// A container where elements are guaranteed to be projectable with V.
template <typename T, typename V>
concept ProjectablePointContainerWith = std::ranges::random_access_range<T> && std::ranges::sized_range<T> &&
                                        ProjectablePointWith<std::ranges::range_value_t<T>, V>;

/// @brief Indices of the two vertices extreme (least / greatest projection) along a direction.
/// @tparam V a vector type supporting Dot (Vector2D / Vector3D).
/// @tparam R a forward range of points, each supporting ToVector().Dot(V) (e.g. std::vector<Point2D/3D>).
/// @param vertices ordered polygon vertices (CCW). For the convex fast-path they must form a convex ring.
/// @param is_convex when true, uses Daniel Sunday's O(log n) binary search; otherwise an O(n) linear scan.
/// @param dir direction to measure extremeness along (e.g. a line's direction).
/// @returns {min_index, max_index} — indices into @p vertices of the least- and greatest-projected points.
/// @throws std::invalid_argument if @p vertices is empty.
///
/// The convex binary search only ever compares scalar projections proj(i) = vertices[i].ToVector().Dot(dir),
/// so it is dimension-agnostic: the same code drives Point2D and Point3D rings. Defined in
/// calc_utils/convex_hull2d.cpp; only the two instantiations below are available (extern template
/// suppresses implicit instantiation elsewhere, matching convex_hull_monotone_chain / is_convex in this
/// same header).
template <VectorType V, ProjectablePointContainerWith<V> R>
std::pair<std::size_t, std::size_t> extreme_points(R const& vertices, bool is_convex, V const& dir);

extern template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point2D> const&, bool, Vector2D const&);
extern template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point3D> const&, bool, Vector3D const&);

struct MinBoundingRectResult {
  double u_axis_x, u_axis_y;  // unit edge direction (in View2D space)
  double v_axis_x, v_axis_y;  // CCW perpendicular (in View2D space)
  double half_len_u, half_len_v;
  double center_u, center_v;  // center as offset from an arbitrary origin (in u,v local coords)
  // The center in the 2D projection is: origin + center_u * u_axis + center_v * v_axis
  // where origin is the first hull point (p0 of the best edge).
  // For the caller to recover the 2D center:
  //   center_2d_x = origin_x + center_u * u_axis_x + center_v * v_axis_x
  //   center_2d_y = origin_y + center_u * u_axis_y + center_v * v_axis_y
  double origin_x, origin_y;  // origin point (first point of best edge) in View2D x,y space
};

// Everything below projects points through a View2D (2D natively, or 3D via a dominant-axis / custom
// projection) before operating on them. Grouped together since they all share that one dependency.
namespace view {

// Note: Point2D is NOT declared here. Unqualified lookup walks up to namespace geompp
// and finds geompp::Point2D (which is fully defined via "point2d.hpp" above).
// Do NOT add 'class Point2D;' or 'using Point2D = ...' here — MSVC mangles alias
// names differently from the canonical type in explicit template instantiations.

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull (generic, index-based)
/// @param points random-access range of Point2D or Point3D
/// @param view   projects each point to 2D x/y coordinates
/// @returns list of indices into `points` that form the convex hull in CCW order
template <PointContainer Points>
std::vector<std::size_t> convex_hull_monotone_chain(Points const& points, View2D const& view);

extern template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point2D> const&, View2D const&);

extern template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point3D> const&, View2D const&);

/// @brief Whether a ring of points is wound counter-clockwise, via the sign of the shoelace/signed area
/// computed directly from the coordinates @p view projects to — the same building block behind
/// triangulate_impl's Winding check (2D: View2D::XY(); 3D: the polygon's own dominant-axis view).
/// @param points ring vertices (Point2D or Point3D), implicitly closed.
/// @param view   projects each vertex to 2D x/y coordinates.
template <PointContainer Points>
bool is_ccw(Points const& points, View2D const& view);

extern template bool is_ccw(std::vector<Point2D> const&, View2D const&);
extern template bool is_ccw(std::vector<Point3D> const&, View2D const&);

/// @brief Whether a ring of points has any collinear consecutive triplet, projected through @p view — the
/// building block behind triangulate_impl's Collinearity check. A duplicate vertex is just the degenerate
/// case of three collinear points (one of the two edge vectors in the cross-product test collapses to
/// zero), so this single test also catches duplicates without a separate check.
/// @param points ring vertices (Point2D or Point3D), implicitly closed.
/// @param view   projects each vertex to 2D x/y coordinates.
template <PointContainer Points>
bool has_collinears(Points const& points, View2D const& view);

extern template bool has_collinears(std::vector<Point2D> const&, View2D const&);
extern template bool has_collinears(std::vector<Point3D> const&, View2D const&);

/// @brief Core convexity check: all consecutive cross products have the same sign.
/// Does NOT check holes — callers are responsible for that guard.
template <PointContainer Points>
bool is_convex(Points const& vertices, View2D const& view);

extern template bool is_convex(std::vector<Point2D> const&, View2D const&);
extern template bool is_convex(std::vector<Point3D> const&, View2D const&);

/// @brief Whether a ring of points has no self-intersections, projected through @p view — the shared
/// building block behind geompp::is_simple() (called with View2D::XY() for the native-2D overload, or the
/// dominant-axis view of a given/fitted normal for the 3D overloads) and triangulate_impl's own per-view
/// simplicity check, so both go through the same Shamos-Hoey sweep rather than duplicating it.
/// @param points ring vertices (Point2D or Point3D), implicitly closed — no holes.
/// @param view   projects each vertex to 2D x/y coordinates.
template <PointContainer Points>
bool is_simple(Points const& points, View2D const& view);

extern template bool is_simple(std::vector<Point2D> const&, View2D const&);
extern template bool is_simple(std::vector<Point3D> const&, View2D const&);

/// @brief Rotating calipers (Freeman & Shapira 1975 / Toussaint 1983) on a convex hull.
/// Projects points through `view` into 2D, computes the minimum-area bounding rectangle.
/// Requires at least 3 non-degenerate points with a valid convex hull.
/// @param hull_indices indices of convex hull points in CCW order (from convex_hull_monotone_chain)
/// @param points the original point container
/// @param view 2D projection used for x/y extraction
/// @returns MinBoundingRectResult with axes, half-lengths, and center in View2D 2D space
template <PointContainer Points>
MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const& hull_indices, Points const& points,
                                        View2D const& view);

extern template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point2D> const&,
                                                        View2D const&);
extern template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point3D> const&,
                                                        View2D const&);

}  // namespace view

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull
/// @param points cloud of points
/// @returns list of indices of the points (from the original vector) that form a convex hull
/// @throws algorithm based throw logic
std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points);

/// @brief Returns true if a 2D polygon (CCW outer ring + optional holes) is convex.
/// A polygon with holes is never convex.
bool is_convex(std::vector<Point2D> const& vertices, std::vector<std::vector<Point2D>> const& holes);

}  // namespace detail

/// @brief Finds the two vertices of a polygon that are extreme (least / greatest projection) along a line.
/// Uses Daniel Sunday's O(log n) binary search when the polygon is convex, else an O(n) linear scan.
/// @param polygon The polygon whose vertices are searched (holes are ignored — only the outer ring matters).
/// @param line    The line whose Direction() defines the axis of projection.
/// @returns ExtremePoints{min_point, max_point} — the outer-ring vertices with least / greatest projection.
ExtremePoints<Point2D> find_extreme_points(Polygon2D const& polygon, Line2D const& line);

}  // namespace geometry

}  // namespace geompp
