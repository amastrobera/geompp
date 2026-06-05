#pragma once

#include "constants.hpp"
#include "point2d.hpp"

#include <compare>
#include <concepts>
#include <optional>
#include <queue>
#include <set>
#include <vector>

namespace geompp {

class Polygon2D;
class LineSegment2D;
class SegmentRange2D;

std::partial_ordering compare_event_point(Point2D a, Point2D b);  // for ordering events in the sweep line algorithm

// any container that exposes a count and indexed access to LineSegment2D-like elements
template <typename Segments>
concept SegmentList = requires(Segments const& s, std::size_t i) {
  { s.size() } -> std::convertible_to<std::size_t>;
  { s[i].First() } -> std::convertible_to<Point2D>;  // First()/Last() return Point2D const&
  { s[i].Last() } -> std::convertible_to<Point2D>;
};

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

  bool operator<(Event2D const& other) const;  // for the priority quene in EventQueue2D
};

// EventQueue2D never needs the segments themselves: it builds the events once, then only ever manipulates
// integer SegmentIds. So it stays a plain (non-template) class with the three convenience constructors.
class EventQueue2D {
 public:
  EventQueue2D(std::vector<LineSegment2D> const& segments);
  EventQueue2D(SegmentRange2D const& segments);  // lazy view over a point sequence (polyline / polygon boundary)
  EventQueue2D(Polygon2D const& polygon);        // ideal case to check for self-intersections of a polygon
  ~EventQueue2D() = default;

  std::optional<Event2D> Next();
  bool Empty() const;
  bool Contains(Event2D const& event) const;  // true if the event queue contains an event for the given segment index

 private:
  // expose protected `c` member of std::priority_queue so Swap can iterate underlying storage
  struct EventPriorityQueue : std::priority_queue<Event2D> {
    using std::priority_queue<Event2D>::c;
  };
  EventPriorityQueue EVENTS;  // Event2D implements operator<
};

struct SweepLineSegment2D {
  std::size_t EdgeId;
  // neighbour links are NOT part of operator<, so they are safe to mutate inside a (const) std::set node:
  // `mutable` lets us update them through the set's const iterators; `const*` because set nodes are const.
  mutable SweepLineSegment2D const* Above;
  mutable SweepLineSegment2D const* Below;
  Point2D Left;
  Point2D Right;

  SweepLineSegment2D(std::size_t id, LineSegment2D const& segment);
  ~SweepLineSegment2D() = default;

  bool operator<(SweepLineSegment2D const& other) const;  // for the RB Tree in SweepLine2D
};

// SweepLine2D is the one place that resolves event.SegmentId back to a full LineSegment2D, so it is templated
// on the concrete SegmentList it points at. Member functions are DECLARED here but DEFINED in calc_utils2d.cpp;
// the .cpp then explicitly instantiates SweepLine2D for the concrete SegmentList types (see bottom of the .cpp).
// Only those listed types may be used with SweepLine2D.
template <SegmentList Segments>
class SweepLine2D {
 public:
  // NOTE: stores a non-owning pointer — `segments` must outlive this SweepLine2D (the driver owns it).
  explicit SweepLine2D(Segments const& segments) : PTR_SEGMENTS(&segments) {}
  ~SweepLine2D() = default;

  SweepLineSegment2D const* Find(std::size_t seg_id) const;  // throw is ID not found
  void Swap(std::size_t seg1_id,
            std::size_t seg2_id);  // swap two segments already in the sweep line, throws if seg_id is out of range in
                                   // PTR_SEGMENTS
  SweepLineSegment2D const* Add(
      std::size_t seg_id);  // returns the added node, throws if seg_id is out of range in PTR_SEGMENTS
  void Remove(SweepLineSegment2D const*& segment);  // takes the handle by reference and nulls it, so the caller's
                                                    // pointer can't be reused after removal
  std::optional<Point2D> Intersection(SweepLineSegment2D const* seg1, SweepLineSegment2D const* seg2)
      const;  // throws if seg_id is out of range in PTR_SEGMENTS
  bool Intersect(SweepLineSegment2D const* seg1,
                 SweepLineSegment2D const* seg2) const;  // throws if seg_id is out of range in PTR_SEGMENTS

 private:
  Segments const* PTR_SEGMENTS;  // bound from `Segments const&`, so the pointee is const

  std::set<SweepLineSegment2D> ACTIVE_SEGMENTS;  // indices of segments currently intersecting the sweep line, ordered
                                                 // by their intersection point with the sweep line
};

template <SegmentList Segments>
bool has_intersections(Segments const& polygon_segments);  // the Shamos-Hoey algorithm for checking polygon simplicity
                                                           // (no self-intersections)

struct IntersectionEvent2D {
  Point2D Point;  // point of intersections, returned by the Bentley-Ottmann algorithm in `intersections` below;
  std::vector<std::size_t>
      SegmentIds;  // a point can be the intersection of 2 or more segments
                   // TODO: make a templated (or not) wrapper of std::vector to control it always has 2 elements

  bool operator<(IntersectionEvent2D const& other) const;
  bool operator==(IntersectionEvent2D const& other) const;
};

template <SegmentList Segments>
std::vector<IntersectionEvent2D> find_intersections(
    Segments const& segments);  // the Bentley-Ottmann algorithm for finding all
                                // intersection points among a set of segments

}  // namespace geompp
