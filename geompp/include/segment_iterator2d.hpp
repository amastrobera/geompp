#pragma once

#include "constants.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"

#include <cstddef>
#include <format>
#include <iterator>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace geompp {

inline namespace geometry {

class SegmentIterator2D {
 public:
  using value_type        = LineSegment2D;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;
  using pointer           = void;
  using reference         = LineSegment2D;

  SegmentIterator2D(std::vector<Point2D> const* pts, std::size_t i, bool closed)
      : PTS(pts), I(i), CLOSED(closed) {}

  // Unchecked on purpose: PTS was already validated once, in full, by the SegmentRange2D that produced this
  // iterator (see SegmentRange2D's constructor) — see the friendship comment on LineSegment2D.
  LineSegment2D operator*() const {
    std::size_t j = CLOSED ? (I + 1) % PTS->size() : I + 1;
    return LineSegment2D((*PTS)[I], (*PTS)[j]);
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
  // Validates every consecutive pair (including the closing edge, when closed) once, up front — the same
  // degeneracy check LineSegment2D::Make() does, but paid once here instead of on every operator[]/operator*()
  // access. begin()/end()/operator[] then build segments through LineSegment2D's private constructor (this
  // class is a friend), skipping that now-redundant re-check.
  SegmentRange2D(std::vector<Point2D> const& pts, bool closed = false) : PTS(&pts), CLOSED(closed) {
    std::size_t n = pts.size();
    std::size_t edges = closed ? n : (n > 0 ? n - 1 : 0);
    for (std::size_t i = 0; i < edges; ++i) {
      std::size_t j = closed ? (i + 1) % n : i + 1;
      if (pts[i].AlmostEquals(pts[j])) {
        throw std::runtime_error(std::format("point {} and {} are too close with {} decimals precision",
                                             pts[i].ToWkt(), pts[j].ToWkt(), DECIMAL_PRECISION));
      }
    }
  }

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
    return LineSegment2D((*PTS)[i], (*PTS)[j]);
  }

 private:
  std::vector<Point2D> const* PTS;
  bool CLOSED;
};

}  // namespace geometry

}  // namespace geompp

namespace std::ranges {
template <>
inline constexpr bool enable_borrowed_range<geompp::SegmentRange2D> = true;
}
