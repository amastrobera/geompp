#include "calc_utils2d.hpp"

#include "constants.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "segment_iterator2d.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <optional>
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

}  // namespace geompp_tests
