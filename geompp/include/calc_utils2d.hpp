#pragma once

#include "concepts.hpp"
#include "constants.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"
#include "view2d.hpp"

#include <compare>
#include <optional>
#include <queue>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <vector>

namespace geompp {

// Forward declarations for types only needed by reference in EventQueue2D constructors.
// Placed here (not inside detail) so name lookup from within detail finds geompp::Polygon2D
// and geompp::SegmentRange2D, not shadow types.
class Polygon2D;
class SegmentRange2D;
class Line2D;

/// @brief The pair of points on a shape that are extreme (least / greatest) along a given direction.
/// @tparam PointT Point2D or Point3D.
template <typename PointT>
struct ExtremePoints {
  PointT min_point;
  PointT max_point;
};

template <typename LineSegmentT>
struct PolygonTangents {
  LineSegmentT left;
  LineSegmentT right;
};

namespace detail {

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
/// so it is dimension-agnostic: the same code drives Point2D and Point3D rings. Defined in calc_utils2d.cpp;
/// only the two instantiations below are available (extern template suppresses implicit instantiation
/// elsewhere, matching convex_hull_monotone_chain / is_convex in this same header).
template <VectorType V, ProjectablePointContainerWith<V> R>
std::pair<std::size_t, std::size_t> extreme_points(R const& vertices, bool is_convex, V const& dir);

extern template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point2D> const&, bool, Vector2D const&);
extern template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point3D> const&, bool, Vector3D const&);

std::partial_ordering compare_event_point(Point2D a, Point2D b);  // for ordering events in the sweep line algorithm

enum class EventType2D {
  UNKN = -1,
  LEFT,
  INTERSECTION,
  RIGHT
};  // it's critical to respect the LEFT < INTERSECTION < RIGHT order for the event queue processing

struct Event2D {
  EventType2D Type;
  Point2D Point;
  std::size_t SegmentId;  // reference to the segment that generated the event (if EventType::LEFT or EventType::RIGHT)
  std::optional<std::size_t> InterSegmentId;  // if EventType::INTERSECTION this is the second segment involved in the
                                              // intersection, otherwise std::nullopt

  bool operator<(Event2D const& other) const;   // for the priority queue in EventQueue2D
  bool operator>(Event2D const& other) const;   // required by std::greater<Event2D> (EventMinHeap)
  bool operator==(Event2D const& other) const;  // for EventQueue2D::Contains
};

// EventQueue2D never needs the segments themselves: it builds the events once, then only ever manipulates
// integer SegmentIds. So it stays a plain (non-template) class with the three convenience constructors.
class EventQueue2D {
 public:
  EventQueue2D(std::vector<LineSegment2D> const& segments);
  EventQueue2D(SegmentRange2D const& segments);  // lazy view over a point sequence (polyline / polygon boundary)
  EventQueue2D(Polygon2D const& polygon);        // ideal case to check for self-intersections of a polygon
  ~EventQueue2D() = default;

  std::optional<Event2D> Top() const;
  std::optional<Event2D> Pop();
  bool Empty() const;
  bool Contains(Event2D const& event) const;  // true if the event queue contains an event for the given segment index
  void Push(Event2D const& event);            // simply adds the event and does not check if it is present already

 private:
  struct EventMinHeap : std::priority_queue<Event2D, std::vector<Event2D>, std::greater<Event2D>> {
    using std::priority_queue<Event2D, std::vector<Event2D>, std::greater<Event2D>>::priority_queue;
    using std::priority_queue<Event2D, std::vector<Event2D>, std::greater<Event2D>>::c;
  };
  EventMinHeap EVENTS;
};

template <SegmentList Segments>
struct SweepLineComparator {
  const double& sweep_x;     // Direct reference to the SweepLine's master variable
  Segments const* segments;  // bound from `Segments const&`, so the pointee is const

  bool operator()(std::size_t id1, std::size_t id2) const;

 private:
  double GetYAtX(LineSegment2D const& seg, double x) const;
};

// SweepLine2D is the one place that resolves event.SegmentId back to a full LineSegment2D, so it is templated
// on the concrete SegmentList it points at. Member functions are DECLARED here but DEFINED in calc_utils2d.cpp;
// the .cpp then explicitly instantiates SweepLine2D for the concrete SegmentList types (see bottom of the .cpp).
// Only those listed types may be used with SweepLine2D.
template <SegmentList Segments>
class SweepLine2D {
 public:
  // NOTE: stores a non-owning pointer — `segments` must outlive this SweepLine2D (the driver owns it).
  explicit SweepLine2D(Segments const& segments);
  ~SweepLine2D() = default;

  struct IdSegPair {
    std::size_t Id;
    LineSegment2D Seg;  // stored by value: SegmentRange2D::operator[] returns temporaries, pointers would dangle
  };

  struct SweepLineElement2D {
    std::optional<IdSegPair> Segment;
    std::optional<IdSegPair> Above;
    std::optional<IdSegPair> Below;
  };

  /// @brief
  /// @param seg_id index of the SegmentList [0, N-1]
  /// @returns (Segment, Above, Below) = pointers to the segment of index seg_id in the SegmentList,  the above and
  /// below segment pointers. If seg_id not found (std::nullopt, std::nullopt, std::nullopt)
  /// @throws std::out_of_range if seg_id not in [0, N-1] range, warning log if seg_id not found
  SweepLineElement2D Get(std::size_t seg_id) const;

  /// @brief Adds a segment in the tree, sorted
  /// @param seg_id index of the SegmentList [0, N-1]
  /// @returns (Segment = ptr to segment just inserted, Above / Below = ptr to above or below segments in the tree).
  /// Above and Below may be either or both std::nullopt. If seg_id not found (std::nullopt, std::nullopt, std::nullopt)
  /// @throws std::out_of_range if seg_id not in [0, N-1] range, std::logic_error if insertion in tree not possible,
  /// warning log if seg_id not found
  SweepLineElement2D Add(std::size_t seg_id);

  /// @brief Remove the seg_id from the tree, which remains sorted
  /// @param seg_id index of the SegmentList [0, N-1]
  /// @returns (Segment = std::nullopt, Above = ptr to above segment before deletion, Below = ptr to below segment
  /// before deletion). Either or both Above and Below can be std::nullopt. If seg_id not found (std::nullopt,
  /// std::nullopt, std::nullopt)
  /// @throws std::out_of_range if seg_id not in [0, N-1] range, std::logic_error if insertion in tree not possible,
  /// warning log if seg_id not found
  SweepLineElement2D Remove(std::size_t seg_id);

  /// @brief adjusts the current sweep x coordinate to a desired value, and lets the algorithms continue
  /// @param val usually the X of the next_event in the EventQueue.Pop() or the current X + EPSILON
  void SetX(double val);

  double GetX() const;

 private:
  double SWEEP_X;

  Segments const* PTR_SEGMENTS;  // bound from `Segments const&`, so the pointee is const

  std::vector<std::size_t> ACTIVE_SEGMENTS;  // indices of segments currently intersecting the sweep line, kept sorted
                                             // by geometric y at SWEEP_X; lower_bound gives O(log n) search
};

bool shares_endpoint(LineSegment2D const& a, LineSegment2D const& b);  // true if a and b share a First()/Last()

/// @brief Hooks invoked during the Shamos-Hoey sweep (see run_shamos_hoey). Both hooks return true to stop the
/// sweep immediately (short-circuit), false to keep scanning.
/// OnStart fires once a segment becomes active (its LEFT event). OnIntersection fires whenever two active,
/// non-endpoint-sharing neighbors on the sweep line are found to cross (checked at both LEFT and RIGHT events).
template <typename Visitor, typename Segments>
concept ShamosHoeyVisitor2D = requires(Visitor& v, typename SweepLine2D<Segments>::IdSegPair const& seg) {
  { v.OnStart(seg) }
  ->std::convertible_to<bool>;
  { v.OnIntersection(seg, seg) }
  ->std::convertible_to<bool>;
};

/// @brief The Shamos-Hoey sweep, generalized with a visitor so callers can implement different algorithms
/// (existence check, counting, collection, ...) on top of the same O(n log n) neighbor-adjacency scan.
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @param visitor  called at each LEFT event (OnStart) and at each detected neighbor crossing (OnIntersection);
/// the sweep stops as soon as either hook returns true.
/// @returns true if the sweep was stopped early by the visitor, false if the whole queue was drained.
/// @throws less than 2 segments arguments, or algorithm based throw logic
template <SegmentList Segments, typename Visitor>
requires ShamosHoeyVisitor2D<Visitor, Segments> bool run_shamos_hoey(Segments const& segments, Visitor& visitor) {
  if (segments.size() < 2) {
    throw std::invalid_argument("provided less than 2 segments, cannot check for intersections");
  }

  EventQueue2D event_queue(segments);
  SweepLine2D<Segments> sweep_line(segments);

  while (!event_queue.Empty()) {
    auto event_opt = event_queue.Pop();
    if (!event_opt.has_value()) {
      throw std::logic_error("Event queue is unexpectedly empty");
    }
    auto event = event_opt.value();

    sweep_line.SetX(event.Point.x());

    std::size_t seg_id = event.SegmentId;

    if (event.Type == EventType2D::LEFT) {
      auto triplet = sweep_line.Add(seg_id);
      if (!triplet.Segment) {
        throw std::logic_error("Could not find the segment corresponding to the LEFT event in the sweep line");
      }

      if (visitor.OnStart(*triplet.Segment)) {
        return true;
      }

      if (triplet.Above && !shares_endpoint(triplet.Segment->Seg, triplet.Above->Seg) &&
          intersect(triplet.Segment->Seg, triplet.Above->Seg) &&
          visitor.OnIntersection(*triplet.Segment, *triplet.Above)) {
        return true;
      }
      if (triplet.Below && !shares_endpoint(triplet.Below->Seg, triplet.Segment->Seg) &&
          intersect(triplet.Below->Seg, triplet.Segment->Seg) &&
          visitor.OnIntersection(*triplet.Below, *triplet.Segment)) {
        return true;
      }

    } else if (event.Type == EventType2D::RIGHT) {
      auto triplet = sweep_line.Remove(seg_id);

      if (triplet.Above && triplet.Below && !shares_endpoint(triplet.Below->Seg, triplet.Above->Seg) &&
          intersect(triplet.Below->Seg, triplet.Above->Seg) && visitor.OnIntersection(*triplet.Below, *triplet.Above)) {
        return true;
      }
    }
  }

  return false;
}

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

struct IntersectionEvent2D {
  Point2D Point;  // point of intersections, returned by the Bentley-Ottmann algorithm in `intersections` below;
  std::vector<std::size_t>
      SegmentIds;  // a point can be the intersection of 2 or more segments
                   // TODO: make a templated (or not) wrapper of std::vector to control it always has 2 elements

  bool operator<(IntersectionEvent2D const& other) const;
  bool operator==(IntersectionEvent2D const& other) const;
};

/// @brief Hooks invoked during the Bentley-Ottmann sweep (see run_bentley_ottmann).
/// OnStart/OnIntersection return true to stop the sweep immediately (short-circuit), false to keep
/// scanning. OnStart fires once a segment becomes active (its LEFT event). OnIntersection fires once
/// per confirmed crossing between two segments — possibly several times for the same point when 3+
/// segments meet there; the visitor owns whatever deduplication/collection it needs.
/// TestPair supplies the actual segment-pair predicate the sweep uses to decide whether two segments
/// that just became adjacent in the sweep-line status interact at all, and where: it returns 0 points
/// (no interaction), 1 (a transversal crossing), or 2 (collinear, overlapping — the shared sub-segment's
/// endpoints). Pulling this out of the algorithm and into the visitor means a visitor that wants
/// LineSegment2D::Intersection() alone, or Intersection() || Overlap() together, is a policy choice made
/// once per visitor — not a second, separately-implemented pass over the whole segment set. Because the
/// test only ever runs on pairs the sweep-line already brought adjacent, adding the Overlap() case costs
/// nothing asymptotically: still the same O((n+k) log n) the sweep already guarantees for crossings,
/// since two collinear overlapping segments are necessarily adjacent in the sweep-line ordering
/// throughout their shared x-range (same argument that makes the crossing case correct in the first
/// place).
template <typename Visitor, typename Segments>
concept BentleyOttmannVisitor2D = requires(Visitor& v, typename SweepLine2D<Segments>::IdSegPair const& seg,
                                           IntersectionEvent2D const& hit) {
  { v.OnStart(seg) }
  ->std::convertible_to<bool>;
  { v.OnIntersection(hit) }
  ->std::convertible_to<bool>;
  { v.TestPair(seg.Seg, seg.Seg) }
  ->std::convertible_to<std::vector<Point2D>>;
};

/// @brief The Bentley-Ottmann sweep, generalized with a visitor so callers can implement different algorithms
/// (collection, counting, early-exit, ...) and different segment-pair predicates (exact crossings only, or
/// crossings plus collinear overlap) on top of the same O((n+k) log n) scan.
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @param visitor  called at each LEFT event (OnStart), for each newly-adjacent pair (TestPair), and at
/// each confirmed crossing/overlap-endpoint (OnIntersection); the sweep stops as soon as OnStart or
/// OnIntersection returns true.
/// @returns true if the sweep was stopped early by the visitor, false if the whole queue was drained.
/// @throws less than 2 segments arguments, or algorithm based throw logic
template <SegmentList Segments, typename Visitor>
requires BentleyOttmannVisitor2D<Visitor, Segments> bool run_bentley_ottmann(Segments const& segments,
                                                                             Visitor& visitor) {
  if (segments.size() < 2) {
    throw std::invalid_argument("less than 2 segments provided, cannot check for intersections");
  }

  EventQueue2D event_queue(segments);
  SweepLine2D<Segments> sweep_line(segments);

  while (!event_queue.Empty()) {
    auto event_opt = event_queue.Pop();
    if (!event_opt.has_value()) {
      throw std::logic_error("Event queue is unexpectedly empty");
    }
    auto event = event_opt.value();

    // Advance SWEEP_X only for non-intersection events.
    // For INTERSECTION events the handler's own Remove→SetX(x+ε)→Add cycle advances SWEEP_X past
    // the crossing.  Advancing here would land exactly on the crossing x, where both segments have
    // equal y and the tiebreaker (id1 < id2) gives the wrong pre-crossing adjacency order, causing
    // Get() to mis-navigate the set and the adjacency check to fail spuriously.
    if (event.Type != EventType2D::INTERSECTION) {
      if (compare(sweep_line.GetX(), event.Point.x()) < 0) {
        sweep_line.SetX(event.Point.x());
      }
    }

    if (event.Type == EventType2D::LEFT) {
      auto elem = sweep_line.Add(event.SegmentId);
      if (!elem.Segment) {
        throw std::logic_error("Could not add the segment corresponding to the LEFT event in the sweep line");
      }

      if (visitor.OnStart(*elem.Segment)) {
        return true;
      }

      if (elem.Above && !shares_endpoint(elem.Segment->Seg, elem.Above->Seg)) {
        for (auto const& pt : visitor.TestPair(elem.Segment->Seg, elem.Above->Seg)) {
          event_queue.Push(Event2D{EventType2D::INTERSECTION, pt, elem.Segment->Id, elem.Above->Id});
        }
      }

      if (elem.Below && !shares_endpoint(elem.Below->Seg, elem.Segment->Seg)) {
        for (auto const& pt : visitor.TestPair(elem.Below->Seg, elem.Segment->Seg)) {
          event_queue.Push(Event2D{EventType2D::INTERSECTION, pt, elem.Below->Id, elem.Segment->Id});
        }
      }

    } else if (event.Type == EventType2D::RIGHT) {
      auto elem = sweep_line.Get(event.SegmentId);
      if (!elem.Segment) {  // impossible at this stage
        throw std::logic_error("Could not find the segment corresponding to the RIGHT event in the sweep line");
      }

      auto above_elem = elem.Above;
      auto below_elem = elem.Below;

      sweep_line.Remove(event.SegmentId);  // automatically resets the above/below neighbours of the segment being
                                           // removed to the new neighbours after removal

      if (above_elem && below_elem && !shares_endpoint(above_elem->Seg, below_elem->Seg)) {
        for (auto const& pt : visitor.TestPair(above_elem->Seg, below_elem->Seg)) {
          auto inter_event = Event2D{EventType2D::INTERSECTION, pt, below_elem->Id, above_elem->Id};
          if (!event_queue.Contains(inter_event)) {
            event_queue.Push(inter_event);
          }
        }
      }

    } else if (event.Type == EventType2D::INTERSECTION) {
      std::size_t seg1_id = event.SegmentId;
      std::size_t seg2_id = event.InterSegmentId.value();  // guaranteed from the logic above (and .value()
                                                           // automatically throws std::bad_optional_access if empty)

      auto inter_event = IntersectionEvent2D{event.Point, {seg1_id, seg2_id}};

      if (visitor.OnIntersection(inter_event)) {
        return true;
      }

      // in the logic LEFT, and RIGHT I have guaranteed to always have seg1 < seg2 in Event{INTERSECTION, seg1, seg2}
      // at this point: segB < seg1 < seg2 < segA

      // in order to move the segments (seg1 -> up, seg2 -> down) we have to
      // (1) Remove them
      // (2) SetX
      // (3) Add them back in the queue
      // (4) check the new above/below intersections
      // ... here we go.

      // Skip the swap if this crossing is already behind the sweep line.  This happens with concurrent
      // intersections: the first pair advances sweep_x to x+ε; all subsequent pairs at the same x are
      // already in the past and must not be re-swapped (doing so would cycle back to already-processed
      // pairs and loop indefinitely).
      if (inter_event.Point.x() < sweep_line.GetX()) {
        continue;
      }

      // verify seg1 and seg2 are still adjacent — a stale event (queued before another segment was inserted
      // between them) must be skipped to avoid corrupting sweep line order
      auto seg1_check = sweep_line.Get(seg1_id);
      if (!seg1_check.Segment || !seg1_check.Above || seg1_check.Above->Id != seg2_id) {
        continue;
      }

      // (1) Remove segments (save the neighbors for later)
      sweep_line.Remove(seg1_id);
      sweep_line.Remove(seg2_id);

      // (2) set X to a bigger value (according to the decimal precision)
      if (compare(sweep_line.GetX(), inter_event.Point.x()) <= 0) {
        sweep_line.SetX(inter_event.Point.x() + DOUBLE_EPSILON);
      }

      // (3) add the segments back
      auto new_seg1 = sweep_line.Add(seg1_id);
      if (!new_seg1.Segment) {
        throw std::logic_error("Could not add the segment corresponding NEW SEG1 in the sweep line");
      }
      auto new_seg2 = sweep_line.Add(seg2_id);
      if (!new_seg2.Segment) {
        throw std::logic_error("Could not add the segment corresponding NEW SEG2 in the sweep line");
      }

      // (4) check the new above/below intersections
      // now : segB < seg2 < seg1 < segA
      if (new_seg1.Above && !shares_endpoint(new_seg1.Segment->Seg, new_seg1.Above->Seg)) {
        for (auto const& pt : visitor.TestPair(new_seg1.Segment->Seg, new_seg1.Above->Seg)) {
          auto inter_ev = Event2D{EventType2D::INTERSECTION, pt, new_seg1.Segment->Id, new_seg1.Above->Id};
          if (!event_queue.Contains(inter_ev)) {
            event_queue.Push(inter_ev);
          }
        }
      }

      if (new_seg2.Below && !shares_endpoint(new_seg2.Below->Seg, new_seg2.Segment->Seg)) {
        for (auto const& pt : visitor.TestPair(new_seg2.Below->Seg, new_seg2.Segment->Seg)) {
          auto inter_ev = Event2D{EventType2D::INTERSECTION, pt, new_seg2.Below->Id, new_seg2.Segment->Id};
          if (!event_queue.Contains(inter_ev)) {
            event_queue.Push(inter_ev);
          }
        }
      }
    }
  }

  return false;
}

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
/// KNOWN LIMITATION: correctness depends on split having placed a vertex at every crossing. The
/// underlying Bentley-Ottmann sweep (find_intersections) occasionally MISSES a genuine crossing on
/// certain configurations, which leaves a fragment straddling the other operand's boundary and corrupts
/// the result (a whole component may be dropped). This is rare but catastrophic when it hits; it is a
/// sweep bug upstream of the classification. See the DISABLED_Randomized_* property tests and the
/// DISABLED_BooleanOp_MissedCrossing repro in test_polygon2d.cpp, which document it for a future fix.
///
/// @returns each disjoint result component as {outer ring, hole rings}, in no particular order. Empty if
/// the operation produces no area (e.g. Intersection of disjoint polygons).
std::vector<std::pair<std::vector<Point2D>, std::vector<std::vector<Point2D>>>> boolean_op(
    std::vector<Point2D> const& subj_outer, std::vector<std::vector<Point2D>> const& subj_holes,
    std::vector<Point2D> const& clip_outer, std::vector<std::vector<Point2D>> const& clip_holes, BooleanOp op);

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

/// @brief Core convexity check: all consecutive cross products have the same sign.
/// Does NOT check holes — callers are responsible for that guard.
template <PointContainer Points>
bool is_convex(Points const& vertices, View2D const& view);

extern template bool is_convex(std::vector<Point2D> const&, View2D const&);
extern template bool is_convex(std::vector<Point3D> const&, View2D const&);

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
/// (Bentley-Ottmann), splits at those points, and returns one ring per bounded face.
/// Caller classifies outers vs holes via signed_area.
/// @param outer  Outer ring vertices (Point2D or Point3D).
/// @param holes  Inner ring vertices (same type as outer).
/// @param view   Projects each point to 2D x/y coordinates.
/// @returns Closed rings in 2D (vertex sequence; closing vertex not repeated). Must be >= 3 total edges.
template <PointContainer Points>
std::vector<std::vector<Point2D>> simplify_rings(Points const& outer, std::vector<Points> const& holes,
                                                 View2D const& view);

extern template std::vector<std::vector<Point2D>> simplify_rings(std::vector<Point2D> const&,
                                                                 std::vector<std::vector<Point2D>> const&,
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
/// The template definition lives in calc_utils2d.cpp only; this non-template
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

}  // namespace view

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull
/// @param points cloud of points
/// @returns list of indices of the points (from the original vector) that form a convex hull
/// @throws algorithm based throw logic
std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points);

/// @brief Winding-number contribution of a single ring (vertices) around point p.
/// @returns winding number increment/decrement for the ring
int winding_number(std::vector<Point2D> const& vertices, Point2D const& p);

/// @brief Returns true if a 2D polygon (CCW outer ring + optional holes) is convex.
/// A polygon with holes is never convex.
bool is_convex(std::vector<Point2D> const& vertices, std::vector<std::vector<Point2D>> const& holes);

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

/// @brief Finds the two vertices of a polygon that are extreme (least / greatest projection) along a line.
/// Uses Daniel Sunday's O(log n) binary search when the polygon is convex, else an O(n) linear scan.
/// @param polygon The polygon whose vertices are searched (holes are ignored — only the outer ring matters).
/// @param line    The line whose Direction() defines the axis of projection.
/// @returns ExtremePoints{min_point, max_point} — the outer-ring vertices with least / greatest projection.
ExtremePoints<Point2D> find_extreme_points(Polygon2D const& polygon, Line2D const& line);

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
/// sample_quadratic_bezier in calc_utils2d.cpp). p1 itself is not part of the result (it is replaced by the
/// arc) unless `smoothness` is 0, in which case every sample collapses to p1 (no smoothing).
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
/// calc_utils2d.cpp). Note: a bare `int` argument always resolves here and a bare `double` always resolves
/// to the min_distance overload (exact type match beats either implicit conversion); any other numeric type
/// (e.g. std::size_t) is ambiguous between the two and requires an explicit cast at the call site.
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

}  // namespace geompp
