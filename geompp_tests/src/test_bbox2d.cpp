#include "bbox2d.hpp"

#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class BBox2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(BBox2DTest, Constructor) {
  auto bb = g::BBox2D(g::Point2D::Zero(), g::Point2D(3, 4));
  ASSERT_EQ(g::Point2D::Zero(), bb.min());
  ASSERT_EQ(g::Point2D(3, 4), bb.max());
}

TEST_F(BBox2DTest, ConstructorFromLineSegment) {
  auto s = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(3, 4));
  auto bb = g::BBox2D(s);
  ASSERT_EQ(g::Point2D(-1, -2), bb.min());
  ASSERT_EQ(g::Point2D(3, 4), bb.max());
}

TEST_F(BBox2DTest, ConstructorFromTriangle) {
  auto t = g::Triangle2D::Make(g::Point2D(-1, 0), g::Point2D(1, 0), g::Point2D(0, 2));
  auto bb = g::BBox2D(t);
  ASSERT_EQ(g::Point2D(-1, 0), bb.min());
  ASSERT_EQ(g::Point2D(1, 2), bb.max());
}

TEST_F(BBox2DTest, AlmostEquals) {
  auto bb1 = g::BBox2D(g::Point2D::Zero(), g::Point2D(3, 4));
  auto bb2 = g::BBox2D(g::Point2D::Zero(), g::Point2D(3, 4));
  auto bb3 = g::BBox2D(g::Point2D(1, 1), g::Point2D(5, 6));

  ASSERT_TRUE(bb1.AlmostEquals(bb2));
  ASSERT_FALSE(bb1.AlmostEquals(bb3));
  ASSERT_EQ(bb1, bb2);
  ASSERT_NE(bb1, bb3);
}

TEST_F(BBox2DTest, Assignment) {
  auto bb1 = g::BBox2D(g::Point2D::Zero(), g::Point2D(3, 4));
  auto bb2 = g::BBox2D(g::Point2D(1, 1), g::Point2D(5, 6));
  bb2 = bb1;
  ASSERT_EQ(bb1, bb2);
  bb1 = bb1;
  ASSERT_EQ(g::BBox2D(g::Point2D::Zero(), g::Point2D(3, 4)), bb1);
}

TEST_F(BBox2DTest, Contains) {
  geompp::DECIMAL_PRECISION = 4;
  auto bb = g::BBox2D(g::Point2D::Zero(), g::Point2D(3, 4));

  // corners
  ASSERT_TRUE(bb.Contains(g::Point2D::Zero()));
  ASSERT_TRUE(bb.Contains(g::Point2D(3, 0)));
  ASSERT_TRUE(bb.Contains(g::Point2D(3, 4)));
  ASSERT_TRUE(bb.Contains(g::Point2D(0, 4)));

  // interior
  ASSERT_TRUE(bb.Contains(g::Point2D(1.5, 2)));

  // edges
  ASSERT_TRUE(bb.Contains(g::Point2D(1.5, 0)));
  ASSERT_TRUE(bb.Contains(g::Point2D(1.5, 4)));
  ASSERT_TRUE(bb.Contains(g::Point2D(0, 2)));
  ASSERT_TRUE(bb.Contains(g::Point2D(3, 2)));

  // outside
  ASSERT_FALSE(bb.Contains(g::Point2D(-1, 2)));
  ASSERT_FALSE(bb.Contains(g::Point2D(4, 2)));
  ASSERT_FALSE(bb.Contains(g::Point2D(1.5, -1)));
  ASSERT_FALSE(bb.Contains(g::Point2D(1.5, 5)));
}

}  // namespace geompp_tests
