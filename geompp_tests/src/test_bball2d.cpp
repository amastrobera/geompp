#include "bball2d.hpp"
#include "point2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class BBall2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(BBall2DTest, ConstructorCenterRadius) {
  auto b = g::BBall2D(g::Point2D(1.0, 2.0), 5.0);
  ASSERT_TRUE(b.center().AlmostEquals(g::Point2D(1.0, 2.0)));
  ASSERT_NEAR(b.radius(), 5.0, 1e-9);
}

TEST_F(BBall2DTest, ConstructorFromSinglePoint) {
  auto b = g::BBall2D(std::vector<g::Point2D>{{3.0, 4.0}});
  ASSERT_TRUE(b.center().AlmostEquals(g::Point2D(3.0, 4.0)));
  ASSERT_NEAR(b.radius(), 0.0, 1e-9);
}

TEST_F(BBall2DTest, ConstructorFromTwoPoints) {
  auto b = g::BBall2D(std::vector<g::Point2D>{{0.0, 0.0}, {4.0, 0.0}});
  ASSERT_TRUE(b.center().AlmostEquals(g::Point2D(2.0, 0.0)));
  ASSERT_NEAR(b.radius(), 2.0, 1e-9);
}

TEST_F(BBall2DTest, ConstructorFromPointsAllContained) {
  std::vector<g::Point2D> pts = {{0.0, 0.0}, {4.0, 0.0}, {2.0, 3.0}, {-1.0, 1.5}};
  auto b = g::BBall2D(pts);
  for (auto const& p : pts) {
    ASSERT_TRUE(b.Contains(p));
  }
}

TEST_F(BBall2DTest, ConstructorEmptyThrows) {
  ASSERT_THROW(g::BBall2D(std::vector<g::Point2D>{}), std::invalid_argument);
}

TEST_F(BBall2DTest, CopyConstructor) {
  auto b1 = g::BBall2D(g::Point2D(1.0, 2.0), 3.0);
  auto b2 = b1;
  ASSERT_TRUE(b1.AlmostEquals(b2));
}

TEST_F(BBall2DTest, Assignment) {
  auto b1 = g::BBall2D(g::Point2D(1.0, 2.0), 3.0);
  auto b2 = g::BBall2D(g::Point2D(0.0, 0.0), 1.0);
  b2 = b1;
  ASSERT_TRUE(b1.AlmostEquals(b2));
  b1 = b1;
  ASSERT_TRUE(b1.AlmostEquals(g::BBall2D(g::Point2D(1.0, 2.0), 3.0)));
}

TEST_F(BBall2DTest, AlmostEquals) {
  auto b1 = g::BBall2D(g::Point2D(1.0, 2.0), 3.0);
  auto b2 = g::BBall2D(g::Point2D(1.0, 2.0), 3.0);
  auto b3 = g::BBall2D(g::Point2D(0.0, 0.0), 1.0);
  ASSERT_TRUE(b1.AlmostEquals(b2));
  ASSERT_FALSE(b1.AlmostEquals(b3));
  ASSERT_EQ(b1, b2);
  ASSERT_NE(b1, b3);
}

TEST_F(BBall2DTest, Contains) {
  geompp::DECIMAL_PRECISION = 6;
  auto b = g::BBall2D(g::Point2D(0.0, 0.0), 5.0);

  ASSERT_TRUE(b.Contains(g::Point2D(0.0, 0.0)));    // center
  ASSERT_TRUE(b.Contains(g::Point2D(3.0, 4.0)));    // boundary (3-4-5)
  ASSERT_TRUE(b.Contains(g::Point2D(5.0, 0.0)));    // boundary
  ASSERT_TRUE(b.Contains(g::Point2D(0.0, -5.0)));   // boundary
  ASSERT_TRUE(b.Contains(g::Point2D(1.0, 1.0)));    // interior

  ASSERT_FALSE(b.Contains(g::Point2D(4.0, 4.0)));   // dist ≈ 5.657
  ASSERT_FALSE(b.Contains(g::Point2D(6.0, 0.0)));   // dist = 6
}

}  // namespace geompp_tests
