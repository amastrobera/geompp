#pragma once

#include "constants.hpp"
#include "point2d.hpp"

#include <optional>
#include <queue>
#include <vector>

namespace geompp {

class Polygon2D;
class LineSegment2D;

enum class EventType2D { LEFT, RIGHT, INTERSECTION };

struct Event2D {
  EventType2D type;
  Point2D point;
  std::size_t segment;  // reference to the segment that generated the event (if EventType::LEFT or EventType::RIGHT)
  std::optional<std::size_t> inter_segment;  // if EventType::INTERSECTION this is the second segment involved in the
                                             // intersection, otherwise std::nullopt

  bool operator<(Event2D const& other) const;
};

class EventQueue2D {
 public:
  // EventQueue2D(Polygon2D const& polygon); // ideal case to check for self-intersections of a polygon
  EventQueue2D(std::vector<LineSegment2D> const& segments);
  ~EventQueue2D() = default;

  Event2D Pop();
  bool Empty() const;
  void Swap(std::size_t segment_index1,
            std::size_t segment_index2);  // swap the segments that generated the events at index1 and index2

 private:
  std::priority_queue<Event2D> EVENTS;  // Event2D implements operator<
};

// class SweepLine2D {
//     public:
//     SweepLine2D() = default;
//     ~SweepLine2D() = default;

//     void Insert(std::size_t segment_index);
//     void Remove(std::size_t segment_index);
//     std::optional<std::size_t> Above(std::size_t segment_index) const;
//     std::optional<std::size_t> Below(std::size_t segment_index) const;

//     private:
//     std::vector<std::size_t> ACTIVE_SEGMENTS;  // indices of segments currently intersecting the sweep line, ordered
//     by their intersection point with the sweep line
// }

}  // namespace geompp
