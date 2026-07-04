#include "bball3d.hpp"
#include "point3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class BBall3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(BBall3DTest, ConstructorCenterRadius) {
  auto b = g::BBall3D(g::Point3D(1.0, 2.0, 3.0), 5.0);
  ASSERT_TRUE(b.center().AlmostEquals(g::Point3D(1.0, 2.0, 3.0)));
  ASSERT_NEAR(b.radius(), 5.0, 1e-9);
}

TEST_F(BBall3DTest, ConstructorFromSinglePoint) {
  auto b = g::BBall3D(std::vector<g::Point3D>{{1.0, 2.0, 3.0}});
  ASSERT_TRUE(b.center().AlmostEquals(g::Point3D(1.0, 2.0, 3.0)));
  ASSERT_NEAR(b.radius(), 0.0, 1e-9);
}

TEST_F(BBall3DTest, ConstructorFromTwoPoints) {
  auto b = g::BBall3D(std::vector<g::Point3D>{{0.0, 0.0, 0.0}, {4.0, 0.0, 0.0}});
  ASSERT_TRUE(b.center().AlmostEquals(g::Point3D(2.0, 0.0, 0.0)));
  ASSERT_NEAR(b.radius(), 2.0, 1e-9);
}

TEST_F(BBall3DTest, ConstructorFromTwoPointsAlongZ) {
  auto b = g::BBall3D(std::vector<g::Point3D>{{0.0, 0.0, -3.0}, {0.0, 0.0, 3.0}});
  ASSERT_TRUE(b.center().AlmostEquals(g::Point3D(0.0, 0.0, 0.0)));
  ASSERT_NEAR(b.radius(), 3.0, 1e-9);
}

TEST_F(BBall3DTest, ConstructorFromPointsAllContained) {
  std::vector<g::Point3D> pts = {
      {0.0, 0.0, 0.0}, {4.0, 0.0, 0.0}, {0.0, 3.0, 0.0}, {0.0, 0.0, 2.0}, {1.0, 1.0, 1.0}};
  auto b = g::BBall3D(pts);
  for (auto const& p : pts) {
    ASSERT_TRUE(b.Contains(p));
  }
}

TEST_F(BBall3DTest, ConstructorEmptyThrows) {
  ASSERT_THROW(g::BBall3D(std::vector<g::Point3D>{}), std::invalid_argument);
}

TEST_F(BBall3DTest, CopyConstructor) {
  auto b1 = g::BBall3D(g::Point3D(1.0, 2.0, 3.0), 4.0);
  auto b2 = b1;
  ASSERT_TRUE(b1.AlmostEquals(b2));
}

TEST_F(BBall3DTest, Assignment) {
  auto b1 = g::BBall3D(g::Point3D(1.0, 2.0, 3.0), 4.0);
  auto b2 = g::BBall3D(g::Point3D(0.0, 0.0, 0.0), 1.0);
  b2 = b1;
  ASSERT_TRUE(b1.AlmostEquals(b2));
  b1 = b1;
  ASSERT_TRUE(b1.AlmostEquals(g::BBall3D(g::Point3D(1.0, 2.0, 3.0), 4.0)));
}

TEST_F(BBall3DTest, AlmostEquals) {
  auto b1 = g::BBall3D(g::Point3D(1.0, 2.0, 3.0), 4.0);
  auto b2 = g::BBall3D(g::Point3D(1.0, 2.0, 3.0), 4.0);
  auto b3 = g::BBall3D(g::Point3D(0.0, 0.0, 0.0), 1.0);
  ASSERT_TRUE(b1.AlmostEquals(b2));
  ASSERT_FALSE(b1.AlmostEquals(b3));
  ASSERT_EQ(b1, b2);
  ASSERT_NE(b1, b3);
}

TEST_F(BBall3DTest, Contains) {
  geompp::DECIMAL_PRECISION = 6;
  auto b = g::BBall3D(g::Point3D(0.0, 0.0, 0.0), 5.0);

  ASSERT_TRUE(b.Contains(g::Point3D(0.0, 0.0, 0.0)));   // center
  ASSERT_TRUE(b.Contains(g::Point3D(3.0, 4.0, 0.0)));   // boundary (3-4-5 in XY)
  ASSERT_TRUE(b.Contains(g::Point3D(0.0, 0.0, 5.0)));   // boundary along Z
  ASSERT_TRUE(b.Contains(g::Point3D(1.0, 1.0, 1.0)));   // interior

  ASSERT_FALSE(b.Contains(g::Point3D(4.0, 4.0, 0.0)));  // dist ≈ 5.657
  ASSERT_FALSE(b.Contains(g::Point3D(0.0, 0.0, 6.0)));  // dist = 6
}

}  // namespace geompp_tests
