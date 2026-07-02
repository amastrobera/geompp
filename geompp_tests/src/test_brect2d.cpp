#include "brect2d.hpp"
#include "point2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace g = geompp;

namespace geompp_tests {

class BRect2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

// ── Constructor ──────────────────────────────────────────────────────────────

TEST_F(BRect2DTest, ConstructorEmpty_Throws) {
  ASSERT_THROW(g::BRect2D(std::vector<g::Point2D>{}), std::invalid_argument);
}

TEST_F(BRect2DTest, ConstructorSinglePoint_Throws) {
  ASSERT_THROW(g::BRect2D(std::vector<g::Point2D>{{3.0, 4.0}}), std::invalid_argument);
}

TEST_F(BRect2DTest, ConstructorTwoPoints_Throws) {
  ASSERT_THROW(g::BRect2D(std::vector<g::Point2D>{{0.0, 0.0}, {4.0, 0.0}}), std::invalid_argument);
}

TEST_F(BRect2DTest, ConstructorAxisAlignedSquare) {
  // unit square; any axis-aligned OBB has area = 1
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};
  auto r = g::BRect2D(pts);
  ASSERT_TRUE(r.center().AlmostEquals(g::Point2D(0.5, 0.5)));
  ASSERT_NEAR(r.half_len_u(), 0.5, 1e-9);
  ASSERT_NEAR(r.half_len_v(), 0.5, 1e-9);
  ASSERT_NEAR(r.area(), 1.0, 1e-9);
}

TEST_F(BRect2DTest, ConstructorAxisAlignedRectangle) {
  // 4x2 rectangle: OBB aligned with x-axis (first hull edge wins)
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r = g::BRect2D(pts);
  ASSERT_TRUE(r.center().AlmostEquals(g::Point2D(2.0, 1.0)));
  ASSERT_NEAR(r.area(), 8.0, 1e-9);
  ASSERT_NEAR(r.width(), 4.0, 1e-9);
  ASSERT_NEAR(r.height(), 2.0, 1e-9);
}

TEST_F(BRect2DTest, ConstructorNonConvex_SmallArea) {
  // L-shaped non-convex polygon — OBB is tighter than AABB
  std::vector<g::Point2D> pts = {
    {0.0, 0.0}, {4.0, 0.0}, {4.0, 1.0},
    {1.0, 1.0}, {1.0, 4.0}, {0.0, 4.0},
  };
  auto r = g::BRect2D(pts);
  // AABB would be 4x4 = 16; OBB should cover 4x4 area (same in this case)
  ASSERT_TRUE(r.Contains(g::Point2D(0.5, 0.5)));
  ASSERT_TRUE(r.Contains(g::Point2D(3.5, 0.5)));
}

TEST_F(BRect2DTest, ConstructorAllPointsContained) {
  std::vector<g::Point2D> pts = {
    {0.0, 0.0}, {3.0, 0.0}, {3.0, 2.0}, {0.0, 2.0},
    {1.0, 0.5}, {2.0, 1.5},  // interior points
  };
  auto r = g::BRect2D(pts);
  for (auto const& p : pts)
    ASSERT_TRUE(r.Contains(p)) << "OBB must contain all input points";
}

// ── Accessors ────────────────────────────────────────────────────────────────

TEST_F(BRect2DTest, Accessors_AxisesAreUnitVectors) {
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {2.0, 0.0}, {2.0, 1.0}, {0.0, 1.0}};
  auto r = g::BRect2D(pts);
  ASSERT_NEAR(r.axis_u().Length(), 1.0, 1e-9);
  ASSERT_NEAR(r.axis_v().Length(), 1.0, 1e-9);
}

TEST_F(BRect2DTest, Accessors_AxesOrthogonal) {
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {3.0, 1.0}, {2.0, 3.0}, {-1.0, 2.0}};
  auto r = g::BRect2D(pts);
  double dot = r.axis_u().Dot(r.axis_v());
  ASSERT_NEAR(dot, 0.0, 1e-9);
}

TEST_F(BRect2DTest, Accessors_WidthHeightArea) {
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {6.0, 0.0}, {6.0, 3.0}, {0.0, 3.0}};
  auto r = g::BRect2D(pts);
  ASSERT_NEAR(r.width(), 6.0, 1e-9);
  ASSERT_NEAR(r.height(), 3.0, 1e-9);
  ASSERT_NEAR(r.area(), 18.0, 1e-9);
}

// ── Corners ──────────────────────────────────────────────────────────────────

TEST_F(BRect2DTest, Corners_FourDistinctPoints) {
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r = g::BRect2D(pts);
  auto corners = r.Corners();
  ASSERT_EQ(corners.size(), 4u);
  // all four corners must be contained in the rectangle
  for (auto const& c : corners)
    ASSERT_TRUE(r.Contains(c));
}

// ── Contains ─────────────────────────────────────────────────────────────────

TEST_F(BRect2DTest, Contains_Center_True) {
  geompp::DECIMAL_PRECISION = 6;
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r = g::BRect2D(pts);
  ASSERT_TRUE(r.Contains(g::Point2D(2.0, 1.0)));   // center
}

TEST_F(BRect2DTest, Contains_Interior_True) {
  geompp::DECIMAL_PRECISION = 6;
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r = g::BRect2D(pts);
  ASSERT_TRUE(r.Contains(g::Point2D(1.0, 0.5)));
  ASSERT_TRUE(r.Contains(g::Point2D(3.5, 1.8)));
}

TEST_F(BRect2DTest, Contains_Boundary_True) {
  geompp::DECIMAL_PRECISION = 6;
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r = g::BRect2D(pts);
  ASSERT_TRUE(r.Contains(g::Point2D(0.0, 0.0)));   // corner
  ASSERT_TRUE(r.Contains(g::Point2D(4.0, 1.0)));   // edge midpoint
}

TEST_F(BRect2DTest, Contains_Outside_False) {
  geompp::DECIMAL_PRECISION = 6;
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r = g::BRect2D(pts);
  ASSERT_FALSE(r.Contains(g::Point2D(5.0, 1.0)));   // beyond u-axis
  ASSERT_FALSE(r.Contains(g::Point2D(2.0, 3.0)));   // beyond v-axis
  ASSERT_FALSE(r.Contains(g::Point2D(-1.0, 1.0)));  // negative u
}

// ── AlmostEquals / operator== ─────────────────────────────────────────────────

TEST_F(BRect2DTest, AlmostEquals_SameRect) {
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r1 = g::BRect2D(pts);
  auto r2 = g::BRect2D(pts);
  ASSERT_TRUE(r1.AlmostEquals(r2));
  ASSERT_EQ(r1, r2);
}

TEST_F(BRect2DTest, AlmostEquals_DifferentRect) {
  std::vector<g::Point2D> pts1 = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  std::vector<g::Point2D> pts2 = {{0.0, 0.0}, {6.0, 0.0}, {6.0, 2.0}, {0.0, 2.0}};
  auto r1 = g::BRect2D(pts1);
  auto r2 = g::BRect2D(pts2);
  ASSERT_FALSE(r1.AlmostEquals(r2));
  ASSERT_NE(r1, r2);
}

// ── Copy / Assignment ─────────────────────────────────────────────────────────

TEST_F(BRect2DTest, CopyConstructor) {
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  auto r1 = g::BRect2D(pts);
  auto r2 = r1;
  ASSERT_TRUE(r1.AlmostEquals(r2));
}

TEST_F(BRect2DTest, Assignment) {
  std::vector<g::Point2D> pts1 = {{0.0, 0.0}, {4.0, 0.0}, {4.0, 2.0}, {0.0, 2.0}};
  std::vector<g::Point2D> pts2 = {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}};
  auto r1 = g::BRect2D(pts1);
  auto r2 = g::BRect2D(pts2);
  r2 = r1;
  ASSERT_TRUE(r2.AlmostEquals(r1));
  r1 = r1;  // self-assignment
  ASSERT_TRUE(r1.AlmostEquals(g::BRect2D(pts1)));
}

}  // namespace geompp_tests
