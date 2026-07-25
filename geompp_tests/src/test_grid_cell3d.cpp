#include "grid_cell3d.hpp"

#include "point3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class GridCell3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(GridCell3DTest, FromPoint_QuantizesByFloorDivision) {
  auto cell = g::GridCell3D::FromPoint(g::Point3D(2.5, 3.5, 4.5), 1.0);
  EXPECT_EQ(2, cell.x);
  EXPECT_EQ(3, cell.y);
  EXPECT_EQ(4, cell.z);
}

TEST_F(GridCell3DTest, FromPoint_NegativeCoordinates_FloorsTowardNegativeInfinity) {
  auto cell = g::GridCell3D::FromPoint(g::Point3D(-0.5, -1.5, -2.5), 1.0);
  EXPECT_EQ(-1, cell.x);
  EXPECT_EQ(-2, cell.y);
  EXPECT_EQ(-3, cell.z);
}

TEST_F(GridCell3DTest, FromPoint_SameCell_IdenticalPoints) {
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(1.0, 1.0, 1.0), 0.1);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(1.0, 1.0, 1.0), 0.1);
  EXPECT_TRUE(c1 == c2);
}

TEST_F(GridCell3DTest, FromPoint_NearbyPointsWithinSameCell_Merge) {
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(10.1, 10.1, 10.1), 1.0);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(10.9, 10.9, 10.9), 1.0);
  EXPECT_TRUE(c1 == c2);
}

TEST_F(GridCell3DTest, FromPoint_DistantPoints_DontMerge) {
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(0.0, 0.0, 0.0), 1.0);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(100.0, 100.0, 100.0), 1.0);
  EXPECT_FALSE(c1 == c2);
}

TEST_F(GridCell3DTest, FromPoint_BoundaryStraddle_KnownLimitation) {
  // Documented limitation: two points closer together than epsilon can still land in
  // different cells if they straddle a cell boundary along any axis.
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(0.99, 0.0, 0.0), 1.0);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(1.01, 0.0, 0.0), 1.0);
  EXPECT_FALSE(c1 == c2) << "points 0.02 apart straddle the x=1.0 cell boundary at epsilon=1.0";
}

TEST_F(GridCell3DTest, FromPoint_DefaultEpsilon_TracksDecimalPrecision) {
  g::DECIMAL_PRECISION = 1;  // epsilon = 0.1
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(0.0, 0.0, 0.0));
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(0.05, 0.0, 0.0));
  EXPECT_TRUE(c1 == c2);  // both within the same 0.1-wide cell
}

}  // namespace geompp_tests
