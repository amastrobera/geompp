#include "calc_utils3d.hpp"

#include "constants.hpp"
#include "line3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <gtest/gtest.h>

namespace g  = geompp;
namespace gd = geompp::detail;

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
  gd::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(2, -1, 0), g::Point3D(2, 1, 0), sc, tc);

  EXPECT_EQ(2.0, g::round(sc));
  EXPECT_EQ(0.5, g::round(tc));
}

TEST_F(CalcUtils3DTest, DistanceLineToLine_Skew) {
  // L1 along X-axis at z=0: (0,0,0) → (1,0,0)
  // L2 along Y-axis at z=5: (0,-1,5) → (0,1,5)
  // Closest approach: (0,0,0) and (0,0,5); sc=0, tc=0.5
  double sc, tc;
  gd::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, -1, 5), g::Point3D(0, 1, 5), sc, tc);

  EXPECT_EQ(0.0, g::round(sc));
  EXPECT_EQ(0.5, g::round(tc));
}

TEST_F(CalcUtils3DTest, DistanceLineToLine_ParallelDistinct) {
  // Two parallel lines along X, offset by 3 in Y. Determinant is zero → parallel branch.
  // sc=0 by convention; tc places L2's closest point at the perpendicular foot from L1_P0.
  double sc, tc;
  gd::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 3, 0), g::Point3D(1, 3, 0), sc, tc);

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
  gd::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(5, 0, 0), g::Point3D(7, 0, 0), sc, tc);

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
  EXPECT_ANY_THROW(gd::distance_line_to_line(g::Point3D(1, 2, 3), g::Point3D(1, 2, 3), g::Point3D(0, 0, 0),
                                            g::Point3D(1, 0, 0), sc, tc));
  // L2 is zero-length
  EXPECT_ANY_THROW(gd::distance_line_to_line(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(5, 5, 5),
                                            g::Point3D(5, 5, 5), sc, tc));
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_Intersecting) {
  // L1 along X (0..1); L2 vertical in XY at x=2 (2,-1,0)→(2,1,0). Meet at (2,0,0).
  double sc, tc;
  auto pt = gd::line_intersection(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(2, -1, 0),
                                         g::Point3D(2, 1, 0), sc, tc);

  ASSERT_TRUE(pt.has_value());
  EXPECT_EQ(g::Point3D(2, 0, 0), *pt);
  EXPECT_EQ(2.0, g::round(sc));
  EXPECT_EQ(0.5, g::round(tc));
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_SkewLinesNoIntersection) {
  // L1 along X at z=0; L2 along Y at z=5 → skew, perpendicular distance 5, no intersection point.
  double sc, tc;
  auto pt = gd::line_intersection(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, -1, 5),
                                         g::Point3D(0, 1, 5), sc, tc);

  EXPECT_FALSE(pt.has_value());
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_ParallelDistinctNoIntersection) {
  // Two parallel lines along X offset by 3 in Y → determinant zero → nullopt.
  double sc, tc;
  auto pt = gd::line_intersection(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 3, 0),
                                         g::Point3D(1, 3, 0), sc, tc);

  EXPECT_FALSE(pt.has_value());
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_OverlapNoIntersection) {
  // Collinear lines along X → determinant zero → nullopt (treated as parallel, no unique point).
  double sc, tc;
  auto pt = gd::line_intersection(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(3, 0, 0),
                                         g::Point3D(5, 0, 0), sc, tc);

  EXPECT_FALSE(pt.has_value());
}

TEST_F(CalcUtils3DTest, IntersectionLineToLine_IntersectAtEndpointParams) {
  // L1 (0..2) along X, L2 from (0,-3,0) to (0,1,0). Cross at origin (sc=0 on L1, tc=0.75 on L2).
  double sc, tc;
  auto pt = gd::line_intersection(g::Point3D(0, 0, 0), g::Point3D(2, 0, 0), g::Point3D(0, -3, 0),
                                         g::Point3D(0, 1, 0), sc, tc);

  ASSERT_TRUE(pt.has_value());
  EXPECT_EQ(g::Point3D(0, 0, 0), *pt);
  EXPECT_EQ(0.0, g::round(sc));
  EXPECT_EQ(0.75, g::round(tc));
}

// ---- principal_axes / principal_normal / principal_direction ----------------

TEST_F(CalcUtils3DTest, PrincipalAxes_PlanarXYCloud_ZIsNormal) {
  // Flat cloud in the XY plane — the normal (Z) must be nearly (0, 0, ±1)
  std::vector<g::Point3D> cloud = {
      {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
      {0, 0.1, 0}, {1, 0.1, 0}, {2, 0.1, 0}, {3, 0.1, 0}};
  auto frame = g::principal_axes(cloud);
  EXPECT_NEAR(0.0, frame.Z.x(), 1e-3);
  EXPECT_NEAR(0.0, frame.Z.y(), 1e-3);
  EXPECT_NEAR(1.0, std::abs(frame.Z.z()), 1e-3);
}

TEST_F(CalcUtils3DTest, PrincipalAxes_ElongatedAlongX_XIsLongest) {
  // Cloud spread mainly along X — primary direction (X) must be nearly (±1, 0, 0)
  std::vector<g::Point3D> cloud = {
      {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
      {0, 0.1, 0}, {1, 0.1, 0}, {2, 0.1, 0}, {3, 0.1, 0}};
  auto frame = g::principal_axes(cloud);
  EXPECT_NEAR(1.0, std::abs(frame.X.x()), 1e-3);
  EXPECT_NEAR(0.0, frame.X.y(), 1e-3);
  EXPECT_NEAR(0.0, frame.X.z(), 1e-3);
}

TEST_F(CalcUtils3DTest, PrincipalAxes_AxesAreOrthogonal) {
  std::vector<g::Point3D> cloud = {
      {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
      {0, 0.1, 0}, {1, 0.1, 0}, {2, 0.1, 0}, {3, 0.1, 0}};
  auto frame = g::principal_axes(cloud);
  EXPECT_NEAR(0.0, frame.X.Dot(frame.Y), 1e-9);
  EXPECT_NEAR(0.0, frame.X.Dot(frame.Z), 1e-9);
  EXPECT_NEAR(0.0, frame.Y.Dot(frame.Z), 1e-9);
}

TEST_F(CalcUtils3DTest, PrincipalAxes_AxesAreUnitVectors) {
  std::vector<g::Point3D> cloud = {
      {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
      {0, 0.1, 0}, {1, 0.1, 0}, {2, 0.1, 0}, {3, 0.1, 0}};
  auto frame = g::principal_axes(cloud);
  EXPECT_NEAR(1.0, frame.X.Length(), 1e-9);
  EXPECT_NEAR(1.0, frame.Y.Length(), 1e-9);
  EXPECT_NEAR(1.0, frame.Z.Length(), 1e-9);
}

TEST_F(CalcUtils3DTest, PrincipalNormal_PlanarCloud_MatchesBasisZ) {
  // Flat XY cloud: principal_normal delegates to principal_axes().Z
  std::vector<g::Point3D> cloud = {
      {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
      {0, 0.1, 0}, {1, 0.1, 0}, {2, 0.1, 0}, {3, 0.1, 0}};
  auto n = g::principal_normal(cloud);
  EXPECT_NEAR(0.0, n.x(), 1e-3);
  EXPECT_NEAR(0.0, n.y(), 1e-3);
  EXPECT_NEAR(1.0, std::abs(n.z()), 1e-3);
}

TEST_F(CalcUtils3DTest, PrincipalDirection_ElongatedAlongX_MatchesBasisX) {
  // Elongated cloud: principal_direction delegates to principal_axes().X
  std::vector<g::Point3D> cloud = {
      {0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 0, 0},
      {0, 0.1, 0}, {1, 0.1, 0}, {2, 0.1, 0}, {3, 0.1, 0}};
  auto d = g::principal_direction(cloud);
  EXPECT_NEAR(1.0, std::abs(d.x()), 1e-3);
  EXPECT_NEAR(0.0, d.y(), 1e-3);
  EXPECT_NEAR(0.0, d.z(), 1e-3);
}

TEST_F(CalcUtils3DTest, PrincipalAxes_TooFewPoints_Throws) {
  // principal_axes requires at least 3 points
  std::vector<g::Point3D> two_pts = {{0, 0, 0}, {1, 0, 0}};
  EXPECT_THROW(g::principal_axes(two_pts), std::runtime_error);
}

// ---- find_extreme_points (Polygon3D × Line3D) -------------------------------

TEST_F(CalcUtils3DTest, ExtremePoints_ConvexDiamondXY_AlongX) {
  // Planar CCW diamond in the XY plane; projecting onto +X isolates the left/right tips.
  auto diamond = g::Polygon3D::Make(
      {g::Point3D(2, 0, 0), g::Point3D(4, 2, 0), g::Point3D(2, 4, 0), g::Point3D(0, 2, 0)});
  ASSERT_TRUE(diamond.IsConvex());  // exercises the O(log n) Sunday binary search
  auto ex = g::find_extreme_points(diamond, g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0)));
  EXPECT_EQ(g::Point3D(0, 2, 0), ex.min_point);
  EXPECT_EQ(g::Point3D(4, 2, 0), ex.max_point);
}

TEST_F(CalcUtils3DTest, ExtremePoints_ConvexTiltedPlane) {
  // A convex parallelogram in the tilted plane x = z (normal ~ (-1,0,1)), CCW w.r.t. that normal.
  // The projection-based search is dimension-agnostic, so the convex path drives 3D too.
  auto para = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(2, 0, 2), g::Point3D(2, 2, 2), g::Point3D(0, 2, 0)});
  ASSERT_TRUE(para.IsConvex());
  // direction (1,1,0): proj = x + y → min at (0,0,0)=0, max at (2,2,2)=4
  auto ex = g::find_extreme_points(para, g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0)));
  EXPECT_EQ(g::Point3D(0, 0, 0), ex.min_point);
  EXPECT_EQ(g::Point3D(2, 2, 2), ex.max_point);
}

TEST_F(CalcUtils3DTest, ExtremePoints_ConcavePolygon_BruteForcePath) {
  // Non-convex planar "dart" in XY → O(n) linear scan.
  auto dart = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0),
                                  g::Point3D(2, 1, 0), g::Point3D(0, 4, 0)});
  ASSERT_FALSE(dart.IsConvex());
  // direction (1,2,0): proj = x + 2y → min at (0,0,0)=0, max at (4,4,0)=12
  auto ex = g::find_extreme_points(dart, g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 2, 0)));
  EXPECT_EQ(g::Point3D(0, 0, 0), ex.min_point);
  EXPECT_EQ(g::Point3D(4, 4, 0), ex.max_point);
}

}  // namespace geompp_tests
