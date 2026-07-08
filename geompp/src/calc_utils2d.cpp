#include "calc_utils2d.hpp"

#include "line2d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "segment_iterator2d.hpp"
#include "vector2d.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <algorithm>
#include <cassert>
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

bool shares_endpoint(LineSegment2D const& a, LineSegment2D const& b) {
  return a.First() == b.First() || a.First() == b.Last() || a.Last() == b.First() || a.Last() == b.Last();
}

}  // namespace

std::optional<Point2D> line_intersection(Point2D const& p0, Point2D const& p1, Point2D const& other_p0,
                                         Point2D const& other_p1, double& sc, double& tc) {
  try {
    // 2D intersection via perp-product:
    //   L(s) = p0 + s*(p1-p0),  L(t) = other_p0 + t*(other_p1-other_p0)
    //   s = -(w0 . vp) / (u . vp),  t = (w0 . up) / (v . up)
    auto u = p1 - p0;
    auto v = other_p1 - other_p0;
    auto vp = v.Perp();
    auto up = u.Perp();
    auto w0 = p0 - other_p0;

    if (compare(u.Dot(vp), 0) == 0 || compare(v.Dot(up), 0) == 0) {
      sc = tc = std::numeric_limits<double>::quiet_NaN();
      return std::nullopt;
    }

    sc = -w0.Dot(vp) / u.Dot(vp);
    tc = w0.Dot(up) / v.Dot(up);

    return p0 + (u * sc);

  } catch (...) {
    GEOMPP_LOG(WARNING) << "unexpected error while computing line intersection";
  }
  sc = tc = std::numeric_limits<double>::quiet_NaN();
  return std::nullopt;
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
bool has_intersections(Segments const& segments) {
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
std::vector<IntersectionEvent2D> find_intersections(Segments const& segments) {
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
          event_queue.Push(Event2D{EventType2D::INTERSECTION, inter_p.value(), elem.Segment->Id, elem.Above->Id});
        }
      }

      if (elem.Below && !shares_endpoint(elem.Below->Seg, elem.Segment->Seg)) {
        if (auto inter_p = elem.Below->Seg.Intersection(elem.Segment->Seg)) {
          event_queue.Push(Event2D{EventType2D::INTERSECTION, inter_p.value(), elem.Below->Id, elem.Segment->Id});
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
          auto inter_event = Event2D{EventType2D::INTERSECTION, inter_p.value(), below_elem->Id, above_elem->Id};
          if (!event_queue.Contains(inter_event)) {
            event_queue.Push(inter_event);
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
          auto inter_ev = Event2D{EventType2D::INTERSECTION, inter_p.value(), new_seg1.Segment->Id, new_seg1.Above->Id};
          if (!event_queue.Contains(inter_ev)) {
            event_queue.Push(inter_ev);
          }
        }
      }

      if (new_seg2.Below && !shares_endpoint(new_seg2.Below->Seg, new_seg2.Segment->Seg)) {
        if (auto inter_p = new_seg2.Below->Seg.Intersection(new_seg2.Segment->Seg)) {
          auto inter_ev = Event2D{EventType2D::INTERSECTION, inter_p.value(), new_seg2.Below->Id, new_seg2.Segment->Id};
          if (!event_queue.Contains(inter_ev)) {
            event_queue.Push(inter_ev);
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
template bool has_intersections(std::vector<LineSegment2D> const&);
template bool has_intersections(SegmentRange2D const&);

// for a free function template the instantiation deduces the parameter from the argument type:
template std::vector<IntersectionEvent2D> find_intersections(std::vector<LineSegment2D> const&);
template std::vector<IntersectionEvent2D> find_intersections(SegmentRange2D const&);

// Everything in namespace view projects points through a View2D before operating on them.
// Grouped together since they all share that one dependency.
namespace view {

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

}  // namespace view

template <VectorType V, ProjectablePointContainerWith<V> R>
std::pair<std::size_t, std::size_t> extreme_points(R const& vertices, bool is_convex, V const& dir) {
  long const n = static_cast<long>(std::ranges::distance(vertices));
  if (n == 0) {
    throw std::invalid_argument("extreme_points: empty vertex range");
  }

  // projection of vertex i (cyclic index) onto the direction
  auto proj = [&](long i) -> double {
    return vertices[static_cast<std::size_t>(((i % n) + n) % n)].ToVector().Dot(dir);
  };

  // convex fast-path — Daniel Sunday's O(log n) binary search for the extreme vertex of a convex CCW
  // polygon in a direction (cf. the KACTL "extrVertex"). search(+1) returns argmax(proj); search(-1)
  // returns argmax(-proj) = argmin(proj). Ties (an edge perpendicular to dir) resolve to one endpoint.
  if (is_convex) {
    auto sgn = [](double d) -> int { return static_cast<int>(d > 0) - static_cast<int>(d < 0); };

    auto search = [&](double s) -> std::size_t {
      auto g = [&](long i) { return s * proj(i); };                                 // maximise s * proj
      auto cmp = [&](long i, long j) { return sgn(g(j) - g(i)); };                  // sign( g(j) - g(i) )
      auto extr = [&](long i) { return cmp(i + 1, i) >= 0 && cmp(i, i - 1) < 0; };  // i is the peak
      if (extr(0)) {
        return 0;
      }
      long lo = 0, hi = n;
      while (lo + 1 < hi) {
        long m = (lo + hi) / 2;
        if (extr(m)) {
          return static_cast<std::size_t>(m);
        }
        int ls = cmp(lo + 1, lo);
        int ms = cmp(m + 1, m);
        bool go_hi = (ls < ms) || (ls == ms && ls == cmp(lo, m));
        if (go_hi) {
          hi = m;
        } else {
          lo = m;
        }
      }
      return static_cast<std::size_t>(lo);
    };

    std::size_t max_i = search(1.0);
    std::size_t min_i = search(-1.0);
    return {min_i, max_i};
  }

  // brute force O(n): a concave ring has no monotone structure to exploit
  std::size_t min_i = 0, max_i = 0;
  double min_v = proj(0), max_v = proj(0);
  for (long i = 1; i < n; ++i) {
    double v = proj(i);
    if (v < min_v) {
      min_v = v;
      min_i = static_cast<std::size_t>(i);
    }
    if (v > max_v) {
      max_v = v;
      max_i = static_cast<std::size_t>(i);
    }
  }
  return {min_i, max_i};
}

template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point2D> const&, bool, Vector2D const&);
template std::pair<std::size_t, std::size_t> extreme_points(std::vector<Point3D> const&, bool, Vector3D const&);

namespace view {

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

}  // namespace view

std::vector<std::size_t> convex_hull_indices(std::vector<Point2D> const& points) {
  return view::convex_hull_monotone_chain(points, View2D::XY());
}

namespace view {

template <PointContainer Points>
std::vector<std::vector<Point2D>> simplify_rings(Points const& outer, std::vector<Points> const& holes,
                                                 View2D const& view) {
  // --- Stage 1: project rings to 2D segments ---
  std::vector<LineSegment2D> segs;
  {
    int n = static_cast<int>(std::ranges::size(outer));
    segs.reserve(n);
    for (int i = 0; i < n; ++i) {
      segs.emplace_back(LineSegment2D::Make(Point2D(view.x(outer[i]), view.y(outer[i])),
                                            Point2D(view.x(outer[(i + 1) % n]), view.y(outer[(i + 1) % n]))));
    }
  }
  for (auto const& hole : holes) {
    int nh = static_cast<int>(std::ranges::size(hole));
    for (int i = 0; i < nh; ++i) {
      segs.emplace_back(LineSegment2D::Make(Point2D(view.x(hole[i]), view.y(hole[i])),
                                            Point2D(view.x(hole[(i + 1) % nh]), view.y(hole[(i + 1) % nh]))));
    }
  }

  if (segs.size() < 3) {
    throw std::invalid_argument("simplify_rings: need at least 3 segments");
  }

  // --- Stage 2: split every segment at its crossing points ---
  // Qualified deliberately: an unqualified call would ADL onto geompp::find_intersections
  // (segs is std::vector<LineSegment2D>, and LineSegment2D lives in geompp) and, being a
  // non-template exact match, that overload wins over this template — silently returning
  // std::vector<Point2D> instead of std::vector<IntersectionEvent2D>.
  auto crossings = detail::find_intersections(segs);

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

template std::vector<std::vector<Point2D>> simplify_rings(std::vector<Point2D> const&,
                                                          std::vector<std::vector<Point2D>> const&, View2D const&);

template std::vector<std::vector<Point2D>> simplify_rings(std::vector<Point3D> const&,
                                                          std::vector<std::vector<Point3D>> const&, View2D const&);

}  // namespace view

int winding_number(std::vector<Point2D> const& vertices, Point2D const& p) {
  int wn = 0;
  int n = static_cast<int>(vertices.size());
  for (int i1 = 0; i1 < n; ++i1) {
    int i2 = (i1 + 1) % n;
    auto const& v1 = vertices[i1];
    auto const& v2 = vertices[i2];
    if (compare(v1.y(), p.y()) <= 0) {   // edge starts below p
      if (compare(v2.y(), p.y()) > 0) {  //   ends above p (upward crossing)
        if (is_left(v1, v2, p)) {        //   p left of the edge
          ++wn;
        }
      }
    } else {                              // edge starts above p
      if (compare(v2.y(), p.y()) <= 0) {  //   ends below p (downward crossing)
        if (is_right(v1, v2, p)) {        //   p right of the edge
          --wn;
        }
      }
    }
  }
  return wn;
}

namespace view {

template <PointContainer Points>
bool is_convex(Points const& vertices, View2D const& view) {
  int n = static_cast<int>(vertices.size());
  bool seen_positive = false;
  bool seen_negative = false;
  for (int i = 0; i < n; ++i) {
    double x0 = view.x(vertices[i]), y0 = view.y(vertices[i]);
    double x1 = view.x(vertices[(i + 1) % n]), y1 = view.y(vertices[(i + 1) % n]);
    double x2 = view.x(vertices[(i + 2) % n]), y2 = view.y(vertices[(i + 2) % n]);
    double cross = (x1 - x0) * (y2 - y1) - (y1 - y0) * (x2 - x1);
    auto ord = compare(cross, 0.0);
    if (ord == 0) {
      continue;
    }
    if (ord > 0) {
      seen_positive = true;
    } else {
      seen_negative = true;
    }
    if (seen_positive && seen_negative) {
      return false;
    }
  }
  return true;
}

template bool is_convex(std::vector<Point2D> const&, View2D const&);
template bool is_convex(std::vector<Point3D> const&, View2D const&);

}  // namespace view

bool is_convex(std::vector<Point2D> const& vertices, std::vector<std::vector<Point2D>> const& holes) {
  if (!holes.empty() || vertices.size() < 3) {
    return false;
  }
  return view::is_convex(vertices, View2D::XY());
}

namespace view {

template <PointContainer Points>
bool is_on_perimeter(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py) {
  Point2D test_pt(px, py);
  auto check_ring = [&](auto const& ring) -> bool {
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      Point2D v1(view.x(ring[i]), view.y(ring[i]));
      Point2D v2(view.x(ring[(i + 1) % n]), view.y(ring[(i + 1) % n]));
      if (LineSegment2D::Make(v1, v2).Contains(test_pt)) {
        return true;
      }
    }
    return false;
  };
  if (check_ring(outer)) {
    return true;
  }
  for (auto const& hole : holes) {
    if (check_ring(hole)) {
      return true;
    }
  }
  return false;
}

template bool is_on_perimeter(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&, View2D const&,
                              double, double);
template bool is_on_perimeter(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&, View2D const&,
                              double, double);

template <PointContainer Points>
bool polygon_contains(Points const& outer, std::vector<Points> const& holes, View2D const& view, double px, double py) {
  auto ring_winding = [&](auto const& ring) -> int {
    int wn = 0;
    int n = static_cast<int>(ring.size());
    for (int i = 0; i < n; ++i) {
      double v1y = view.y(ring[i]);
      double v2y = view.y(ring[(i + 1) % n]);
      if (compare(v1y, py) <= 0) {
        if (compare(v2y, py) > 0) {
          double v1x = view.x(ring[i]), v2x = view.x(ring[(i + 1) % n]);
          if (compare((v2x - v1x) * (py - v1y) - (v2y - v1y) * (px - v1x), 0.0) > 0) {
            ++wn;
          }
        }
      } else {
        if (compare(v2y, py) <= 0) {
          double v1x = view.x(ring[i]), v2x = view.x(ring[(i + 1) % n]);
          if (compare((v2x - v1x) * (py - v1y) - (v2y - v1y) * (px - v1x), 0.0) < 0) {
            --wn;
          }
        }
      }
    }
    return wn;
  };

  int wn = ring_winding(outer);
  if (wn == 0) {  // early terminate if outside
    return false;
  }

  for (auto const& hole : holes) {
    wn += ring_winding(hole);

    if (wn == 0) {  // since valid holes cannot overlap each other
                    // early terminate if inside one of the holes
      return false;
    }
  }
  return wn != 0;
}

template bool polygon_contains(std::vector<Point2D> const&, std::vector<std::vector<Point2D>> const&, View2D const&,
                               double, double);
template bool polygon_contains(std::vector<Point3D> const&, std::vector<std::vector<Point3D>> const&, View2D const&,
                               double, double);

template <PointContainer Points, Point P>
std::vector<std::pair<double, double>> compute_parametric_intersection_intervals(
    Points const& outer_coplanar_ccw, std::vector<Points> const& holes_coplanar_cw, bool is_convex_input,
    P const& line_p0, P const& line_p1, View2D const& view) {
  // Debug-mode invariant checks. Disabled in Release (NDEBUG defined).
  assert(are_ccw(outer_coplanar_ccw));
  if constexpr (std::is_same_v<typename Points::value_type, Point3D>) {
    assert(are_coplanar(outer_coplanar_ccw));
  }
  assert(!is_convex_input || is_convex(outer_coplanar_ccw, view));
  assert(!is_convex_input || holes_coplanar_cw.empty());
  for (auto const& hole : holes_coplanar_cw) {
    assert(are_cw(hole));
    if constexpr (std::is_same_v<typename Points::value_type, Point3D>) {
      assert(are_coplanar(hole));
    }
  }

  // actual algorithm

  // Project the line endpoints to 2D once; all geometry lives in this view's plane.
  // edge e(i): v_0 = outer[i], v_1 = outer[(i+1)%n]  (CCW order)
  //   ex = v1x - v0x,  ey = v1y - v0y
  //   outward normal for CCW ring: n = (ey, -ex)
  //   N = -n · (P0 - Vi)  =  -(ey, -ex) · (p0x - v0x,  p0y - v0y)
  //                       = -(ey * (p0x - v0x) + (-ex) * (p0y - v0y))
  //                       = -(p0x - v0x) * ey + (p0y - v0y)*ex
  //   D = n · (P1 - P0)   =  dx*ey - dy*ex
  double p0x = view.x(line_p0), p0y = view.y(line_p0);
  double dx = view.x(line_p1) - p0x;
  double dy = view.y(line_p1) - p0y;

  auto edge_ND = [&](P const& v0, P const& v1) -> std::pair<double, double> {
    double v0x = view.x(v0), v0y = view.y(v0);
    double ex = view.x(v1) - v0x, ey = view.y(v1) - v0y;
    double N = -(p0x - v0x) * ey + (p0y - v0y) * ex;
    double D = dx * ey - dy * ex;
    return {N, D};
  };

  std::vector<std::pair<double, double>> t_list;

  if (is_convex_input) {  // quick exit on entering and leaving edges
    double t_e = std::numeric_limits<double>::min();
    bool is_t_e_set = false;
    double t_l = std::numeric_limits<double>::max();
    bool is_t_l_set = false;
    std::size_t n = outer_coplanar_ccw.size();

    for (std::size_t i = 0; i < n; ++i) {
      auto const& v_0 = outer_coplanar_ccw[i];
      auto const& v_1 = outer_coplanar_ccw[(i + 1) % n];
      auto [N, D] = edge_ND(v_0, v_1);

      auto D_compare_to_0 = compare(D, 0);
      if (D_compare_to_0 == 0) {  // line is parallel to edge e(i)
        if (compare(N, 0) < 0) {  // P0 is outside the edge e(i)
          break;                  // early terminate (valid for convex: one miss = total miss)

        } else {
          continue;  // ignore edge e(i)
        }
      }

      double t = N / D;
      if (D_compare_to_0 < 0) {  // the line is ENTERING polygon through the edge e(i)
        if (!is_t_e_set) {       // init
          t_e = t;
          is_t_e_set = true;

        } else {
          if (compare(t, t_e) > 0) {  // update our t_e to the greatest of the old and new value
            t_e = t;
          }
        }

        if (is_t_e_set && is_t_l_set &&  // if both are set
            compare(t_e, t_l) > 0) {     // quick rejection if not t_e < t_l
          break;
        }

      } else {              // the line is LEAVING polygon through the edge e(i), on D_compare_to_0 > 0
        if (!is_t_l_set) {  // init
          t_l = t;
          is_t_l_set = true;

        } else {
          if (compare(t, t_l) < 0) {  // update our t_l to the smallest of the old and new value
            t_l = t;
          }
        }

        if (is_t_e_set && is_t_l_set &&  // if both are set
            compare(t_e, t_l) > 0) {     // quick rejection if not t_e < t_l
          break;
        }
      }
    }

    // only if we have both an ENTERING edge and a LEAVING edge we have an intersection,
    // otherwise it's an overlap (OnPerimeter)
    if (is_t_e_set && is_t_l_set && compare(t_e, t_l) < 0) {
      t_list.emplace_back(t_e, t_l);
    }

  } else {  // Collect, Sort, Parity

    // we don't need to care about separating t_e from t_l
    // the Jordan Curve Theorem guarantees that an infinite line crossing a closed shape will always alternate:
    // Enter-> Leave-> Enter -> Leave, always paired up
    // (try it graphically on a piece of paper: outer CCW and inner CW guarantee this)

    // the list of ts will always have an even number
    // (mathematically guaranteed if we close the for-loop of points)
    // so we can expect to always have something like this once sorted (<t_e, t_l>, <t_e, t_l>, <t_e, t_l>, ...)

    // special case: overlap with vertex: the values of (sorted) t_e and t_l are equal: we will remove them

    std::vector<double> t_all;
    std::size_t n = outer_coplanar_ccw.size();

    for (std::size_t i = 0; i < n; ++i) {
      auto const& v_0 = outer_coplanar_ccw[i];
      auto const& v_1 = outer_coplanar_ccw[(i + 1) % n];  // guarantees the loop of points to be closed
      auto [N, D] = edge_ND(v_0, v_1);

      auto D_compare_to_0 = compare(D, 0);
      if (D_compare_to_0 == 0) {  // line is parallel to edge e(i)
        continue;  // skip — parallel to one edge does not mean the line misses the whole concave polygon
      }

      double t = N / D;
      t_all.push_back(t);
    }

    // sort the list
    if (!t_all.empty()) {
      // sort ASC, using the std::partial_ordering and the optimized ranges algorithm
      std::ranges::sort(t_all);
      // remove consecutive duplicates
      remove_all_duplicated_elements(t_all);

      if (!t_all.empty()) {
        std::size_t m = t_all.size();
        if (m % 2 != 0) {  // impossible (mathematically) to get an odd number
          throw std::runtime_error("found an odd number of t_all intervals in sorted vector");
        }

        for (std::size_t i = 0; i < m - 1; i += 2) {
          t_list.emplace_back(t_all[i], t_all[i + 1]);
        }
      }
    }
  }

  return t_list;
}

std::vector<std::pair<double, double>> compute_intersection_intervals_2d(
    std::vector<Point2D> const& outer_coplanar_ccw, std::vector<std::vector<Point2D>> const& holes_coplanar_cw,
    bool is_convex_input, Point2D const& line_p0, Point2D const& line_p1, View2D const& view) {
  return compute_parametric_intersection_intervals(outer_coplanar_ccw, holes_coplanar_cw, is_convex_input, line_p0,
                                                   line_p1, view);
}

template <PointContainer Points, Point P>
double distance_to(Points const& outer_loop, bool is_convex, P const& line_p0, P const& line_p1, View2D const& view) {
  std::size_t n = std::ranges::size(outer_loop);
  if (n == 0) {
    throw std::invalid_argument("distance_to: empty vertex range");
  }

  double lx0 = view.x(line_p0), ly0 = view.y(line_p0);
  double ldx = view.x(line_p1) - lx0, ldy = view.y(line_p1) - ly0;
  double llen = std::sqrt(ldx * ldx + ldy * ldy);
  if (compare(llen, 0.0) == 0) {
    throw std::invalid_argument("distance_to: line_p0, line_p1 project onto a zero-length line");
  }
  double nx = -ldy / llen, ny = ldx / llen;  // unit, perpendicular to the (projected) line

  auto signed_dist = [&](std::size_t i) {
    return (view.x(outer_loop[i]) - lx0) * nx + (view.y(outer_loop[i]) - ly0) * ny;
  };

  // convex: binary search based O(LogN) algorithm
  if (is_convex) {
    // Daniel Sunday's O(log n) extreme-vertex binary search (cf. detail::extreme_points), reimplemented
    // here on view-projected scalars: view.x()/y() have no native-vector equivalent to dot a direction
    // against, so the native detail::extreme_points can't be reused for the projected case.
    long const ln = static_cast<long>(n);
    auto proj = [&](long i) { return signed_dist(static_cast<std::size_t>(((i % ln) + ln) % ln)); };
    auto sgn = [](double d) -> int { return static_cast<int>(d > 0) - static_cast<int>(d < 0); };
    auto search = [&](double s) -> std::size_t {
      auto g = [&](long i) { return s * proj(i); };
      auto cmp = [&](long i, long j) { return sgn(g(j) - g(i)); };
      auto extr = [&](long i) { return cmp(i + 1, i) >= 0 && cmp(i, i - 1) < 0; };
      if (extr(0)) {
        return 0;
      }
      long lo = 0, hi = ln;
      while (lo + 1 < hi) {
        long m = (lo + hi) / 2;
        if (extr(m)) {
          return static_cast<std::size_t>(m);
        }
        int ls = cmp(lo + 1, lo);
        int ms = cmp(m + 1, m);
        bool go_hi = (ls < ms) || (ls == ms && ls == cmp(lo, m));
        if (go_hi) {
          hi = m;
        } else {
          lo = m;
        }
      }
      return static_cast<std::size_t>(lo);
    };

    double d_max = signed_dist(search(1.0));
    double d_min = signed_dist(search(-1.0));

    // polygon straddles the line if the extreme signed offsets have opposite sign (or touch)
    if (compare(d_min, 0) <= 0 && compare(d_max, 0) >= 0) {
      return 0.0;
    }

    return std::min(std::abs(d_min), std::abs(d_max));
  }

  // non convex: brute force O(N) algorithm
  double min_d = std::numeric_limits<double>::infinity();
  for (std::size_t i = 0; i < n; ++i) {
    double d0 = signed_dist(i);
    double d1 = signed_dist((i + 1) % n);

    // the signed distance is affine along the edge, so its rate of change is exactly d1 - d0;
    // solving d0 + t*(d1-d0) = 0 gives the parametric crossing point without any extra dot products
    double denom = d1 - d0;
    if (compare(denom, 0.0) != 0) {
      double t = -d0 / denom;
      if (is_in_range(t, 0.0, 1.0)) {
        return 0.0;  // this edge crosses the line
      }
    }

    // no crossing: the closest point on this edge to the line is one of its endpoints
    min_d = std::min({min_d, std::abs(d0), std::abs(d1)});
  }

  return min_d;
}

template double distance_to(std::vector<Point2D> const&, bool, Point2D const&, Point2D const&, View2D const&);
template double distance_to(std::vector<Point3D> const&, bool, Point3D const&, Point3D const&, View2D const&);

}  // namespace view

double distance_to(std::vector<Point2D> const& outer_loop, bool is_convex, Point2D const& line_p0,
                   Point2D const& line_p1) {
  return view::distance_to(outer_loop, is_convex, line_p0, line_p1, View2D::XY());
}

namespace view {

namespace {

// Cross-product orientation tests, projected through `view` (dimension-agnostic: works for Point2D or Point3D).
template <Point P>
bool is_left(P const& v1, P const& v2, P const& p, View2D const& view) {
  double cross = (view.x(v2) - view.x(v1)) * (view.y(p) - view.y(v1)) -
                 (view.y(v2) - view.y(v1)) * (view.x(p) - view.x(v1));
  return compare(cross, 0.0) > 0;
}

template <Point P>
bool is_right(P const& v1, P const& v2, P const& p, View2D const& view) {
  double cross = (view.x(v2) - view.x(v1)) * (view.y(p) - view.y(v1)) -
                 (view.y(v2) - view.y(v1)) * (view.x(p) - view.x(v1));
  return compare(cross, 0.0) < 0;
}

// tells if v1 is above v2, with respect to point p (ref)
template <Point P>
bool is_above(P const& p, P const& v1, P const& v2, View2D const& view) {
  return is_left(p, v1, v2, view);
}
// tells if v1 is below v2, with respect to point p (ref)
template <Point P>
bool is_below(P const& p, P const& v1, P const& v2, View2D const& view) {
  return is_right(p, v1, v2, view);
}

/// @brief finds the index of outer_loop corresponding to the RIGHT tangent from point p (Dan Sunday's O(log n)
/// binary search). Assumes outer_loop is CONVEX and CCW, projected through `view`.
template <PointContainer Points, Point P>
std::size_t r_tangent(Points const& outer_loop, P const& p, View2D const& view) {
  std::size_t n = outer_loop.size();
  if (n < 3) {
    throw std::invalid_argument("r_tangent requires at least 3 vertices");
  }

  // test if the first vertex is a local max
  if (is_below(p, outer_loop[1], outer_loop[0], view) && !is_above(p, outer_loop[n - 1], outer_loop[0], view)) {
    return 0;
  }

  std::size_t a = 0, b = n;
  std::size_t max_iter = n + 1;  // each iteration strictly halves [a,b], so this is a generous safety bound
  while (max_iter-- > 0) {
    if (b - a <= 1) {  // narrowed to a single edge: whichever endpoint is more "above" wins
      return is_above(p, outer_loop[a % n], outer_loop[b % n], view) ? a % n : b % n;
    }
    std::size_t c = (a + b) / 2;
    bool dnC = is_below(p, outer_loop[(c + 1) % n], outer_loop[c % n], view);
    if (dnC && !is_above(p, outer_loop[(c + n - 1) % n], outer_loop[c % n], view)) {  // found the max tangent point
      return c % n;
    }
    // no max yet, continue the binary search, in either sub-chain [a,c] or [c,b]
    bool upA = is_above(p, outer_loop[(a + 1) % n], outer_loop[a % n], view);
    if (upA) {
      if (dnC) {
        b = c;
      } else if (is_above(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    } else {
      if (!dnC) {
        a = c;
      } else if (is_below(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    }
  }

  throw std::logic_error("loop above max_iter without finding c in binary search (r_tangent)");
}

/// @brief finds the index of outer_loop corresponding to the LEFT tangent from point p (Dan Sunday's O(log n)
/// binary search). Assumes outer_loop is CONVEX and CCW, projected through `view`.
template <PointContainer Points, Point P>
std::size_t l_tangent(Points const& outer_loop, P const& p, View2D const& view) {
  std::size_t n = outer_loop.size();
  if (n < 3) {
    throw std::invalid_argument("l_tangent requires at least 3 vertices");
  }

  // test if the first vertex is a local min
  if (!is_below(p, outer_loop[1], outer_loop[0], view) && is_above(p, outer_loop[n - 1], outer_loop[0], view)) {
    return 0;
  }

  std::size_t a = 0, b = n;
  std::size_t max_iter = n + 1;  // each iteration strictly halves [a,b], so this is a generous safety bound
  while (max_iter-- > 0) {
    if (b - a <= 1) {  // narrowed to a single edge: whichever endpoint is more "below" wins
      return is_below(p, outer_loop[a % n], outer_loop[b % n], view) ? a % n : b % n;
    }
    std::size_t c = (a + b) / 2;
    bool dnC = is_below(p, outer_loop[(c + 1) % n], outer_loop[c % n], view);
    if (!dnC && is_above(p, outer_loop[(c + n - 1) % n], outer_loop[c % n], view)) {  // found the min tangent point
      return c % n;
    }
    // no min yet, continue the binary search, in either sub-chain [a,c] or [c,b]
    bool dnA = is_below(p, outer_loop[(a + 1) % n], outer_loop[a % n], view);
    if (dnA) {
      if (!dnC) {
        b = c;
      } else if (is_below(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    } else {
      if (dnC) {
        a = c;
      } else if (is_above(p, outer_loop[a % n], outer_loop[c % n], view)) {
        b = c;
      } else {
        a = c;
      }
    }
  }

  throw std::logic_error("loop above max_iter without finding c in binary search (l_tangent)");
}

// Reduces `loop` to its convex hull (projected through `view`) when it isn't already convex. Returns the hull
// points alongside the indices (into the ORIGINAL loop) each hull point came from, so callers can map hull-local
// results back to the caller's index space. When `already_convex`, both are trivial identity pass-throughs.
template <PointContainer Points>
std::pair<std::vector<std::ranges::range_value_t<Points>>, std::vector<std::size_t>> to_convex_loop(
    Points const& loop, bool already_convex, View2D const& view) {
  using PointT = std::ranges::range_value_t<Points>;

  std::vector<std::size_t> idx;
  if (already_convex) {
    idx.resize(loop.size());
    std::iota(idx.begin(), idx.end(), std::size_t{0});
  } else {
    idx = convex_hull_monotone_chain(loop, view);
  }

  std::vector<PointT> pts;
  pts.reserve(idx.size());
  for (auto i : idx) {
    pts.push_back(loop[i]);
  }
  return {std::move(pts), std::move(idx)};
}

}  // namespace

template <PointContainer Points, Point P>
std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(Points const& outer_loop, bool is_convex, P const& p,
                                                              View2D const& view) {
  // if the polygon is convex we can use the binary search, O(logN)
  if (is_convex) {
    return {l_tangent(outer_loop, p, view), r_tangent(outer_loop, p, view)};
  }

  // otherwise reduce to the convex hull first — a tangent from an external point can only ever touch a hull
  // vertex — then map the hull-local result back to an index into the original outer_loop
  auto [hull_pts, hull_idx] = to_convex_loop(outer_loop, /*already_convex=*/false, view);
  return {hull_idx[l_tangent(hull_pts, p, view)], hull_idx[r_tangent(hull_pts, p, view)]};
}

template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const&, bool,
                                                                       Point2D const&, View2D const&);
template std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point3D> const&, bool,
                                                                       Point3D const&, View2D const&);

template <PointContainer Points>
std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(Points const& loop1, bool is_convex1, Points const& loop2,
                                                             bool is_convex2, View2D const& view) {
  auto [cv1, idx1] = to_convex_loop(loop1, is_convex1, view);
  auto [cv2, idx2] = to_convex_loop(loop2, is_convex2, view);

  std::size_t m = cv1.size();
  std::size_t n = cv2.size();
  if (m < 3 || n < 3) {
    throw std::invalid_argument("poly_poly_RL_tangent_to requires polygons with at least 3 vertices");
  }

  std::size_t i1 = r_tangent(cv1, cv2[0], view);
  std::size_t i2 = l_tangent(cv2, cv1[i1], view);

  std::size_t max_iter = m * n + 1;
  bool done = false;
  while (!done && max_iter-- > 0) {
    done = true;

    std::size_t guard1 = 0;
    while (!is_left(cv2[i2], cv1[i1], cv1[(i1 + 1) % m], view)) {
      i1 = (i1 + 1) % m;
      if (++guard1 > m) {
        throw std::logic_error("poly_poly_RL_tangent_to: i1 walk failed to converge");
      }
    }

    std::size_t guard2 = 0;
    while (!is_right(cv1[i1], cv2[i2], cv2[(i2 + n - 1) % n], view)) {
      i2 = (i2 + n - 1) % n;
      done = false;
      if (++guard2 > n) {
        throw std::logic_error("poly_poly_RL_tangent_to: i2 walk failed to converge");
      }
    }
  }
  if (!done) {
    throw std::logic_error("poly_poly_RL_tangent_to exceeded max iterations without converging");
  }

  return {idx1[i1], idx2[i2]};
}

template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const&, bool,
                                                                      std::vector<Point2D> const&, bool,
                                                                      View2D const&);
template std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point3D> const&, bool,
                                                                      std::vector<Point3D> const&, bool,
                                                                      View2D const&);

}  // namespace view

std::pair<std::size_t, std::size_t> point_poly_tangent_lr_to(std::vector<Point2D> const& outer_loop, bool is_convex,
                                                             Point2D const& p) {
  return view::point_poly_tangent_lr_to(outer_loop, is_convex, p, View2D::XY());
}

std::pair<std::size_t, std::size_t> poly_poly_RL_tangent_to(std::vector<Point2D> const& loop1, bool is_convex1,
                                                            std::vector<Point2D> const& loop2, bool is_convex2) {
  return view::poly_poly_RL_tangent_to(loop1, is_convex1, loop2, is_convex2, View2D::XY());
}

}  // namespace detail

ExtremePoints<Point2D> find_extreme_points(Polygon2D const& polygon, Line2D const& line) {
  auto [min_i, max_i] = detail::extreme_points(polygon.Perimeter(), polygon.IsConvex(), line.Direction());
  return {polygon[min_i], polygon[max_i]};
}

double distance_to(Polygon2D const& polygon, Line2D const& line) {
  return detail::distance_to(polygon.Perimeter(), polygon.IsConvex(), line.First(), line.Last());
}

PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Point2D const& p) {
  auto [left_i, right_i] = detail::point_poly_tangent_lr_to(polygon.Perimeter(), polygon.IsConvex(), p);
  return {LineSegment2D::Make(p, polygon[left_i]), LineSegment2D::Make(p, polygon[right_i])};
}

PolygonTangents<LineSegment2D> tangents_to(Polygon2D const& polygon, Polygon2D const& other) {
  auto [RL_poly_i, RL_other_i] =
      detail::poly_poly_RL_tangent_to(polygon.Perimeter(), polygon.IsConvex(), other.Perimeter(), other.IsConvex());
  auto [LR_other_i, LR_poly_i] =
      detail::poly_poly_RL_tangent_to(other.Perimeter(), other.IsConvex(), polygon.Perimeter(), polygon.IsConvex());
  return {LineSegment2D::Make(polygon[RL_poly_i], other[RL_other_i]),
          LineSegment2D::Make(polygon[LR_poly_i], other[LR_other_i])};
}

}  // namespace geompp
