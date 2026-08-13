#pragma once

#include "constants.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"

#include <compare>
#include <concepts>
#include <cstddef>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace geompp {

inline namespace geometry {

// Forward declarations for types only needed by reference in EventQueue2D constructors.
// Placed here (not inside detail) so name lookup from within detail finds geompp::Polygon2D
// and geompp::SegmentRange2D, not shadow types.
class Polygon2D;
class SegmentRange2D;

namespace detail {

// Only consumer is this file's own sweep-line engine (EventQueue2D, SweepLine2D, run_shamos_hoey,
// run_bentley_ottmann) plus calc_utils2d's has_intersections()/find_intersections(), which already include
// this header — so the concept lives here rather than in a shared concepts file.
// any container that exposes a count and indexed access to LineSegment2D-like elements
template <typename Segments>
concept SegmentList = requires(Segments const& s, std::size_t i) {
  { s.size() } -> std::convertible_to<std::size_t>;
  { s[i].First() } -> std::convertible_to<Point2D>;
  { s[i].Last() } -> std::convertible_to<Point2D>;
};

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
// on the concrete SegmentList it points at. Member functions are DECLARED here but DEFINED in sweep_line2d.cpp;
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

  struct ReverseRunResult2D {
    bool Ok = false;                     // false: seg_ids were not found contiguously adjacent (stale event,
                                         // caller should skip) — every other field is nullopt when false
    std::optional<IdSegPair> NewTop;     // the run's new topmost segment (== the run's old bottommost)
    std::optional<IdSegPair> NewBottom;  // the run's new bottommost segment (== the run's old topmost)
    std::optional<IdSegPair> AboveRun;   // neighbor above the whole run — unmoved by the reversal
    std::optional<IdSegPair> BelowRun;   // neighbor below the whole run — unmoved by the reversal
  };

  /// @brief Reverses, in place, the contiguous run of currently-active segments named by @p seg_ids — the
  /// O(1)-per-swap replacement for the old Remove/SetX(x+eps)/Add cycle used to react to a confirmed
  /// crossing. A 2-element @p seg_ids is the common pairwise-crossing case (equivalent to swapping two
  /// adjacent elements); 3+ elements handles several segments meeting at exactly one point in a single
  /// reversal instead of a cascade of pairwise swaps that could desync mid-cascade (reversing [A,B,C] to
  /// [C,B,A] realizes all three pairwise crossings (A,B), (B,C), and (A,C) at once — the pairs (A,B) and
  /// (B,C) were already tested when they first became adjacent, so no pair inside the run needs retesting,
  /// only the two new outer pairs this call reports via AboveRun/BelowRun).
  /// Locates the run with exactly ONE comparator-driven lookup (for an arbitrary anchor id from @p
  /// seg_ids), then expands outward by direct vector indexing (no further comparator calls) to confirm the
  /// rest of @p seg_ids occupy the immediately-adjacent slots. Every id in @p seg_ids is already active
  /// (unlike Add(), which places a segment the comparator has never seen before), so the anchor lookup's
  /// existing exact-id fallback (see Get()) makes this safe even if SWEEP_X has gone stale relative to the
  /// true post-crossing arrangement: the comparator can only fail to find the anchor (triggering the
  /// linear-scan fallback), never report a wrong id as a match, since ids are unique in ACTIVE_SEGMENTS.
  /// @param seg_ids the ids expected to form one contiguous run, in any order; must have size() >= 2.
  /// @throws std::invalid_argument if seg_ids.size() < 2.
  ReverseRunResult2D ReverseRun(std::vector<std::size_t> const& seg_ids);

  /// @brief adjusts the current sweep x coordinate to a desired value, and lets the algorithms continue
  /// @param val usually the X of the next_event in the EventQueue.Pop() or the current X + EPSILON
  void SetX(double val);

  double GetX() const;

 private:
  // Locates seg_id's current index in ACTIVE_SEGMENTS via lower_bound (comparator at current SWEEP_X),
  // falling back to a linear scan if the comparator doesn't land exactly on seg_id. Since ids are unique,
  // an exact match from lower_bound is always trustworthy regardless of whether SWEEP_X / the comparator's
  // ordering is currently a perfect fit for the whole vector — the fallback is what makes that safe.
  std::optional<std::size_t> FindIndex(std::size_t seg_id) const;

  double SWEEP_X;

  Segments const* PTR_SEGMENTS;  // bound from `Segments const&`, so the pointee is const

  std::vector<std::size_t> ACTIVE_SEGMENTS;  // indices of segments currently intersecting the sweep line, kept sorted
                                             // by geometric y at SWEEP_X; lower_bound gives O(log n) search
};

bool shares_endpoint(LineSegment2D const& a, LineSegment2D const& b);  // true if a and b share a First()/Last()

/// @brief Hooks invoked during the Shamos-Hoey sweep (see run_shamos_hoey). OnStart/OnIntersection return
/// true to stop the sweep immediately (short-circuit), false to keep scanning.
/// OnStart fires once a segment becomes active (its LEFT event). IsIntersecting supplies the actual
/// segment-pair predicate the sweep uses to decide whether two segments that just became adjacent in the
/// sweep-line status count as intersecting at all — mirroring BentleyOttmannVisitor2D's TestPair, this is
/// entirely the visitor's call, including whatever it decides about segments that merely share an endpoint
/// (most visitors will want to exclude those via shares_endpoint(), but the algorithm no longer forces
/// that choice). OnIntersection fires whenever IsIntersecting returns true for a checked pair (checked at
/// both LEFT and RIGHT events).
template <typename Visitor, typename Segments>
concept ShamosHoeyVisitor2D = requires(Visitor& v, typename SweepLine2D<Segments>::IdSegPair const& seg) {
  { v.OnStart(seg) }
  ->std::convertible_to<bool>;
  { v.IsIntersecting(seg.Seg, seg.Seg) }
  ->std::convertible_to<bool>;
  { v.OnIntersection(seg, seg) }
  ->std::convertible_to<bool>;
};

/// @brief The Shamos-Hoey sweep, generalized with a visitor so callers can implement different algorithms
/// (existence check, counting, collection, ...) and different segment-pair predicates on top of the same
/// O(n log n) neighbor-adjacency scan.
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @param visitor  called at each LEFT event (OnStart), for each newly-adjacent pair (IsIntersecting), and
/// at each confirmed intersection (OnIntersection); the sweep stops as soon as OnStart or OnIntersection
/// returns true.
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

      if (triplet.Above && visitor.IsIntersecting(triplet.Segment->Seg, triplet.Above->Seg) &&
          visitor.OnIntersection(*triplet.Segment, *triplet.Above)) {
        return true;
      }
      if (triplet.Below && visitor.IsIntersecting(triplet.Below->Seg, triplet.Segment->Seg) &&
          visitor.OnIntersection(*triplet.Below, *triplet.Segment)) {
        return true;
      }

    } else if (event.Type == EventType2D::RIGHT) {
      auto triplet = sweep_line.Remove(seg_id);

      if (triplet.Above && triplet.Below && visitor.IsIntersecting(triplet.Below->Seg, triplet.Above->Seg) &&
          visitor.OnIntersection(*triplet.Below, *triplet.Above)) {
        return true;
      }
    }
  }

  return false;
}

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
/// place). TestPair is called for EVERY newly-adjacent pair, including ones that share an endpoint — the
/// algorithm no longer filters those out itself (see shares_endpoint()); a visitor that wants the usual
/// "adjacent-in-the-polygon, not a real crossing" exclusion calls shares_endpoint() inside its own
/// TestPair, same as CollectIntersectionsVisitor2D does.
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

      if (elem.Above) {
        for (auto const& pt : visitor.TestPair(elem.Segment->Seg, elem.Above->Seg)) {
          event_queue.Push(Event2D{EventType2D::INTERSECTION, pt, elem.Segment->Id, elem.Above->Id});
        }
      }

      if (elem.Below) {
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

      if (above_elem && below_elem) {
        for (auto const& pt : visitor.TestPair(above_elem->Seg, below_elem->Seg)) {
          auto inter_event = Event2D{EventType2D::INTERSECTION, pt, below_elem->Id, above_elem->Id};
          if (!event_queue.Contains(inter_event)) {
            event_queue.Push(inter_event);
          }
        }
      }

    } else if (event.Type == EventType2D::INTERSECTION) {
      // Same-point INTERSECTION events sort together (Event2D::operator< orders by point before type), so
      // every queued intersection at this exact point is already contiguous at the front of the queue.
      // Gather them all before touching the sweep line: 2 segments crossing gives exactly one (below,
      // above) pair; 3+ segments meeting at one point give the several adjacent pairs whose ids' union is
      // the whole run — reacting to them one at a time (the old Remove/SetX/Add cascade) is what could
      // desync mid-cascade, since resolving the first pair changes adjacency for the rest.
      std::vector<std::pair<std::size_t, std::size_t>> pairs_here;  // each is (below_id, above_id)
      std::set<std::size_t> ids_here;

      auto collect = [&](Event2D const& ev) {
        std::size_t below_id = ev.SegmentId;
        std::size_t above_id = ev.InterSegmentId.value();  // guaranteed by construction of INTERSECTION events
        pairs_here.emplace_back(below_id, above_id);
        ids_here.insert(below_id);
        ids_here.insert(above_id);
      };
      collect(event);

      while (true) {
        auto next = event_queue.Top();
        if (!next || next->Type != EventType2D::INTERSECTION || !(next->Point == event.Point)) {
          break;
        }
        collect(event_queue.Pop().value());
      }

      for (auto const& [below_id, above_id] : pairs_here) {
        if (visitor.OnIntersection(IntersectionEvent2D{event.Point, {below_id, above_id}})) {
          return true;
        }
      }

      // A pair can be rediscovered here even though it already crossed: TestPair is purely geometric (two
      // non-parallel segments have exactly one crossing, full stop — it has no notion of "already
      // resolved"), and a later reversal elsewhere in the run can put two already-crossed segments back
      // into direct adjacency, so the "new outer pair" test below re-finds their old crossing point. Reject
      // anything behind where the sweep has already reached: this is the same guard the old Remove/SetX
      // (x+ε)/Add cycle got for free by construction (a stale crossing's x could never be ahead of the just
      // -advanced sweep), now made explicit since nothing here calls SetX on the hot path anymore.
      if (compare(event.Point.x(), sweep_line.GetX()) < 0) {
        continue;
      }

      // Reverse the whole run in one O(run size) positional swap — no comparator consulted at the
      // crossing x, so no risk of the tie-break picking the wrong slot right where two segments' y-values
      // are equal. A false Ok means the run isn't (or is no longer) contiguous — a stale event from before
      // some other segment was inserted between them — so it's skipped, same as the old adjacency check
      // did for the pairwise case.
      std::vector<std::size_t> run(ids_here.begin(), ids_here.end());
      if (run.size() < 2) {
        continue;
      }
      auto swap_result = sweep_line.ReverseRun(run);
      if (!swap_result.Ok) {
        continue;
      }

      // Advance past this point so a later rediscovery of an already-applied crossing (see above) gets
      // rejected by the guard at the top of this branch instead of re-swapping and looping forever.
      if (compare(sweep_line.GetX(), event.Point.x()) < 0) {
        sweep_line.SetX(event.Point.x());
      }

      // Only the two new OUTER pairs need testing: every pair inside the run was already adjacent (and
      // therefore already tested) before the reversal, just in the opposite order.
      if (swap_result.AboveRun) {
        for (auto const& pt : visitor.TestPair(swap_result.NewTop->Seg, swap_result.AboveRun->Seg)) {
          auto inter_ev = Event2D{EventType2D::INTERSECTION, pt, swap_result.NewTop->Id, swap_result.AboveRun->Id};
          if (!event_queue.Contains(inter_ev)) {
            event_queue.Push(inter_ev);
          }
        }
      }

      if (swap_result.BelowRun) {
        for (auto const& pt : visitor.TestPair(swap_result.BelowRun->Seg, swap_result.NewBottom->Seg)) {
          auto inter_ev = Event2D{EventType2D::INTERSECTION, pt, swap_result.BelowRun->Id, swap_result.NewBottom->Id};
          if (!event_queue.Contains(inter_ev)) {
            event_queue.Push(inter_ev);
          }
        }
      }
    }
  }

  return false;
}

}  // namespace detail
}  // namespace geometry

}  // namespace geompp
