#include "sweep_line2d.hpp"

#include "polygon2d.hpp"
#include "segment_iterator2d.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cstddef>
#include <limits>

namespace geompp {

inline namespace geometry {
namespace detail {

namespace {

// build the LEFT / RIGHT events for every segment; works for any SegmentList (vector, SegmentRange2D, ...)
template <SegmentList Segments>
std::vector<Event2D> build_events(Segments const& segments) {
  std::vector<Event2D> all;
  all.reserve(segments.size() * 2);

  for (std::size_t i = 0; i < segments.size(); ++i) {
    auto const& seg = segments[i];  // ref to element (vector) or lifetime-extended temporary (range)

    auto ev1 = Event2D{EventType2D::UNKN, seg.First(), i, std::nullopt};
    auto ev2 = Event2D{EventType2D::UNKN, seg.Last(), i, std::nullopt};
    if (ev1 < ev2) {
      ev1.Type = EventType2D::LEFT;
      ev2.Type = EventType2D::RIGHT;
    } else {
      ev1.Type = EventType2D::RIGHT;
      ev2.Type = EventType2D::LEFT;
    }

    all.emplace_back(ev1);
    all.emplace_back(ev2);
  }

  return all;
}

template <SegmentList Segments>
double min_sweep_x(Segments const& segments) {
  double min_x = std::numeric_limits<double>::infinity();
  for (std::size_t i = 0; i < segments.size(); ++i) {
    auto const& seg = segments[i];
    min_x = std::min(min_x, std::min(seg.First().x(), seg.Last().x()));
  }
  return min_x;
}

}  // namespace

bool shares_endpoint(LineSegment2D const& a, LineSegment2D const& b) {
  return a.First() == b.First() || a.First() == b.Last() || a.Last() == b.First() || a.Last() == b.Last();
}

std::partial_ordering compare_event_point(Point2D a, Point2D b) {
  auto compare_x = compare(a.x(), b.x());
  if (compare_x != std::partial_ordering::equivalent) {
    return compare_x;
  }
  auto compare_y = compare(a.y(), b.y());
  return compare_y;
}

// ------- event queue -------
EventQueue2D::EventQueue2D(std::vector<LineSegment2D> const& segments) {
  EVENTS = EventMinHeap(std::greater<Event2D>{}, build_events(segments));
}

EventQueue2D::EventQueue2D(SegmentRange2D const& segments) {
  EVENTS = EventMinHeap(std::greater<Event2D>{}, build_events(segments));
}

EventQueue2D::EventQueue2D(Polygon2D const& polygon) : EventQueue2D(polygon.ToSegments()) {}

bool Event2D::operator<(Event2D const& other) const {
  // order by x coordinate, then by y coordinate, then by event type (LEFT < INTERSECTION < RIGHT)
  auto compare_x = compare(Point.x(), other.Point.x());
  if (compare_x != std::partial_ordering::equivalent) {
    return compare_x < 0;
  }

  auto compare_y = compare(Point.y(), other.Point.y());
  if (compare_y != std::partial_ordering::equivalent) {
    return compare_y < 0;
  }

  return Type < other.Type;  // relies on the order of the EventType2D enum
                             // prioritize LEFT < INTERSECTION < RIGHT events (guaranteed by EventType2D enum order)
                             // note on D.Sunday book: helps to process edge cases
}

bool Event2D::operator>(Event2D const& other) const { return other < *this; }

bool Event2D::operator==(Event2D const& other) const {
  return Type == other.Type && Point == other.Point && SegmentId == other.SegmentId &&
         InterSegmentId == other.InterSegmentId;
}

std::optional<Event2D> EventQueue2D::Top() const {
  if (EVENTS.empty()) {
    return std::nullopt;
  }
  return EVENTS.top();
}

std::optional<Event2D> EventQueue2D::Pop() {
  if (EVENTS.empty()) {
    return std::nullopt;
  }
  Event2D top = EVENTS.top();
  EVENTS.pop();
  return top;
}

bool EventQueue2D::Empty() const { return EVENTS.empty(); }

void EventQueue2D::Push(Event2D const& event) { EVENTS.push(event); }

bool EventQueue2D::Contains(Event2D const& event) const {
  for (const auto& queued_event : EVENTS.c) {
    if (queued_event == event) {
      return true;
    }
    // INTERSECTION(A,B) and INTERSECTION(B,A) are the same geometric event: check the reversed pair too
    if (event.Type == EventType2D::INTERSECTION && queued_event.Type == EventType2D::INTERSECTION &&
        queued_event.Point == event.Point && event.InterSegmentId.has_value() &&
        queued_event.InterSegmentId.has_value() && queued_event.SegmentId == event.InterSegmentId.value() &&
        queued_event.InterSegmentId.value() == event.SegmentId) {
      return true;
    }
  }
  return false;
}

// ------- sweep line -------

bool IntersectionEvent2D::operator<(IntersectionEvent2D const& other) const {
  return compare_event_point(Point, other.Point) < 0;  // order by the intersection point
}

bool IntersectionEvent2D::operator==(IntersectionEvent2D const& other) const {
  return compare_event_point(Point, other.Point) == 0;  // order by the intersection point
}

template <SegmentList Segments>
double SweepLineComparator<Segments>::GetYAtX(LineSegment2D const& seg, double x) const {
  auto is_seg_reverse = compare_event_point(seg.First(), seg.Last()) > 0;

  auto p0 = is_seg_reverse ? seg.Last() : seg.First();
  auto p1 = is_seg_reverse ? seg.First() : seg.Last();

  if (compare(x, p0.x()) < 0) {
    return p0.y();
  }

  if (compare(x, p1.x()) > 0) {
    return p1.y();
  }

  if (compare(p1.x(), p0.x()) == 0) {  // vertical segment: avoid division by zero
    return (p0.y() + p1.y()) / 2.0;
  }
  // linear interpolation
  return p0.y() + ((p1.y() - p0.y()) / (p1.x() - p0.x())) * (x - p0.x());
}

template <SegmentList Segments>
bool SweepLineComparator<Segments>::operator()(std::size_t id1, std::size_t id2) const {
  if (id1 >= segments->size() || id2 >= segments->size()) {
    throw std::out_of_range("SegmentId is out of range of the segments list");
  }

  double y1 = GetYAtX((*segments)[id1], sweep_x);
  double y2 = GetYAtX((*segments)[id2], sweep_x);

  if (compare(y1, y2) != std::partial_ordering::equivalent) {
    return compare(y1, y2) < 0;
  }
  // Equal y at sweep_x: evaluate at the midpoint of the segments' active x-overlap.
  // This is always strictly interior — avoids the endpoint ambiguity that plagues
  // forward/backward delta looks (forward reverses shared-RIGHT order; backward
  // reverses shared-LEFT order).
  auto seg_left_x = [](LineSegment2D const& s) { return std::min(s.First().x(), s.Last().x()); };
  auto seg_right_x = [](LineSegment2D const& s) { return std::max(s.First().x(), s.Last().x()); };
  double overlap_lo = std::max(seg_left_x((*segments)[id1]), seg_left_x((*segments)[id2]));
  double overlap_hi = std::min(seg_right_x((*segments)[id1]), seg_right_x((*segments)[id2]));
  if (overlap_lo < overlap_hi) {
    double mid_x = (overlap_lo + overlap_hi) * 0.5;
    double y1_mid = GetYAtX((*segments)[id1], mid_x);
    double y2_mid = GetYAtX((*segments)[id2], mid_x);
    if (compare(y1_mid, y2_mid) != std::partial_ordering::equivalent) {
      return compare(y1_mid, y2_mid) < 0;
    }
  }
  return id1 < id2;  // final fallback (parallel / coincident segments)
}

// SweepLine2D template members: defined here, emitted for other TUs by the explicit instantiations below.
// Each definition needs its own `template <SegmentList Segments>` and the SweepLine2D<Segments>:: qualifier.

template <SegmentList Segments>
SweepLine2D<Segments>::SweepLine2D(Segments const& segments)
    : SWEEP_X(min_sweep_x(segments)), PTR_SEGMENTS(&segments) {}

template <SegmentList Segments>
std::optional<std::size_t> SweepLine2D<Segments>::FindIndex(std::size_t seg_id) const {
  SweepLineComparator<Segments> comp{SWEEP_X, PTR_SEGMENTS};
  auto it = std::lower_bound(ACTIVE_SEGMENTS.begin(), ACTIVE_SEGMENTS.end(), seg_id, comp);

  if (it == ACTIVE_SEGMENTS.end() || *it != seg_id) {
    // lower_bound didn't land exactly on seg_id — either the comparator's tie-break sent it to the wrong
    // slot, or seg_id genuinely isn't active. Since ids are unique in ACTIVE_SEGMENTS, an exact match can
    // never be a false positive, so falling back to an O(n) linear scan whenever it ISN'T an exact match
    // is always safe, regardless of how stale SWEEP_X is relative to the vector's true order.
    it = std::find(ACTIVE_SEGMENTS.begin(), ACTIVE_SEGMENTS.end(), seg_id);
    if (it == ACTIVE_SEGMENTS.end()) {
      return std::nullopt;
    }
  }
  return static_cast<std::size_t>(std::distance(ACTIVE_SEGMENTS.begin(), it));
}

template <SegmentList Segments>
typename SweepLine2D<Segments>::SweepLineElement2D SweepLine2D<Segments>::Get(std::size_t seg_id) const {
  if (seg_id >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("SegmentId is out of range of the segments list");
  }

  auto idx = FindIndex(seg_id);
  if (!idx) {
    GEOMPP_LOG(WARNING) << "requested a segment ID missing from the SweepLine active list";
    return SweepLineElement2D{std::nullopt, std::nullopt, std::nullopt};
  }

  auto seg_elem = IdSegPair{ACTIVE_SEGMENTS[*idx], (*PTR_SEGMENTS)[ACTIVE_SEGMENTS[*idx]]};

  std::optional<IdSegPair> above_elem = std::nullopt;
  if (*idx + 1 < ACTIVE_SEGMENTS.size()) {
    auto above_id = ACTIVE_SEGMENTS[*idx + 1];
    if (above_id >= PTR_SEGMENTS->size()) {
      throw std::out_of_range("SegmentId (above) is out of range of the segments list");
    }
    above_elem = IdSegPair{above_id, (*PTR_SEGMENTS)[above_id]};
  }

  std::optional<IdSegPair> below_elem = std::nullopt;
  if (*idx > 0) {
    auto below_id = ACTIVE_SEGMENTS[*idx - 1];
    if (below_id >= PTR_SEGMENTS->size()) {
      throw std::out_of_range("SegmentId (below) is out of range of the segments list");
    }
    below_elem = IdSegPair{below_id, (*PTR_SEGMENTS)[below_id]};
  }

  return SweepLineElement2D{seg_elem, above_elem, below_elem};
}

template <SegmentList Segments>
typename SweepLine2D<Segments>::SweepLineElement2D SweepLine2D<Segments>::Add(std::size_t seg_id) {
  if (seg_id >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("SegmentId is out of range of the segments list");
  }

  SweepLineComparator<Segments> comp{SWEEP_X, PTR_SEGMENTS};
  auto it = std::lower_bound(ACTIVE_SEGMENTS.begin(), ACTIVE_SEGMENTS.end(), seg_id, comp);

  if (it != ACTIVE_SEGMENTS.end() && *it == seg_id) {
    throw std::logic_error("Attempted to insert a segment into the sweep line that is already active");
  }

  ACTIVE_SEGMENTS.insert(it, seg_id);
  return Get(seg_id);
}

template <SegmentList Segments>
typename SweepLine2D<Segments>::SweepLineElement2D SweepLine2D<Segments>::Remove(std::size_t seg_id) {
  if (seg_id >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("SegmentId is out of range of the segments list");
  }

  auto idx = FindIndex(seg_id);
  if (!idx) {
    GEOMPP_LOG(ERROR) << "Attempted to remove a segment from the sweep line that is not active";
    return SweepLineElement2D{std::nullopt, std::nullopt, std::nullopt};
  }

  auto triplet_before_remove = Get(seg_id);
  ACTIVE_SEGMENTS.erase(ACTIVE_SEGMENTS.begin() + *idx);

  return SweepLineElement2D{std::nullopt, triplet_before_remove.Above, triplet_before_remove.Below};
}

template <SegmentList Segments>
typename SweepLine2D<Segments>::ReverseRunResult2D SweepLine2D<Segments>::ReverseRun(
    std::vector<std::size_t> const& seg_ids) {
  if (seg_ids.size() < 2) {
    throw std::invalid_argument("ReverseRun requires at least 2 segment ids");
  }

  auto anchor_idx = FindIndex(seg_ids.front());
  if (!anchor_idx) {
    return ReverseRunResult2D{};  // Ok = false: not active at all — fully stale
  }

  std::set<std::size_t> wanted(seg_ids.begin(), seg_ids.end());

  // Expand outward from the anchor by direct vector indexing only — no further comparator calls — to see
  // whether the rest of `wanted` occupies the immediately-adjacent slots. Anything less than fully
  // contiguous means the run isn't (or is no longer) intact, so the caller should treat it as stale.
  std::size_t lo = *anchor_idx;
  std::size_t hi = *anchor_idx;
  std::size_t found = 1;

  while (found < wanted.size() && lo > 0 && wanted.count(ACTIVE_SEGMENTS[lo - 1]) > 0) {
    --lo;
    ++found;
  }
  while (found < wanted.size() && hi + 1 < ACTIVE_SEGMENTS.size() && wanted.count(ACTIVE_SEGMENTS[hi + 1]) > 0) {
    ++hi;
    ++found;
  }

  if (found != wanted.size()) {
    return ReverseRunResult2D{};  // Ok = false: not contiguous — stale event
  }

  std::reverse(ACTIVE_SEGMENTS.begin() + static_cast<std::ptrdiff_t>(lo),
               ACTIVE_SEGMENTS.begin() + static_cast<std::ptrdiff_t>(hi) + 1);

  ReverseRunResult2D result;
  result.Ok = true;
  result.NewTop = IdSegPair{ACTIVE_SEGMENTS[hi], (*PTR_SEGMENTS)[ACTIVE_SEGMENTS[hi]]};
  result.NewBottom = IdSegPair{ACTIVE_SEGMENTS[lo], (*PTR_SEGMENTS)[ACTIVE_SEGMENTS[lo]]};
  if (lo > 0) {
    auto id = ACTIVE_SEGMENTS[lo - 1];
    result.BelowRun = IdSegPair{id, (*PTR_SEGMENTS)[id]};
  }
  if (hi + 1 < ACTIVE_SEGMENTS.size()) {
    auto id = ACTIVE_SEGMENTS[hi + 1];
    result.AboveRun = IdSegPair{id, (*PTR_SEGMENTS)[id]};
  }
  return result;
}

template <SegmentList Segments>
void SweepLine2D<Segments>::SetX(double val) {
  SWEEP_X = val;
}

template <SegmentList Segments>
double SweepLine2D<Segments>::GetX() const {
  return SWEEP_X;
}

// explicit instantiations — emit the templated members/functions for each concrete SegmentList the driver uses.
// Add a line here for every type you instantiate with; only these types will link.
template class SweepLineComparator<std::vector<LineSegment2D>>;
template class SweepLineComparator<SegmentRange2D>;

// explicit instantiations — emit the templated members/functions for each concrete SegmentList the driver uses.
// Add a line here for every type you instantiate with; only these types will link.
template class SweepLine2D<std::vector<LineSegment2D>>;
template class SweepLine2D<SegmentRange2D>;

}  // namespace detail
}  // namespace geometry

}  // namespace geompp
