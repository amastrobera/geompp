#include "calc_utils2d.hpp"

#include "line_segment2d.hpp"
#include "polygon2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <stdexcept>

namespace geompp {

bool Event2D::operator<(Event2D const& other) const {
  // order by x coordinate, then by y coordinate, then by event type (LEFT < INTERSECTION < RIGHT)

  if (auto compare_x = compare(point.x(), other.point.x())) {
    return compare_x < 0;
  }

  if (auto compare_y = compare(point.y(), other.point.y())) {
    return compare_y < 0;
  }

  return type < other.type;  // relies on the order of the EventType2D enum
                             // prioritize LEFT < INTERSECTION < RIGHT events
                             // note on D.Sunday book: helps to process edge cases
}

Event2D EventQueue2D::Pop() {
  if (EVENTS.empty()) {
    throw std::runtime_error("EventQueue is empty");
  }
  Event2D top = EVENTS.top();
  EVENTS.pop();
  return top;
}

bool EventQueue2D::Empty() const { return EVENTS.empty(); }

void EventQueue2D::Swap(std::size_t segment_index1, std::size_t segment_index2) {
  for (auto& event : EVENTS.get_container()) {
    if (event.segment == segment_index1) {
      event.segment = segment_index2;

    } else if (event.segment == segment_index2) {
      event.segment = segment_index1;
    }

    if (event.inter_segment.has_value()) {
      if (event.inter_segment.value() == segment_index1) {
        event.inter_segment = segment_index2;

      } else if (event.inter_segment.value() == segment_index2) {
        event.inter_segment = segment_index1;
      }
    }
  }
}

}  // namespace geompp
