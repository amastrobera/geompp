#include "calc_utils2d.hpp"

#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "segment_iterator2d.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <limits>
#include <numeric>
#include <set>
#include <stdexcept>
#include <variant>

namespace geompp {

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

bool shares_endpoint(LineSegment2D const& a, LineSegment2D const& b) {
  return a.First() == b.First() || a.First() == b.Last() || a.Last() == b.First() || a.Last() == b.Last();
}

}  // namespace

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
typename SweepLine2D<Segments>::SweepLineElement2D SweepLine2D<Segments>::Get(std::size_t seg_id) const {
  if (seg_id >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("SegmentId is out of range of the segments list");
  }

  SweepLineComparator<Segments> comp{SWEEP_X, PTR_SEGMENTS};
  auto it = std::lower_bound(ACTIVE_SEGMENTS.begin(), ACTIVE_SEGMENTS.end(), seg_id, comp);

  if (it == ACTIVE_SEGMENTS.end() || *it != seg_id) {
    // lower_bound failed — the vector is temporarily unsorted (e.g. concurrent
    // intersection where multiple Remove/Add cycles leave segments misordered).
    // Fall back to O(n) linear scan so the algorithm can continue.
    it = std::find(ACTIVE_SEGMENTS.begin(), ACTIVE_SEGMENTS.end(), seg_id);
    if (it == ACTIVE_SEGMENTS.end()) {
      GEOMPP_LOG(WARNING) << "requested a segment ID missing from the SweepLine active list";
      return SweepLineElement2D{std::nullopt, std::nullopt, std::nullopt};
    }
  }

  auto seg_elem = IdSegPair{*it, (*PTR_SEGMENTS)[*it]};

  std::optional<IdSegPair> above_elem = std::nullopt;
  auto above_iter = std::next(it);
  if (above_iter != ACTIVE_SEGMENTS.end()) {
    if (*above_iter >= PTR_SEGMENTS->size()) {
      throw std::out_of_range("SegmentId (above) is out of range of the segments list");
    }
    above_elem = IdSegPair{*above_iter, (*PTR_SEGMENTS)[*above_iter]};
  }

  std::optional<IdSegPair> below_elem = std::nullopt;
  if (it != ACTIVE_SEGMENTS.begin()) {
    auto below_iter = std::prev(it);
    if (*below_iter >= PTR_SEGMENTS->size()) {
      throw std::out_of_range("SegmentId (below) is out of range of the segments list");
    }
    below_elem = IdSegPair{*below_iter, (*PTR_SEGMENTS)[*below_iter]};
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

  SweepLineComparator<Segments> comp{SWEEP_X, PTR_SEGMENTS};
  auto it = std::lower_bound(ACTIVE_SEGMENTS.begin(), ACTIVE_SEGMENTS.end(), seg_id, comp);

  if (it == ACTIVE_SEGMENTS.end() || *it != seg_id) {
    it = std::find(ACTIVE_SEGMENTS.begin(), ACTIVE_SEGMENTS.end(), seg_id);
    if (it == ACTIVE_SEGMENTS.end()) {
      GEOMPP_LOG(ERROR) << "Attempted to remove a segment from the sweep line that is not active";
      return SweepLineElement2D{std::nullopt, std::nullopt, std::nullopt};
    }
  }

  auto triplet_before_remove = Get(seg_id);
  ACTIVE_SEGMENTS.erase(it);

  return SweepLineElement2D{std::nullopt, triplet_before_remove.Above, triplet_before_remove.Below};
}

template <SegmentList Segments>
void SweepLine2D<Segments>::SetX(double val) {
  SWEEP_X = val;
}

template <SegmentList Segments>
double SweepLine2D<Segments>::GetX() const {
  return SWEEP_X;
}

// ------- free functions -------

template <SegmentList Segments>
bool has_intersections_impl(Segments const& segments) {
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

    // set the sweepline X
    sweep_line.SetX(event.Point.x());

    std::size_t seg_id = event.SegmentId;

    if (event.Type == EventType2D::LEFT) {
      auto triplet = sweep_line.Add(seg_id);
      if (!triplet.Segment) {
        throw std::logic_error("Could not find the segment corresponding to the LEFT event in the sweep line");
      }

      if ((triplet.Above && !shares_endpoint(triplet.Segment->Seg, triplet.Above->Seg) &&
           intersect(triplet.Segment->Seg, triplet.Above->Seg)) ||
          (triplet.Below && !shares_endpoint(triplet.Below->Seg, triplet.Segment->Seg) &&
           intersect(triplet.Below->Seg, triplet.Segment->Seg))) {
        return true;
      }

    } else if (event.Type == EventType2D::RIGHT) {
      auto triplet = sweep_line.Remove(event.SegmentId);

      if (triplet.Above && triplet.Below && !shares_endpoint(triplet.Below->Seg, triplet.Above->Seg) &&
          intersect(triplet.Below->Seg, triplet.Above->Seg)) {
        return true;
      }
    }
  }

  return false;  // no intersections found
}

template <SegmentList Segments>
std::vector<IntersectionEvent2D> find_intersections_impl(Segments const& segments) {
  if (segments.size() < 2) {
    throw std::invalid_argument("less than 2 segments provided, cannot check for intersections");
  }

  EventQueue2D event_queue(segments);
  SweepLine2D<Segments> sweep_line(segments);

  std::set<IntersectionEvent2D> output_list;

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

      if (elem.Above && !shares_endpoint(elem.Segment->Seg, elem.Above->Seg)) {
        if (auto inter_p = elem.Segment->Seg.Intersection(elem.Above->Seg)) {
          if (std::holds_alternative<Point2D>(inter_p.value())) {
            event_queue.Push(Event2D{EventType2D::INTERSECTION, std::get<Point2D>(inter_p.value()), elem.Segment->Id,
                                     elem.Above->Id});
          }
        }
      }

      if (elem.Below && !shares_endpoint(elem.Below->Seg, elem.Segment->Seg)) {
        if (auto inter_p = elem.Below->Seg.Intersection(elem.Segment->Seg)) {
          if (std::holds_alternative<Point2D>(inter_p.value())) {
            event_queue.Push(Event2D{EventType2D::INTERSECTION, std::get<Point2D>(inter_p.value()), elem.Below->Id,
                                     elem.Segment->Id});
          }
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
        if (auto inter_p = above_elem->Seg.Intersection(below_elem->Seg)) {
          if (std::holds_alternative<Point2D>(inter_p.value())) {
            auto inter_event =
                Event2D{EventType2D::INTERSECTION, std::get<Point2D>(inter_p.value()), below_elem->Id, above_elem->Id};
            if (!event_queue.Contains(inter_event)) {
              event_queue.Push(inter_event);
            }
          }
        }
      }

    } else if (event.Type == EventType2D::INTERSECTION) {
      std::size_t seg1_id = event.SegmentId;
      std::size_t seg2_id = event.InterSegmentId.value();  // guaranteed from the logic above (and .value()
                                                           // automatically throws std::bad_optional_access if empty)

      // save the intersection event to the output list
      auto inter_event = IntersectionEvent2D{event.Point, {seg1_id, seg2_id}};

      // are const so they must be extracted, modified, and re-inserted; also `vector::contains` doesn't exist
      auto it = output_list.find(inter_event);
      if (it != output_list.end()) {
        // avoid duplicates in the output list: increase the number of intersecting segments on the same point, rather
        // than increasing the number of (equal) points with 2 segments
        IntersectionEvent2D updated = *it;
        output_list.erase(it);
        if (std::find(updated.SegmentIds.begin(), updated.SegmentIds.end(), seg1_id) == updated.SegmentIds.end()) {
          updated.SegmentIds.push_back(seg1_id);
        }
        if (std::find(updated.SegmentIds.begin(), updated.SegmentIds.end(), seg2_id) == updated.SegmentIds.end()) {
          updated.SegmentIds.push_back(seg2_id);
        }
        output_list.insert(std::move(updated));

      } else {
        output_list.emplace(inter_event);
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
        if (auto inter_p = new_seg1.Segment->Seg.Intersection(new_seg1.Above->Seg)) {
          if (std::holds_alternative<Point2D>(inter_p.value())) {
            auto inter_ev = Event2D{EventType2D::INTERSECTION, std::get<Point2D>(inter_p.value()), new_seg1.Segment->Id,
                                    new_seg1.Above->Id};
            if (!event_queue.Contains(inter_ev)) {
              event_queue.Push(inter_ev);
            }
          }
        }
      }

      if (new_seg2.Below && !shares_endpoint(new_seg2.Below->Seg, new_seg2.Segment->Seg)) {
        if (auto inter_p = new_seg2.Below->Seg.Intersection(new_seg2.Segment->Seg)) {
          if (std::holds_alternative<Point2D>(inter_p.value())) {
            auto inter_ev = Event2D{EventType2D::INTERSECTION, std::get<Point2D>(inter_p.value()), new_seg2.Below->Id,
                                    new_seg2.Segment->Id};
            if (!event_queue.Contains(inter_ev)) {
              event_queue.Push(inter_ev);
            }
          }
        }
      }
    }
  }

  return std::vector<IntersectionEvent2D>(output_list.begin(), output_list.end());
}

// explicit instantiations — emit the templated members/functions for each concrete SegmentList the driver uses.
// Add a line here for every type you instantiate with; only these types will link.
template class SweepLineComparator<std::vector<LineSegment2D>>;
template class SweepLineComparator<SegmentRange2D>;

// explicit instantiations — emit the templated members/functions for each concrete SegmentList the driver uses.
// Add a line here for every type you instantiate with; only these types will link.
template class SweepLine2D<std::vector<LineSegment2D>>;
template class SweepLine2D<SegmentRange2D>;

// for a free function template the instantiation deduces the parameter from the argument type:
template bool has_intersections_impl(std::vector<LineSegment2D> const&);
template bool has_intersections_impl(SegmentRange2D const&);

// for a free function template the instantiation deduces the parameter from the argument type:
template std::vector<IntersectionEvent2D> find_intersections_impl(std::vector<LineSegment2D> const&);
template std::vector<IntersectionEvent2D> find_intersections_impl(SegmentRange2D const&);

std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points) {
  size_t n = points.size();
  if (n < 3) {
    // log a warning ?
    return {0, 1};
  }

  // 1. Create an index map: [0, 1, 2, ..., n-1]
  std::vector<std::size_t> indices(n);
  std::iota(indices.begin(), indices.end(), 0);  // Fills vector with sequential integers

  // 2. Sort the INDICES, not the points
  std::sort(indices.begin(), indices.end(), [&points](std::size_t idx1, std::size_t idx2) {
    return compare_event_point(points[idx1], points[idx2]) == std::partial_ordering::less;
  });

  // 3. Build the hull using indices
  std::vector<std::size_t> hull_indices;
  hull_indices.reserve(2 * n);

  // Build the Lower Hull
  for (std::size_t i = 0; i < n; ++i) {
    // While the turn is NOT a strict left turn, pop the bad vertex.
    // We look at the second-to-last hull point (v1), the last hull point (v2), and the candidate point.
    while (hull_indices.size() >= 2 &&
           !is_left(points[hull_indices[hull_indices.size() - 2]], points[hull_indices.back()], points[indices[i]])) {
      hull_indices.pop_back();
    }
    hull_indices.push_back(indices[i]);
  }

  // Build the Upper Hull
  std::size_t lower_hull_size = hull_indices.size();
  for (ptrdiff_t i = static_cast<ptrdiff_t>(n) - 2; i >= 0; --i) {
    while (hull_indices.size() > lower_hull_size &&
           !is_left(points[hull_indices[hull_indices.size() - 2]], points[hull_indices.back()], points[indices[i]])) {
      hull_indices.pop_back();
    }
    hull_indices.push_back(indices[i]);
  }

  // Remove the redundant last closing index
  if (!hull_indices.empty()) {
    hull_indices.pop_back();
  }

  return hull_indices;
}

}  // namespace geompp
