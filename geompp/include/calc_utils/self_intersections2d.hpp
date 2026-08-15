#pragma once

#include "../constants.hpp"
#include "../line_segment2d.hpp"
#include "../point2d.hpp"
#include "../sweep_line2d.hpp"

#include <optional>
#include <vector>

namespace geompp {

inline namespace geometry {

namespace detail {

/// @brief Intersection of two infinite lines defined by two points each, returning parametric values.
/// @param p0, p1            Two points on the first line.
/// @param other_p0, other_p1  Two points on the second line.
/// @param sc  Output: parameter along the first line at the intersection.
/// @param tc  Output: parameter along the second line at the intersection.
/// @return The intersection point, or std::nullopt for parallel lines.
std::optional<Point2D> line_intersection(Point2D const& p0, Point2D const& p1, Point2D const& other_p0,
                                         Point2D const& other_p1, double& sc, double& tc);

/// @brief the Shamos-Hoey algorithm for checking polygon simplicity (no self-intersections).
/// Implemented as run_shamos_hoey() with a visitor that stops at the first crossing found.
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @returns true - if any intersection exists
/// @throws less than 2 segments arguments, or algorithm based throw logic
template <SegmentList Segments>
bool has_intersections(Segments const& segments);

/// @brief the Bentley-Ottmann algorithm for finding all intersection points among a set of segments
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @returns list of intersection points - in sorted order bottom-left to top-right (the intersecting 2+ segment IDs are
/// also reported)
/// @throws less than 2 segments arguments, or algorithm based throw logic
/// Implemented as run_bentley_ottmann() with a visitor that collects every crossing found.
template <SegmentList Segments>
std::vector<IntersectionEvent2D> find_intersections(Segments const& segments);

/// @brief Splits every segment in @p segs at each crossing point Bentley-Ottmann finds among them, so
/// that no two segments in the result cross except at shared endpoints — including collinear,
/// partially-overlapping pairs, split at their shared sub-segment's endpoints (find_intersections'
/// sweep-line visitor tries LineSegment2D::Intersection then falls back to LineSegment2D::Overlap for
/// exactly this case, at no extra asymptotic cost). Shared by simplify_rings() and boolean_op() — the
/// step that turns an arbitrary segment soup into one ready for half-edge face tracing.
std::vector<LineSegment2D> split_segments_at_crossings(std::vector<LineSegment2D> const& segs);

}  // namespace detail

/// @brief Builds the closed ring of edges connecting consecutive points, including the edge from the last
/// point back to the first — the free-function equivalent of Polygon2D::ToSegments(), for callers who have
/// a raw point loop rather than a Polygon2D instance.
/// @param points ring vertices, in order (must NOT repeat the first point at the end — the closing edge is
/// added automatically).
/// @throws std::runtime_error if two consecutive points (including the closing edge) are closer than the
/// current DECIMAL_PRECISION allows.
std::vector<LineSegment2D> to_segments(std::vector<Point2D> const& points);

/// @brief Whether a closed ring of points has no self-intersections — the free-function equivalent of
/// Polygon2D::IsSimple(), for a single loop with no holes.
/// @param points ring vertices, in order (implicitly closed).
/// @throws std::invalid_argument if fewer than 2 edges result (mirrors has_intersections()'s own guard).
bool is_simple(std::vector<Point2D> const& points);

}  // namespace geometry

}  // namespace geompp
