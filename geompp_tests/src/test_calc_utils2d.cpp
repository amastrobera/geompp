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
// SweepLineSegment2D
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, SweepLineSegment_NormalizesLeftAndRight) {
  // First endpoint is to the right of the Last -> constructor swaps so Left <= Right.
  auto seg = g::LineSegment2D::Make(g::Point2D(5, 5), g::Point2D(1, 1));
  g::SweepLineSegment2D node(3, seg);

  EXPECT_EQ(node.EdgeId, 3u);
  EXPECT_DOUBLE_EQ(node.Left.x(), 1.0);
  EXPECT_DOUBLE_EQ(node.Left.y(), 1.0);
  EXPECT_DOUBLE_EQ(node.Right.x(), 5.0);
  EXPECT_DOUBLE_EQ(node.Right.y(), 5.0);
  EXPECT_EQ(node.Above, nullptr);
  EXPECT_EQ(node.Below, nullptr);
}

// --------------------------------------------------------------------------------------------------
// SweepLine2D
// --------------------------------------------------------------------------------------------------

using SweepLineVec = g::SweepLine2D<std::vector<g::LineSegment2D>>;

TEST_F(CalcUtils2DTest, SweepLine_AddReturnsNodeForSegment) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  auto const* node = sweep.Add(0);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->EdgeId, 0u);
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

TEST_F(CalcUtils2DTest, SweepLine_FindLocatesAddedSegment) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  EXPECT_EQ(sweep.Find(0), nullptr);  // not added yet

  sweep.Add(0);

  auto const* found = sweep.Find(0);
  ASSERT_NE(found, nullptr);
  EXPECT_EQ(found->EdgeId, 0u);
}

TEST_F(CalcUtils2DTest, SweepLine_FindThrowsOnOutOfRangeSegmentId) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  EXPECT_THROW(sweep.Find(9), std::out_of_range);
}

TEST_F(CalcUtils2DTest, SweepLine_AddLinksNeighbours) {
  // With the provisional comparator, segment 0 (lower) orders before segment 1 (higher),
  // so segment 1 ends up Above segment 0.
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0)),  // id 0, lower
      g::LineSegment2D::Make(g::Point2D(0, 5), g::Point2D(10, 5)),  // id 1, higher
  };
  SweepLineVec sweep(segments);

  auto const* n0 = sweep.Add(0);
  auto const* n1 = sweep.Add(1);
  ASSERT_NE(n0, nullptr);
  ASSERT_NE(n1, nullptr);

  // n0 is the lowest: nothing below it, n1 above it
  EXPECT_EQ(n0->Below, nullptr);
  ASSERT_NE(n0->Above, nullptr);
  EXPECT_EQ(n0->Above->EdgeId, 1u);

  // n1 is the highest: n0 below it, nothing above
  EXPECT_EQ(n1->Above, nullptr);
  ASSERT_NE(n1->Below, nullptr);
  EXPECT_EQ(n1->Below->EdgeId, 0u);
}

TEST_F(CalcUtils2DTest, SweepLine_RemoveErasesAndNullsHandle) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  sweep.Add(0);
  g::SweepLineSegment2D const* s = sweep.Find(0);
  ASSERT_NE(s, nullptr);

  sweep.Remove(s);

  EXPECT_EQ(s, nullptr);                // caller's handle nulled
  EXPECT_EQ(sweep.Find(0), nullptr);    // gone from the structure
}

TEST_F(CalcUtils2DTest, SweepLine_RemoveNullHandleThrows) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D const* s = nullptr;
  EXPECT_THROW(sweep.Remove(s), std::invalid_argument);
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
  g::SweepLineSegment2D const* mid = sweep.Add(1);
  sweep.Add(2);
  ASSERT_NE(mid, nullptr);

  sweep.Remove(mid);  // removing the middle should make 0 and 2 neighbours

  auto const* low = sweep.Find(0);
  auto const* high = sweep.Find(2);
  ASSERT_NE(low, nullptr);
  ASSERT_NE(high, nullptr);

  ASSERT_NE(low->Above, nullptr);
  EXPECT_EQ(low->Above->EdgeId, 2u);
  ASSERT_NE(high->Below, nullptr);
  EXPECT_EQ(high->Below->EdgeId, 0u);
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectionReturnsCrossingPoint) {
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 10)),  // id 0: diagonal
      g::LineSegment2D::Make(g::Point2D(0, 10), g::Point2D(10, 0)),  // id 1: anti-diagonal, crosses 0 at (5,5)
  };
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D a(0, segments[0]);
  g::SweepLineSegment2D b(1, segments[1]);

  auto pt = sweep.Intersection(&a, &b);
  ASSERT_TRUE(pt.has_value());
  EXPECT_DOUBLE_EQ(pt->x(), 5.0);
  EXPECT_DOUBLE_EQ(pt->y(), 5.0);
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectionParallelReturnsNullopt) {
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0)),  // id 0
      g::LineSegment2D::Make(g::Point2D(0, 5), g::Point2D(10, 5)),  // id 1: parallel, never meets
  };
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D a(0, segments[0]);
  g::SweepLineSegment2D b(1, segments[1]);

  EXPECT_FALSE(sweep.Intersection(&a, &b).has_value());
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectionNullArgumentThrows) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D a(0, segments[0]);
  EXPECT_THROW(sweep.Intersection(nullptr, &a), std::invalid_argument);
  EXPECT_THROW(sweep.Intersection(&a, nullptr), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectionOutOfRangeEdgeIdThrows) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D good(0, segments[0]);
  g::SweepLineSegment2D bad(99, segments[0]);  // EdgeId past the end of the segments list
  EXPECT_THROW(sweep.Intersection(&bad, &good), std::out_of_range);
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectReturnsTrueWhenCrossing) {
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 10)),  // id 0
      g::LineSegment2D::Make(g::Point2D(0, 10), g::Point2D(10, 0)),  // id 1: crosses 0
  };
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D a(0, segments[0]);
  g::SweepLineSegment2D b(1, segments[1]);
  EXPECT_TRUE(sweep.Intersect(&a, &b));
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectReturnsFalseWhenParallel) {
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0)),  // id 0
      g::LineSegment2D::Make(g::Point2D(0, 5), g::Point2D(10, 5)),  // id 1: parallel
  };
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D a(0, segments[0]);
  g::SweepLineSegment2D b(1, segments[1]);
  EXPECT_FALSE(sweep.Intersect(&a, &b));
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectNullArgumentThrows) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D a(0, segments[0]);
  EXPECT_THROW(sweep.Intersect(nullptr, &a), std::invalid_argument);
  EXPECT_THROW(sweep.Intersect(&a, nullptr), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, SweepLine_IntersectOutOfRangeEdgeIdThrows) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  g::SweepLineSegment2D good(0, segments[0]);
  g::SweepLineSegment2D bad(99, segments[0]);
  EXPECT_THROW(sweep.Intersect(&bad, &good), std::out_of_range);
}

TEST_F(CalcUtils2DTest, SweepLine_WorksWithSegmentRange) {
  // exercises the SweepLine2D<SegmentRange2D> explicit instantiation
  std::vector<g::Point2D> pts{g::Point2D(0, 0), g::Point2D(10, 0), g::Point2D(20, 0)};
  g::SegmentRange2D range(pts);
  g::SweepLine2D<g::SegmentRange2D> sweep(range);

  auto const* node = sweep.Add(0);
  ASSERT_NE(node, nullptr);
  EXPECT_EQ(node->EdgeId, 0u);
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
