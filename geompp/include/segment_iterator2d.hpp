#pragma once

#include "line_segment2d.hpp"
#include "point2d.hpp"

#include <cstddef>
#include <iterator>
#include <ranges>
#include <vector>

namespace geompp {

class SegmentIterator2D {
 public:
  using value_type        = LineSegment2D;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  using pointer           = void;
  using reference         = LineSegment2D;

  SegmentIterator2D(std::vector<Point2D> const* pts, std::size_t i, bool closed)
      : PTS(pts), I(i), CLOSED(closed) {}

  LineSegment2D operator*() const {
    std::size_t j = CLOSED ? (I + 1) % PTS->size() : I + 1;
    return LineSegment2D::Make((*PTS)[I], (*PTS)[j]);
  }

  SegmentIterator2D& operator++() { ++I; return *this; }
  SegmentIterator2D  operator++(int) { auto tmp = *this; ++I; return tmp; }

  bool operator==(SegmentIterator2D const& o) const { return I == o.I; }
  bool operator!=(SegmentIterator2D const& o) const { return I != o.I; }

 private:
  std::vector<Point2D> const* PTS;
  std::size_t I;
  bool CLOSED;
};

class SegmentRange2D {
 public:
  SegmentRange2D(std::vector<Point2D> const& pts, bool closed = false)
      : PTS(&pts), CLOSED(closed) {}

  SegmentIterator2D begin() const { return {PTS, 0, CLOSED}; }
  SegmentIterator2D end()   const {
    std::size_t n = PTS->size();
    return {PTS, CLOSED ? n : (n > 0 ? n - 1 : 0), CLOSED};
  }
  std::size_t size() const {
    std::size_t n = PTS->size();
    return CLOSED ? n : (n > 0 ? n - 1 : 0);
  }
  LineSegment2D operator[](std::size_t i) const {
    std::size_t j = CLOSED ? (i + 1) % PTS->size() : i + 1;
    return LineSegment2D::Make((*PTS)[i], (*PTS)[j]);
  }

 private:
  std::vector<Point2D> const* PTS;
  bool CLOSED;
};

}  // namespace geompp

namespace std::ranges {
template <>
inline constexpr bool enable_borrowed_range<geompp::SegmentRange2D> = true;
}
