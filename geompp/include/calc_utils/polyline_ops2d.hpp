#pragma once

#include "../constants.hpp"
#include "../generic_concepts.hpp"
#include "../point2d.hpp"
#include "../point3d.hpp"

#include <vector>

namespace geompp {

inline namespace geometry {

template <PointContainer Points>
Points dist_decimation(Points const& points, double threshold);

extern template std::vector<Point2D> dist_decimation(std::vector<Point2D> const&, double threshold);
extern template std::vector<Point3D> dist_decimation(std::vector<Point3D> const&, double threshold);

template <PointContainer Points>
Points rdp_decimation(Points const& points, double threshold);

extern template std::vector<Point2D> rdp_decimation(std::vector<Point2D> const&, double threshold);
extern template std::vector<Point3D> rdp_decimation(std::vector<Point3D> const&, double threshold);

template <PointContainer Points>
Points vw_decimation(Points const& points, double threshold);

extern template std::vector<Point2D> vw_decimation(std::vector<Point2D> const&, double threshold);
extern template std::vector<Point3D> vw_decimation(std::vector<Point3D> const&, double threshold);

/// @brief Rounds the corner at p1 with a quadratic Bezier arc tangent to p0-p1 and p1-p2, density-sampled.
/// The tangent points are trimmed in from p1 by up to `smoothness` fraction of the shorter adjacent edge,
/// then the arc between them is sampled roughly `min_distance` apart (see bezier_trimmed_tangents /
/// sample_quadratic_bezier in calc_utils/polyline_ops2d.cpp). p1 itself is not part of the result (it is
/// replaced by the arc) unless `smoothness` is 0, in which case every sample collapses to p1 (no smoothing).
/// @param p0 point before the corner.
/// @param p1 the corner being smoothed.
/// @param p2 point after the corner.
/// @param smoothness in [0, 1]: fraction of the shorter adjacent edge (p0-p1 or p1-p2) to trim into tangent
/// points. 0 leaves the corner sharp (every sample collapses to p1); 1 trims half of the shorter edge.
/// @param min_distance target spacing between consecutive sampled points along the arc.
/// @param min_segment_length skip trimming on a side whose adjacent edge (p0-p1 or p1-p2) is at or below
/// this length — that tangent point collapses to p1 instead (same fallback as the exact-zero-length guard).
/// Defaults to DOUBLE_EPSILON (matching PolylineExpansionParams::min_segment_length and this codebase's
/// usual epsilon-parameter default), so an edge indistinguishable from zero at the current
/// DECIMAL_PRECISION is never trimmed into, without the caller having to opt in. Note the effective
/// threshold is `min_segment_length + DOUBLE_EPSILON`, not exactly `min_segment_length` — the internal
/// `compare()` used for the check has its own DOUBLE_EPSILON-wide tolerance band on top of whatever value
/// is passed here. If *both* adjacent edges are at or below it, the whole corner collapses to p1 (no
/// curve at all — every sample is p1), giving the caller a way to skip smoothing tiny/noisy corners
/// entirely (pass 0.0 to only skip on an edge that's truly, exactly zero-length).
/// @returns points sampled from the tangent point near p0 to the tangent point near p2, inclusive.
/// @throws std::invalid_argument if min_distance <= 0.
template <typename PointT>
std::vector<PointT> bezier_smoothing_2(PointT p0, PointT p1, PointT p2, double smoothness, double min_distance,
                                       double min_segment_length = DOUBLE_EPSILON);

extern template std::vector<Point2D> bezier_smoothing_2(Point2D p0, Point2D p1, Point2D p2, double smoothness,
                                                        double min_distance, double min_segment_length);
extern template std::vector<Point3D> bezier_smoothing_2(Point3D p0, Point3D p1, Point3D p2, double smoothness,
                                                        double min_distance, double min_segment_length);

/// @brief Same as bezier_smoothing_2(p0, p1, p2, smoothness, min_distance), but takes an exact sample count
/// instead of a distance-derived one. Kept as a distinct overload rather than a parameter that reinterprets
/// min_distance — the two controls don't share a unit or a precedence rule (see the design note in
/// calc_utils/polyline_ops2d.cpp). Note: a bare `int` argument always resolves here and a bare `double`
/// always resolves to the min_distance overload (exact type match beats either implicit conversion); any
/// other numeric type (e.g. std::size_t) is ambiguous between the two and requires an explicit cast at the
/// call site.
/// @param num_segments number of segments to divide the arc into; the result has num_segments + 1 points.
/// @param min_segment_length see the min_distance overload — same skip-trim/skip-corner behavior and
/// DOUBLE_EPSILON default.
/// @returns num_segments + 1 points sampled evenly from the tangent point near p0 to the tangent point near p2.
/// @throws std::invalid_argument if num_segments < 1.
template <typename PointT>
std::vector<PointT> bezier_smoothing_2(PointT p0, PointT p1, PointT p2, double smoothness, int num_segments,
                                       double min_segment_length = DOUBLE_EPSILON);

extern template std::vector<Point2D> bezier_smoothing_2(Point2D p0, Point2D p1, Point2D p2, double smoothness,
                                                        int num_segments, double min_segment_length);
extern template std::vector<Point3D> bezier_smoothing_2(Point3D p0, Point3D p1, Point3D p2, double smoothness,
                                                        int num_segments, double min_segment_length);

/// @brief Rounds every inner corner of @p input (index 1 through size()-2) with a quadratic Bezier arc via
/// bezier_smoothing_2 — the shared implementation behind Polyline2D::Expand() / Polyline3D::Expand(). The
/// true first/last points of @p input are always preserved unsmoothed. p0/p1/p2 for each corner are always
/// read from @p input directly, never from the output already built up: bezier_smoothing_2's trim is
/// bounded by the *true* adjacent edge lengths (each corner's trim is independently capped at half of its
/// shared edge with a neighbor, so adjacent corners' arcs can touch but never cross), and substituting an
/// already-trimmed output point would both shrink that apparent edge length (under-trimming) and stop p1
/// from being the actual corner vertex being rounded.
/// @param input the knots to round; must have at least 1 point (0 and 1-point input is a no-op).
/// @param settings bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to
/// `{0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON}`.
/// @returns @p input with every inner corner replaced by its rounded arc; consecutive duplicate points
/// (e.g. from a corner fully skipped via min_segment_length) are collapsed to one.
template <typename PointT>
std::vector<PointT> polyline_expansion(std::vector<PointT> const& input,
                                       PolylineExpansionParams const& settings = PolylineExpansionParams{});

extern template std::vector<Point2D> polyline_expansion(std::vector<Point2D> const& input,
                                                        PolylineExpansionParams const& settings);
extern template std::vector<Point3D> polyline_expansion(std::vector<Point3D> const& input,
                                                        PolylineExpansionParams const& settings);

}  // namespace geometry

}  // namespace geompp
