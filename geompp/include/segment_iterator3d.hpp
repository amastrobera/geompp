#pragma once

#include "line_segment3d.hpp"
#include "point3d.hpp"

#include <cstddef>
#include <iterator>
#include <ranges>
#include <vector>

namespace geompp {

inline namespace geometry {

class SegmentIterator3D {
 public:
  using value_type        = LineSegment3D;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  using pointer           = void;
  using reference         = LineSegment3D;

  SegmentIterator3D(std::vector<Point3D> const* pts, std::size_t i, bool closed)
      : PTS(pts), I(i), CLOSED(closed) {}

  LineSegment3D operator*() const {
    std::size_t j = CLOSED ? (I + 1) % PTS->size() : I + 1;
    return LineSegment3D::Make((*PTS)[I], (*PTS)[j]);
  }

  SegmentIterator3D& operator++() { ++I; return *this; }
  SegmentIterator3D  operator++(int) { auto tmp = *this; ++I; return tmp; }

  bool operator==(SegmentIterator3D const& o) const { return I == o.I; }
  bool operator!=(SegmentIterator3D const& o) const { return I != o.I; }

 private:
  std::vector<Point3D> const* PTS;
  std::size_t I;
  bool CLOSED;
};

class SegmentRange3D {
 public:
  SegmentRange3D(std::vector<Point3D> const& pts, bool closed = false)
      : PTS(&pts), CLOSED(closed) {}

  SegmentIterator3D begin() const { return {PTS, 0, CLOSED}; }
  SegmentIterator3D end()   const {
    std::size_t n = PTS->size();
    return {PTS, CLOSED ? n : (n > 0 ? n - 1 : 0), CLOSED};
  }
  std::size_t size() const {
    std::size_t n = PTS->size();
    return CLOSED ? n : (n > 0 ? n - 1 : 0);
  }
  LineSegment3D operator[](std::size_t i) const {
    std::size_t j = CLOSED ? (i + 1) % PTS->size() : i + 1;
    return LineSegment3D::Make((*PTS)[i], (*PTS)[j]);
  }

 private:
  std::vector<Point3D> const* PTS;
  bool CLOSED;
};

}  // namespace geometry

}  // namespace geompp

namespace std::ranges {
template <>
inline constexpr bool enable_borrowed_range<geompp::SegmentRange3D> = true;
}
