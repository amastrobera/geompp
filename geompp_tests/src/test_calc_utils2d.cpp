#include "calc_utils2d.hpp"

#include "constants.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "segment_iterator2d.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <vector>

namespace g = geompp;

namespace geompp_tests {

class CalcUtils2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

// --------------------------------------------------------------------------------------------------
// Event2D
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, Event2D_OrdersByXThenYThenType) {
  g::Event2D a{g::EventType2D::LEFT, g::Point2D(0, 0), 0, std::nullopt};
  g::Event2D b{g::EventType2D::LEFT, g::Point2D(1, 0), 0, std::nullopt};
  g::Event2D c{g::EventType2D::LEFT, g::Point2D(0, 1), 0, std::nullopt};

  // smaller x is less
  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);

  // same x, smaller y is less
  EXPECT_TRUE(a < c);
  EXPECT_FALSE(c < a);
}

TEST_F(CalcUtils2DTest, Event2D_SamePointOrdersByTypeEnum) {
  // EventType2D enum order is LEFT(0) < RIGHT(1) < INTERSECTION(2)
  g::Event2D left{g::EventType2D::LEFT, g::Point2D(2, 2), 0, std::nullopt};
  g::Event2D right{g::EventType2D::RIGHT, g::Point2D(2, 2), 0, std::nullopt};

  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

// --------------------------------------------------------------------------------------------------
// EventQueue2D
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, EventQueue_EmptyWhenNoSegments) {
  std::vector<g::LineSegment2D> segments;
  g::EventQueue2D queue(segments);

  EXPECT_TRUE(queue.Empty());
  EXPECT_FALSE(queue.Next().has_value());
}

TEST_F(CalcUtils2DTest, EventQueue_BuildsTwoEventsPerSegment) {
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0)),
      g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(3, 1)),
  };
  g::EventQueue2D queue(segments);

  std::size_t count = 0;
  while (!queue.Empty()) {
    EXPECT_TRUE(queue.Next().has_value());
    ++count;
  }
  EXPECT_EQ(count, 4u);
  EXPECT_TRUE(queue.Empty());
}

TEST_F(CalcUtils2DTest, EventQueue_NextReturnsInPriorityOrder) {
  // std::priority_queue is a max-heap, so Next() returns events from greatest to smallest (by Event2D::operator<).
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0)),
      g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(3, 1)),
  };
  g::EventQueue2D queue(segments);

  std::vector<g::Event2D> popped;
  while (auto ev = queue.Next()) {
    popped.push_back(*ev);
  }

  ASSERT_EQ(popped.size(), 4u);

  // greatest first: (3,1) then (2,0) then (1,1) then (0,0)
  EXPECT_DOUBLE_EQ(popped.front().Point.x(), 3.0);
  EXPECT_DOUBLE_EQ(popped.back().Point.x(), 0.0);

  // monotonically non-increasing
  for (std::size_t i = 1; i < popped.size(); ++i) {
    EXPECT_FALSE(popped[i - 1] < popped[i]) << "events not in non-increasing order at index " << i;
  }
}

TEST_F(CalcUtils2DTest, EventQueue_AssignsLeftAndRightTypes) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0))};
  g::EventQueue2D queue(segments);

  std::optional<g::EventType2D> type_at_origin;
  std::optional<g::EventType2D> type_at_far;
  while (auto ev = queue.Next()) {
    if (ev->Point.x() == 0.0) {
      type_at_origin = ev->Type;
    } else if (ev->Point.x() == 2.0) {
      type_at_far = ev->Type;
    }
  }

  ASSERT_TRUE(type_at_origin.has_value());
  ASSERT_TRUE(type_at_far.has_value());
  EXPECT_EQ(*type_at_origin, g::EventType2D::LEFT);   // smaller endpoint
  EXPECT_EQ(*type_at_far, g::EventType2D::RIGHT);     // larger endpoint
}

TEST_F(CalcUtils2DTest, EventQueue_FromSegmentRange) {
  std::vector<g::Point2D> pts{g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(2, 0)};
  g::SegmentRange2D range(pts);  // open polyline -> 2 segments
  g::EventQueue2D queue(range);

  std::size_t count = 0;
  while (auto ev = queue.Next()) {
    ++count;
  }
  EXPECT_EQ(count, 4u);  // 2 segments * 2 events
}

TEST_F(CalcUtils2DTest, EventQueue_FromPolygon) {
  g::Polygon2D triangle = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(2, 3)});
  g::EventQueue2D queue(triangle);

  std::size_t count = 0;
  while (auto ev = queue.Next()) {
    ++count;
  }
  EXPECT_EQ(count, 6u);  // closed triangle -> 3 segments * 2 events
}

// --------------------------------------------------------------------------------------------------
// SweepLine2D
// --------------------------------------------------------------------------------------------------

using SweepLineVec = g::SweepLine2D<std::vector<g::LineSegment2D>>;

TEST_F(CalcUtils2DTest, SweepLine_AddReturnsElementForSegment) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  auto elem = sweep.Add(0);
  ASSERT_TRUE(elem.Segment.has_value());
  EXPECT_EQ(elem.Segment->Id, 0u);
  EXPECT_NE(elem.Segment->Seg, nullptr);
}

TEST_F(CalcUtils2DTest, SweepLine_AddThrowsOnOutOfRangeSegmentId) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  EXPECT_THROW(sweep.Add(5), std::out_of_range);
}

TEST_F(CalcUtils2DTest, SweepLine_AddThrowsOnDuplicate) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  sweep.Add(0);
  EXPECT_THROW(sweep.Add(0), std::logic_error);
}

TEST_F(CalcUtils2DTest, SweepLine_GetLocatesAddedSegment) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  EXPECT_FALSE(sweep.Get(0).Segment.has_value());  // not added yet

  sweep.Add(0);

  auto elem = sweep.Get(0);
  ASSERT_TRUE(elem.Segment.has_value());
  EXPECT_EQ(elem.Segment->Id, 0u);
}

TEST_F(CalcUtils2DTest, SweepLine_GetThrowsOnOutOfRangeSegmentId) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  EXPECT_THROW(sweep.Get(9), std::out_of_range);
}

TEST_F(CalcUtils2DTest, SweepLine_AddLinksNeighbours) {
  // With the provisional comparator, segment 0 (lower) orders before segment 1 (higher),
  // so segment 1 ends up Above segment 0.
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0)),  // id 0, lower
      g::LineSegment2D::Make(g::Point2D(0, 5), g::Point2D(10, 5)),  // id 1, higher
  };
  SweepLineVec sweep(segments);

  sweep.Add(0);
  sweep.Add(1);

  auto elem0 = sweep.Get(0);
  auto elem1 = sweep.Get(1);
  ASSERT_TRUE(elem0.Segment.has_value());
  ASSERT_TRUE(elem1.Segment.has_value());

  // elem0 is the lowest: nothing below it, elem1 above it
  EXPECT_FALSE(elem0.Below.has_value());
  ASSERT_TRUE(elem0.Above.has_value());
  EXPECT_EQ(elem0.Above->Id, 1u);

  // elem1 is the highest: elem0 below it, nothing above
  EXPECT_FALSE(elem1.Above.has_value());
  ASSERT_TRUE(elem1.Below.has_value());
  EXPECT_EQ(elem1.Below->Id, 0u);
}

TEST_F(CalcUtils2DTest, SweepLine_RemoveErasesSegment) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  sweep.Add(0);
  EXPECT_TRUE(sweep.Get(0).Segment.has_value());

  sweep.Remove(0);
  EXPECT_FALSE(sweep.Get(0).Segment.has_value());
}

TEST_F(CalcUtils2DTest, SweepLine_RemoveThrowsOnOutOfRangeSegmentId) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  EXPECT_THROW(sweep.Remove(99), std::out_of_range);
}

TEST_F(CalcUtils2DTest, SweepLine_RemoveStitchesNeighbours) {
  // three stacked segments: id 0 (low) < id 1 (middle) < id 2 (high) by the provisional comparator
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0)),
      g::LineSegment2D::Make(g::Point2D(0, 5), g::Point2D(10, 5)),
      g::LineSegment2D::Make(g::Point2D(0, 10), g::Point2D(10, 10)),
  };
  SweepLineVec sweep(segments);

  sweep.Add(0);
  sweep.Add(1);
  sweep.Add(2);

  sweep.Remove(1);  // removing the middle should make 0 and 2 neighbours

  auto low = sweep.Get(0);
  auto high = sweep.Get(2);
  ASSERT_TRUE(low.Segment.has_value());
  ASSERT_TRUE(high.Segment.has_value());

  ASSERT_TRUE(low.Above.has_value());
  EXPECT_EQ(low.Above->Id, 2u);
  ASSERT_TRUE(high.Below.has_value());
  EXPECT_EQ(high.Below->Id, 0u);
}

TEST_F(CalcUtils2DTest, SweepLine_RemoveReturnsNeighboursBeforeDeletion) {
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0)),
      g::LineSegment2D::Make(g::Point2D(0, 5), g::Point2D(10, 5)),
  };
  SweepLineVec sweep(segments);

  sweep.Add(0);
  sweep.Add(1);

  auto removed = sweep.Remove(1);  // Remove returns (Segment=nullopt, Above/Below = neighbors before deletion)
  EXPECT_FALSE(removed.Segment.has_value());
  ASSERT_TRUE(removed.Below.has_value());
  EXPECT_EQ(removed.Below->Id, 0u);
}

TEST_F(CalcUtils2DTest, SweepLine_WorksWithSegmentRange) {
  // exercises the SweepLine2D<SegmentRange2D> explicit instantiation
  std::vector<g::Point2D> pts{g::Point2D(0, 0), g::Point2D(10, 0), g::Point2D(20, 0)};
  g::SegmentRange2D range(pts);
  g::SweepLine2D<g::SegmentRange2D> sweep(range);

  auto elem = sweep.Add(0);
  ASSERT_TRUE(elem.Segment.has_value());
  EXPECT_EQ(elem.Segment->Id, 0u);
}

// --------------------------------------------------------------------------------------------------
// IntersectionEvent2D
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, IntersectionEvent2D_OrdersByPoint) {
  g::IntersectionEvent2D a{g::Point2D(0, 0), {0, 1}};
  g::IntersectionEvent2D b{g::Point2D(1, 0), {0, 1}};

  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);
}

TEST_F(CalcUtils2DTest, IntersectionEvent2D_EqualWhenSamePointAndIds) {
  g::IntersectionEvent2D a{g::Point2D(1, 1), {0, 1}};
  g::IntersectionEvent2D b{g::Point2D(1, 1), {0, 1}};

  EXPECT_TRUE(a == b);
}

TEST_F(CalcUtils2DTest, IntersectionEvent2D_NotEqualWhenDifferentPoint) {
  g::IntersectionEvent2D a{g::Point2D(1, 1), {0, 1}};
  g::IntersectionEvent2D b{g::Point2D(2, 2), {0, 1}};

  EXPECT_FALSE(a == b);
}

// --------------------------------------------------------------------------------------------------
// has_intersections (Shamos–Hoey) and find_intersections (Bentley–Ottmann)
//
// NOTE: these encode the INTENDED behaviour of correct Shamos–Hoey / Bentley–Ottmann implementations.
// Correctness currently rides on the provisional sweep-status comparator, so they are flagged for
// re-verification once the real ordering is in place.
// --------------------------------------------------------------------------------------------------

// closed ring of a unit square — no self-intersections
static std::vector<g::LineSegment2D> SquareRing() {
  return {
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0)),
      g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(1, 1)),
      g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1)),
      g::LineSegment2D::Make(g::Point2D(0, 1), g::Point2D(0, 0)),
  };
}

// closed ring whose edges (4,0)->(1,3) and (3,3)->(0,0) cross at (2,2)
static std::vector<g::LineSegment2D> SelfIntersectingRing() {
  return {
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(4, 0)),
      g::LineSegment2D::Make(g::Point2D(4, 0), g::Point2D(1, 3)),
      g::LineSegment2D::Make(g::Point2D(1, 3), g::Point2D(3, 3)),
      g::LineSegment2D::Make(g::Point2D(3, 3), g::Point2D(0, 0)),
  };
}

TEST_F(CalcUtils2DTest, HasIntersections_SimpleRingIsFalse) {
  EXPECT_FALSE(g::has_intersections(SquareRing()));
}

TEST_F(CalcUtils2DTest, HasIntersections_SelfIntersectingRingIsTrue) {
  EXPECT_TRUE(g::has_intersections(SelfIntersectingRing()));
}

TEST_F(CalcUtils2DTest, HasIntersections_TooFewSegmentsThrows) {
  std::vector<g::LineSegment2D> two{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0)),
      g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(0, 0)),
  };
  EXPECT_THROW(g::has_intersections(two), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, FindIntersections_SimpleRingIsEmpty) {
  auto hits = g::find_intersections(SquareRing());
  EXPECT_TRUE(hits.empty());
}

TEST_F(CalcUtils2DTest, FindIntersections_SelfIntersectingRingReportsCrossing) {
  auto hits = g::find_intersections(SelfIntersectingRing());
  ASSERT_FALSE(hits.empty());

  // the (2,2) crossing should be among the reported intersection points
  bool found_22 = false;
  for (auto const& h : hits) {
    if (h.Point.x() == 2.0 && h.Point.y() == 2.0) {
      found_22 = true;
    }
  }
  EXPECT_TRUE(found_22) << "expected the (2,2) crossing in the reported intersections";
}

}  // namespace geompp_tests
