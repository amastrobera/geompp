#include "calc_utils3d.hpp"

#include "constants.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class CalcUtils3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(CalcUtils3DTest, DistanceLineToLine_Intersecting) {
  // X-axis: (0,0,0) → (1,0,0)
  // Vertical line in XY at x=2: (2,-1,0) → (2,1,0)
  // They meet at (2,0,0): sc=2 (since L1 has length 1), tc=0.5 (midpoint of L2)
  double sc, tc;
  g::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(2, -1, 0), g::Point3D(2, 1, 0), sc, tc);

  EXPECT_EQ(2.0, g::round(sc));
  EXPECT_EQ(0.5, g::round(tc));
}

TEST_F(CalcUtils3DTest, DistanceLineToLine_Skew) {
  // L1 along X-axis at z=0: (0,0,0) → (1,0,0)
  // L2 along Y-axis at z=5: (0,-1,5) → (0,1,5)
  // Closest approach: (0,0,0) and (0,0,5); sc=0, tc=0.5
  double sc, tc;
  g::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, -1, 5), g::Point3D(0, 1, 5), sc, tc);

  EXPECT_EQ(0.0, g::round(sc));
  EXPECT_EQ(0.5, g::round(tc));
}

TEST_F(CalcUtils3DTest, DistanceLineToLine_ParallelDistinct) {
  // Two parallel lines along X, offset by 3 in Y. Determinant is zero → parallel branch.
  // sc=0 by convention; tc places L2's closest point at the perpendicular foot from L1_P0.
  double sc, tc;
  g::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 3, 0), g::Point3D(1, 3, 0), sc, tc);

  EXPECT_EQ(0.0, g::round(sc));
  // foot of P0 onto L2 is L2_P0 itself ⇒ tc=0
  EXPECT_EQ(0.0, g::round(tc));

  // verify the resulting distance is 3 by reconstructing the closest points
  auto U = g::Point3D(1, 0, 0) - g::Point3D(0, 0, 0);
  auto V = g::Point3D(1, 3, 0) - g::Point3D(0, 3, 0);
  auto P = g::Point3D(0, 0, 0) + (U * sc);
  auto Q = g::Point3D(0, 3, 0) + (V * tc);
  EXPECT_EQ(3.0, g::round(P.DistanceTo(Q)));
}

TEST_F(CalcUtils3DTest, DistanceLineToLine_Overlap) {
  // Two collinear lines along X-axis. Parallel branch should return zero distance.
  double sc, tc;
  g::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(5, 0, 0), g::Point3D(7, 0, 0), sc, tc);

  // sc=0 by convention; reconstructed closest points should coincide
  auto U = g::Point3D(1, 0, 0) - g::Point3D(0, 0, 0);
  auto V = g::Point3D(7, 0, 0) - g::Point3D(5, 0, 0);
  auto P = g::Point3D(0, 0, 0) + (U * sc);
  auto Q = g::Point3D(5, 0, 0) + (V * tc);
  EXPECT_EQ(0.0, g::round(P.DistanceTo(Q)));
}

TEST_F(CalcUtils3DTest, DistanceLineToLine_ZeroLengthInputs) {
  double sc, tc;
  // L1 is zero-length
  EXPECT_ANY_THROW(g::distance_line_to_line(g::Point3D(1, 2, 3), g::Point3D(1, 2, 3), g::Point3D(0, 0, 0),
                                            g::Point3D(1, 0, 0), sc, tc));
  // L2 is zero-length
  EXPECT_ANY_THROW(g::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(5, 5, 5),
                                            g::Point3D(5, 5, 5), sc, tc));
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_Intersecting) {
  // L1 along X (0..1); L2 vertical in XY at x=2 (2,-1,0)→(2,1,0). Meet at (2,0,0).
  double sc, tc;
  auto pt = g::intersection_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(2, -1, 0),
                                         g::Point3D(2, 1, 0), sc, tc);

  ASSERT_TRUE(pt.has_value());
  EXPECT_EQ(g::Point3D(2, 0, 0), *pt);
  EXPECT_EQ(2.0, g::round(sc));
  EXPECT_EQ(0.5, g::round(tc));
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_SkewLinesNoIntersection) {
  // L1 along X at z=0; L2 along Y at z=5 → skew, perpendicular distance 5, no intersection point.
  double sc, tc;
  auto pt = g::intersection_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, -1, 5),
                                         g::Point3D(0, 1, 5), sc, tc);

  EXPECT_FALSE(pt.has_value());
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_ParallelDistinctNoIntersection) {
  // Two parallel lines along X offset by 3 in Y → determinant zero → nullopt.
  double sc, tc;
  auto pt = g::intersection_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 3, 0),
                                         g::Point3D(1, 3, 0), sc, tc);

  EXPECT_FALSE(pt.has_value());
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_OverlapNoIntersection) {
  // Collinear lines along X → determinant zero → nullopt (treated as parallel, no unique point).
  double sc, tc;
  auto pt = g::intersection_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(3, 0, 0),
                                         g::Point3D(5, 0, 0), sc, tc);

  EXPECT_FALSE(pt.has_value());
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_IntersectAtEndpointParams) {
  // L1 (0..2) along X, L2 from (0,-3,0) to (0,1,0). Cross at origin (sc=0 on L1, tc=0.75 on L2).
  double sc, tc;
  auto pt = g::intersection_line_to_line(g::Point3D(0, 0, 0), g::Point3D(2, 0, 0), g::Point3D(0, -3, 0),
                                         g::Point3D(0, 1, 0), sc, tc);

  ASSERT_TRUE(pt.has_value());
  EXPECT_EQ(g::Point3D(0, 0, 0), *pt);
  EXPECT_EQ(0.0, g::round(sc));
  EXPECT_EQ(0.75, g::round(tc));
}

}  // namespace geompp_tests
