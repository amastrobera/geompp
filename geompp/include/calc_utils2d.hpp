#pragma once

#include "constants.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"

#include <compare>
#include <concepts>
#include <functional>
#include <optional>
#include <queue>
#include <set>
#include <vector>

namespace geompp {

class Polygon2D;
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

  bool operator<(Event2D const& other) const;         // for the priority queue in EventQueue2D
  bool operator>(Event2D const& other) const;         // required by std::greater<Event2D> (EventMinHeap)
  bool operator==(Event2D const& other) const;        // for EventQueue2D::Contains
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

  std::set<std::size_t, SweepLineComparator<Segments>>
      ACTIVE_SEGMENTS;  // indices of segments currently intersecting the sweep line, ordered
                        // by their intersection point with the sweep line
};

/// @brief the Shamos-Hoey algorithm for checking polygon simplicity (no self-intersections)
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

/// @brief the Bentley-Ottmann algorithm for finding all intersection points among a set of segments
/// @param segments list of segments (can be generic list of segments or segments of the polygon)
/// @returns list of intersection points - in sorted order bottom-left to top-right (the intersecting 2+ segment IDs are
/// also reported)
/// @throws less than 2 segments arguments, or algorithm based throw logic
template <SegmentList Segments>
std::vector<IntersectionEvent2D> find_intersections(Segments const& segments);

}  // namespace geompp
