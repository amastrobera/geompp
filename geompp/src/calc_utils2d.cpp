#include "calc_utils2d.hpp"

#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "segment_iterator2d.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include "point3d.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <map>
#include <numbers>
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

template <PointContainer Points>
std::vector<std::size_t> convex_hull_monotone_chain(Points const& points, View2D const& view) {
  std::size_t n = std::ranges::size(points);
  if (n < 3) {
    return {};
  }

  std::vector<std::size_t> indices(n);
  std::iota(indices.begin(), indices.end(), std::size_t{0});

  std::sort(indices.begin(), indices.end(), [&](std::size_t i, std::size_t j) {
    double xi = view.x(points[i]), xj = view.x(points[j]);
    if (xi != xj) {
      return xi < xj;
    }
    return view.y(points[i]) < view.y(points[j]);
  });

  auto is_left = [&](std::size_t o, std::size_t a, std::size_t b) {
    auto [ox, oy] = view.xy(points[o]);
    auto [ax, ay] = view.xy(points[a]);
    auto [bx, by] = view.xy(points[b]);
    return compare((ax - ox) * (by - oy) - (ay - oy) * (bx - ox), 0) > 0;
  };

  std::vector<std::size_t> hull;
  hull.reserve(2 * n);

  // Lower hull
  for (std::size_t i = 0; i < n; ++i) {
    while (hull.size() >= 2 && !is_left(hull[hull.size() - 2], hull.back(), indices[i])) {
      hull.pop_back();
    }
    hull.push_back(indices[i]);
  }

  // Upper hull
  std::size_t lower_hull_size = hull.size();
  for (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(n) - 2; i >= 0; --i) {
    while (hull.size() > lower_hull_size && !is_left(hull[hull.size() - 2], hull.back(), indices[i])) {
      hull.pop_back();
    }
    hull.push_back(indices[i]);
  }

  if (!hull.empty()) {
    hull.pop_back();
  }
  return hull;
}

// template specialization
template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point2D> const&, View2D const&);

template std::vector<std::size_t> convex_hull_monotone_chain(std::vector<Point3D> const&, View2D const&);

template <PointContainer Points>
MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const& hull_indices, Points const& points,
                                        View2D const& view) {
  std::size_t m = hull_indices.size();

  double best_area = std::numeric_limits<double>::max();
  double best_ux = 1.0, best_uy = 0.0, best_vx = 0.0, best_vy = 1.0;
  double best_umin = 0.0, best_umax = 0.0, best_vmin = 0.0, best_vmax = 0.0;
  double best_ox = 0.0, best_oy = 0.0;

  for (std::size_t i = 0; i < m; ++i) {
    auto [p0x, p0y] = view.xy(points[hull_indices[i]]);
    auto [p1x, p1y] = view.xy(points[hull_indices[(i + 1) % m]]);

    double ex = p1x - p0x;
    double ey = p1y - p0y;
    double elen = std::sqrt(ex * ex + ey * ey);
    if (compare(elen, 0.0) == 0) {
      continue;
    }

    double ux = ex / elen;
    double uy = ey / elen;
    double vx = -uy;
    double vy = ux;

    double u_min = std::numeric_limits<double>::max();
    double u_max = -std::numeric_limits<double>::max();
    double v_min = std::numeric_limits<double>::max();
    double v_max = -std::numeric_limits<double>::max();

    for (std::size_t j = 0; j < m; ++j) {
      auto [qx, qy] = view.xy(points[hull_indices[j]]);
      double dx = qx - p0x;
      double dy = qy - p0y;
      double pu = dx * ux + dy * uy;
      double pv = dx * vx + dy * vy;
      if (pu < u_min) {
        u_min = pu;
      }
      if (pu > u_max) {
        u_max = pu;
      }
      if (pv < v_min) {
        v_min = pv;
      }
      if (pv > v_max) {
        v_max = pv;
      }
    }

    double rect_area = (u_max - u_min) * (v_max - v_min);
    if (compare(rect_area, best_area) < 0) {
      best_area = rect_area;
      best_ux = ux;
      best_uy = uy;
      best_vx = vx;
      best_vy = vy;
      best_umin = u_min;
      best_umax = u_max;
      best_vmin = v_min;
      best_vmax = v_max;
      best_ox = p0x;
      best_oy = p0y;
    }
  }

  double cu = (best_umin + best_umax) / 2.0;
  double cv = (best_vmin + best_vmax) / 2.0;

  return MinBoundingRectResult{
      best_ux, best_uy, best_vx, best_vy, (best_umax - best_umin) / 2.0, (best_vmax - best_vmin) / 2.0,
      cu,      cv,      best_ox, best_oy,
  };
}

template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point2D> const&,
                                                 View2D const&);
template MinBoundingRectResult min_bounding_rect(std::vector<std::size_t> const&, std::vector<Point3D> const&,
                                                 View2D const&);

std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points) {
  return convex_hull_monotone_chain(points, View2D::XY());
}

std::vector<std::vector<Point2D>> simplify_rings_impl(std::vector<LineSegment2D> const& segs) {
  if (segs.size() < 3) {
    throw std::invalid_argument("simplify_rings_impl: need at least 3 segments");
  }

  // --- Stage 2: split every segment at its crossing points ---
  auto crossings = find_intersections_impl(segs);

  // map: segment index → crossing points on that segment
  std::map<std::size_t, std::vector<Point2D>> seg_cp;
  for (auto const& ev : crossings) {
    for (auto id : ev.SegmentIds) {
      seg_cp[id].push_back(ev.Point);
    }
  }

  std::vector<LineSegment2D> split;
  split.reserve(segs.size() + crossings.size() * 2);
  for (std::size_t i = 0; i < segs.size(); ++i) {
    auto it = seg_cp.find(i);
    if (it == seg_cp.end()) {
      split.push_back(segs[i]);
      continue;
    }
    // sort crossing points by parameter t along this segment
    std::vector<Point2D> cps = it->second;
    std::sort(cps.begin(), cps.end(),
              [&](Point2D const& a, Point2D const& b) { return segs[i].Location(a) < segs[i].Location(b); });
    cps.erase(std::unique(cps.begin(), cps.end(), [](Point2D const& a, Point2D const& b) { return a.AlmostEquals(b); }),
              cps.end());

    Point2D prev = segs[i].First();
    for (auto const& cp : cps) {
      if (!prev.AlmostEquals(cp)) {
        split.push_back(LineSegment2D::Make(prev, cp));
      }
      prev = cp;
    }
    if (!prev.AlmostEquals(segs[i].Last())) {
      split.push_back(LineSegment2D::Make(prev, segs[i].Last()));
    }
  }

  // --- Stage 3: build planar graph, trace half-edge faces ---

  // Assign stable integer IDs to unique vertices (using rounded keys)
  double scale = std::pow(10.0, static_cast<double>(DECIMAL_PRECISION));
  auto vkey = [scale](Point2D const& p) -> std::pair<long long, long long> {
    return {llround(p.x() * scale), llround(p.y() * scale)};
  };

  std::map<std::pair<long long, long long>, int> vid_map;
  std::vector<Point2D> verts;
  auto get_vid = [&](Point2D const& p) -> int {
    auto k = vkey(p);
    auto it = vid_map.find(k);
    if (it != vid_map.end()) {
      return it->second;
    }
    int id = static_cast<int>(verts.size());
    vid_map[k] = id;
    verts.push_back(p);
    return id;
  };

  // Undirected adjacency list — push all edges, deduplicate after sorting.
  std::vector<std::vector<int>> adj;
  for (auto const& seg : split) {
    int a = get_vid(seg.First());
    int b = get_vid(seg.Last());
    if (static_cast<int>(adj.size()) <= std::max(a, b)) {
      adj.resize(static_cast<std::size_t>(std::max(a, b)) + 1);
    }
    adj[a].push_back(b);
    adj[b].push_back(a);
  }
  adj.resize(verts.size());

  // Sort each neighbor list by polar angle CCW around the vertex, then deduplicate.
  // Precompute angles alongside neighbor IDs for O(log degree) lookup in the half-edge walk.
  int nv = static_cast<int>(verts.size());
  std::vector<std::vector<double>> adj_angles(nv);
  for (int v = 0; v < nv; ++v) {
    std::sort(adj[v].begin(), adj[v].end(), [&](int a, int b) {
      double ax = verts[a].x() - verts[v].x(), ay = verts[a].y() - verts[v].y();
      double bx = verts[b].x() - verts[v].x(), by = verts[b].y() - verts[v].y();
      return std::atan2(ay, ax) < std::atan2(by, bx);
    });
    adj[v].erase(std::unique(adj[v].begin(), adj[v].end()), adj[v].end());
    adj_angles[v].resize(adj[v].size());
    for (int k = 0; k < static_cast<int>(adj[v].size()); ++k) {
      adj_angles[v][k] = std::atan2(verts[adj[v][k]].y() - verts[v].y(), verts[adj[v][k]].x() - verts[v].x());
    }
  }

  // Walk half-edges: for directed (cur_from→cur_to), the next half-edge in the
  // same face is (cur_to→w) where w is the first CCW neighbor from cur_to after
  // the reversed-incoming direction (i.e., the direction back toward cur_from).
  std::set<std::pair<int, int>> used;
  std::vector<std::vector<Point2D>> rings;

  for (int u = 0; u < nv; ++u) {
    for (int v0 : adj[u]) {
      if (used.count({u, v0})) {
        continue;
      }

      std::vector<Point2D> ring;
      int cur_from = u, cur_to = v0;

      while (true) {
        auto edge = std::make_pair(cur_from, cur_to);
        if (used.count(edge)) {
          break;
        }
        used.insert(edge);
        ring.push_back(verts[cur_from]);

        double rev_angle = std::atan2(verts[cur_from].y() - verts[cur_to].y(), verts[cur_from].x() - verts[cur_to].x());

        auto const& angles = adj_angles[cur_to];
        auto const& nbrs = adj[cur_to];
        int deg = static_cast<int>(nbrs.size());
        int next_to = -1;

        if (deg > 0) {
          auto it = std::upper_bound(angles.begin(), angles.end(), rev_angle);
          if (it == angles.end()) {
            next_to = nbrs[0];
          } else {
            next_to = nbrs[static_cast<int>(it - angles.begin())];
          }
        }

        if (next_to == -1) {
          break;
        }
        cur_from = cur_to;
        cur_to = next_to;
      }

      if (ring.size() >= 3) {
        rings.push_back(std::move(ring));
      }
    }
  }

  return rings;
}

}  // namespace geompp
