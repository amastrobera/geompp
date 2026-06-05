#include "calc_utils2d.hpp"

#include "line_segment2d.hpp"
#include "polygon2d.hpp"
#include "segment_iterator2d.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <cstddef>
#include <iterator>
#include <stdexcept>

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
  // {} default-constructs std::less<Event2D>, which uses operator<(Event2D) for ordering
  EVENTS = EventPriorityQueue(std::priority_queue<Event2D>({}, build_events(segments)));
}

EventQueue2D::EventQueue2D(SegmentRange2D const& segments) {
  // {} default-constructs std::less<Event2D>, which uses operator<(Event2D) for ordering
  EVENTS = EventPriorityQueue(std::priority_queue<Event2D>({}, build_events(segments)));
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

std::optional<Event2D> EventQueue2D::Next() {
  if (EVENTS.empty()) {
    return std::nullopt;
  }
  Event2D top = EVENTS.top();
  EVENTS.pop();
  return top;
}

bool EventQueue2D::Empty() const { return EVENTS.empty(); }

bool EventQueue2D::Contains(Event2D const& event) const {
  for (const auto& queued_event : EVENTS.c) {
    if (queued_event == event) {
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

SweepLineSegment2D::SweepLineSegment2D(std::size_t id, LineSegment2D const& segment)
    : EdgeId(id), Above(nullptr), Below(nullptr), Left(segment.First()), Right(segment.Last()) {
  if (compare_event_point(Left, Right) > 0) {
    std::swap(Left, Right);
  }
}

// PROVISIONAL total order: by left endpoint, then right endpoint, then EdgeId (the EdgeId tiebreaker keeps
// distinct segments distinct in the std::set even when they share endpoints). This is NOT the real sweep-line
// status order (y at the current sweep x) — it only gives std::set a deterministic, total comparison so the
// structure works and is testable. TODO: replace with the true sweep-status comparator.
bool SweepLineSegment2D::operator<(SweepLineSegment2D const& other) const {
  auto cmp_left = compare_event_point(Left, other.Left);
  if (cmp_left != std::partial_ordering::equivalent) {
    return cmp_left < 0;
  }
  auto cmp_right = compare_event_point(Right, other.Right);
  if (cmp_right != std::partial_ordering::equivalent) {
    return cmp_right < 0;
  }
  return EdgeId < other.EdgeId;
}

// SweepLine2D template members: defined here, emitted for other TUs by the explicit instantiations below.
// Each definition needs its own `template <SegmentList Segments>` and the SweepLine2D<Segments>:: qualifier.

template <SegmentList Segments>
SweepLineSegment2D const* SweepLine2D<Segments>::Find(std::size_t seg_id) const {
  if (seg_id >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("SegmentId is out of range of the segments list");
  }
  auto const& orig_seg = (*PTR_SEGMENTS)[seg_id];
  SweepLineSegment2D temp_sl_seg(seg_id, orig_seg);

  auto it = ACTIVE_SEGMENTS.find(temp_sl_seg);
  if (it == ACTIVE_SEGMENTS.end()) {
    return nullptr;
  }
  return &*it;
}

template <SegmentList Segments>
void SweepLine2D<Segments>::Swap(std::size_t seg1_id, std::size_t seg2_id) {
  // we assume that a segment can already have been treated, and maybe the LEFT or the INTERSECTION events are gone, but
  // the RIGHT event is still there
  // the EventType2D enum order and the operator< guarantee that RIGHT is the last event standing for each segment in
  // the sweep line
  auto* seg1 = Find(seg1_id);
  auto* seg2 = Find(seg2_id);
  // if either is not found, throw
  if (seg1 == nullptr || seg2 == nullptr) {
    return;  // nothing to do
  }

  // swap the references if any
  if (seg1->Above) {
    seg1->Above->Below = &*seg2;
  }
  if (seg1->Below) {
    seg1->Below->Above = &*seg2;
  }
  if (seg2->Above) {
    seg2->Above->Below = &*seg1;
  }
  if (seg2->Below) {
    seg2->Below->Above = &*seg1;
  }

  // swap the above
  std::swap(seg1->Above, seg2->Above);
  // swap the below
  std::swap(seg1->Below, seg2->Below);

  // remove the item
  ACTIVE_SEGMENTS.erase(*seg1);
  ACTIVE_SEGMENTS.erase(*seg2);

  // reinsert
  ACTIVE_SEGMENTS.insert(*seg1);
  ACTIVE_SEGMENTS.insert(*seg2);
}

template <SegmentList Segments>
SweepLineSegment2D const* SweepLine2D<Segments>::Add(std::size_t seg_id) {
  if (seg_id >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("SegmentId is out of range of the segments list");
  }
  LineSegment2D const& orig_seg = (*PTR_SEGMENTS)[seg_id];

  // insert() returns {iterator, bool}: `it` points at the stored node, `inserted` says whether it was new.
  auto [it, inserted] = ACTIVE_SEGMENTS.insert(SweepLineSegment2D(seg_id, orig_seg));
  if (!inserted) {
    throw std::logic_error("Attempted to insert a segment into the sweep line that is already active");
  }

  // link to the neighbour ABOVE (successor in the ordering)
  auto next_it = std::next(it);
  if (next_it != ACTIVE_SEGMENTS.end()) {
    it->Above = &*next_it;
    next_it->Below = &*it;
  }

  // link to the neighbour BELOW (predecessor) — compute std::prev only AFTER the begin() guard
  if (it != ACTIVE_SEGMENTS.begin()) {
    auto prev_it = std::prev(it);
    it->Below = &*prev_it;
    prev_it->Above = &*it;
  }

  return &*it;
}

template <SegmentList Segments>
void SweepLine2D<Segments>::Remove(SweepLineSegment2D const*& segment) {
  if (segment == nullptr) {
    throw std::invalid_argument("Cannot remove a null segment from the sweep line");
  }

  auto it = ACTIVE_SEGMENTS.find(*segment);
  if (it == ACTIVE_SEGMENTS.end()) {
    GEOMPP_LOG(ERROR) << "Attempted to remove a segment from the sweep line that is not active";
    return;
  }

  // reset the next
  auto next_it = std::next(it);
  if (next_it != ACTIVE_SEGMENTS.end()) {
    next_it->Below = it->Below;
  }

  // reset the prev — compute std::prev only AFTER the begin() guard (std::prev(begin()) is UB)
  if (it != ACTIVE_SEGMENTS.begin()) {
    auto prev_it = std::prev(it);
    prev_it->Above = it->Above;
  }

  // remove the item
  ACTIVE_SEGMENTS.erase(it);

  // nullify the caller's pointer to prevent reuse after removal
  segment = nullptr;
}

template <SegmentList Segments>
std::optional<Point2D> SweepLine2D<Segments>::Intersection(SweepLineSegment2D const* seg1,
                                                           SweepLineSegment2D const* seg2) const {
  if (seg1 == nullptr || seg2 == nullptr) {
    throw std::invalid_argument("Cannot compute intersection with a null segment");
  }

  if (seg1->EdgeId >= PTR_SEGMENTS->size() || seg2->EdgeId >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("Segment EdgeId is out of range of the segments list");
  }

  auto const& line_seg1 = (*PTR_SEGMENTS)[seg1->EdgeId];
  auto const& line_seg2 = (*PTR_SEGMENTS)[seg2->EdgeId];

  auto seg_inter = line_seg1.Intersection(line_seg2);
  if (seg_inter.has_value() && std::holds_alternative<Point2D>(seg_inter.value())) {
    return std::get<Point2D>(seg_inter.value());
  }
  return std::nullopt;
}

template <SegmentList Segments>
bool SweepLine2D<Segments>::Intersect(SweepLineSegment2D const* seg1, SweepLineSegment2D const* seg2) const {
  if (seg1 == nullptr || seg2 == nullptr) {
    throw std::invalid_argument("Cannot compute intersection with a null segment");
  }

  if (seg1->EdgeId >= PTR_SEGMENTS->size() || seg2->EdgeId >= PTR_SEGMENTS->size()) {
    throw std::out_of_range("Segment EdgeId is out of range of the segments list");
  }

  auto const& line_seg1 = (*PTR_SEGMENTS)[seg1->EdgeId];
  auto const& line_seg2 = (*PTR_SEGMENTS)[seg2->EdgeId];

  return intersect(line_seg1, line_seg2);  // quicker than computing the intersection, just checking if it exists:
}

// ------- free functions -------

template <SegmentList Segments>
bool has_intersections(Segments const& polygon_segments) {
  if (polygon_segments.size() < 3) {
    throw std::invalid_argument("A polygon must have at least 3 segments");
  }

  std::size_t n = polygon_segments.size();
  if (polygon_segments[0].First() != polygon_segments[n - 1].Last()) {
    throw std::invalid_argument("The segments do not form a closed polygon");
  }

  EventQueue2D event_queue(polygon_segments);
  SweepLine2D<Segments> sweep_line(polygon_segments);

  while (!event_queue.Empty()) {
    auto event_opt = event_queue.Next();
    if (!event_opt.has_value()) {
      throw std::logic_error("Event queue is unexpectedly empty");
    }
    auto event = event_opt.value();

    if (event.Type == EventType2D::LEFT) {
      auto* seg_node = sweep_line.Add(event.SegmentId);
      // guard the neighbours: Above/Below are null at the top/bottom of the status set, and Intersect throws on null
      if ((seg_node->Above != nullptr && sweep_line.Intersect(seg_node, seg_node->Above)) ||
          (seg_node->Below != nullptr && sweep_line.Intersect(seg_node, seg_node->Below))) {
        return false;  // found an intersection, the polygon is not simple
      }

    } else {
      auto* seg_node = sweep_line.Find(event.SegmentId);
      if (seg_node == nullptr) {
        throw std::logic_error("Could not find the segment corresponding to the RIGHT event in the sweep line");
      }
      // the two neighbours become adjacent once this segment leaves; only test if both exist
      if (seg_node->Above != nullptr && seg_node->Below != nullptr &&
          sweep_line.Intersect(seg_node->Above, seg_node->Below)) {
        return false;  // found an intersection, the polygon is not simple
      }
      sweep_line.Remove(seg_node);
    }
  }

  return true;  // no intersections found, the polygon is simple
}

template <SegmentList Segments>
std::vector<IntersectionEvent2D> find_intersections(Segments const& polygon_segments) {
  if (polygon_segments.size() < 3) {
    throw std::invalid_argument("A polygon must have at least 3 segments");
  }

  std::size_t n = polygon_segments.size();
  if (polygon_segments[0].First() != polygon_segments[n - 1].Last()) {
    throw std::invalid_argument("The segments do not form a closed polygon");
  }

  EventQueue2D event_queue(polygon_segments);
  SweepLine2D<Segments> sweep_line(polygon_segments);

  std::set<IntersectionEvent2D> output_list;

  while (!event_queue.Empty()) {
    auto event_opt = event_queue.Next();
    if (!event_opt.has_value()) {
      throw std::logic_error("Event queue is unexpectedly empty");
    }
    auto event = event_opt.value();

    if (event.Type == EventType2D::LEFT) {
      auto* seg_node = sweep_line.Add(event.SegmentId);

      // check if the immediate neighbours above and below intersect, and if so add the intersection event to the queue
      if (seg_node->Above != nullptr) {
        if (auto inter_p = sweep_line.Intersection(seg_node, seg_node->Above)) {
          event_queue.EVENTS.push(Event2D{EventType2D::INTERSECTION, inter_p.value(), seg_node->EdgeId,
                                          seg_node->Above->EdgeId});  // below then above IDs
        }
      }

      if (seg_node->Below != nullptr) {
        if (auto inter_p = sweep_line.Intersection(seg_node, seg_node->Below)) {
          event_queue.EVENTS.push(Event2D{EventType2D::INTERSECTION, inter_p.value(), seg_node->Below->EdgeId,
                                          seg_node->EdgeId});  // below then above IDs
        }
      }

    } else if (event.Type == EventType2D::RIGHT) {
      auto* seg_node = sweep_line.Find(event.SegmentId);
      if (seg_node == nullptr) {  // impossible at this stage
        throw std::logic_error("Could not find the segment corresponding to the RIGHT event in the sweep line");
      }

      auto* above = seg_node->Above;
      auto* below = seg_node->Below;

      sweep_line.Remove(seg_node);  // automatically resets the above/below neighbours of the segment being removed to
                                    // the new neighbours after removal

      if (auto inter_p = sweep_line.Intersection(above, below)) {
        auto inter_event =
            Event2D{EventType2D::INTERSECTION, inter_p.value(), below->EdgeId, above->EdgeId};  // below then above IDs
        if (!event_queue.Contains(inter_event)) {
          event_queue.EVENTS.push(inter_event);
        }
      }

    } else if (event.Type == EventType2D::INTERSECTION) {
      std::size_t seg1_id = event.SegmentId;
      std::size_t seg2_id = event.InterSegmentId.value();

      // save the intersection event to the output list
      auto inter_event = IntersectionEvent2D{event.Point, {seg1_id, seg2_id}};
      // avoid duplicates in the output list
      if (auto output_to_update = output_list.find(inter_event) != output_list.end()) {
        if (!output_to_update->SegmentIds.contains(seg1_id)) {
          output_to_update->SegmentIds.push_back(seg1_id);
        }
        if (!output_to_update->SegmentIds.contains(seg2_id)) {
          output_to_update->SegmentIds.push_back(seg2_id);
        }
      } else {
        output_list.emplace(inter_event);
      }

      // At this stage both the SegmentId and InterSegmentId must exist inside the sweep line
      // In fact - INTERSECTION Event comes before the Right (guaranteed by opearotor< of Event2D and the
      // EventType2D enum order)
      // swap the segments in the sweep line status (past the intersection point the below becomes above, and viceversa)
      sweep_line.Swap(seg1_id, seg2_id);

      // ... and we can find both segments in the sweep line by using the RIGHT event
      auto* ref_seg1 = sweep_line.Find(seg1_id);
      auto* ref_seg2 = sweep_line.Find(seg2_id);
      if (ref_seg1 == nullptr || ref_seg2 == nullptr) {  // impossible
        continue;                                        // nothing to do
      }

      // since we swapped them, they have new neighbours, so we need to check if the new neighbours intersect and if so
      // add the intersection event to the queue
      auto* ref_above2 = ref_seg2->Above;
      if (ref_above2 != nullptr) {
        if (auto inter_p = sweep_line.Intersection(ref_seg2, ref_above2)) {
          auto inter_event = Event2D{EventType2D::INTERSECTION, inter_p.value(), ref_seg2->EdgeId,
                                     ref_above2->EdgeId};  // below then above IDs
          if (!event_queue.Contains(inter_event)) {
            event_queue.EVENTS.push(inter_event);
          }
        }
      }
      auto* ref_below1 = ref_seg1->Below;
      if (ref_below1 != nullptr) {
        if (auto inter_p = sweep_line.Intersection(ref_seg1, ref_below1)) {
          auto inter_event = Event2D{EventType2D::INTERSECTION, inter_p.value(), ref_below1->EdgeId,
                                     ref_seg1->EdgeId};  // below then above IDs
          if (!event_queue.Contains(inter_event)) {
            event_queue.EVENTS.push(inter_event);
          }
        }
      }
    }
  }

  return output_list;
}

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

}  // namespace geompp
