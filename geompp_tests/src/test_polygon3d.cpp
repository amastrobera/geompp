#include "polygon3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Polygon3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Polygon3DTest, Constructor) {
  auto p = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  ASSERT_EQ(4, p.Size());

  ASSERT_EQ(g::Point3D::Zero(), p[0]);
  ASSERT_EQ(g::Point3D(1, 0, 0), p[1]);
  ASSERT_EQ(g::Point3D(1, 1, 0), p[2]);
  ASSERT_EQ(g::Point3D(0, 1, 0), p[3]);

  EXPECT_ANY_THROW(p[4]);  // out of range

  // fewer than 3 unique points throws
  EXPECT_ANY_THROW(g::Polygon3D::Make({}));
  EXPECT_ANY_THROW(g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D::Zero(), g::Point3D::Zero()}));
  EXPECT_ANY_THROW(g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0)}));

  // CW outer ring throws
  EXPECT_ANY_THROW(g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(1, 1, 0), g::Point3D(1, 0, 0)}));

  // non-coplanar points throw
  EXPECT_ANY_THROW(g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 1), g::Point3D(1, 1, 2)}));
}

TEST_F(Polygon3DTest, AlmostEquals) {
  auto p1 = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)});
  auto p2 = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)});
  auto p3 = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(2, 2, 0)});

  ASSERT_TRUE(p1.AlmostEquals(p2));
  ASSERT_FALSE(p1.AlmostEquals(p3));
  ASSERT_EQ(p1, p2);
  ASSERT_NE(p1, p3);

  // different sizes are not equal
  auto p4 = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  ASSERT_FALSE(p1.AlmostEquals(p4));
}

TEST_F(Polygon3DTest, Assignment) {
  auto p1 = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)});
  auto p2 = g::Polygon3D::Make({g::Point3D(5, 5, 5), g::Point3D(6, 5, 5), g::Point3D(6, 6, 5)});
  p2 = p1;
  ASSERT_EQ(p1, p2);
}

TEST_F(Polygon3DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto p = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  ASSERT_EQ("POLYGON ((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))", p.ToWkt());

  // CCW triangle on XY plane at DP=2
  geompp::DECIMAL_PRECISION = 2;
  auto p2 = g::Polygon3D::Make(
      {g::Point3D(0.123, 0.456, 0), g::Point3D(8.789, 0.123, 0), g::Point3D(4.321, 7.654, 0)});
  ASSERT_EQ("POLYGON ((0.12 0.46 0, 8.79 0.12 0, 4.32 7.65 0, 0.12 0.46 0))", p2.ToWkt());

  // FromWkt round-trips correctly
  geompp::DECIMAL_PRECISION = 4;
  auto q = g::Polygon3D::FromWkt("POLYGON ((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))");
  EXPECT_EQ(4, q.Size());
  EXPECT_EQ(g::Point3D(0, 0, 0), q[0]);
  EXPECT_EQ(g::Point3D(1, 0, 0), q[1]);
  // whitespace tolerance
  auto qw = g::Polygon3D::FromWkt("POLYGON ((  0 0 0  ,  1 0 0  ,  1 1 0  ,  0 1 0  ,  0 0 0  ))");
  EXPECT_EQ(4, qw.Size());
  EXPECT_EQ(g::Point3D(0, 0, 0), qw[0]);
  EXPECT_EQ(g::Point3D(1, 0, 0), qw[1]);
  // invalid input still throws
  EXPECT_ANY_THROW(g::Polygon3D::FromWkt("anything"));
}

TEST_F(Polygon3DTest, WktWithHoles) {
  geompp::DECIMAL_PRECISION = 4;
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  ASSERT_EQ("POLYGON ((0 0 0, 4 0 0, 4 4 0, 0 4 0, 0 0 0), (1 1 0, 1 3 0, 3 3 0, 3 1 0, 1 1 0))", p.ToWkt());
}

TEST_F(Polygon3DTest, WithHoles_Valid) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  ASSERT_EQ(4, p.Size());  // outer ring has 4 vertices
}

TEST_F(Polygon3DTest, HasHoles_False_WhenNoHoles) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  auto p = g::Polygon3D::Make(outer);
  EXPECT_FALSE(p.HasHoles());
  EXPECT_TRUE(p.Holes().empty());
}

TEST_F(Polygon3DTest, HasHoles_True_WithHole) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  EXPECT_TRUE(p.HasHoles());
  ASSERT_EQ(1u, p.Holes().size());
  ASSERT_EQ(4u, p.Holes()[0].size());
  EXPECT_TRUE(p.Holes()[0][0].AlmostEquals(g::Point3D(1, 1, 0)));
  EXPECT_TRUE(p.Holes()[0][2].AlmostEquals(g::Point3D(3, 3, 0)));
}

TEST_F(Polygon3DTest, WithHoles_PerimeterCW_Throws) {
  std::vector<g::Point3D> cw_outer = {
      g::Point3D(0, 0, 0), g::Point3D(0, 4, 0), g::Point3D(4, 4, 0), g::Point3D(4, 0, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)};
  EXPECT_ANY_THROW(g::Polygon3D::Make(cw_outer, {hole}));
}

TEST_F(Polygon3DTest, WithHoles_HoleCCW_Throws) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> ccw_hole = {
      g::Point3D(1, 1, 0), g::Point3D(3, 1, 0), g::Point3D(3, 3, 0), g::Point3D(1, 3, 0)};
  EXPECT_ANY_THROW(g::Polygon3D::Make(outer, {ccw_hole}));
}

TEST_F(Polygon3DTest, WithHoles_HoleTooFewPoints_Throws) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> bad_hole = {g::Point3D(1, 1, 0), g::Point3D(2, 1, 0)};
  EXPECT_ANY_THROW(g::Polygon3D::Make(outer, {bad_hole}));
}

TEST_F(Polygon3DTest, WithHoles_NonCoplanar_Throws) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  // hole not on the same plane
  std::vector<g::Point3D> non_coplanar_hole = {
      g::Point3D(1, 1, 1), g::Point3D(1, 3, 2), g::Point3D(3, 3, 3)};
  EXPECT_ANY_THROW(g::Polygon3D::Make(outer, {non_coplanar_hole}));
}

TEST_F(Polygon3DTest, WithHoles_CoplanarHole_NoThrow) {
  // Polygon on the YZ plane (x=0).  BasisX is dominant, so AxisU=BasisY, AxisV=BasisZ.
  // Outer ring: (0,0)→(4,0)→(4,4)→(0,4) in projected space — CCW (2A=+32).
  // Hole: (1,1)→(1,3)→(3,3)→(3,1) in projected space — CW (2A=−8).
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(0, 4, 0), g::Point3D(0, 4, 4), g::Point3D(0, 0, 4)};
  std::vector<g::Point3D> coplanar_hole = {
      g::Point3D(0, 1, 1), g::Point3D(0, 1, 3), g::Point3D(0, 3, 3), g::Point3D(0, 3, 1)};
  EXPECT_NO_THROW(g::Polygon3D::Make(outer, {coplanar_hole}));
}

TEST_F(Polygon3DTest, WithHoles_SelfIntersectingHole_Throws) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(6, 0, 0), g::Point3D(6, 6, 0), g::Point3D(0, 6, 0)};
  // Bowtie hole: (1,1)->(3,1)->(1,3)->(3,3) crosses itself.
  std::vector<g::Point3D> bowtie_hole = {
      g::Point3D(1, 1, 0), g::Point3D(3, 1, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)};
  EXPECT_ANY_THROW(g::Polygon3D::Make(outer, {bowtie_hole}));
}

TEST_F(Polygon3DTest, WithHoles_TwoHolesOverlap_Throws) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(10, 0, 0), g::Point3D(10, 10, 0), g::Point3D(0, 10, 0)};
  std::vector<g::Point3D> hole_a = {
      g::Point3D(1, 1, 0), g::Point3D(1, 5, 0), g::Point3D(5, 5, 0), g::Point3D(5, 1, 0)};
  std::vector<g::Point3D> hole_b = {
      g::Point3D(3, 3, 0), g::Point3D(3, 7, 0), g::Point3D(7, 7, 0), g::Point3D(7, 3, 0)};  // overlaps hole_a
  EXPECT_ANY_THROW(g::Polygon3D::Make(outer, {hole_a, hole_b}));
}

TEST_F(Polygon3DTest, WithHoles_TwoHolesTouchAtVertex_DoesNotThrow) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(10, 0, 0), g::Point3D(10, 10, 0), g::Point3D(0, 10, 0)};
  // Two squares touching diagonally at the single shared corner (5,5,0), no shared edge.
  std::vector<g::Point3D> hole_a = {
      g::Point3D(3, 3, 0), g::Point3D(3, 5, 0), g::Point3D(5, 5, 0), g::Point3D(5, 3, 0)};
  std::vector<g::Point3D> hole_b = {
      g::Point3D(5, 5, 0), g::Point3D(5, 7, 0), g::Point3D(7, 7, 0), g::Point3D(7, 5, 0)};
  EXPECT_NO_THROW(g::Polygon3D::Make(outer, {hole_a, hole_b}));
}

TEST_F(Polygon3DTest, WithHoles_HoleStrikesThroughOuter_Throws) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(10, 0, 0), g::Point3D(10, 10, 0), g::Point3D(0, 10, 0)};
  // Hole straddles the outer boundary at x=10: half inside, half poking out past x=10.
  std::vector<g::Point3D> hole = {
      g::Point3D(8, 4, 0), g::Point3D(8, 6, 0), g::Point3D(12, 6, 0), g::Point3D(12, 4, 0)};
  EXPECT_ANY_THROW(g::Polygon3D::Make(outer, {hole}));
}

TEST_F(Polygon3DTest, WithHoles_HoleFlushAgainstOuterEdge_DoesNotThrow) {
  // Hole touches the outer boundary along a full edge (not just a vertex) — must NOT be rejected as
  // "striking through".
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(2, 2, 0), g::Point3D(2, 4, 0), g::Point3D(4, 4, 0), g::Point3D(4, 2, 0)};
  EXPECT_NO_THROW(g::Polygon3D::Make(outer, {hole}));
}

TEST_F(Polygon3DTest, WithHoles_HoleEntirelyOutsideOuter_Throws) {
  // Hole never crosses or touches the outer boundary, so the strikes-through check alone lets it slip by,
  // but it sits wholly outside the outer square (same plane, disjoint region) — must be rejected by the
  // containment check.
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(10, 0, 0), g::Point3D(10, 10, 0), g::Point3D(0, 10, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(20, 20, 0), g::Point3D(20, 22, 0), g::Point3D(22, 22, 0), g::Point3D(22, 20, 0)};
  EXPECT_ANY_THROW(g::Polygon3D::Make(outer, {hole}));
}

TEST_F(Polygon3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "polygon3d.wkt").string();
  auto p = g::Polygon3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});

  p.ToFile(path);
  ASSERT_TRUE(fs::exists(path));
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Polygon3DTest, FromFile) {
  std::string path = (test_res_path / "polygon3d" / "polygon.wkt").string();
  ASSERT_TRUE(fs::exists(path));
  auto p = g::Polygon3D::FromFile(path);
  EXPECT_EQ(4, p.Size());
}

// ---- Centroid ---------------------------------------------------------------

TEST_F(Polygon3DTest, Centroid_Square) {
  // 4×4 square on XY plane → centroid = (2,2,0)
  auto p = g::Polygon3D::Make({
      g::Point3D(0,0,0), g::Point3D(4,0,0),
      g::Point3D(4,4,0), g::Point3D(0,4,0)});
  auto c = p.Centroid();
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(0.0, c.z(), 1e-9);
}

TEST_F(Polygon3DTest, Centroid_ElevatedSquare) {
  // same square at z=5 → centroid z must follow the plane
  auto p = g::Polygon3D::Make({
      g::Point3D(0,0,5), g::Point3D(4,0,5),
      g::Point3D(4,4,5), g::Point3D(0,4,5)});
  auto c = p.Centroid();
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(5.0, c.z(), 1e-9);
}

TEST_F(Polygon3DTest, Centroid_Triangle) {
  // right triangle base=4, height=3 → (4/3, 1, 0)
  auto p = g::Polygon3D::Make({
      g::Point3D(0,0,0), g::Point3D(4,0,0), g::Point3D(0,3,0)});
  auto c = p.Centroid();
  EXPECT_NEAR(4.0/3.0, c.x(), 1e-6);
  EXPECT_NEAR(1.0,     c.y(), 1e-6);
  EXPECT_NEAR(0.0,     c.z(), 1e-9);
}

TEST_F(Polygon3DTest, Centroid_NonXYPlane) {
  // 4×4 CCW square on the YZ plane → centroid = (0,2,2)
  auto p = g::Polygon3D::Make({
      g::Point3D(0,0,0), g::Point3D(0,4,0),
      g::Point3D(0,4,4), g::Point3D(0,0,4)});
  auto c = p.Centroid();
  EXPECT_NEAR(0.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(2.0, c.z(), 1e-9);
}

TEST_F(Polygon3DTest, Centroid_SquareWithCenteredHole) {
  // 4×4 outer square, 2×2 centered hole — centroid stays at center (2,2,0)
  std::vector<g::Point3D> outer = {
      g::Point3D(0,0,0), g::Point3D(4,0,0),
      g::Point3D(4,4,0), g::Point3D(0,4,0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1,1,0), g::Point3D(1,3,0),
      g::Point3D(3,3,0), g::Point3D(3,1,0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  auto c = p.Centroid();
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(0.0, c.z(), 1e-9);
}

TEST_F(Polygon3DTest, Centroid_SquareWithOffCenterHole) {
  // 6×1 outer, 2×1 CW hole on the right half
  // outer: (0,0)-(6,0)-(6,1)-(0,1), area=6, centroid=(3,0.5)
  // hole:  (4,0)-(4,1)-(6,1)-(6,0), area=-2, centroid=(5,0.5)
  // total_area = 4
  // cx = (6*3 + (-2)*5) / 4 = (18-10)/4 = 8/4 = 2
  // cy = (6*0.5 + (-2)*0.5) / 4 = 2/4 = 0.5
  std::vector<g::Point3D> outer = {
      g::Point3D(0,0,0), g::Point3D(6,0,0),
      g::Point3D(6,1,0), g::Point3D(0,1,0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(4,0,0), g::Point3D(4,1,0),
      g::Point3D(6,1,0), g::Point3D(6,0,0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  auto c = p.Centroid();
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(0.5, c.y(), 1e-9);
  EXPECT_NEAR(0.0, c.z(), 1e-9);
}

TEST_F(Polygon3DTest, Centroid_SelfIntersectingOuterOnTiltedPlane_MatchesSimplifyWeightedAverage) {
  // Same cross-validation as Polygon2D's equivalent test, on a tilted (non-axis-aligned) plane so a
  // naive 2D-projected centroid/area would be wrong too (foreshortening) — see Area()'s tilted-plane test.
  auto tilt = [](double x, double y) { return g::Point3D(x, y, 0.3 * x + 0.2 * y); };
  auto p = g::Polygon3D::Make({tilt(0, 0), tilt(4, 0), tilt(1, 3), tilt(3, 3)});
  ASSERT_FALSE(p.IsSimple());

  double total_area = 0.0, wx = 0.0, wy = 0.0, wz = 0.0;
  for (auto const& piece : p.Simplify()) {
    double a = piece.Area();
    auto c = piece.Centroid();
    total_area += a;
    wx += a * c.x();
    wy += a * c.y();
    wz += a * c.z();
  }

  auto actual = p.Centroid();
  EXPECT_NEAR(wx / total_area, actual.x(), 1e-9);
  EXPECT_NEAR(wy / total_area, actual.y(), 1e-9);
  EXPECT_NEAR(wz / total_area, actual.z(), 1e-9);
}

// ---- Area -------------------------------------------------------------------

TEST_F(Polygon3DTest, Area_Square) {
  // 4×4 square on XY plane → area = 16
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0),
      g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  EXPECT_NEAR(16.0, p.Area(), 1e-9);
}

TEST_F(Polygon3DTest, Area_Triangle) {
  // right triangle on XY plane: base=4, height=3 → area = 6
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 3, 0)});
  EXPECT_NEAR(6.0, p.Area(), 1e-9);
}

TEST_F(Polygon3DTest, Area_NonXYPlane) {
  // 4×4 square on the YZ plane → same area = 16
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(0, 4, 0),
      g::Point3D(0, 4, 4), g::Point3D(0, 0, 4)});
  EXPECT_NEAR(16.0, p.Area(), 1e-9);
}

TEST_F(Polygon3DTest, Area_SquareWithHole) {
  // 4×4 outer (area=16) minus 2×2 CW hole (area=4) = 12
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0),
      g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 3, 0),
      g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  EXPECT_NEAR(12.0, p.Area(), 1e-9);
}

TEST_F(Polygon3DTest, Area_FarFromOrigin) {
  // Same 4×4 square translated far from the origin — validates the reference-point
  // subtraction keeps cross products numerically well-conditioned
  double ox = 1e8, oy = 1e8;
  auto p = g::Polygon3D::Make({
      g::Point3D(ox,     oy,     0), g::Point3D(ox + 4, oy,     0),
      g::Point3D(ox + 4, oy + 4, 0), g::Point3D(ox,     oy + 4, 0)});
  EXPECT_NEAR(16.0, p.Area(), 1e-6);
}

// Regression test for a real bug found while investigating why two independently-Simplify()'d Polygon3D
// pieces of the same physical plane could disagree on GetPlane()'s normal direction: Make()'s outer plane
// was built from Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]) — a LOCAL
// quantity (the turn pivoting at vertex[0] toward vertex[1] and vertex[2]) — while the CCW validation that
// had just accepted those same points used a completely different, GLOBAL reference (are_ccw() auto-fits
// closest_world_plane_to(), which picks a world axis by magnitude alone, discarding sign). The two agree
// for a convex polygon, but can disagree whenever vertex[0] happens to be a reflex (concave) corner of an
// otherwise-valid CCW polygon: From3Points' local turn there points opposite the polygon's true winding.
// This L-shaped hexagon has exactly one reflex corner (at index 2 in `base`); rotating which vertex starts
// the list is purely a relabeling — Area() must stay the SAME positive value regardless of where in the
// ring the reflex corner happens to land as vertex[0].
// Regression test for a real bug: Polygon3D::Make() builds GetPlane() from
// Plane::From3Points(unique_points[0], unique_points[1], unique_points[2]) — a LOCAL quantity (the turn at
// vertex 0) that can point opposite the polygon's GLOBAL winding when that corner is reflex, even though
// the polygon itself is validly CCW (are_ccw() checks the whole ring, not just vertex 0's local turn). This
// L-shaped hexagon has exactly one reflex corner; rotating which vertex starts the list is purely a
// relabeling, so Area() — now abs() internally, precisely because GetPlane()'s sign isn't reliable — must
// come out the same positive value regardless of where the reflex corner lands as vertex 0. GetPlane()'s
// normal direction itself is deliberately NOT asserted here: Make() doesn't promise a canonical sign (see
// canonical_view_plane in polygon3d.cpp, which corrects it locally where it's actually needed instead).
TEST_F(Polygon3DTest, Area_ReflexFirstVertex_SignIsStableAcrossRotation) {
  std::vector<g::Point3D> base = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 2, 0),
      g::Point3D(2, 2, 0), g::Point3D(2, 4, 0), g::Point3D(0, 4, 0),
  };
  for (int start = 0; start < static_cast<int>(base.size()); ++start) {
    std::vector<g::Point3D> rotated;
    for (int i = 0; i < static_cast<int>(base.size()); ++i) {
      rotated.push_back(base[(start + i) % base.size()]);
    }
    auto p = g::Polygon3D::Make(rotated);
    EXPECT_NEAR(12.0, p.Area(), 1e-9) << "rotation start=" << start;
    EXPECT_NEAR(1.0, p.GetPlane().normal().z(), 1e-9) << "rotation start=" << start;
  }
}

// Two coplanar polygons built from vertex lists that start at different points around their own rings (so
// their raw, uncanonicalized From3Points normals would very plausibly disagree in sign) must still report
// the same GetPlane() normal — the property Polygon3D::Make() now guarantees by checking
// are_ccw(unique_points, outer_plane) against the very plane it's about to store, rather than trusting
// From3Points' sign — which is what boolean_op_multi's shared-view projection (to_ring_pieces_3d) relies
// on instead of a defensive re-check.
TEST_F(Polygon3DTest, GetPlane_NormalIsConsistentAcrossIndependentlyConstructedCoplanarPolygons) {
  auto a = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  auto b = g::Polygon3D::Make(
      {g::Point3D(4, 4, 0), g::Point3D(0, 4, 0), g::Point3D(0, 0, 0), g::Point3D(4, 0, 0)});  // same ring, rotated
  EXPECT_TRUE(a.GetPlane().normal().AlmostEquals(b.GetPlane().normal()));
}

// ---- Perimeter --------------------------------------------------------------

TEST_F(Polygon3DTest, Perimeter_Square) {
  // 1×1 square on XY → 4 sides of length 1
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(1, 0, 0),
      g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  EXPECT_NEAR(4.0, p.PerimeterSize(), 1e-9);
}

TEST_F(Polygon3DTest, Perimeter_NonXYPlane) {
  // 1×1 square on the YZ plane → same perimeter = 4
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(0, 1, 0),
      g::Point3D(0, 1, 1), g::Point3D(0, 0, 1)});
  EXPECT_NEAR(4.0, p.PerimeterSize(), 1e-9);
}

TEST_F(Polygon3DTest, Perimeter_Triangle) {
  // 3-4-5 right triangle → perimeter = 12
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 3, 0)});
  EXPECT_NEAR(12.0, p.PerimeterSize(), 1e-9);
}

// ---- GetPlane ---------------------------------------------------------------

TEST_F(Polygon3DTest, GetPlane_XYPlane) {
  // CCW square on XY plane → normal = +Z
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(1, 0, 0),
      g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  EXPECT_TRUE(p.GetPlane().normal().AlmostEquals(g::Vector3D::BasisZ()));
}

TEST_F(Polygon3DTest, GetPlane_ElevatedPlane) {
  // Same shape at z=5 → still normal = +Z, all vertices on plane
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 5), g::Point3D(1, 0, 5),
      g::Point3D(1, 1, 5), g::Point3D(0, 1, 5)});
  EXPECT_TRUE(p.GetPlane().normal().AlmostEquals(g::Vector3D::BasisZ()));
}

TEST_F(Polygon3DTest, GetPlane_AllVerticesOnPlane) {
  // Every outer vertex must satisfy plane.Contains
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0),
      g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  auto pl = p.GetPlane();
  for (int i = 0; i < p.Size(); ++i) {
    EXPECT_TRUE(pl.Contains(p[i]));
  }
}

TEST_F(Polygon3DTest, GetPlane_WithHoles) {
  // Polygon with holes must still return the correct plane
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0),
      g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 3, 0),
      g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  EXPECT_TRUE(p.GetPlane().normal().AlmostEquals(g::Vector3D::BasisZ()));
}

TEST_F(Polygon3DTest, DistanceTo) {
  auto poly = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_NEAR(0.0, poly.DistanceTo(g::Point3D(0.5, 0.5, 0)), 1e-9);  // interior
  EXPECT_NEAR(0.0, poly.DistanceTo(g::Point3D(0.0, 0.5, 0)), 1e-9);  // on boundary (edge)
  EXPECT_NEAR(0.0, poly.DistanceTo(g::Point3D(1.0, 1.0, 0)), 1e-9);  // on boundary (vertex)
  EXPECT_NEAR(1.0, poly.DistanceTo(g::Point3D(2.0, 0.5, 0)), 1e-9);  // outside in-plane, nearest edge x=1
  EXPECT_NEAR(std::sqrt(2.0), poly.DistanceTo(g::Point3D(2.0, 2.0, 0)), 1e-9);  // outside, nearest corner

  // off-plane: Contains() is false regardless of in-plane position, so this measures true 3D distance
  // to the nearest edge/vertex — even directly above the interior, there is no "inside" shortcut, since
  // distance is always measured to the boundary, never to a projected interior region.
  EXPECT_NEAR(std::sqrt(1.25), poly.DistanceTo(g::Point3D(0.5, 0.5, 1.0)), 1e-9);  // above the center:
                                                                                    // 0.5 in-plane to the
                                                                                    // nearest edge + 1.0
                                                                                    // perpendicular
  EXPECT_NEAR(std::sqrt(2.0), poly.DistanceTo(g::Point3D(2.0, 0.5, 1.0)), 1e-9);  // above + outside in-plane
}

TEST_F(Polygon3DTest, DistanceTo_WithHole) {
  // Same as Polygon2D's WithHole case, lifted into the XY plane: a point in the hole must measure to
  // the HOLE's boundary, not the outer ring.
  auto outer = std::vector<g::Point3D>{g::Point3D(0,0,0), g::Point3D(4,0,0), g::Point3D(4,4,0), g::Point3D(0,4,0)};
  auto hole  = std::vector<g::Point3D>{g::Point3D(1,3,0), g::Point3D(3,3,0), g::Point3D(3,1,0), g::Point3D(1,1,0)};
  auto poly  = g::Polygon3D::Make(outer, {hole});

  EXPECT_NEAR(0.0, poly.DistanceTo(g::Point3D(0.5, 0.5, 0)), 1e-9);  // in the solid region
  EXPECT_NEAR(1.0, poly.DistanceTo(g::Point3D(2.0, 2.0, 0)), 1e-9);  // hole center
  EXPECT_NEAR(0.0, poly.DistanceTo(g::Point3D(1.0, 2.0, 0)), 1e-9);  // on the hole boundary
}

TEST_F(Polygon3DTest, Contains) {
  // unit square in XY plane
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_TRUE(sq.Contains(g::Point3D(0.5, 0.5, 0)));   // center
  EXPECT_TRUE(sq.Contains(g::Point3D(0.1, 0.1, 0)));   // near corner
  EXPECT_TRUE(sq.Contains(g::Point3D(0.9, 0.9, 0)));   // near corner
  EXPECT_FALSE(sq.Contains(g::Point3D(-0.1, 0.5, 0))); // left of square
  EXPECT_FALSE(sq.Contains(g::Point3D(1.1, 0.5, 0)));  // right of square
  EXPECT_FALSE(sq.Contains(g::Point3D(0.5, 0.5, 1)));  // off-plane
  EXPECT_FALSE(sq.Contains(g::Point3D(0.5, 0.5, -1))); // off-plane

  // 4x4 square with 2x2 centred hole, in XY plane
  auto outer = std::vector<g::Point3D>{{0,0,0}, {4,0,0}, {4,4,0}, {0,4,0}};
  auto hole  = std::vector<g::Point3D>{{1,1,0}, {1,3,0}, {3,3,0}, {3,1,0}};
  auto poly  = g::Polygon3D::Make(outer, {hole});
  EXPECT_TRUE(poly.Contains(g::Point3D(0.5, 0.5, 0)));  // inner ring of outer, outside hole
  EXPECT_TRUE(poly.Contains(g::Point3D(3.5, 3.5, 0)));  // inner ring of outer, outside hole
  EXPECT_FALSE(poly.Contains(g::Point3D(2, 2, 0)));     // inside hole
  EXPECT_FALSE(poly.Contains(g::Point3D(-1, 2, 0)));    // outside outer
  EXPECT_FALSE(poly.Contains(g::Point3D(2, 2, 1)));     // above plane

  // non-XY plane: square in YZ plane at x=0
  auto yz_sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(0,1,0), g::Point3D(0,1,1), g::Point3D(0,0,1)});
  EXPECT_TRUE(yz_sq.Contains(g::Point3D(0, 0.5, 0.5)));   // center of YZ square
  EXPECT_FALSE(yz_sq.Contains(g::Point3D(1, 0.5, 0.5)));  // off-plane
}

TEST_F(Polygon3DTest, Contains_OnBoundary) {
  // Boundary is included (delegates to Polygon2D after projection).
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});

  // vertices
  EXPECT_TRUE(sq.Contains(g::Point3D(0, 0, 0)));
  EXPECT_TRUE(sq.Contains(g::Point3D(1, 0, 0)));
  EXPECT_TRUE(sq.Contains(g::Point3D(1, 1, 0)));
  EXPECT_TRUE(sq.Contains(g::Point3D(0, 1, 0)));

  // edge midpoints
  EXPECT_TRUE(sq.Contains(g::Point3D(0.5, 0,   0)));  // bottom edge
  EXPECT_TRUE(sq.Contains(g::Point3D(1,   0.5, 0)));  // right edge
  EXPECT_TRUE(sq.Contains(g::Point3D(0.5, 1,   0)));  // top edge
  EXPECT_TRUE(sq.Contains(g::Point3D(0,   0.5, 0)));  // left edge

  // non-XY plane: YZ square at x=0
  auto yz = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(0,1,0), g::Point3D(0,1,1), g::Point3D(0,0,1)});
  EXPECT_TRUE(yz.Contains(g::Point3D(0, 0.5, 0)));  // bottom edge midpoint
  EXPECT_TRUE(yz.Contains(g::Point3D(0, 0,   0)));  // vertex

  // hole boundary
  auto outer = std::vector<g::Point3D>{{0,0,0}, {4,0,0}, {4,4,0}, {0,4,0}};
  auto hole  = std::vector<g::Point3D>{{1,1,0}, {1,3,0}, {3,3,0}, {3,1,0}};
  auto poly  = g::Polygon3D::Make(outer, {hole});
  EXPECT_TRUE(poly.Contains(g::Point3D(2, 0, 0)));  // outer bottom edge
  EXPECT_TRUE(poly.Contains(g::Point3D(2, 1, 0)));  // hole bottom edge
}

TEST_F(Polygon3DTest, IsOnPerimeter_True) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});

  // all four vertices
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(0,   0,   0)));
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(1,   0,   0)));
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(1,   1,   0)));
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(0,   1,   0)));

  // edge midpoints
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(0.5, 0,   0)));  // bottom
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(1,   0.5, 0)));  // right
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(0.5, 1,   0)));  // top
  EXPECT_TRUE(sq.IsOnPerimeter(g::Point3D(0,   0.5, 0)));  // left

  // non-XY plane: YZ square at x=0
  auto yz = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(0,1,0), g::Point3D(0,1,1), g::Point3D(0,0,1)});
  EXPECT_TRUE(yz.IsOnPerimeter(g::Point3D(0, 0.5, 0)));  // bottom edge midpoint
  EXPECT_TRUE(yz.IsOnPerimeter(g::Point3D(0, 0,   0)));  // vertex

  // hole boundary
  auto outer = std::vector<g::Point3D>{{0,0,0}, {4,0,0}, {4,4,0}, {0,4,0}};
  auto hole  = std::vector<g::Point3D>{{1,1,0}, {1,3,0}, {3,3,0}, {3,1,0}};
  auto poly  = g::Polygon3D::Make(outer, {hole});
  EXPECT_TRUE(poly.IsOnPerimeter(g::Point3D(2, 0, 0)));  // outer bottom
  EXPECT_TRUE(poly.IsOnPerimeter(g::Point3D(4, 2, 0)));  // outer right
  EXPECT_TRUE(poly.IsOnPerimeter(g::Point3D(2, 1, 0)));  // hole bottom
  EXPECT_TRUE(poly.IsOnPerimeter(g::Point3D(1, 2, 0)));  // hole left
}

TEST_F(Polygon3DTest, IsOnPerimeter_False) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});

  EXPECT_FALSE(sq.IsOnPerimeter(g::Point3D(0.5, 0.5, 0)));   // interior
  EXPECT_FALSE(sq.IsOnPerimeter(g::Point3D(-0.1, 0.5, 0)));  // outside left
  EXPECT_FALSE(sq.IsOnPerimeter(g::Point3D(0.5,  0.5, 0.01)));  // off-plane

  // interior of polygon with hole is not boundary
  auto outer = std::vector<g::Point3D>{{0,0,0}, {4,0,0}, {4,4,0}, {0,4,0}};
  auto hole  = std::vector<g::Point3D>{{1,1,0}, {1,3,0}, {3,3,0}, {3,1,0}};
  auto poly  = g::Polygon3D::Make(outer, {hole});
  EXPECT_FALSE(poly.IsOnPerimeter(g::Point3D(0.5, 0.5, 0)));  // interior strip
  EXPECT_FALSE(poly.IsOnPerimeter(g::Point3D(2,   2,   0)));  // inside hole
}

TEST_F(Polygon3DTest, ToSegments) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto segs = p.ToSegments();
  ASSERT_EQ(4, segs.size());  // N vertices -> N segments (closed)
  EXPECT_EQ(g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0)), segs[0]);
  EXPECT_EQ(g::LineSegment3D::Make(g::Point3D(0, 1, 0), g::Point3D(0, 0, 0)), segs[3]);  // wraps back
  int count = 0;
  for (auto const& s : segs) {
    EXPECT_NEAR(1.0, s.Length(), 1e-9);
    ++count;
  }
  EXPECT_EQ(4, count);
}

// NOTE: IsSimple() correctness depends on the (currently provisional) sweep-line comparator; these encode the
// intended behaviour and are expected to be re-verified once the real sweep-status ordering is in place.
TEST_F(Polygon3DTest, IsSimple_ConvexSquareIsSimple) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_TRUE(p.IsSimple());
}

TEST_F(Polygon3DTest, IsSimple_ConvexSquare_YZPlane_IsSimple) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0,0,0), g::Point3D(0,1,0), g::Point3D(0,1,1), g::Point3D(0,0,1)});
  EXPECT_TRUE(p.IsSimple());
}

TEST_F(Polygon3DTest, IsSimple_SelfIntersectingIsNotSimple) {
  // CCW (positive area) but the (4,0,0)->(1,3,0) and (3,3,0)->(0,0,0) edges cross
  auto p = g::Polygon3D::Make(
      {g::Point3D(0,0,0), g::Point3D(4,0,0), g::Point3D(1,3,0), g::Point3D(3,3,0)});
  EXPECT_FALSE(p.IsSimple());
}

// ---- IsConvex ---------------------------------------------------------------

TEST_F(Polygon3DTest, IsConvex_Square_XYPlane_True) {
  // A unit square in the XY plane is convex (CCW when viewed from +Z)
  auto p = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  EXPECT_TRUE(p.IsConvex());
}

TEST_F(Polygon3DTest, IsConvex_YZPlane_True) {
  // A unit square in the YZ plane is convex (CCW when viewed from +X)
  auto p = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(0, 1, 1), g::Point3D(0, 0, 1)});
  EXPECT_TRUE(p.IsConvex());
}

TEST_F(Polygon3DTest, IsConvex_ConcavePolygon_False) {
  // Arrow/dent shape in XY plane — concave at (2,2,0)
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0),
      g::Point3D(2, 2, 0), g::Point3D(0, 4, 0)});
  EXPECT_FALSE(p.IsConvex());
}

TEST_F(Polygon3DTest, IsConvex_WithHole_False) {
  // Any polygon with a hole is non-convex by definition
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)};
  auto p = g::Polygon3D::Make(outer, {hole});
  EXPECT_FALSE(p.IsConvex());
}

// ---- ConvexHull ---------------------------------------------------------------

TEST_F(Polygon3DTest, ConvexHull_AlreadyConvex_ReturnsSamePoints) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto hull = p.ConvexHull();
  EXPECT_TRUE(p.AlmostEquals(hull));
}

TEST_F(Polygon3DTest, ConvexHull_ConcavePolygon_DropsInnerVertex) {
  // Arrow/dent shape in XY plane — the dent at (2,2,0) must be excluded from the hull
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0),
      g::Point3D(2, 2, 0), g::Point3D(0, 4, 0)});
  auto hull = p.ConvexHull();

  EXPECT_EQ(4u, hull.Size());
  for (std::size_t i = 0; i < hull.Size(); ++i) {
    EXPECT_FALSE(hull[i].AlmostEquals(g::Point3D(2, 2, 0))) << "dent vertex must not survive the hull";
  }
}

TEST_F(Polygon3DTest, ConvexHull_TiltedPlane_ReturnsFourCorners) {
  // Unit square in the YZ plane (X=0) — hull should keep exactly the 4 corners
  auto p = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(0, 1, 1), g::Point3D(0, 0, 1)});
  auto hull = p.ConvexHull();
  EXPECT_EQ(4u, hull.Size());
  EXPECT_TRUE(hull.IsConvex());
}

TEST_F(Polygon3DTest, ConvexHull_WithHole_IgnoresHole) {
  std::vector<g::Point3D> outer = {
      g::Point3D(0, 0, 0), g::Point3D(3, 0, 0), g::Point3D(3, 3, 0), g::Point3D(0, 3, 0)};
  std::vector<g::Point3D> hole = {
      g::Point3D(1, 1, 0), g::Point3D(1, 2, 0), g::Point3D(2, 2, 0), g::Point3D(2, 1, 0)};
  auto p = g::Polygon3D::Make(outer, {hole});

  auto hull = p.ConvexHull();
  EXPECT_EQ(4u, hull.Size());
  EXPECT_NEAR(9.0, hull.Area(), 1e-9);
  EXPECT_FALSE(hull.HasHoles());
}

// ---- Simplify ---------------------------------------------------------------

TEST_F(Polygon3DTest, Simplify_AlreadySimple_ReturnsSelf) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  auto result = p.Simplify();
  ASSERT_EQ(1u, result.size());
  EXPECT_TRUE(p.AlmostEquals(result[0]));
}

TEST_F(Polygon3DTest, Simplify_BowtieInXYPlane_YieldsTwoSimpleTriangles) {
  // Same bowtie as Polygon2D test, lifted into XY plane (z=0): dominant axis = Z.
  // A(0,0,0), B(4,0,0), C(1,3,0), D(3,3,0) — B→C and D→A cross at X(2,2,0).
  auto p = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  ASSERT_FALSE(p.IsSimple());

  auto result = p.Simplify();
  ASSERT_EQ(2u, result.size());

  EXPECT_TRUE(result[0].IsSimple());
  EXPECT_TRUE(result[1].IsSimple());

  double a0 = result[0].Area(), a1 = result[1].Area();
  bool areas_match = (std::abs(a0 - 4.0) < 0.01 && std::abs(a1 - 1.0) < 0.01) ||
                     (std::abs(a0 - 1.0) < 0.01 && std::abs(a1 - 4.0) < 0.01);
  EXPECT_TRUE(areas_match) << "areas: " << a0 << ", " << a1;
}

TEST_F(Polygon3DTest, Simplify_BowtieInYZPlane_YieldsTwoSimpleTriangles) {
  // Bowtie in the YZ plane (x=0): dominant axis = X.
  auto p = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(0, 4, 0), g::Point3D(0, 1, 3), g::Point3D(0, 3, 3)});
  ASSERT_FALSE(p.IsSimple());

  auto result = p.Simplify();
  ASSERT_EQ(2u, result.size());

  EXPECT_TRUE(result[0].IsSimple());
  EXPECT_TRUE(result[1].IsSimple());

  for (auto const& poly : result) {
    for (int i = 0; i < static_cast<int>(poly.Size()); ++i) {
      EXPECT_NEAR(0.0, poly[i].x(), 1e-9) << "vertex x should be 0 (YZ plane)";
    }
  }
}

TEST_F(Polygon3DTest, Simplify_BowtieInXZPlane_YieldsTwoSimpleTriangles) {
  // Bowtie in the XZ plane (y=0): dominant axis = Y, projection flips chirality.
  // CCW when viewed from +Y: reversed vertex order.
  auto p = g::Polygon3D::Make({g::Point3D(3, 0, 3), g::Point3D(1, 0, 3), g::Point3D(4, 0, 0), g::Point3D(0, 0, 0)});
  ASSERT_FALSE(p.IsSimple());

  auto result = p.Simplify();
  ASSERT_EQ(2u, result.size());

  EXPECT_TRUE(result[0].IsSimple());
  EXPECT_TRUE(result[1].IsSimple());

  for (auto const& poly : result) {
    for (int i = 0; i < static_cast<int>(poly.Size()); ++i) {
      EXPECT_NEAR(0.0, poly[i].y(), 1e-9) << "vertex y should be 0 (XZ plane)";
    }
  }
}

TEST_F(Polygon3DTest, Simplify_BowtieAreasSum) {
  // Total area of the two sub-triangles should equal the sum of the parts (4 + 1 = 5).
  auto p = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  auto result = p.Simplify();
  ASSERT_EQ(2u, result.size());
  double total = result[0].Area() + result[1].Area();
  EXPECT_NEAR(5.0, total, 0.01);
}

TEST_F(Polygon3DTest, Area_SelfIntersectingOuterInXYPlane) {
  // Same bowtie as Simplify_BowtieAreasSum, checked directly via Area() (its slow path) instead of
  // summing Simplify() pieces by hand.
  auto p = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  EXPECT_FALSE(p.IsSimple());
  EXPECT_NEAR(5.0, p.Area(), 1e-6);
}

TEST_F(Polygon3DTest, Area_SelfIntersectingOuterOnTiltedPlane_MatchesSimplifySum) {
  // On a non-axis-aligned plane, a naive 2D-projected shoelace on the decomposed loops would be wrong
  // (foreshortening) — Area()'s slow path unprojects back to 3D and uses the plane-aware signed_area
  // instead. Cross-validated against the independently-correct Simplify() + per-piece fast-path Area()
  // pipeline, rather than hand-deriving the tilted value.
  auto tilt = [](double x, double y) { return g::Point3D(x, y, 0.3 * x + 0.2 * y); };
  auto p = g::Polygon3D::Make({tilt(0, 0), tilt(4, 0), tilt(1, 3), tilt(3, 3)});
  EXPECT_FALSE(p.IsSimple());
  double simplify_total = 0.0;
  for (auto const& piece : p.Simplify()) {
    simplify_total += piece.Area();
  }
  EXPECT_NEAR(simplify_total, p.Area(), 1e-6);
}

TEST_F(Polygon3DTest, Simplify_ResultsAreCoplanar) {
  // All result polygons from a 3D Simplify must lie on the same plane as the original.
  auto p = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  auto plane = p.GetPlane();
  for (auto const& poly : p.Simplify()) {
    for (int i = 0; i < static_cast<int>(poly.Size()); ++i) {
      EXPECT_NEAR(0.0, plane.DistanceTo(poly[i]), 1e-9) << "vertex not on original plane";
    }
  }
}

// ---- Intersection (Line3D / Ray3D / LineSegment3D) --------------------------

TEST_F(Polygon3DTest, Intersection_Line_HitsCenter) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto line = g::Line3D::Make(g::Point3D(0.5, 0.5, -1), g::Point3D(0.5, 0.5, 1));
  auto result = sq.Intersection(line);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->AlmostEquals(g::Point3D(0.5, 0.5, 0)));
}

TEST_F(Polygon3DTest, Intersection_Line_MissesOutside) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto line = g::Line3D::Make(g::Point3D(2, 0.5, -1), g::Point3D(2, 0.5, 1));
  EXPECT_FALSE(sq.Intersection(line).has_value());
}

TEST_F(Polygon3DTest, Intersection_Line_ParallelToPlane_Miss) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto line = g::Line3D::Make(g::Point3D(0, 0, 1), g::Point3D(1, 1, 1));  // parallel at z=1
  EXPECT_FALSE(sq.Intersection(line).has_value());
}

TEST_F(Polygon3DTest, Intersects_Line_True) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_TRUE(sq.Intersects(g::Line3D::Make(g::Point3D(0.5, 0.5, -1), g::Point3D(0.5, 0.5, 1))));
}

TEST_F(Polygon3DTest, Intersects_Line_False) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_FALSE(sq.Intersects(g::Line3D::Make(g::Point3D(2, 0.5, -1), g::Point3D(2, 0.5, 1))));
}

TEST_F(Polygon3DTest, Intersection_Ray_HitsCenter) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto ray = g::Ray3D::Make(g::Point3D(0.5, 0.5, 1), g::Vector3D(0, 0, -1));
  auto result = sq.Intersection(ray);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->AlmostEquals(g::Point3D(0.5, 0.5, 0)));
}

TEST_F(Polygon3DTest, Intersection_Ray_PointingAway) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto ray = g::Ray3D::Make(g::Point3D(0.5, 0.5, 1), g::Vector3D(0, 0, 1));
  EXPECT_FALSE(sq.Intersection(ray).has_value());
}

TEST_F(Polygon3DTest, Intersection_Ray_MissesOutside) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto ray = g::Ray3D::Make(g::Point3D(2, 0.5, 1), g::Vector3D(0, 0, -1));
  EXPECT_FALSE(sq.Intersection(ray).has_value());
}

TEST_F(Polygon3DTest, Intersects_Ray_True) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_TRUE(sq.Intersects(g::Ray3D::Make(g::Point3D(0.5, 0.5, 1), g::Vector3D(0, 0, -1))));
}

TEST_F(Polygon3DTest, Intersects_Ray_False) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_FALSE(sq.Intersects(g::Ray3D::Make(g::Point3D(0.5, 0.5, 1), g::Vector3D(0, 0, 1))));
}

TEST_F(Polygon3DTest, Intersection_Segment_Pierces) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto seg = g::LineSegment3D::Make(g::Point3D(0.5, 0.5, -1), g::Point3D(0.5, 0.5, 1));
  auto result = sq.Intersection(seg);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->AlmostEquals(g::Point3D(0.5, 0.5, 0)));
}

TEST_F(Polygon3DTest, Intersection_Segment_TooShort) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto seg = g::LineSegment3D::Make(g::Point3D(0.5, 0.5, 0.5), g::Point3D(0.5, 0.5, 1));
  EXPECT_FALSE(sq.Intersection(seg).has_value());
}

TEST_F(Polygon3DTest, Intersection_Segment_MissesOutside) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  auto seg = g::LineSegment3D::Make(g::Point3D(2, 0.5, -1), g::Point3D(2, 0.5, 1));
  EXPECT_FALSE(sq.Intersection(seg).has_value());
}

TEST_F(Polygon3DTest, Intersects_Segment_True) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_TRUE(sq.Intersects(g::LineSegment3D::Make(g::Point3D(0.5, 0.5, -1), g::Point3D(0.5, 0.5, 1))));
}

TEST_F(Polygon3DTest, Intersects_Segment_False) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});
  EXPECT_FALSE(sq.Intersects(g::LineSegment3D::Make(g::Point3D(0.5, 0.5, 0.5), g::Point3D(0.5, 0.5, 1))));
}

TEST_F(Polygon3DTest, Intersection_Line_NonXYPlane) {
  // YZ square at x=0; normal = +X. Line along X passes through center (0, 0.5, 0.5).
  auto yz = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(0,1,0), g::Point3D(0,1,1), g::Point3D(0,0,1)});
  auto line = g::Line3D::Make(g::Point3D(-1, 0.5, 0.5), g::Point3D(1, 0.5, 0.5));
  auto result = yz.Intersection(line);
  ASSERT_TRUE(result.has_value());
  EXPECT_TRUE(result->AlmostEquals(g::Point3D(0, 0.5, 0.5)));
}

#pragma region Boolean Operations (Coplanar)

TEST_F(Polygon3DTest, Union_CoplanarOverlappingSquares) {
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto b = g::Polygon3D::Make(
      {g::Point3D(0.5, 0.5, 0), g::Point3D(1.5, 0.5, 0), g::Point3D(1.5, 1.5, 0), g::Point3D(0.5, 1.5, 0)});

  auto result = a.Union(b);
  ASSERT_EQ(1u, result.size());
  EXPECT_NEAR(1.75, result[0].Area(), 1e-6);
}

TEST_F(Polygon3DTest, Intersection_Polygon_CoplanarOverlappingSquares) {
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto b = g::Polygon3D::Make(
      {g::Point3D(0.5, 0.5, 0), g::Point3D(1.5, 0.5, 0), g::Point3D(1.5, 1.5, 0), g::Point3D(0.5, 1.5, 0)});

  auto result = a.Intersection(b);
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(std::holds_alternative<std::vector<g::Polygon3D>>(*result));
  auto const& polys = std::get<std::vector<g::Polygon3D>>(*result);
  ASSERT_EQ(1u, polys.size());
  EXPECT_NEAR(0.25, polys[0].Area(), 1e-6);
}

TEST_F(Polygon3DTest, Difference_CoplanarNestedNonTouching_ProducesHole) {
  auto a = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(10, 0, 0), g::Point3D(10, 10, 0), g::Point3D(0, 10, 0)});
  auto b = g::Polygon3D::Make({g::Point3D(4, 4, 0), g::Point3D(6, 4, 0), g::Point3D(6, 6, 0), g::Point3D(4, 6, 0)});

  auto result = a.Difference(b);
  ASSERT_EQ(1u, result.size());
  EXPECT_TRUE(result[0].HasHoles());
  EXPECT_NEAR(100.0 - 4.0, result[0].Area(), 1e-6);
}

// ── Self-intersecting operands ──────────────────────────────────────────────
// Same bowtie as Polygon2D's SelfIntersectingBowtie_* tests (area 5 = 4 + 1, see Simplify_BowtieAreasSum
// above), lifted into the XY plane, cross-validated against Simplify()'s already-trusted decomposition.
// Polygon3D::Union/Difference/Xor/Intersection route through the same Simplify()-decomposed,
// source-tagged engine as Polygon2D (run_boolean_op_3d -> to_ring_pieces_3d -> detail::boolean_op_multi),
// so a self-intersecting operand needs the same coverage here that motivated the 2D tests.

namespace {
double bowtie3d_test_area_sum(std::vector<g::Polygon3D> const& pieces) {
  double a = 0.0;
  for (auto const& p : pieces) {
    a += p.Area();
  }
  return a;
}

// Same reasoning as Polygon2D's bowtie_test_has_repeated_vertex: a vertex coordinate appearing more than
// once in the SAME ring is the exact signature of the pinch-point tracer bug fixed in
// calc_utils2d.cpp's trace_directed_boundary — not reliably caught by IsSimple() alone, since two
// non-adjacent edges that merely share an endpoint are excluded from the Shamos-Hoey crossing check.
bool bowtie3d_test_has_repeated_vertex(g::Polygon3D const& poly) {
  int n = static_cast<int>(poly.Size());
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (poly[i].AlmostEquals(poly[j])) {
        return true;
      }
    }
  }
  return false;
}
}  // namespace

TEST_F(Polygon3DTest, SelfIntersectingBowtie_Intersection_WithContainingSquare_MatchesSimplifyTotalArea) {
  auto bowtie = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  auto containing = g::Polygon3D::Make(
      {g::Point3D(-1, -1, 0), g::Point3D(5, -1, 0), g::Point3D(5, 5, 0), g::Point3D(-1, 5, 0)});

  auto simplified = bowtie.Simplify();
  double simplify_total_area = bowtie3d_test_area_sum(simplified);
  ASSERT_GT(simplify_total_area, 0.0);

  auto result = bowtie.Intersection(containing);
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(std::holds_alternative<std::vector<g::Polygon3D>>(*result));
  auto const& polys = std::get<std::vector<g::Polygon3D>>(*result);
  // Also asserts piece COUNT, not just total area: a pinch-point tracing bug once merged the bowtie's two
  // lobes into one self-touching ring whose area happened to still cancel out to the (wrong) total.
  ASSERT_EQ(simplified.size(), polys.size());
  for (auto const& piece : polys) {
    EXPECT_TRUE(piece.IsSimple());
    EXPECT_FALSE(bowtie3d_test_has_repeated_vertex(piece));
  }
  double intersection_total_area = bowtie3d_test_area_sum(polys);
  EXPECT_NEAR(simplify_total_area, intersection_total_area, 1e-6);
}

TEST_F(Polygon3DTest, SelfIntersectingBowtie_AsClip_Difference_WithContainingSquare_MatchesRemainder) {
  auto bowtie = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  auto containing = g::Polygon3D::Make(
      {g::Point3D(-1, -1, 0), g::Point3D(5, -1, 0), g::Point3D(5, 5, 0), g::Point3D(-1, 5, 0)});

  double simplify_total_area = bowtie3d_test_area_sum(bowtie.Simplify());
  auto result = containing.Difference(bowtie);
  ASSERT_EQ(1u, result.size());
  EXPECT_TRUE(result[0].HasHoles());
  EXPECT_EQ(2u, result[0].Holes().size());
  EXPECT_FALSE(bowtie3d_test_has_repeated_vertex(result[0]));
  double result_area = bowtie3d_test_area_sum(result);
  EXPECT_NEAR(containing.Area() - simplify_total_area, result_area, 1e-6);
}

TEST_F(Polygon3DTest, SelfIntersectingBowtie_Union_WithContainingSquare_CollapsesToSquare) {
  auto bowtie = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0)});
  auto containing = g::Polygon3D::Make(
      {g::Point3D(-1, -1, 0), g::Point3D(5, -1, 0), g::Point3D(5, 5, 0), g::Point3D(-1, 5, 0)});

  auto result = bowtie.Union(containing);
  ASSERT_EQ(1u, result.size());
  EXPECT_NEAR(containing.Area(), result[0].Area(), 1e-6);
  EXPECT_FALSE(result[0].HasHoles());
}

// 3D counterpart of Polygon2D's SelfIntersectingPentagram_* tests, lifted into the XY plane: the pentagram
// is the only construction in this suite whose Simplify() pieces share FULL edges with each other (each
// point-triangle shares one edge with the central pentagon), rather than just touching at a point like the
// bowtie's lobes — the scenario detail::cancel_coincident_same_operand_pairs exists for. Same coordinates
// as the 2D version, with z=0.
TEST_F(Polygon3DTest, SelfIntersectingPentagram_SimplifyYieldsPentagonPlusFiveTriangles) {
  auto pentagram = g::Polygon3D::Make({
      g::Point3D(0.0, 1.0, 0), g::Point3D(-0.587785, -0.809017, 0), g::Point3D(0.951057, 0.309017, 0),
      g::Point3D(-0.951057, 0.309017, 0), g::Point3D(0.587785, -0.809017, 0),
  });
  ASSERT_FALSE(pentagram.IsSimple());

  auto pieces = pentagram.Simplify();
  ASSERT_EQ(6u, pieces.size());  // central pentagon + 5 point-triangles
  int triangles = 0, pentagons = 0;
  for (auto const& p : pieces) {
    if (p.Size() == 3) {
      ++triangles;
    } else if (p.Size() == 5) {
      ++pentagons;
    }
  }
  EXPECT_EQ(5, triangles);
  EXPECT_EQ(1, pentagons);
}

// The decisive check for detail::cancel_coincident_same_operand_pairs in the 3D engine (run_boolean_op_3d
// -> to_ring_pieces_3d -> detail::boolean_op_multi): Intersection with a fully-containing square must
// recover the pentagram's occupied region as ONE star-shaped ring, not the 6 separate pieces Simplify()
// produces — if same-operand edge cancellation didn't work, the internal pentagon/triangle seams would
// leak into the result as spurious extra boundaries (see the 2D version's docstring for the disable-and-
// confirm verification of this same mechanism).
TEST_F(Polygon3DTest, SelfIntersectingPentagram_Intersection_WithContainingSquare_MergesIntoOneStar) {
  auto pentagram = g::Polygon3D::Make({
      g::Point3D(0.0, 1.0, 0), g::Point3D(-0.587785, -0.809017, 0), g::Point3D(0.951057, 0.309017, 0),
      g::Point3D(-0.951057, 0.309017, 0), g::Point3D(0.587785, -0.809017, 0),
  });
  auto containing = g::Polygon3D::Make(
      {g::Point3D(-2, -2, 0), g::Point3D(2, -2, 0), g::Point3D(2, 2, 0), g::Point3D(-2, 2, 0)});

  double simplify_total_area = bowtie3d_test_area_sum(pentagram.Simplify());

  auto result = pentagram.Intersection(containing);
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(std::holds_alternative<std::vector<g::Polygon3D>>(*result));
  auto const& polys = std::get<std::vector<g::Polygon3D>>(*result);
  ASSERT_EQ(1u, polys.size());
  EXPECT_FALSE(polys[0].HasHoles());
  EXPECT_EQ(10u, polys[0].Size());  // a 5-pointed star outline has 10 vertices (5 outer + 5 inner)
  EXPECT_FALSE(bowtie3d_test_has_repeated_vertex(polys[0]));
  EXPECT_NEAR(simplify_total_area, polys[0].Area(), 1e-6);
}

TEST_F(Polygon3DTest, Union_NotCoplanar_Throws) {
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto b = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(0, 1, 1), g::Point3D(0, 0, 1)});
  EXPECT_THROW(a.Union(b), std::logic_error);
  EXPECT_THROW(a.Difference(b), std::logic_error);
  EXPECT_THROW(a.Xor(b), std::logic_error);
}

// Coplanarity checks (Plane::AlmostEquals, called with its default epsilon = DOUBLE_EPSILON) already
// respect whatever DECIMAL_PRECISION the caller sets beforehand — no dedicated tolerance parameter is
// needed on Union/Difference/Xor/Intersection/Intersects, since C++ default arguments are evaluated
// fresh at each call, not baked in once.
TEST_F(Polygon3DTest, Union_NearlyCoplanar_TinyOffsetWithinDefaultPrecision) {
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto b = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0.0001), g::Point3D(1, 0, 0.0001), g::Point3D(1, 1, 0.0001), g::Point3D(0, 1, 0.0001)});

  auto result = a.Union(b);
  ASSERT_EQ(1u, result.size());
  EXPECT_NEAR(1.0, result[0].Area(), 1e-6);
}

// Coplanarity checks (Plane::AlmostEquals, called with its default epsilon = DOUBLE_EPSILON) already
// respect whatever DECIMAL_PRECISION the caller sets beforehand — no dedicated tolerance parameter is
// needed on Union/Difference/Xor/Intersection/Intersects, since C++ default arguments are evaluated
// fresh at each call, not baked in once. Uses 100-unit-scale geometry so the loosened epsilon (0.1)
// stays small relative to the polygons' own size — see Polygon2D's
// Union_LoosePrecision_ReliableWhenEpsilonStaysSmallRelativeToScale for why that headroom matters.
TEST_F(Polygon3DTest, Union_NearlyCoplanar_LooseEpsilonAllowsIt) {
  auto a = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(100, 0, 0), g::Point3D(100, 100, 0), g::Point3D(0, 100, 0)});
  // b's plane is offset by 0.05 from a's — beyond default precision (DP_THREE, epsilon 0.001), within a
  // looser one, and negligible next to this geometry's 100-unit scale.
  auto b = g::Polygon3D::Make({g::Point3D(50, 50, 0.05), g::Point3D(150, 50, 0.05), g::Point3D(150, 150, 0.05),
                               g::Point3D(50, 150, 0.05)});

  EXPECT_THROW(a.Union(b), std::logic_error);  // default precision: 0.05 > 0.001, rejected

  g::DECIMAL_PRECISION = 1;  // epsilon = 0.1 > 0.05, still tiny next to the 100-unit scale
  auto result = a.Union(b);
  ASSERT_EQ(1u, result.size());
  EXPECT_NEAR(17500.0, result[0].Area(), 1e-3);
}

TEST_F(Polygon3DTest, Intersects_Polygon_Coplanar) {
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto overlapping = g::Polygon3D::Make(
      {g::Point3D(0.5, 0.5, 0), g::Point3D(1.5, 0.5, 0), g::Point3D(1.5, 1.5, 0), g::Point3D(0.5, 1.5, 0)});
  auto disjoint =
      g::Polygon3D::Make({g::Point3D(5, 5, 0), g::Point3D(6, 5, 0), g::Point3D(6, 6, 0), g::Point3D(5, 6, 0)});

  EXPECT_TRUE(a.Intersects(overlapping));
  EXPECT_FALSE(a.Intersects(disjoint));
}

#pragma endregion

#pragma region Boolean Operations (Non-Coplanar)

TEST_F(Polygon3DTest, Intersection_Polygon_PlanesCrossing_ReturnsSegment) {
  // A lies flat on z=0, spanning x:0..4, y:0..4.
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  // B is a vertical "wall" on the plane y=2, spanning x:1..3, z:-1..3 — it slices through A.
  auto b = g::Polygon3D::Make(
      {g::Point3D(1, 2, 3), g::Point3D(3, 2, 3), g::Point3D(3, 2, -1), g::Point3D(1, 2, -1)});

  auto result = a.Intersection(b);
  ASSERT_TRUE(result.has_value());
  ASSERT_TRUE(std::holds_alternative<std::vector<g::LineSegment3D>>(*result));
  auto const& segs = std::get<std::vector<g::LineSegment3D>>(*result);
  ASSERT_EQ(1u, segs.size());

  // The shared chord lies at y=2, z=0, x in [1, 3] (where both bounded regions cover the shared line).
  bool matches_forward =
      segs[0].First().AlmostEquals(g::Point3D(1, 2, 0)) && segs[0].Last().AlmostEquals(g::Point3D(3, 2, 0));
  bool matches_reverse =
      segs[0].First().AlmostEquals(g::Point3D(3, 2, 0)) && segs[0].Last().AlmostEquals(g::Point3D(1, 2, 0));
  EXPECT_TRUE(matches_forward || matches_reverse);
}

TEST_F(Polygon3DTest, Intersects_Polygon_PlanesCrossing) {
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  auto b = g::Polygon3D::Make(
      {g::Point3D(1, 2, 3), g::Point3D(3, 2, 3), g::Point3D(3, 2, -1), g::Point3D(1, 2, -1)});
  EXPECT_TRUE(a.Intersects(b));
}

TEST_F(Polygon3DTest, Intersection_Polygon_PlanesCrossing_ButBoundsMiss_ReturnsNullopt) {
  // Same crossing planes as above, but B's x-range (10..12) never overlaps A's (0..4) along the shared line.
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)});
  auto b = g::Polygon3D::Make(
      {g::Point3D(10, 2, 3), g::Point3D(12, 2, 3), g::Point3D(12, 2, -1), g::Point3D(10, 2, -1)});

  auto result = a.Intersection(b);
  EXPECT_FALSE(result.has_value());
  EXPECT_FALSE(a.Intersects(b));
}

TEST_F(Polygon3DTest, Intersection_Polygon_ParallelDistinctPlanes_ReturnsNullopt) {
  auto a = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto b = g::Polygon3D::Make({g::Point3D(0, 0, 5), g::Point3D(1, 0, 5), g::Point3D(1, 1, 5), g::Point3D(0, 1, 5)});

  auto result = a.Intersection(b);
  EXPECT_FALSE(result.has_value());
  EXPECT_FALSE(a.Intersects(b));
}

#pragma endregion

}  // namespace geompp_tests
