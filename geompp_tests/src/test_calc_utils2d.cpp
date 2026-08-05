#include "calc_utils2d.hpp"

#include "constants.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "segment_iterator2d.hpp"
#include "triangle2d.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <optional>
#include <random>
#include <set>
#include <vector>

namespace g  = geompp;
namespace gd = geompp::detail;

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
  gd::Event2D a{gd::EventType2D::LEFT, g::Point2D(0, 0), 0, std::nullopt};
  gd::Event2D b{gd::EventType2D::LEFT, g::Point2D(1, 0), 0, std::nullopt};
  gd::Event2D c{gd::EventType2D::LEFT, g::Point2D(0, 1), 0, std::nullopt};

  // smaller x is less
  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);

  // same x, smaller y is less
  EXPECT_TRUE(a < c);
  EXPECT_FALSE(c < a);
}

TEST_F(CalcUtils2DTest, Event2D_SamePointOrdersByTypeEnum) {
  // EventType2D enum order is LEFT(0) < INTERSECTION(1) < RIGHT(2)
  gd::Event2D left{gd::EventType2D::LEFT, g::Point2D(2, 2), 0, std::nullopt};
  gd::Event2D intersection{gd::EventType2D::INTERSECTION, g::Point2D(2, 2), 0, std::nullopt};
  gd::Event2D right{gd::EventType2D::RIGHT, g::Point2D(2, 2), 0, std::nullopt};

  EXPECT_TRUE(left < intersection);
  EXPECT_TRUE(intersection < right);
  EXPECT_TRUE(left < right);
  EXPECT_FALSE(right < left);
}

TEST_F(CalcUtils2DTest, Event2D_EqualWhenAllFieldsMatch) {
  gd::Event2D a{gd::EventType2D::LEFT, g::Point2D(1, 2), 3, std::nullopt};
  gd::Event2D b{gd::EventType2D::LEFT, g::Point2D(1, 2), 3, std::nullopt};
  gd::Event2D c{gd::EventType2D::RIGHT, g::Point2D(1, 2), 3, std::nullopt};

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
}

// --------------------------------------------------------------------------------------------------
// EventQueue2D
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, EventQueue_EmptyWhenNoSegments) {
  std::vector<g::LineSegment2D> segments;
  gd::EventQueue2D queue(segments);

  EXPECT_TRUE(queue.Empty());
  EXPECT_FALSE(queue.Top().has_value());  // Top() peeks; returns nullopt on empty queue
}

TEST_F(CalcUtils2DTest, EventQueue_BuildsTwoEventsPerSegment) {
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0)),
      g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(3, 1)),
  };
  gd::EventQueue2D queue(segments);

  std::size_t count = 0;
  while (!queue.Empty()) {
    EXPECT_TRUE(queue.Pop().has_value());
    ++count;
  }
  EXPECT_EQ(count, 4u);
  EXPECT_TRUE(queue.Empty());
}

TEST_F(CalcUtils2DTest, EventQueue_PopReturnsInPriorityOrder) {
  // EventMinHeap is a min-heap, so Pop() returns events from smallest to greatest (by Event2D::operator<).
  std::vector<g::LineSegment2D> segments{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0)),
      g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(3, 1)),
  };
  gd::EventQueue2D queue(segments);

  std::vector<gd::Event2D> popped;
  while (auto ev = queue.Pop()) {
    popped.push_back(*ev);
  }

  ASSERT_EQ(popped.size(), 4u);

  // smallest first: (0,0) then (1,1) then (2,0) then (3,1)
  EXPECT_DOUBLE_EQ(popped.front().Point.x(), 0.0);
  EXPECT_DOUBLE_EQ(popped.back().Point.x(), 3.0);

  // monotonically non-decreasing
  for (std::size_t i = 1; i < popped.size(); ++i) {
    EXPECT_FALSE(popped[i] < popped[i - 1]) << "events not in non-decreasing order at index " << i;
  }
}

TEST_F(CalcUtils2DTest, EventQueue_TopPeeksWithoutConsuming) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0))};
  gd::EventQueue2D queue(segments);

  auto first = queue.Top();
  auto second = queue.Top();
  ASSERT_TRUE(first.has_value());
  ASSERT_TRUE(second.has_value());
  // Same event returned both times — Top() does not consume
  EXPECT_EQ(first->SegmentId, second->SegmentId);
  EXPECT_EQ(first->Type, second->Type);
  EXPECT_FALSE(queue.Empty());

  // Pop() then returns the same event Top() was showing
  auto popped = queue.Pop();
  ASSERT_TRUE(popped.has_value());
  EXPECT_EQ(popped->SegmentId, first->SegmentId);
  EXPECT_EQ(popped->Type, first->Type);
}

TEST_F(CalcUtils2DTest, EventQueue_PushAddsEvent) {
  std::vector<g::LineSegment2D> empty;
  gd::EventQueue2D queue(empty);
  EXPECT_TRUE(queue.Empty());

  gd::Event2D ev{gd::EventType2D::LEFT, g::Point2D(5, 5), 0, std::nullopt};
  queue.Push(ev);

  EXPECT_FALSE(queue.Empty());
  auto top = queue.Top();
  ASSERT_TRUE(top.has_value());
  EXPECT_DOUBLE_EQ(top->Point.x(), 5.0);
}

TEST_F(CalcUtils2DTest, EventQueue_ContainsFindsEvent) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0))};
  gd::EventQueue2D queue(segments);

  // The ctor built a LEFT event at (0,0) for segment 0
  gd::Event2D left_at_origin{gd::EventType2D::LEFT, g::Point2D(0, 0), 0, std::nullopt};
  gd::Event2D absent{gd::EventType2D::INTERSECTION, g::Point2D(99, 99), 0, std::nullopt};

  EXPECT_TRUE(queue.Contains(left_at_origin));
  EXPECT_FALSE(queue.Contains(absent));
}

TEST_F(CalcUtils2DTest, EventQueue_AssignsLeftAndRightTypes) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0))};
  gd::EventQueue2D queue(segments);

  std::optional<gd::EventType2D> type_at_origin;
  std::optional<gd::EventType2D> type_at_far;
  while (auto ev = queue.Pop()) {
    if (ev->Point.x() == 0.0) {
      type_at_origin = ev->Type;
    } else if (ev->Point.x() == 2.0) {
      type_at_far = ev->Type;
    }
  }

  ASSERT_TRUE(type_at_origin.has_value());
  ASSERT_TRUE(type_at_far.has_value());
  EXPECT_EQ(*type_at_origin, gd::EventType2D::LEFT);   // smaller endpoint
  EXPECT_EQ(*type_at_far, gd::EventType2D::RIGHT);     // larger endpoint
}

TEST_F(CalcUtils2DTest, EventQueue_FromSegmentRange) {
  std::vector<g::Point2D> pts{g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(2, 0)};
  g::SegmentRange2D range(pts);  // open polyline -> 2 segments
  gd::EventQueue2D queue(range);

  std::size_t count = 0;
  while (queue.Pop()) {
    ++count;
  }
  EXPECT_EQ(count, 4u);  // 2 segments * 2 events
}

TEST_F(CalcUtils2DTest, EventQueue_FromPolygon) {
  g::Polygon2D triangle = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(2, 3)});
  gd::EventQueue2D queue(triangle);

  std::size_t count = 0;
  while (queue.Pop()) {
    ++count;
  }
  EXPECT_EQ(count, 6u);  // closed triangle -> 3 segments * 2 events
}

// --------------------------------------------------------------------------------------------------
// SweepLine2D
// --------------------------------------------------------------------------------------------------

using SweepLineVec = gd::SweepLine2D<std::vector<g::LineSegment2D>>;

TEST_F(CalcUtils2DTest, SweepLine_AddReturnsElementForSegment) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  auto elem = sweep.Add(0);
  ASSERT_TRUE(elem.Segment.has_value());
  EXPECT_EQ(elem.Segment->Id, 0u);
  // Seg is stored by value; verify it has the expected left endpoint
  EXPECT_DOUBLE_EQ(elem.Segment->Seg.First().x(), 0.0);
  EXPECT_DOUBLE_EQ(elem.Segment->Seg.First().y(), 0.0);
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
  // segment 0 (y=0, lower) orders before segment 1 (y=5, higher) in the sweep line
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
  // three stacked segments: id 0 (low) < id 1 (middle) < id 2 (high)
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

TEST_F(CalcUtils2DTest, SweepLine_SetXUpdatesGetX) {
  std::vector<g::LineSegment2D> segments{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(10, 0))};
  SweepLineVec sweep(segments);

  double initial = sweep.GetX();
  sweep.SetX(initial + 3.0);
  EXPECT_DOUBLE_EQ(sweep.GetX(), initial + 3.0);
}

TEST_F(CalcUtils2DTest, SweepLine_WorksWithSegmentRange) {
  // exercises the SweepLine2D<SegmentRange2D> explicit instantiation
  std::vector<g::Point2D> pts{g::Point2D(0, 0), g::Point2D(10, 0), g::Point2D(20, 0)};
  g::SegmentRange2D range(pts);
  gd::SweepLine2D<g::SegmentRange2D> sweep(range);

  auto elem = sweep.Add(0);
  ASSERT_TRUE(elem.Segment.has_value());
  EXPECT_EQ(elem.Segment->Id, 0u);
  EXPECT_DOUBLE_EQ(elem.Segment->Seg.First().x(), 0.0);
}

// --------------------------------------------------------------------------------------------------
// IntersectionEvent2D
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, IntersectionEvent2D_OrdersByPoint) {
  gd::IntersectionEvent2D a{g::Point2D(0, 0), {0, 1}};
  gd::IntersectionEvent2D b{g::Point2D(1, 0), {0, 1}};

  EXPECT_TRUE(a < b);
  EXPECT_FALSE(b < a);
}

TEST_F(CalcUtils2DTest, IntersectionEvent2D_EqualWhenSamePoint) {
  // equality is defined only on Point (not SegmentIds)
  gd::IntersectionEvent2D a{g::Point2D(1, 1), {0, 1}};
  gd::IntersectionEvent2D b{g::Point2D(1, 1), {2, 3}};

  EXPECT_TRUE(a == b);
}

TEST_F(CalcUtils2DTest, IntersectionEvent2D_NotEqualWhenDifferentPoint) {
  gd::IntersectionEvent2D a{g::Point2D(1, 1), {0, 1}};
  gd::IntersectionEvent2D b{g::Point2D(2, 2), {0, 1}};

  EXPECT_FALSE(a == b);
}

// --------------------------------------------------------------------------------------------------
// has_intersections (Shamos–Hoey)
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

// 5-edge star polygon (pentagram): each edge crosses exactly 2 others → 5 proper intersections
// Vertices are the outer tips of a regular pentagon at radius 4, connected in skip-2 order.
static std::vector<g::LineSegment2D> Pentagram() {
  const double R  = 4.0;
  const double PI = 3.14159265358979323846;
  auto tip = [&](int k) {
    double a = PI / 2.0 - k * 2.0 * PI / 5.0;
    return g::Point2D(R * std::cos(a), R * std::sin(a));
  };
  auto v0 = tip(0), v1 = tip(1), v2 = tip(2), v3 = tip(3), v4 = tip(4);
  // skip-2 winding: v0→v2→v4→v1→v3→v0
  return {
      g::LineSegment2D::Make(v0, v2),
      g::LineSegment2D::Make(v2, v4),
      g::LineSegment2D::Make(v4, v1),
      g::LineSegment2D::Make(v1, v3),
      g::LineSegment2D::Make(v3, v0),
  };
}

// 4 segments all crossing at a single point (3, 3) — exercises concurrent-intersection merging
static std::vector<g::LineSegment2D> ConcurrentStar() {
  return {
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(6, 6)),  // diagonal ↗
      g::LineSegment2D::Make(g::Point2D(0, 6), g::Point2D(6, 0)),  // diagonal ↘
      g::LineSegment2D::Make(g::Point2D(0, 3), g::Point2D(6, 3)),  // horizontal
      g::LineSegment2D::Make(g::Point2D(2, 1), g::Point2D(4, 5)),  // slope 2
  };
}

// closed ring whose edges (4,0)->(1,3) and (3,3)->(0,0) cross
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
  std::vector<g::LineSegment2D> one{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0))};
  EXPECT_THROW(g::has_intersections(one), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, HasIntersections_ParallelSegmentsIsFalse) {
  // two disjoint horizontal segments — no intersection
  std::vector<g::LineSegment2D> segs{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(4, 0)),
      g::LineSegment2D::Make(g::Point2D(0, 2), g::Point2D(4, 2)),
  };
  EXPECT_FALSE(g::has_intersections(segs));
}

TEST_F(CalcUtils2DTest, HasIntersections_CrossingSegmentsIsTrue) {
  // X-shaped cross: (0,0)→(2,2) and (0,2)→(2,0) meet at (1,1)
  std::vector<g::LineSegment2D> segs{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 2)),
      g::LineSegment2D::Make(g::Point2D(0, 2), g::Point2D(2, 0)),
  };
  EXPECT_TRUE(g::has_intersections(segs));
}

// --------------------------------------------------------------------------------------------------
// find_intersections (Bentley–Ottmann)
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, FindIntersections_TooFewSegmentsThrows) {
  std::vector<g::LineSegment2D> one{g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0))};
  EXPECT_THROW(g::find_intersections(one), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, FindIntersections_SimpleRingIsEmpty) {
  auto hits = g::find_intersections(SquareRing());
  EXPECT_TRUE(hits.empty());
}

TEST_F(CalcUtils2DTest, FindIntersections_SelfIntersectingRingReportsCrossing) {
  auto hits = g::find_intersections(SelfIntersectingRing());
  ASSERT_FALSE(hits.empty());

  bool found = false;
  for (auto const& p : hits) {
    if (p.AlmostEquals(g::Point2D(2.0, 2.0))) {
      found = true;
    }
  }
  EXPECT_TRUE(found) << "expected the (2,2) crossing among reported intersections";
}

TEST_F(CalcUtils2DTest, FindIntersections_CrossingSegmentsReportsPoint) {
  // X-shaped cross: (0,0)→(2,2) and (0,2)→(2,0) meet at (1,1)
  std::vector<g::LineSegment2D> segs{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 2)),
      g::LineSegment2D::Make(g::Point2D(0, 2), g::Point2D(2, 0)),
  };
  auto hits = g::find_intersections(segs);
  ASSERT_EQ(hits.size(), 1u);
  EXPECT_DOUBLE_EQ(hits[0].x(), 1.0);
  EXPECT_DOUBLE_EQ(hits[0].y(), 1.0);
}

TEST_F(CalcUtils2DTest, FindIntersections_ParallelSegmentsIsEmpty) {
  std::vector<g::LineSegment2D> segs{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(4, 0)),
      g::LineSegment2D::Make(g::Point2D(0, 2), g::Point2D(4, 2)),
  };
  EXPECT_TRUE(g::find_intersections(segs).empty());
}

// --------------------------------------------------------------------------------------------------
// Star polygon (pentagram) — every edge crosses exactly two others
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, HasIntersections_StarPolygon_IsTrue) {
  EXPECT_TRUE(g::has_intersections(Pentagram()));
}

TEST_F(CalcUtils2DTest, FindIntersections_StarPolygon_FiveDistinctIntersections) {
  auto hits = g::find_intersections(Pentagram());
  ASSERT_EQ(hits.size(), 5u);
}

// --------------------------------------------------------------------------------------------------
// Concurrent star — k segments all crossing the same point
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, HasIntersections_ConcurrentStar_IsTrue) {
  EXPECT_TRUE(g::has_intersections(ConcurrentStar()));
}

// All 4 segments pass through (3,3). The algorithm deduplicates concurrent events into one point.
TEST_F(CalcUtils2DTest, FindIntersections_ConcurrentStar_MergesIntoOneEvent) {
  auto hits = g::find_intersections(ConcurrentStar());
  ASSERT_EQ(hits.size(), 1u);
  EXPECT_DOUBLE_EQ(hits[0].x(), 3.0);
  EXPECT_DOUBLE_EQ(hits[0].y(), 3.0);
}

// --------------------------------------------------------------------------------------------------
// Three mutually intersecting segments — exact coordinates and output ordering
// --------------------------------------------------------------------------------------------------

// seg0 (0,0)→(6,2) ∩ seg2 (1,0)→(5,4) = (1.5, 0.5)
// seg1 (0,3)→(6,1) ∩ seg2 (1,0)→(5,4) = (3.0, 2.0)
// seg0 (0,0)→(6,2) ∩ seg1 (0,3)→(6,1) = (4.5, 1.5)
TEST_F(CalcUtils2DTest, FindIntersections_ThreeSegmentsThreeDistinctPoints) {
  std::vector<g::LineSegment2D> segs{
      g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(6, 2)),
      g::LineSegment2D::Make(g::Point2D(0, 3), g::Point2D(6, 1)),
      g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(5, 4)),
  };
  auto hits = g::find_intersections(segs);
  ASSERT_EQ(hits.size(), 3u);
  // output is sorted bottom-left → top-right (x then y)
  EXPECT_DOUBLE_EQ(hits[0].x(), 1.5);
  EXPECT_DOUBLE_EQ(hits[0].y(), 0.5);
  EXPECT_DOUBLE_EQ(hits[1].x(), 3.0);
  EXPECT_DOUBLE_EQ(hits[1].y(), 2.0);
  EXPECT_DOUBLE_EQ(hits[2].x(), 4.5);
  EXPECT_DOUBLE_EQ(hits[2].y(), 1.5);
}

// --------------------------------------------------------------------------------------------------
// SegmentRange2D exercised at algorithm level (not just SweepLine unit level)
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, HasIntersections_WorksWithSegmentRange_SimplePolygon) {
  std::vector<g::Point2D> pts{g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 2), g::Point2D(0, 2)};
  g::SegmentRange2D range(pts, /*closed=*/true);
  EXPECT_FALSE(gd::has_intersections(range));
}

TEST_F(CalcUtils2DTest, FindIntersections_WorksWithSegmentRange_SelfIntersecting) {
  // same shape as SelfIntersectingRing but constructed as a SegmentRange2D
  std::vector<g::Point2D> pts{g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(1, 3), g::Point2D(3, 3)};
  g::SegmentRange2D range(pts, /*closed=*/true);
  EXPECT_FALSE(gd::find_intersections(range).empty());
}

// ---- find_extreme_points (Polygon2D × Line2D) -------------------------------

TEST_F(CalcUtils2DTest, ExtremePoints_ConvexDiamond_AlongX) {
  // CCW diamond; projecting onto +X isolates the left/right tips uniquely.
  auto diamond = g::Polygon2D::Make({g::Point2D(2, 0), g::Point2D(4, 2), g::Point2D(2, 4), g::Point2D(0, 2)});
  ASSERT_TRUE(diamond.IsConvex());  // exercises the O(log n) Sunday binary search
  auto ex = g::find_extreme_points(diamond, g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0)));
  EXPECT_EQ(g::Point2D(0, 2), ex.min_point);
  EXPECT_EQ(g::Point2D(4, 2), ex.max_point);
}

TEST_F(CalcUtils2DTest, ExtremePoints_ConvexDiamond_AlongY) {
  auto diamond = g::Polygon2D::Make({g::Point2D(2, 0), g::Point2D(4, 2), g::Point2D(2, 4), g::Point2D(0, 2)});
  auto ex = g::find_extreme_points(diamond, g::Line2D::Make(g::Point2D(0, 0), g::Point2D(0, 1)));
  EXPECT_EQ(g::Point2D(2, 0), ex.min_point);
  EXPECT_EQ(g::Point2D(2, 4), ex.max_point);
}

TEST_F(CalcUtils2DTest, ExtremePoints_ConvexSquare_Diagonal) {
  // Diagonal direction (1,1): projection = x + y, extreme at opposite corners.
  auto square = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  auto ex = g::find_extreme_points(square, g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 1)));
  EXPECT_EQ(g::Point2D(0, 0), ex.min_point);
  EXPECT_EQ(g::Point2D(4, 4), ex.max_point);
}

TEST_F(CalcUtils2DTest, ExtremePoints_ConvexHexagon_ObliqueDir) {
  auto hex = g::Polygon2D::Make({g::Point2D(2, 0), g::Point2D(4, 1), g::Point2D(4, 3), g::Point2D(2, 4),
                                 g::Point2D(0, 3), g::Point2D(0, 1)});
  ASSERT_TRUE(hex.IsConvex());
  // direction (3,1): proj = 3x + y → min at (0,1)=1, max at (4,3)=15
  auto ex = g::find_extreme_points(hex, g::Line2D::Make(g::Point2D(0, 0), g::Point2D(3, 1)));
  EXPECT_EQ(g::Point2D(0, 1), ex.min_point);
  EXPECT_EQ(g::Point2D(4, 3), ex.max_point);
}

TEST_F(CalcUtils2DTest, ExtremePoints_ConcavePolygon_BruteForcePath) {
  // Non-convex "dart": the inner dip at (2,1) makes it concave → O(n) linear scan.
  auto dart = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(2, 1),
                                  g::Point2D(0, 4)});
  ASSERT_FALSE(dart.IsConvex());
  // direction (1,2): proj = x + 2y → min at (0,0)=0, max at (4,4)=12
  auto ex = g::find_extreme_points(dart, g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 2)));
  EXPECT_EQ(g::Point2D(0, 0), ex.min_point);
  EXPECT_EQ(g::Point2D(4, 4), ex.max_point);
}

TEST_F(CalcUtils2DTest, ExtremePoints_PolygonWithHole_IgnoresHole) {
  // Holes make the polygon non-convex; only the outer ring participates in the search.
  std::vector<g::Point2D> outer = {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto poly = g::Polygon2D::Make(outer, {hole});
  auto ex = g::find_extreme_points(poly, g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 2)));
  EXPECT_EQ(g::Point2D(0, 0), ex.min_point);
  EXPECT_EQ(g::Point2D(4, 4), ex.max_point);
}

TEST_F(CalcUtils2DTest, ExtremePoints_ConvexAndBruteForceAgree) {
  // Same convex geometry evaluated via both code paths must yield identical extremes.
  // Direction (3,1) is not aligned with this hexagon's symmetry axis, so projections are
  // all distinct (no ties) and both algorithms must agree on a unique index.
  std::vector<g::Point2D> verts = {g::Point2D(2, 0), g::Point2D(4, 1), g::Point2D(4, 3), g::Point2D(2, 4),
                                   g::Point2D(0, 3), g::Point2D(0, 1)};
  auto dir = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(3, 1)).Direction();
  auto convex = gd::extreme_points(verts, /*is_convex=*/true, dir);
  auto brute = gd::extreme_points(verts, /*is_convex=*/false, dir);
  EXPECT_EQ(brute.first, convex.first);
  EXPECT_EQ(brute.second, convex.second);
}

// ---- distance_to (Polygon2D × Line2D) ---------------------------------------

TEST_F(CalcUtils2DTest, DistanceTo_ConvexSquare_LineCrossing_IsZero) {
  auto square = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  ASSERT_TRUE(square.IsConvex());
  auto line = g::Line2D::Make(g::Point2D(2, -1), g::Point2D(2, 5));
  EXPECT_NEAR(0.0, g::distance_to(square, line), 1e-9);
}

TEST_F(CalcUtils2DTest, DistanceTo_ConvexSquare_LineOutside) {
  auto square = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  auto line = g::Line2D::Make(g::Point2D(6, -1), g::Point2D(6, 5));
  EXPECT_NEAR(2.0, g::distance_to(square, line), 1e-9);
}

TEST_F(CalcUtils2DTest, DistanceTo_NonConvexDart_LineCrossing_IsZero) {
  // Same dart as ExtremePoints_ConcavePolygon_BruteForcePath: a vertical line through its middle
  // crosses the bottom edge, so the non-convex brute-force branch must report zero.
  auto dart = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(2, 1),
                                  g::Point2D(0, 4)});
  ASSERT_FALSE(dart.IsConvex());
  auto line = g::Line2D::Make(g::Point2D(2, -1), g::Point2D(2, 5));
  EXPECT_NEAR(0.0, g::distance_to(dart, line), 1e-9);
}

TEST_F(CalcUtils2DTest, DistanceTo_NonConvexDart_LineOutside) {
  auto dart = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(2, 1),
                                  g::Point2D(0, 4)});
  auto line = g::Line2D::Make(g::Point2D(10, -1), g::Point2D(10, 5));
  EXPECT_NEAR(6.0, g::distance_to(dart, line), 1e-9);
}

TEST_F(CalcUtils2DTest, DistanceTo_PolygonWithHole_IgnoresHole) {
  // Holes make the polygon non-convex; only the outer ring participates — same answer as the
  // hole-less square in DistanceTo_ConvexSquare_LineOutside.
  std::vector<g::Point2D> outer = {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto poly = g::Polygon2D::Make(outer, {hole});
  auto line = g::Line2D::Make(g::Point2D(6, -1), g::Point2D(6, 5));
  EXPECT_NEAR(2.0, g::distance_to(poly, line), 1e-9);
}

// ---- tangents_to (Polygon2D x Point2D / Polygon2D) --------------------------

TEST_F(CalcUtils2DTest, TangentsTo_ConvexSquare_Point) {
  auto square = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  ASSERT_TRUE(square.IsConvex());  // exercises the O(log n) binary search
  auto t = g::tangents_to(square, g::Point2D(10, -2));
  EXPECT_EQ(g::Point2D(10, -2), t.left.First());
  EXPECT_EQ(g::Point2D(0, 0), t.left.Last());
  EXPECT_EQ(g::Point2D(10, -2), t.right.First());
  EXPECT_EQ(g::Point2D(4, 4), t.right.Last());
}

TEST_F(CalcUtils2DTest, TangentsTo_ConvexHexagon_Point_ExercisesFullBinarySearch) {
  // A larger convex loop and a distant, off-axis point force the binary search to actually
  // iterate (not just hit the "vertex 0" shortcut). Verified independently via the supporting-line
  // cross-product test (both candidates keep every other vertex on one consistent side of p->v).
  auto hex = g::Polygon2D::Make({g::Point2D(2, 0), g::Point2D(4, 1), g::Point2D(4, 3), g::Point2D(2, 4),
                                 g::Point2D(0, 3), g::Point2D(0, 1)});
  ASSERT_TRUE(hex.IsConvex());
  auto t = g::tangents_to(hex, g::Point2D(20, 7));
  EXPECT_EQ(g::Point2D(2, 0), t.left.Last());
  EXPECT_EQ(g::Point2D(2, 4), t.right.Last());
}

TEST_F(CalcUtils2DTest, TangentsTo_NonConvexDart_Point_ReducesToConvexHull) {
  // The dart's concave notch (2,1) is interior to its convex hull, so it must never be
  // returned as a tangent point. Viewed from directly left, the tangent points are exactly the
  // endpoints of the flat left edge (0,0)-(0,4) — same answer as the hull-equivalent square would
  // give, which also exercises the hull-index -> original-index mapping (dart has 5 vertices, its
  // hull only 4, so a broken mapping would very likely land on the wrong vertex).
  auto dart = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(2, 1),
                                  g::Point2D(0, 4)});
  ASSERT_FALSE(dart.IsConvex());
  auto t = g::tangents_to(dart, g::Point2D(-6, 2));
  EXPECT_EQ(g::Point2D(0, 4), t.left.Last());
  EXPECT_EQ(g::Point2D(0, 0), t.right.Last());
}

TEST_F(CalcUtils2DTest, TangentsTo_ConvexSquares_Polygon) {
  // Two congruent squares, one translated by (10,1) relative to the other (no rotation/scaling):
  // the outer common tangents connect corresponding corners and are parallel to the translation
  // vector — verified independently via the supporting-line cross-product test.
  auto squareA = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  auto squareB = g::Polygon2D::Make({g::Point2D(10, 1), g::Point2D(14, 1), g::Point2D(14, 5), g::Point2D(10, 5)});
  auto t = g::tangents_to(squareA, squareB);
  EXPECT_EQ(g::Point2D(0, 4), t.left.First());
  EXPECT_EQ(g::Point2D(10, 5), t.left.Last());
  EXPECT_EQ(g::Point2D(4, 0), t.right.First());
  EXPECT_EQ(g::Point2D(14, 1), t.right.Last());
}

TEST_F(CalcUtils2DTest, TangentsTo_NonConvexDarts_Polygon_ReducesBothToConvexHull) {
  // Same layout as TangentsTo_ConvexSquares_Polygon, but both shapes are darts with an interior
  // notch. Neither notch is on its hull, so the result must match the square/square case exactly.
  auto dartA = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(2, 1),
                                   g::Point2D(0, 4)});
  auto dartB = g::Polygon2D::Make({g::Point2D(10, 1), g::Point2D(14, 1), g::Point2D(14, 5), g::Point2D(12, 2),
                                   g::Point2D(10, 5)});
  ASSERT_FALSE(dartA.IsConvex());
  ASSERT_FALSE(dartB.IsConvex());
  auto t = g::tangents_to(dartA, dartB);
  EXPECT_EQ(g::Point2D(0, 4), t.left.First());
  EXPECT_EQ(g::Point2D(10, 5), t.left.Last());
  EXPECT_EQ(g::Point2D(4, 0), t.right.First());
  EXPECT_EQ(g::Point2D(14, 1), t.right.Last());
}

// ---- dist_decimation / rdp_decimation / vw_decimation -----------------------

TEST_F(CalcUtils2DTest, DistDecimation_TooFewPoints_ReturnsUnchanged) {
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(1, 1)};
  auto result = g::dist_decimation(points, 5.0);
  ASSERT_EQ(2u, result.size());
  EXPECT_EQ(points[0], result[0]);
  EXPECT_EQ(points[1], result[1]);
}

TEST_F(CalcUtils2DTest, DistDecimation_ClusteredPoints_RemovesWithinThreshold) {
  // two tight clusters near (0,0) and (5,0), then a far endpoint: each cluster should
  // collapse to its first member, measured radially from the last *kept* point.
  std::vector<g::Point2D> points{g::Point2D(0, 0),   g::Point2D(0.1, 0), g::Point2D(0.2, 0),
                                 g::Point2D(5, 0),   g::Point2D(5.1, 0), g::Point2D(10, 0)};
  auto result = g::dist_decimation(points, 1.0);
  std::vector<g::Point2D> expected{g::Point2D(0, 0), g::Point2D(5, 0), g::Point2D(10, 0)};
  ASSERT_EQ(expected.size(), result.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(expected[i])) << "index " << i;
  }
}

TEST_F(CalcUtils2DTest, DistDecimation_ThresholdZero_DropsOnlySubEpsilonPoints) {
  // compare() is epsilon-aware even at threshold 0: a point within DOUBLE_EPSILON of the last
  // kept point is still dropped, while a genuinely distinct one is kept.
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(0.0001, 0), g::Point2D(5, 0)};
  auto result = g::dist_decimation(points, 0.0);
  std::vector<g::Point2D> expected{g::Point2D(0, 0), g::Point2D(5, 0)};
  ASSERT_EQ(expected.size(), result.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(expected[i])) << "index " << i;
  }
}

TEST_F(CalcUtils2DTest, RdpDecimation_TooFewPoints_ReturnsUnchanged) {
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(1, 1)};
  auto result = g::rdp_decimation(points, 5.0);
  ASSERT_EQ(2u, result.size());
  EXPECT_EQ(points[0], result[0]);
  EXPECT_EQ(points[1], result[1]);
}

TEST_F(CalcUtils2DTest, RdpDecimation_CollinearPoints_CollapsesToEndpoints) {
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(3, 0),
                                 g::Point2D(4, 0)};
  auto result = g::rdp_decimation(points, 0.5);
  std::vector<g::Point2D> expected{g::Point2D(0, 0), g::Point2D(4, 0)};
  ASSERT_EQ(expected.size(), result.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(expected[i])) << "index " << i;
  }
}

TEST_F(CalcUtils2DTest, RdpDecimation_SingleSpike_KeepsPeakDiscardsShoulders) {
  // a triangular spike on an otherwise straight path: (2,0) and (6,0) sit ~1.56 units off their
  // local chord (below threshold=2), while the peak (4,5) sits 5 units off the outer chord (above
  // it) — so only the peak should survive.
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(4, 5), g::Point2D(6, 0),
                                 g::Point2D(8, 0)};
  auto result = g::rdp_decimation(points, 2.0);
  std::vector<g::Point2D> expected{g::Point2D(0, 0), g::Point2D(4, 5), g::Point2D(8, 0)};
  ASSERT_EQ(expected.size(), result.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(expected[i])) << "index " << i;
  }
}

TEST_F(CalcUtils2DTest, VwDecimation_TooFewPoints_ReturnsUnchanged) {
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(1, 1)};
  auto result = g::vw_decimation(points, 5.0);
  ASSERT_EQ(2u, result.size());
  EXPECT_EQ(points[0], result[0]);
  EXPECT_EQ(points[1], result[1]);
}

TEST_F(CalcUtils2DTest, VwDecimation_CollinearPoints_RemovesZeroAreaVertices) {
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(3, 0),
                                 g::Point2D(4, 0)};
  auto result = g::vw_decimation(points, 0.5);
  std::vector<g::Point2D> expected{g::Point2D(0, 0), g::Point2D(4, 0)};
  ASSERT_EQ(expected.size(), result.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(expected[i])) << "index " << i;
  }
}

TEST_F(CalcUtils2DTest, VwDecimation_SingleSpike_KeepsHighAreaVertex) {
  // triangle areas: (2,0)=5, (4,5)=10, (6,0)=5 — with threshold=6 the two area-5 vertices are
  // removed (and their neighbor's area recomputed against the wider base), the area-10 peak stays.
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(4, 5), g::Point2D(6, 0),
                                 g::Point2D(8, 0)};
  auto result = g::vw_decimation(points, 6.0);
  std::vector<g::Point2D> expected{g::Point2D(0, 0), g::Point2D(4, 5), g::Point2D(8, 0)};
  ASSERT_EQ(expected.size(), result.size());
  for (std::size_t i = 0; i < expected.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(expected[i])) << "index " << i;
  }
}

TEST_F(CalcUtils2DTest, VwDecimation_ThresholdBelowAllAreas_KeepsAllPoints) {
  std::vector<g::Point2D> points{g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(4, 5), g::Point2D(6, 0),
                                 g::Point2D(8, 0)};
  auto result = g::vw_decimation(points, 1.0);
  ASSERT_EQ(points.size(), result.size());
  for (std::size_t i = 0; i < points.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(points[i])) << "index " << i;
  }
}

// ---- bezier_smoothing_2 (min_distance overload) ------------------------------

TEST_F(CalcUtils2DTest, BezierSmoothing2_MinDistanceNotPositive_Throws) {
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  EXPECT_THROW(g::bezier_smoothing_2(p0, p1, p2, 0.5, 0.0), std::invalid_argument);
  EXPECT_THROW(g::bezier_smoothing_2(p0, p1, p2, 0.5, -1.0), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_EndpointsAreTrimmedTangents) {
  // p0=(0,0), p1=(2,0), p2=(2,2): len1=len2=2, smoothness=1.0 -> k=0.5 -> max_trim=1
  // T0 = p1 + normalize(p0-p1)*1 = (1,0);  T1 = p1 + normalize(p2-p1)*1 = (2,1)
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  auto result = g::bezier_smoothing_2(p0, p1, p2, 1.0, 1.0);
  ASSERT_GE(result.size(), 2u);
  EXPECT_TRUE(result.front().AlmostEquals(g::Point2D(1, 0)));
  EXPECT_TRUE(result.back().AlmostEquals(g::Point2D(2, 1)));
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_SmallerMinDistanceYieldsMorePoints) {
  // same corner as above: approx_length ~= 1.707, so min_distance=1.0 -> 2 points,
  // min_distance=0.5 -> 4 points.
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  auto coarse = g::bezier_smoothing_2(p0, p1, p2, 1.0, 1.0);
  auto fine   = g::bezier_smoothing_2(p0, p1, p2, 1.0, 0.5);
  EXPECT_EQ(2u, coarse.size());
  EXPECT_EQ(4u, fine.size());
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_SmoothnessZero_CollapsesToCorner) {
  // k=0 -> max_trim=0 -> T0=T1=p1, so every sample equals the un-trimmed corner point.
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  auto result = g::bezier_smoothing_2(p0, p1, p2, 0.0, 0.5);
  for (auto const& p : result) {
    EXPECT_TRUE(p.AlmostEquals(p1));
  }
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_CoincidentP0P1_NoNaNCollapsesToCorner) {
  // p0 == p1: len1 == 0, so max_trim == 0 regardless of len2 -> both T0 and T1 degenerate to p1.
  // Before the fix, Normalize() on the zero-length (p0-p1) produced NaN * 0 == NaN.
  g::Point2D p1(1, 1), p2(3, 1);
  auto result = g::bezier_smoothing_2(p1, p1, p2, 0.5, 0.1);
  ASSERT_GE(result.size(), 1u);
  for (auto const& p : result) {
    EXPECT_FALSE(std::isnan(p.x()));
    EXPECT_FALSE(std::isnan(p.y()));
    EXPECT_TRUE(p.AlmostEquals(p1));
  }
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_CoincidentP2P1_NoNaNCollapsesToCorner) {
  // symmetric case: p2 == p1.
  g::Point2D p0(3, 1), p1(1, 1);
  auto result = g::bezier_smoothing_2(p0, p1, p1, 0.5, 0.1);
  for (auto const& p : result) {
    EXPECT_FALSE(std::isnan(p.x()));
    EXPECT_FALSE(std::isnan(p.y()));
    EXPECT_TRUE(p.AlmostEquals(p1));
  }
}

// ---- bezier_smoothing_2 (num_segments overload) -------------------------------

TEST_F(CalcUtils2DTest, BezierSmoothing2_NumSegmentsLessThanOne_Throws) {
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  EXPECT_THROW(g::bezier_smoothing_2(p0, p1, p2, 0.5, 0), std::invalid_argument);
  EXPECT_THROW(g::bezier_smoothing_2(p0, p1, p2, 0.5, -3), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_NumSegments_ProducesExactPointCount) {
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  auto result = g::bezier_smoothing_2(p0, p1, p2, 1.0, 3);
  ASSERT_EQ(4u, result.size());
  EXPECT_TRUE(result.front().AlmostEquals(g::Point2D(1, 0)));  // T0
  EXPECT_TRUE(result.back().AlmostEquals(g::Point2D(2, 1)));   // T1
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_NumSegments_IgnoresCurveLength) {
  // Same corner sampled at a fixed count, regardless of how far apart that puts the points -
  // this overload has no min_distance to honor.
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  auto result = g::bezier_smoothing_2(p0, p1, p2, 1.0, 50);
  EXPECT_EQ(51u, result.size());
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_MinSegmentLength_SkipsTrimOnShortSideOnly) {
  // len(p0,p1) == 0.5 (short), len(p2,p1) == 2 (normal). min_segment_length=1.0 skips trimming only
  // on the short side: T0 falls back to p1, T1 is still computed normally from the trim formula.
  g::Point2D p0(0, 0.5), p1(0, 0), p2(2, 0);  // p0-p1 has length 0.5
  auto result = g::bezier_smoothing_2(p0, p1, p2, 1.0, 3, 1.0);
  ASSERT_EQ(4u, result.size());
  EXPECT_TRUE(result.front().AlmostEquals(p1));               // T0 == p1 (skipped)
  EXPECT_FALSE(result.back().AlmostEquals(p1));                // T1 was trimmed normally
  EXPECT_FALSE(result.back().AlmostEquals(p2));                // ... but not all the way to p2
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_MinSegmentLength_SkipsWholeCornerWhenBothSidesShort) {
  // both adjacent edges have length 0.5; min_segment_length=1.0 collapses both T0 and T1 to p1, so
  // every sample in the curve is p1 (no NaN, no curve at all).
  g::Point2D p0(0, 0), p1(0.5, 0), p2(0.5, 0.5);
  auto result = g::bezier_smoothing_2(p0, p1, p2, 1.0, 4, 1.0);
  for (auto const& pt : result) {
    EXPECT_TRUE(pt.AlmostEquals(p1));
  }
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_MinSegmentLength_DefaultsToDoubleEpsilon) {
  // Omitting min_segment_length uses DOUBLE_EPSILON, not 0.0 (see the doc comment for why: compare()
  // already adds its own DOUBLE_EPSILON tolerance on top, so this is deliberately the codebase's usual
  // epsilon-parameter default, matching PolylineExpansionParams::min_segment_length).
  g::Point2D p0(0, 0), p1(2, 0), p2(2, 2);
  auto with_default = g::bezier_smoothing_2(p0, p1, p2, 1.0, 3);
  auto with_explicit_epsilon = g::bezier_smoothing_2(p0, p1, p2, 1.0, 3, g::DOUBLE_EPSILON);
  ASSERT_EQ(with_default.size(), with_explicit_epsilon.size());
  for (std::size_t i = 0; i < with_default.size(); ++i) {
    EXPECT_TRUE(with_default[i].AlmostEquals(with_explicit_epsilon[i]));
  }
}

TEST_F(CalcUtils2DTest, BezierSmoothing2_MinSegmentLength_ExplicitZero_StillSkipsExactDegenerate) {
  // Passing 0.0 explicitly opts back into "only skip a truly, exactly zero-length edge" — p0 == p1
  // here is always <= any non-negative min_segment_length, so this still can't regress to NaN.
  g::Point2D p1(1, 1), p2(3, 1);
  auto result = g::bezier_smoothing_2(p1, p1, p2, 0.5, 0.1, 0.0);
  for (auto const& pt : result) {
    EXPECT_FALSE(std::isnan(pt.x()));
    EXPECT_TRUE(pt.AlmostEquals(p1));
  }
}

// ---- polyline_expansion (free function behind Polyline2D::Expand / Polyline3D::Expand) -----------

TEST_F(CalcUtils2DTest, PolylineExpansion_TwoPointInput_ReturnsUnchanged) {
  std::vector<g::Point2D> input{g::Point2D(0, 0), g::Point2D(1, 1)};
  auto result = g::polyline_expansion(input, {});
  ASSERT_EQ(2u, result.size());
  EXPECT_TRUE(result[0].AlmostEquals(input[0]));
  EXPECT_TRUE(result[1].AlmostEquals(input[1]));
}

TEST_F(CalcUtils2DTest, PolylineExpansion_SmoothsInnerCorner_TrimmedTangentsAppearInOutput) {
  std::vector<g::Point2D> input{g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 2)};
  g::PolylineExpansionParams settings;
  settings.smoothness = 1.0;
  settings.segments_per_corner = 3;
  auto result = g::polyline_expansion(input, settings);

  EXPECT_TRUE(result.front().AlmostEquals(g::Point2D(0, 0)));
  EXPECT_TRUE(result.back().AlmostEquals(g::Point2D(2, 2)));

  bool has_t0 = false, has_t1 = false;
  for (auto const& p : result) {
    if (p.AlmostEquals(g::Point2D(1, 0))) has_t0 = true;
    if (p.AlmostEquals(g::Point2D(2, 1))) has_t1 = true;
  }
  EXPECT_TRUE(has_t0);
  EXPECT_TRUE(has_t1);
}

TEST_F(CalcUtils2DTest, PolylineExpansion_MultipleCorners_EachCornerUsesItsOwnOriginalKnots) {
  // Two consecutive right-angle corners sharing an edge of length 2, smoothness=1.0 (k=0.5, so each
  // corner's trim is exactly half of its shorter adjacent edge). Regression guard for a bug where
  // p0/p1 for corner i were read back out of the *already built* output buffer instead of the
  // original input knots: that bug would compute corner 2 (input[2]=(2,2)) using a wrong,
  // corner-1-curve-derived "p1" instead of (2,2), producing a different (wrong) trimmed tangent on
  // the far side of corner 2 — (1,2) below would not appear, and/or the shared-edge midpoint would
  // not come out exactly once.
  std::vector<g::Point2D> input{g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 2), g::Point2D(0, 2)};
  g::PolylineExpansionParams settings;
  settings.smoothness = 1.0;
  settings.segments_per_corner = 3;
  auto result = g::polyline_expansion(input, settings);

  EXPECT_TRUE(result.front().AlmostEquals(g::Point2D(0, 0)));
  EXPECT_TRUE(result.back().AlmostEquals(g::Point2D(0, 2)));

  // corner 1's T1 and corner 2's T0 both fall exactly on the shared edge's midpoint (2,1) — trims are
  // each capped at half of the shared edge, so they must meet there exactly, deduplicated to one point.
  int count_midpoint = 0;
  bool has_corner2_t1 = false;
  for (auto const& p : result) {
    if (p.AlmostEquals(g::Point2D(2, 1))) ++count_midpoint;
    if (p.AlmostEquals(g::Point2D(1, 2))) has_corner2_t1 = true;
  }
  EXPECT_EQ(1, count_midpoint);
  EXPECT_TRUE(has_corner2_t1);
}

TEST_F(CalcUtils2DTest, PolylineExpansion_MinSegmentLength_SkipsShortCorner_MatchesOriginal) {
  std::vector<g::Point2D> input{g::Point2D(0, 0), g::Point2D(0.5, 0), g::Point2D(0.5, 0.5)};
  g::PolylineExpansionParams settings;
  settings.smoothness = 1.0;
  settings.segments_per_corner = 4;
  settings.min_segment_length = 1.0;
  auto result = g::polyline_expansion(input, settings);
  ASSERT_EQ(input.size(), result.size());
  for (std::size_t i = 0; i < input.size(); ++i) {
    EXPECT_TRUE(result[i].AlmostEquals(input[i])) << "index " << i;
  }
}

// --------------------------------------------------------------------------------------------------
// clip()
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, Clip_OverlappingSquares_ReturnsIntersectionArea) {
  std::vector<g::Point2D> clipper{g::Point2D(0.5, 0.5), g::Point2D(1.5, 0.5), g::Point2D(1.5, 1.5),
                                  g::Point2D(0.5, 1.5)};
  std::vector<g::Point2D> subject{g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)};

  auto rings = g::clip(clipper, subject);
  ASSERT_EQ(1u, rings.size());
  auto poly = g::Polygon2D::Make(rings[0]);
  EXPECT_NEAR(0.25, poly.Area(), 1e-6);
}

TEST_F(CalcUtils2DTest, Clip_Disjoint_ReturnsEmpty) {
  std::vector<g::Point2D> clipper{g::Point2D(5, 5), g::Point2D(6, 5), g::Point2D(6, 6), g::Point2D(5, 6)};
  std::vector<g::Point2D> subject{g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)};

  auto rings = g::clip(clipper, subject);
  EXPECT_TRUE(rings.empty());
}

TEST_F(CalcUtils2DTest, Clip_SubjectFullyInsideClipper_ReturnsSubject) {
  std::vector<g::Point2D> clipper{g::Point2D(0, 0), g::Point2D(10, 0), g::Point2D(10, 10), g::Point2D(0, 10)};
  std::vector<g::Point2D> subject{g::Point2D(4, 4), g::Point2D(6, 4), g::Point2D(6, 6), g::Point2D(4, 6)};

  auto rings = g::clip(clipper, subject);
  ASSERT_EQ(1u, rings.size());
  auto poly = g::Polygon2D::Make(rings[0]);
  EXPECT_NEAR(4.0, poly.Area(), 1e-6);
}

// Audit for the same class of bug Polygon3D::Make() had (see CHANGELOG / plane_matches_canonical_winding):
// clip()'s Point3D path builds its View2D from Plane::From3Points(subject_loop[0], subject_loop[1],
// subject_loop[2]) with no cross-check against a canonical winding at all — unlike Polygon3D::Make(),
// clip() never validates or promises a CCW/CW convention on its input loops in the first place. Confirms
// that's fine: clip()'s underlying engine (the single-ring, probe-based boolean_op) classifies inside/
// outside via nonzero winding number, which doesn't care about absolute chirality, only that the same view
// is used consistently for both loops — so an unlucky reflex-corner-first vertex ordering in subject_loop
// cannot corrupt the result the way it could for Polygon3D::Make()'s stored, orientation-promising PLANE.
TEST_F(CalcUtils2DTest, Clip_Point3D_ReflexFirstVertex_AreaStableAcrossRotation) {
  // L-shaped hexagon with exactly one reflex corner (area 12), lifted to z=0.
  std::vector<g::Point3D> base = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 2, 0),
      g::Point3D(2, 2, 0), g::Point3D(2, 4, 0), g::Point3D(0, 4, 0),
  };
  std::vector<g::Point3D> clipper = {
      g::Point3D(-1, -1, 0), g::Point3D(5, -1, 0), g::Point3D(5, 5, 0), g::Point3D(-1, 5, 0),
  };
  for (int start = 0; start < static_cast<int>(base.size()); ++start) {
    std::vector<g::Point3D> rotated;
    for (int i = 0; i < static_cast<int>(base.size()); ++i) {
      rotated.push_back(base[(start + i) % base.size()]);
    }
    auto rings = g::clip(clipper, rotated);
    ASSERT_EQ(1u, rings.size()) << "rotation start=" << start;
    double total = 0.0;
    for (auto const& ring : rings) {
      total += std::abs(g::signed_area(ring, std::nullopt));
    }
    EXPECT_NEAR(12.0, total, 1e-9) << "rotation start=" << start;
  }
}

TEST_F(CalcUtils2DTest, Clip_Point3D_CoplanarOverlappingSquares_ReturnsIntersectionArea) {
  std::vector<g::Point3D> clipper{g::Point3D(0.5, 0.5, 0), g::Point3D(1.5, 0.5, 0), g::Point3D(1.5, 1.5, 0),
                                  g::Point3D(0.5, 1.5, 0)};
  std::vector<g::Point3D> subject{g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0),
                                  g::Point3D(0, 1, 0)};

  auto rings = g::clip(clipper, subject);
  ASSERT_EQ(1u, rings.size());
  auto poly = g::Polygon3D::Make(rings[0]);
  EXPECT_NEAR(0.25, poly.Area(), 1e-6);
  // result must actually lie in the shared z=0 plane, not just have the right area
  for (auto const& p : rings[0]) {
    EXPECT_NEAR(0.0, p.z(), 1e-9);
  }
}

TEST_F(CalcUtils2DTest, Clip_Point3D_NonXYPlane_ReturnsIntersectionArea) {
  // Both loops on the plane y=2 (a "vertical wall"), overlapping in a 1x1 square.
  std::vector<g::Point3D> clipper{g::Point3D(0.5, 2, 0.5), g::Point3D(1.5, 2, 0.5), g::Point3D(1.5, 2, 1.5),
                                  g::Point3D(0.5, 2, 1.5)};
  std::vector<g::Point3D> subject{g::Point3D(0, 2, 0), g::Point3D(1, 2, 0), g::Point3D(1, 2, 1),
                                  g::Point3D(0, 2, 1)};

  auto rings = g::clip(clipper, subject);
  ASSERT_EQ(1u, rings.size());
  // Don't route through Polygon3D::Make here: its CCW check locks orientation to match the closest
  // *canonical* world-plane normal (see are_ccw/closest_world_plane_to), not just "some consistent
  // winding" — a ring whose natural winding follows the input order can legitimately fail that even
  // though the ring itself (area, plane) is correct. Check the geometry directly instead.
  EXPECT_NEAR(0.25, std::abs(g::signed_area(rings[0])), 1e-6);
  for (auto const& p : rings[0]) {
    EXPECT_NEAR(2.0, p.y(), 1e-9);
  }
}

TEST_F(CalcUtils2DTest, Clip_Point3D_Disjoint_ReturnsEmpty) {
  std::vector<g::Point3D> clipper{g::Point3D(5, 5, 0), g::Point3D(6, 5, 0), g::Point3D(6, 6, 0),
                                  g::Point3D(5, 6, 0)};
  std::vector<g::Point3D> subject{g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0),
                                  g::Point3D(0, 1, 0)};

  auto rings = g::clip(clipper, subject);
  EXPECT_TRUE(rings.empty());
}

TEST_F(CalcUtils2DTest, Clip_Point3D_NonCoplanar_Throws) {
  std::vector<g::Point3D> clipper{g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(0, 1, 1),
                                  g::Point3D(0, 0, 1)};
  std::vector<g::Point3D> subject{g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0),
                                  g::Point3D(0, 1, 0)};

  EXPECT_THROW(g::clip(clipper, subject), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, Clip_Point3D_TooFewSubjectPoints_Throws) {
  std::vector<g::Point3D> clipper{g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)};
  std::vector<g::Point3D> subject{g::Point3D(0, 0, 0), g::Point3D(1, 0, 0)};

  EXPECT_THROW(g::clip(clipper, subject), std::invalid_argument);
}

// --------------------------------------------------------------------------------------------------
// Benchmark: boolean_op (old, probe-both-sides classify_and_orient — still backs clip()) vs
// boolean_op_multi (new, source-tagged classify_and_orient_source_tagged — backs Polygon2D's boolean
// ops). Both are called directly here (not through Polygon2D) with SINGLE-piece, already-simple operands,
// so the comparison isolates the classification difference: boolean_op probes both operands, left and
// right, for every split segment (4 polygon_contains calls/segment); boolean_op_multi's own-operand side
// is read off the CCW-outer/CW-hole convention instead (2 calls/segment, only for the OTHER operand). That
// 4-vs-2 count is provable directly from reading classify_and_orient / classify_and_orient_source_tagged
// (see calc_utils2d.cpp) — this benchmark exists to check whether it actually shows up in wall-clock time.
//
// Measured result (3 runs, Debug/unoptimized build): a wash at small sizes (~8-10 vertices/operand,
// ratio 0.92-1.0x — boolean_op_multi's extra bookkeeping, split_segments_at_crossings_tagged's tagging
// and cancel_coincident_same_operand_pairs' pass, roughly offsets the halved probe count there) turning
// into a real, if noisy, 3-27% speedup for boolean_op_multi at larger sizes (~40-60 vertices/operand),
// where polygon_contains' O(vertices) cost per probe starts to dominate. So the theoretical halving is
// real but its practical payoff is size-dependent, not a blanket win — worth knowing before citing "half
// the probes" as a general performance claim.
//
// Informational only (prints timing; no assertion on absolute wall-clock numbers, which would be flaky
// across machines/CI) — run with --gtest_filter to see the numbers:
// `--gtest_filter=CalcUtils2DTest.Benchmark_BooleanOpVsBooleanOpMulti`.
TEST_F(CalcUtils2DTest, Benchmark_BooleanOpVsBooleanOpMulti) {
  // Points near the circle's rim (not filled disk) so nearly every point survives onto the hull — a
  // filled-disk sample's hull grows sub-linearly in point count (observed: ~18 vertices even at 2000
  // points), which doesn't reach the polygon sizes where an O(vertices)-per-probe cost would matter.
  auto random_convex_ring = [](std::mt19937& rng, double cx, double cy, double r, int n) {
    std::uniform_real_distribution<double> angle(0.0, 2.0 * 3.14159265358979);
    std::uniform_real_distribution<double> radial_jitter(0.97, 1.0);
    std::vector<g::Point2D> pts;
    pts.reserve(n);
    for (int i = 0; i < n; ++i) {
      double a = angle(rng), rr = r * radial_jitter(rng);
      pts.emplace_back(cx + rr * std::cos(a), cy + rr * std::sin(a));
    }
    return g::convex_hull(pts);
  };

  std::mt19937 rng(42);
  for (int n : {10, 30, 80, 300}) {
    auto subj = random_convex_ring(rng, 400.0, 400.0, 150.0, n);
    auto clip = random_convex_ring(rng, 460.0, 460.0, 150.0, n);
    ASSERT_GE(subj.size(), 3u);
    ASSERT_GE(clip.size(), 3u);

    gd::RingPieces subj_pieces = {{subj, {}}};
    gd::RingPieces clip_pieces = {{clip, {}}};

    int const kIterations = n > 100 ? 15 : 100;

    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < kIterations; ++i) {
      auto result = gd::boolean_op(subj, {}, clip, {}, gd::BooleanOp::Intersection);
      ASSERT_FALSE(result.empty());
    }
    auto t1 = std::chrono::steady_clock::now();
    for (int i = 0; i < kIterations; ++i) {
      auto result = gd::boolean_op_multi(subj_pieces, clip_pieces, gd::BooleanOp::Intersection);
      ASSERT_FALSE(result.empty());
    }
    auto t2 = std::chrono::steady_clock::now();

    double old_us = std::chrono::duration<double, std::micro>(t1 - t0).count() / kIterations;
    double new_us = std::chrono::duration<double, std::micro>(t2 - t1).count() / kIterations;

    std::cout << "n=" << n << " (subj=" << subj.size() << " clip=" << clip.size()
              << " verts): boolean_op=" << old_us << "us  boolean_op_multi=" << new_us
              << "us  ratio=" << (old_us / new_us) << "x\n";
  }
}

// --------------------------------------------------------------------------------------------------
// EarClippingTriangulation sanity check (n=5, convex) — the final leftover triangle after the main
// while(n>3) loop is a known trouble spot (see git history): must cover the last 3 remaining vertices,
// not reuse stale loop-local indices from an earlier iteration.
// --------------------------------------------------------------------------------------------------

TEST_F(CalcUtils2DTest, Triangulate_ConvexPentagon_ProducesThreeTrianglesCoveringFullArea) {
  std::vector<g::Point2D> pentagon = {{0, 0}, {4, 0}, {4, 3}, {2, 5}, {0, 3}};
  ASSERT_TRUE(g::are_ccw(pentagon));

  auto triangles = g::triangulate(pentagon);

  ASSERT_EQ(triangles.size(), 3u);  // n - 2 triangles for a convex n-gon

  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += t.Area();
  }
  EXPECT_NEAR(total_area, 16.0, 1e-6);  // shoelace area of the pentagon above
}

TEST_F(CalcUtils2DTest, Triangulate_ConcaveChevron_ExercisesReflexVertexHandling) {
  // "Chevron" with a notch at (2,1) — the one reflex vertex ear-clipping's optimization (only test
  // candidate ears against reflex vertices) actually needs to exercise. Deliberately NOT (2,2): that
  // point sits exactly on the (0,4)-(4,0) diagonal, a coincidental exact collinearity across a
  // non-adjacent diagonal that's a known hard case for any strict-inequality point-in-triangle test.
  std::vector<g::Point2D> chevron = {{0, 0}, {4, 0}, {4, 4}, {2, 1}, {0, 4}};
  ASSERT_TRUE(g::are_ccw(chevron));

  auto triangles = g::triangulate(chevron);

  ASSERT_EQ(triangles.size(), 3u);  // n - 2 triangles

  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += t.Area();
  }
  EXPECT_NEAR(total_area, 10.0, 1e-6);  // shoelace area of the chevron above
}

TEST_F(CalcUtils2DTest, Triangulate_FewerThanThreePoints_Throws) {
  std::vector<g::Point2D> too_few = {{0, 0}, {1, 0}};
  EXPECT_THROW(g::triangulate(too_few), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, Triangulate_WindingAssert_ThrowsOnClockwiseInput) {
  std::vector<g::Point2D> cw_square = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};  // CW, not CCW
  ASSERT_FALSE(g::are_ccw(cw_square));

  g::TriangulationParams settings{g::TriangulationParams::Strategy::EarClipping,
                                  g::TriangulationParams::Simplicity::Guaranteed,
                                  g::TriangulationParams::Winding::Assert,
                                  g::TriangulationParams::Collinearity::Guaranteed};
  EXPECT_THROW(g::triangulate(cw_square, settings), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, Triangulate_WindingEnforce_FixesClockwiseInput) {
  std::vector<g::Point2D> cw_square = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};  // CW, not CCW
  ASSERT_FALSE(g::are_ccw(cw_square));

  g::TriangulationParams settings{g::TriangulationParams::Strategy::EarClipping,
                                  g::TriangulationParams::Simplicity::Guaranteed,
                                  g::TriangulationParams::Winding::Enforce,
                                  g::TriangulationParams::Collinearity::Guaranteed};
  auto triangles = g::triangulate(cw_square, settings);

  ASSERT_EQ(triangles.size(), 2u);
  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += t.Area();
  }
  EXPECT_NEAR(total_area, 1.0, 1e-9);
}

TEST_F(CalcUtils2DTest, Triangulate_CollinearityAssert_ThrowsOnCollinearPoint) {
  // (2, 0) sits exactly on the edge from (0,0) to (4,0) — a collinear consecutive triplet.
  std::vector<g::Point2D> with_collinear = {{0, 0}, {2, 0}, {4, 0}, {4, 4}, {0, 4}};

  g::TriangulationParams settings{g::TriangulationParams::Strategy::EarClipping,
                                  g::TriangulationParams::Simplicity::Guaranteed,
                                  g::TriangulationParams::Winding::Guaranteed,
                                  g::TriangulationParams::Collinearity::Assert};
  EXPECT_THROW(g::triangulate(with_collinear, settings), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, Triangulate_CollinearityEnforce_RemovesCollinearPointAndSucceeds) {
  std::vector<g::Point2D> with_collinear = {{0, 0}, {2, 0}, {4, 0}, {4, 4}, {0, 4}};

  g::TriangulationParams settings{g::TriangulationParams::Strategy::EarClipping,
                                  g::TriangulationParams::Simplicity::Guaranteed,
                                  g::TriangulationParams::Winding::Guaranteed,
                                  g::TriangulationParams::Collinearity::Enforce};
  auto triangles = g::triangulate(with_collinear, settings);

  ASSERT_EQ(triangles.size(), 2u);  // (2,0) removed -> a 4-vertex square -> 2 triangles
  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += t.Area();
  }
  EXPECT_NEAR(total_area, 16.0, 1e-9);
}

TEST_F(CalcUtils2DTest, Triangulate_SimplicityAssert_ThrowsOnSelfIntersectingInput) {
  // Bowtie: edges (0,0)-(1,1) and (1,0)-(0,1) cross in the middle.
  std::vector<g::Point2D> bowtie = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};

  g::TriangulationParams settings{g::TriangulationParams::Strategy::EarClipping,
                                  g::TriangulationParams::Simplicity::Assert,
                                  g::TriangulationParams::Winding::Guaranteed,
                                  g::TriangulationParams::Collinearity::Guaranteed};
  EXPECT_THROW(g::triangulate(bowtie, settings), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, Triangulate_EarClippingBestFitStrategy_IsTheDefaultAndSucceeds) {
  std::vector<g::Point2D> square = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};

  g::TriangulationParams default_settings{};
  EXPECT_EQ(default_settings.strategy, g::TriangulationParams::Strategy::EarClippingBestFit);

  g::TriangulationParams explicit_settings{g::TriangulationParams::Strategy::EarClippingBestFit};
  auto triangles = g::triangulate(square, explicit_settings);

  ASSERT_EQ(triangles.size(), 2u);
  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += t.Area();
  }
  EXPECT_NEAR(total_area, 1.0, 1e-9);
}

TEST_F(CalcUtils2DTest, Triangulate_MonotonePolygonStrategy_Throws) {
  std::vector<g::Point2D> square = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  g::TriangulationParams settings{g::TriangulationParams::Strategy::MonotonePolygon};
  EXPECT_THROW(g::triangulate(square, settings), std::runtime_error);
}

TEST_F(CalcUtils2DTest, Triangulate_DelaunayStrategy_Throws) {
  std::vector<g::Point2D> square = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  g::TriangulationParams settings{g::TriangulationParams::Strategy::Delaunay};
  EXPECT_THROW(g::triangulate(square, settings), std::runtime_error);
}

#pragma region validate_adjacency / fix_adjacency / triangulate(vector<Polygon2D>)

TEST_F(CalcUtils2DTest, ValidateAdjacency_ConformingSharedEdge_ReturnsNoViolations) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  EXPECT_TRUE(g::validate_adjacency(std::vector<g::Polygon2D>{p0, p1}).empty());
}

TEST_F(CalcUtils2DTest, ValidateAdjacency_TJunction_DetectsViolation) {
  // Two unit squares side by side (share edge (1,0)-(1,1)), plus a roof triangle spanning both squares'
  // top -- its base edge (0,1)-(2,1) passes straight through (1,1) without the two squares' shared
  // vertex being one of ITS endpoints. Classic T-junction.
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto roof = g::Polygon2D::Make({g::Point2D(0, 1), g::Point2D(2, 1), g::Point2D(1, 2)});

  auto violations = g::validate_adjacency(std::vector<g::Polygon2D>{p0, p1, roof});

  ASSERT_FALSE(violations.empty());
  for (auto const& v : violations) {
    EXPECT_FALSE(v.is_non_manifold);
  }
}

TEST_F(CalcUtils2DTest, ValidateAdjacency_NonManifoldEdge_DetectsViolation) {
  // Three triangles all sharing the exact same edge (0,0)-(1,0) -- a full edge with 3 neighbors, not
  // just 1. No amount of vertex splicing can fix this (there's no missing vertex).
  auto a = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0.5, 1)});
  auto b = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(0, 0), g::Point2D(0.5, -1)});
  auto c = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(0, 0), g::Point2D(0.5, -2)});

  auto violations = g::validate_adjacency(std::vector<g::Polygon2D>{a, b, c});

  ASSERT_FALSE(violations.empty());
  EXPECT_TRUE(violations.front().is_non_manifold);
  EXPECT_EQ(violations.front().facet_indices.size(), 3u);
}

TEST_F(CalcUtils2DTest, FixAdjacency_TJunction_SplicesVertexAndPreservesTotalArea) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto roof = g::Polygon2D::Make({g::Point2D(0, 1), g::Point2D(2, 1), g::Point2D(1, 2)});
  std::vector<g::Polygon2D> facets{p0, p1, roof};

  double area_before = 0.0;
  for (auto const& f : facets) {
    area_before += f.Area();
  }

  auto fixed = g::fix_adjacency(facets);

  EXPECT_TRUE(g::validate_adjacency(fixed).empty());
  // fixed rings carry a deliberately-collinear splice vertex, so they must be rebuilt with
  // Collinearity::Guaranteed -- Polygon2D::Make() (which unconditionally strips collinear points)
  // would undo the splice, same reasoning as triangulate(vector<Polygon2D>, Enforce, ...) itself.
  double area_after = 0.0;
  for (auto const& ring : fixed) {
    g::TriangulationParams guaranteed_collinearity;
    guaranteed_collinearity.collinearity = g::TriangulationParams::Collinearity::Guaranteed;
    for (auto const& t : g::triangulate(ring, guaranteed_collinearity)) {
      area_after += t.Area();
    }
  }
  EXPECT_NEAR(area_before, area_after, 1e-9);
}

TEST_F(CalcUtils2DTest, FixAdjacency_NonManifoldEdge_Throws) {
  auto a = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0.5, 1)});
  auto b = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(0, 0), g::Point2D(0.5, -1)});
  auto c = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(0, 0), g::Point2D(0.5, -2)});
  EXPECT_THROW(g::fix_adjacency(std::vector<g::Polygon2D>{a, b, c}), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, TriangulateVectorOfPolygons_DefaultEnforce_FixesTJunctionAndTriangulates) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto roof = g::Polygon2D::Make({g::Point2D(0, 1), g::Point2D(2, 1), g::Point2D(1, 2)});

  auto triangles = g::triangulate(std::vector<g::Polygon2D>{p0, p1, roof});

  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += t.Area();
  }
  EXPECT_NEAR(total_area, 3.0, 1e-9);
}

TEST_F(CalcUtils2DTest, TriangulateVectorOfPolygons_Assert_ThrowsOnTJunction) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto roof = g::Polygon2D::Make({g::Point2D(0, 1), g::Point2D(2, 1), g::Point2D(1, 2)});

  EXPECT_THROW(g::triangulate(std::vector<g::Polygon2D>{p0, p1, roof}, g::AdjacencyConformity::Assert),
              std::invalid_argument);
}

TEST_F(CalcUtils2DTest, TriangulateVectorOfPolygons_Guaranteed_SkipsCheckAndStillTriangulatesEachFacet) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto roof = g::Polygon2D::Make({g::Point2D(0, 1), g::Point2D(2, 1), g::Point2D(1, 2)});

  auto triangles =
      g::triangulate(std::vector<g::Polygon2D>{p0, p1, roof}, g::AdjacencyConformity::Guaranteed);

  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += t.Area();
  }
  EXPECT_NEAR(total_area, 3.0, 1e-9);
}

#pragma endregion

#pragma region detail::view triangulation internals (is_ccw, has_collinears, ear_clipping_triangulation,
// triangulate_impl) — C++-only, never bound to Python/C#, so this is their only test coverage.

TEST_F(CalcUtils2DTest, IsCcw_CcwSquare_ReturnsTrue) {
  std::vector<g::Point2D> ccw_square = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  EXPECT_TRUE(gd::view::is_ccw(ccw_square, g::View2D::XY()));
}

TEST_F(CalcUtils2DTest, IsCcw_CwSquare_ReturnsFalse) {
  std::vector<g::Point2D> cw_square = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
  EXPECT_FALSE(gd::view::is_ccw(cw_square, g::View2D::XY()));
}

TEST_F(CalcUtils2DTest, HasCollinears_CleanSquare_ReturnsFalse) {
  std::vector<g::Point2D> square = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  EXPECT_FALSE(gd::view::has_collinears(square, g::View2D::XY()));
}

TEST_F(CalcUtils2DTest, HasCollinears_CollinearMidpoint_ReturnsTrue) {
  // (2, 0) sits exactly on the edge from (0,0) to (4,0).
  std::vector<g::Point2D> with_collinear = {{0, 0}, {2, 0}, {4, 0}, {4, 4}, {0, 4}};
  EXPECT_TRUE(gd::view::has_collinears(with_collinear, g::View2D::XY()));
}

TEST_F(CalcUtils2DTest, HasCollinears_DuplicateConsecutivePoint_ReturnsTrue) {
  // A duplicate is the degenerate case of collinearity: no separate check needed (see the function's
  // own doc comment) — (1,0) repeated makes one edge vector the zero vector, so the cross product used
  // to test the turn at that vertex is trivially zero too.
  std::vector<g::Point2D> with_duplicate = {{0, 0}, {1, 0}, {1, 0}, {1, 1}, {0, 1}};
  EXPECT_TRUE(gd::view::has_collinears(with_duplicate, g::View2D::XY()));
}

TEST_F(CalcUtils2DTest, EarClippingTriangulation_ConvexSquare_ReturnsValidIndexTriplets) {
  std::vector<g::Point2D> square = {{0, 0}, {2, 0}, {2, 2}, {0, 2}};
  auto tri_indices = gd::view::ear_clipping_triangulation(square, g::View2D::XY());

  ASSERT_EQ(tri_indices.size(), 2u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, square.size());  // every index must be a valid index into the input
    }
    total_area += g::Triangle2D::Make(square[tri[0]], square[tri[1]], square[tri[2]]).Area();
  }
  EXPECT_NEAR(total_area, 4.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingTriangulation_ConcavePentagon_ReturnsValidIndexTriplets) {
  // Same chevron shape as the public-API test above (area 10, one reflex vertex at index 3).
  std::vector<g::Point2D> chevron = {{0, 0}, {4, 0}, {4, 4}, {2, 1}, {0, 4}};
  auto tri_indices = gd::view::ear_clipping_triangulation(chevron, g::View2D::XY());

  ASSERT_EQ(tri_indices.size(), 3u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, chevron.size());
    }
    total_area += g::Triangle2D::Make(chevron[tri[0]], chevron[tri[1]], chevron[tri[2]]).Area();
  }
  EXPECT_NEAR(total_area, 10.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingTriangulation_CombPolygon_RequiresMultipleTraversalLaps) {
  // A 3-tooth "comb" -- the classic adversarial shape for naive ear-clipping: several deep, narrow
  // notches between tall teeth. Unlike every other shape tested here, this one genuinely needs more
  // than one lap around the ring: some vertices get checked, rejected as non-ears (blocked by a
  // reflex vertex elsewhere), and only succeed on a later lap once an unrelated clip shrinks the
  // reflex set. Regression/stress test for exactly that multi-lap code path, not a specific bug.
  std::vector<g::Point2D> comb = {
      {5, 0}, {5, 10}, {4, 10}, {4, 9}, {3, 9}, {3, 10},
      {2, 10}, {2, 9}, {1, 9}, {1, 10}, {0, 10}, {0, 0}};
  auto tri_indices = gd::view::ear_clipping_triangulation(comb, g::View2D::XY());

  ASSERT_EQ(tri_indices.size(), 10u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, comb.size());
    }
    total_area += g::Triangle2D::Make(comb[tri[0]], comb[tri[1]], comb[tri[2]]).Area();
  }
  EXPECT_NEAR(total_area, 48.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingTriangulation_ReflexVertexOnNonAdjacentDiagonal_StaysInsidePolygon) {
  // L-shaped hexagon whose reflex vertex (2, 2) sits exactly on the diagonal between two OTHER
  // (non-adjacent) vertices: (0, 4) and (4, 0) both satisfy x + y == 4, same as (2, 2). Regression
  // test for a bug where the ear-validity check used a strict point-in-triangle test, so a
  // collinear-but-not-strictly-inside reflex vertex failed to disqualify a diagonal that actually
  // exits the polygon through the notch -- producing a triangle with real area outside the polygon
  // even though the total triangle area still happened to sum correctly by coincidence elsewhere.
  std::vector<g::Point2D> l_shape = {{0, 0}, {4, 0}, {4, 2}, {2, 2}, {2, 4}, {0, 4}};
  auto tri_indices = gd::view::ear_clipping_triangulation(l_shape, g::View2D::XY());
  auto polygon = g::Polygon2D::Make(l_shape);

  ASSERT_EQ(tri_indices.size(), 4u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    auto t = g::Triangle2D::Make(l_shape[tri[0]], l_shape[tri[1]], l_shape[tri[2]]);
    total_area += t.Area();
    EXPECT_TRUE(polygon.Contains(t.Centroid())) << "triangle " << t.ToWkt() << " strays outside the polygon";
  }
  EXPECT_NEAR(total_area, polygon.Area(), 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingTriangulation_CollinearMidEdgeVertex_ProducesCorrectAreaAndCount) {
  // A 4x4 square with a redundant vertex (2, 0) sitting exactly on the middle of the bottom edge --
  // (0,0), (2,0), (4,0) are exactly collinear, so (2,0) is neither reflex nor a normal convex ear.
  // Diagnostic for the new are_collinear() skip-branch in ear_clipping_triangulation: after skipping
  // vertex (2,0), the loop must still visit every other vertex with CORRECT i_prev/i_next.
  std::vector<g::Point2D> square_with_midpoint = {{0, 0}, {2, 0}, {4, 0}, {4, 4}, {0, 4}};
  auto tri_indices = gd::view::ear_clipping_triangulation(square_with_midpoint, g::View2D::XY());
  auto polygon = g::Polygon2D::Make({{0, 0}, {4, 0}, {4, 4}, {0, 4}});  // the "true" shape, area 16

  ASSERT_EQ(tri_indices.size(), 3u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, square_with_midpoint.size()) << "index out of range -- stale i_prev/i_next?";
    }
    auto t = g::Triangle2D::Make(square_with_midpoint[tri[0]], square_with_midpoint[tri[1]],
                                 square_with_midpoint[tri[2]]);
    total_area += t.Area();
    EXPECT_TRUE(polygon.Contains(t.Centroid())) << "triangle " << t.ToWkt() << " strays outside the polygon";
  }
  EXPECT_NEAR(total_area, 16.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingTriangulation_CollinearVertexAtRingStart_DoesNotHang) {
  // Same square-with-a-redundant-midpoint shape as the test above, but rotated so the collinear
  // vertex (2, 0) is at INDEX 0 -- i.e. the very first vertex the main loop's i=0 start visits.
  // Diagnostic for the are_collinear() skip-branch: `i = i_next; continue;` never re-derives
  // i_prev/i_next for the new i, so if the loop starts exactly on a collinear vertex, the very next
  // iteration runs with stale (wrong) i_prev/i_next.
  std::vector<g::Point2D> square_with_midpoint = {{2, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}};
  auto tri_indices = gd::view::ear_clipping_triangulation(square_with_midpoint, g::View2D::XY());

  ASSERT_EQ(tri_indices.size(), 3u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, square_with_midpoint.size()) << "index out of range -- stale i_prev/i_next?";
    }
    total_area += g::Triangle2D::Make(square_with_midpoint[tri[0]], square_with_midpoint[tri[1]],
                                      square_with_midpoint[tri[2]]).Area();
  }
  EXPECT_NEAR(total_area, 16.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingBestFitTriangulation_ConcavePentagon_ReturnsValidIndexTriplets) {
  // Same chevron shape as EarClippingTriangulation_ConcavePentagon_ReturnsValidIndexTriplets above.
  std::vector<g::Point2D> chevron = {{0, 0}, {4, 0}, {4, 4}, {2, 1}, {0, 4}};
  auto tri_indices = gd::view::ear_clipping_best_fit_triangulation(chevron, g::View2D::XY());

  ASSERT_EQ(tri_indices.size(), 3u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, chevron.size());
    }
    total_area += g::Triangle2D::Make(chevron[tri[0]], chevron[tri[1]], chevron[tri[2]]).Area();
  }
  EXPECT_NEAR(total_area, 10.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingBestFitTriangulation_CombPolygon_RequiresMultipleLapsPerClip) {
  // Same 3-tooth comb as EarClippingTriangulation_CombPolygon_RequiresMultipleTraversalLaps. Every
  // outer-loop iteration of the best-fit algorithm does a full lap over the current ring by design (to
  // find the best-scoring ear before clipping exactly one), so this exercises that unconditionally,
  // not just as an adversarial case.
  std::vector<g::Point2D> comb = {
      {5, 0}, {5, 10}, {4, 10}, {4, 9}, {3, 9}, {3, 10},
      {2, 10}, {2, 9}, {1, 9}, {1, 10}, {0, 10}, {0, 0}};
  auto tri_indices = gd::view::ear_clipping_best_fit_triangulation(comb, g::View2D::XY());

  ASSERT_EQ(tri_indices.size(), 10u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, comb.size());
    }
    total_area += g::Triangle2D::Make(comb[tri[0]], comb[tri[1]], comb[tri[2]]).Area();
  }
  EXPECT_NEAR(total_area, 48.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingBestFitTriangulation_CollinearVertexAtRingStart_DoesNotHang) {
  // Same regression shape as EarClippingTriangulation_CollinearVertexAtRingStart_DoesNotHang, run
  // against the best-fit algorithm's own (separate) scan loop -- it has its own do/while lap and its
  // own risk of getting stuck if the "advance to next vertex" step isn't unconditional.
  std::vector<g::Point2D> square_with_midpoint = {{2, 0}, {4, 0}, {4, 4}, {0, 4}, {0, 0}};
  auto tri_indices = gd::view::ear_clipping_best_fit_triangulation(square_with_midpoint, g::View2D::XY());

  ASSERT_EQ(tri_indices.size(), 3u);
  double total_area = 0.0;
  for (auto const& tri : tri_indices) {
    for (auto idx : tri) {
      ASSERT_LT(idx, square_with_midpoint.size()) << "index out of range -- stale i_prev/i_next?";
    }
    total_area += g::Triangle2D::Make(square_with_midpoint[tri[0]], square_with_midpoint[tri[1]],
                                      square_with_midpoint[tri[2]]).Area();
  }
  EXPECT_NEAR(total_area, 16.0, 1e-9);
}

TEST_F(CalcUtils2DTest, EarClippingBestFitTriangulation_Star_PicksDifferentDiagonalsThanPlainEarClipping) {
  // A 5-pointed star: EarClipping (first valid ear in scan order) fans every triangle out from one
  // vertex; EarClippingBestFit (best-scoring valid ear each step) clips all 5 outer points first, then
  // fans only the remaining inner pentagon. Both are valid triangulations of the same polygon -- same
  // triangle count and total area -- but via genuinely different diagonals. This is the concrete
  // behavioral difference the two strategies exist to offer.
  std::vector<g::Point2D> star = {
      {3.0, 6.0}, {2.29, 3.97}, {0.15, 3.93}, {1.86, 2.63}, {1.24, 0.57},
      {3.0, 1.8}, {4.76, 0.57}, {4.14, 2.63}, {5.85, 3.93}, {3.71, 3.97},
  };

  auto plain = gd::view::ear_clipping_triangulation(star, g::View2D::XY());
  auto best_fit = gd::view::ear_clipping_best_fit_triangulation(star, g::View2D::XY());

  ASSERT_EQ(plain.size(), 8u);
  ASSERT_EQ(best_fit.size(), 8u);

  auto total_area = [&star](std::vector<std::array<std::size_t, 3>> const& tri_indices) {
    double area = 0.0;
    for (auto const& tri : tri_indices) {
      area += g::Triangle2D::Make(star[tri[0]], star[tri[1]], star[tri[2]]).Area();
    }
    return area;
  };
  EXPECT_NEAR(total_area(plain), total_area(best_fit), 1e-9);

  // Every triangle in `plain`'s fan shares the same one vertex (index 9, (3.71, 3.97)) -- BestFit's
  // point-ears (e.g. the (2.29 3.97, 0.15 3.93, 1.86 2.63) triangle clipped from the star's tip) don't.
  // Directly assert the two index-triplet sets differ, order/rotation aside.
  auto normalize = [](std::array<std::size_t, 3> t) {
    std::sort(t.begin(), t.end());
    return t;
  };
  std::set<std::array<std::size_t, 3>> plain_set, best_fit_set;
  for (auto const& t : plain) plain_set.insert(normalize(t));
  for (auto const& t : best_fit) best_fit_set.insert(normalize(t));
  EXPECT_NE(plain_set, best_fit_set);
}

TEST_F(CalcUtils2DTest, TriangulateImpl_CalledDirectly_GuaranteedPath_ProducesCorrectTriangles) {
  std::vector<g::Point2D> square = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  g::TriangulationParams settings{};  // Strategy::EarClippingBestFit, everything Enforce by default

  auto triangles = gd::view::triangulate_impl(square, g::View2D::XY(), settings);

  ASSERT_EQ(triangles.size(), 2u);
  double total_area = 0.0;
  for (auto const& t : triangles) {
    total_area += g::Triangle2D::Make(t[0], t[1], t[2]).Area();
  }
  EXPECT_NEAR(total_area, 1.0, 1e-9);
}

TEST_F(CalcUtils2DTest, TriangulateImpl_CalledDirectly_AssertPath_ThrowsOnBadInput) {
  std::vector<g::Point2D> cw_square = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};  // CW, not CCW
  g::TriangulationParams settings{g::TriangulationParams::Strategy::EarClipping,
                                  g::TriangulationParams::Simplicity::Guaranteed,
                                  g::TriangulationParams::Winding::Assert,
                                  g::TriangulationParams::Collinearity::Guaranteed};

  EXPECT_THROW(gd::view::triangulate_impl(cw_square, g::View2D::XY(), settings), std::invalid_argument);
}

TEST_F(CalcUtils2DTest, TriangulateImpl_CalledDirectly_FewerThanThreePoints_Throws) {
  std::vector<g::Point2D> too_few = {{0, 0}, {1, 0}};
  EXPECT_THROW(gd::view::triangulate_impl(too_few, g::View2D::XY(), g::TriangulationParams{}),
              std::invalid_argument);
}

#pragma endregion

}  // namespace geompp_tests
