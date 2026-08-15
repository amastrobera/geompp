#pragma once

#include "../generic_concepts.hpp"
#include "../line_segment2d.hpp"
#include "../point2d.hpp"
#include "../view2d.hpp"

#include <utility>
#include <vector>

namespace geompp {

inline namespace geometry {

namespace detail {

// Everything below projects points through a View2D (2D natively, or 3D via a dominant-axis / custom
// projection) before operating on them. Grouped together since they all share that one dependency.
namespace view {

/// @brief Flattens a ring set (outer + holes) into a raw 2D segment list, each vertex projected through
/// @p view. Shared by simplify_rings() and boolean_op() — the common first step before any crossing
/// detection happens.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each point to 2D x/y coordinates.
template <PointContainer Points>
std::vector<LineSegment2D> collect_ring_segments(Points const& outer, std::vector<Points> const& holes,
                                                 View2D const& view);

extern template std::vector<LineSegment2D> collect_ring_segments(std::vector<Point2D> const&,
                                                                 std::vector<std::vector<Point2D>> const&,
                                                                 View2D const&);
extern template std::vector<LineSegment2D> collect_ring_segments(std::vector<Point3D> const&,
                                                                 std::vector<std::vector<Point3D>> const&,
                                                                 View2D const&);

/// @brief Decomposes polygon rings into simple closed rings via half-edge face tracing.
/// Projects each point through @p view, builds 2D segments internally, finds all crossings
/// (Bentley-Ottmann), splits at those points, and traces one ring per face of the resulting planar
/// arrangement — every REAL bounded piece of @p outer's (possibly self-intersecting) shape, each already
/// oriented CCW (positive signed_area). The half-edge walk also always traces exactly one extra ring for
/// the arrangement's unbounded "outside" face (a topological certainty, not a caller-visible edge case) —
/// this function identifies and discards it internally (calibrated against @p outer's own projected
/// orientation, since a view that mirrors chirality — e.g. a Y-dominant-axis View2D — flips which absolute
/// sign means "interior"), so callers never see it and never need to re-derive this calibration themselves.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each point to 2D x/y coordinates.
/// @returns The real interior faces only, each CCW, in 2D (vertex sequence; closing vertex not repeated).
/// Empty if @p outer decomposes into no bounded area at all (fully degenerate/zero-area input).
template <PointContainer Points>
std::vector<std::vector<Point2D>> simplify_rings(Points const& outer, std::vector<Points> const& holes,
                                                 View2D const& view);

extern template std::vector<std::vector<Point2D>> simplify_rings(std::vector<Point2D> const&,
                                                                 std::vector<std::vector<Point2D>> const&,
                                                                 View2D const&);

extern template std::vector<std::vector<Point2D>> simplify_rings(std::vector<Point3D> const&,
                                                                 std::vector<std::vector<Point3D>> const&,
                                                                 View2D const&);

}  // namespace view

enum class BooleanOp { Union, Intersection, Difference, Xor };

/// @brief Set-theoretic boolean operation (union/intersection/difference/xor) between two polygons, each
/// given as an outer ring + hole rings, all already in 2D.
///
/// Internally merges both operands' edges into one segment pool, splits at every crossing
/// (split_segments_at_crossings), then classifies each surviving split segment individually: sample a
/// point just to its left and just to its right (a small nudge along the segment's normal), test both
/// samples for winding-number membership in subject and in clip, and apply @p op's truth table to each
/// side. A segment survives only where that truth-table result actually differs left vs. right — i.e.
/// where the segment is a genuine boundary of the result — and is kept oriented so the "in" side is on
/// its left. The survivors are then traced (half-edge walk, same angular rule as simplify_rings' face
/// tracer) into closed rings, and grouped into outer/hole pairs via a containment test.
///
/// Handles holes and self-intersecting operands (the winding-number membership test is well-defined for
/// self-intersecting input), and an operand fully containing the other with no shared boundary still
/// produces a correct hole.
///
/// Correctness depends on split having placed a vertex at every crossing. The underlying Bentley-Ottmann
/// sweep (find_intersections) used to occasionally MISS a genuine crossing — SweepLine2D's crossing
/// handler re-derived a just-swapped pair's position via the comparator at exactly the x where their
/// y-values are equal, the one point a non-transitive tie-break could hand it a wrong slot. Fixed by
/// SweepLine2D::ReverseRun, a direct O(1) positional swap that never consults the comparator at the
/// crossing x. See BooleanOp_MissedCrossing in test_polygon2d.cpp for the repro and full root-cause notes;
/// Randomized_DifferencePartitionsSubject and DISABLED_Randomized_ResultsAreSimple document the property-
/// test fallout. The latter has one still-open, unrelated failure (a self-intersecting-operand orientation
/// gap, not a sweep-ordering bug — see its own comment).
///
/// @returns each disjoint result component as {outer ring, hole rings}, in no particular order. Empty if
/// the operation produces no area (e.g. Intersection of disjoint polygons).
std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>> boolean_op(
    std::vector<Point2D> const& subj_outer, std::vector<std::vector<Point2D>> const& subj_holes,
    std::vector<Point2D> const& clip_outer, std::vector<std::vector<Point2D>> const& clip_holes, BooleanOp op);

/// @brief One operand of boolean_op_multi: a list of pieces, each already SIMPLE and correctly oriented
/// (CCW outer ring, CW hole rings) — what Polygon2D::Simplify() returns for a self-intersecting polygon,
/// or the polygon itself (as the sole piece) when it's already simple.
using RingPieces = std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>>;

/// @brief Multi-piece, source-tagged variant of boolean_op. Where boolean_op takes one (possibly
/// self-intersecting) outer+holes ring per operand and classifies every split segment by probing BOTH
/// operands left and right, this version requires each operand pre-decomposed into simple, correctly
/// oriented pieces (see RingPieces) — which resolves a self-intersecting operand's ambiguity of "which
/// side is interior" BEFORE classification, rather than during it. That buys an exactness/cost win: a
/// split segment's own-operand side is then known outright from the CCW-outer/CW-hole convention (no
/// probe needed), so classification only ever probes the OTHER operand, left and right — half the probes
/// of boolean_op, and no dependency on the nudge epsilon for the segment's own side. A same-operand
/// internal seam (two of one operand's pieces touching along a whole shared edge, not just a point) is
/// cancelled before classification so the "own side is always interior" assumption stays valid regardless
/// of how many pieces an operand decomposes into.
///
/// Shares the same crossing-detection dependency as boolean_op (see its docs) — this only changes how
/// surviving segments are classified, not find_intersections itself.
///
/// @returns each disjoint result component as {outer ring, hole rings}, in no particular order.
RingPieces boolean_op_multi(RingPieces const& subj_pieces, RingPieces const& clip_pieces, BooleanOp op);

}  // namespace detail

/// @brief Clips @p subject_loop against @p clipper_loop, returning the area both loops share (their set
/// intersection) — the classic "clip a subject polygon by a window polygon" operation, for callers who
/// have raw point loops rather than Polygon2D/Polygon3D instances (no holes, no CCW/CW requirement on
/// input). Works for both Point2D and Point3D loops.
///
/// @param clipper_loop  The clip region's vertices, in order. Last point must NOT repeat the first —
/// the loop is treated as implicitly closed (an edge connects the last vertex back to the first).
/// @param subject_loop  The subject's vertices, same "implicitly closed, no repeated first point"
/// convention.
/// @pre For Point3D input, @p clipper_loop and @p subject_loop must be coplanar — clipping is a set
/// intersection of two flat regions, which only means something on a single shared plane (the plane is
/// fitted from @p subject_loop's first three points). Not applicable to Point2D (already native 2D).
/// @throws std::invalid_argument if @p subject_loop has fewer than 3 points, or (Point3D only) if
/// @p clipper_loop is not coplanar with @p subject_loop.
/// @returns Every ring of the intersection, CCW outer rings and CW hole rings mixed in one flat list
/// (an intersection of two hole-less loops can still have a hole — e.g. two overlapping "L" shapes can
/// intersect into a shape with a hole in the middle — so the caller must be prepared for that; group by
/// signed_area()/orientation and nesting the same way simplify_rings()'s caller would). Empty if the
/// loops don't overlap.
///
/// Uses the same general planar-arrangement engine as Polygon2D::Intersection(Polygon2D) — no special
/// case for convex clippers (a convex-only caller could use the simpler/faster Sutherland-Hodgman
/// algorithm instead, but that's a different algorithm, not offered here).
template <PointContainer Points>
std::vector<Points> clip(Points const& clipper_loop, Points const& subject_loop);

extern template std::vector<std::vector<Point2D>> clip(std::vector<Point2D> const&, std::vector<Point2D> const&);
extern template std::vector<std::vector<Point3D>> clip(std::vector<Point3D> const&, std::vector<Point3D> const&);

}  // namespace geometry

}  // namespace geompp
