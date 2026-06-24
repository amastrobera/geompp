#include "polygon3d.hpp"

#include "line_segment3d.hpp"
#include "plane.hpp"
#include "point3d.hpp"
#include "utils.hpp"

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

// ---- Perimeter --------------------------------------------------------------

TEST_F(Polygon3DTest, Perimeter_Square) {
  // 1×1 square on XY → 4 sides of length 1
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(1, 0, 0),
      g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  EXPECT_NEAR(4.0, p.Perimeter(), 1e-9);
}

TEST_F(Polygon3DTest, Perimeter_NonXYPlane) {
  // 1×1 square on the YZ plane → same perimeter = 4
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(0, 1, 0),
      g::Point3D(0, 1, 1), g::Point3D(0, 0, 1)});
  EXPECT_NEAR(4.0, p.Perimeter(), 1e-9);
}

TEST_F(Polygon3DTest, Perimeter_Triangle) {
  // 3-4-5 right triangle → perimeter = 12
  auto p = g::Polygon3D::Make({
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 3, 0)});
  EXPECT_NEAR(12.0, p.Perimeter(), 1e-9);
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
  for (int i = 0; i < p.Size(); ++i)
    EXPECT_TRUE(pl.Contains(p[i]));
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
  EXPECT_ANY_THROW(poly.DistanceTo(g::Point3D(0.5, 0.5, 0)));
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

TEST_F(Polygon3DTest, IsOnBoundary_True) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});

  // all four vertices
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(0,   0,   0)));
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(1,   0,   0)));
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(1,   1,   0)));
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(0,   1,   0)));

  // edge midpoints
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(0.5, 0,   0)));  // bottom
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(1,   0.5, 0)));  // right
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(0.5, 1,   0)));  // top
  EXPECT_TRUE(sq.IsOnBoundary(g::Point3D(0,   0.5, 0)));  // left

  // non-XY plane: YZ square at x=0
  auto yz = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(0,1,0), g::Point3D(0,1,1), g::Point3D(0,0,1)});
  EXPECT_TRUE(yz.IsOnBoundary(g::Point3D(0, 0.5, 0)));  // bottom edge midpoint
  EXPECT_TRUE(yz.IsOnBoundary(g::Point3D(0, 0,   0)));  // vertex

  // hole boundary
  auto outer = std::vector<g::Point3D>{{0,0,0}, {4,0,0}, {4,4,0}, {0,4,0}};
  auto hole  = std::vector<g::Point3D>{{1,1,0}, {1,3,0}, {3,3,0}, {3,1,0}};
  auto poly  = g::Polygon3D::Make(outer, {hole});
  EXPECT_TRUE(poly.IsOnBoundary(g::Point3D(2, 0, 0)));  // outer bottom
  EXPECT_TRUE(poly.IsOnBoundary(g::Point3D(4, 2, 0)));  // outer right
  EXPECT_TRUE(poly.IsOnBoundary(g::Point3D(2, 1, 0)));  // hole bottom
  EXPECT_TRUE(poly.IsOnBoundary(g::Point3D(1, 2, 0)));  // hole left
}

TEST_F(Polygon3DTest, IsOnBoundary_False) {
  auto sq = g::Polygon3D::Make({g::Point3D(0,0,0), g::Point3D(1,0,0), g::Point3D(1,1,0), g::Point3D(0,1,0)});

  EXPECT_FALSE(sq.IsOnBoundary(g::Point3D(0.5, 0.5, 0)));   // interior
  EXPECT_FALSE(sq.IsOnBoundary(g::Point3D(-0.1, 0.5, 0)));  // outside left
  EXPECT_FALSE(sq.IsOnBoundary(g::Point3D(0.5,  0.5, 0.01)));  // off-plane

  // interior of polygon with hole is not boundary
  auto outer = std::vector<g::Point3D>{{0,0,0}, {4,0,0}, {4,4,0}, {0,4,0}};
  auto hole  = std::vector<g::Point3D>{{1,1,0}, {1,3,0}, {3,3,0}, {3,1,0}};
  auto poly  = g::Polygon3D::Make(outer, {hole});
  EXPECT_FALSE(poly.IsOnBoundary(g::Point3D(0.5, 0.5, 0)));  // interior strip
  EXPECT_FALSE(poly.IsOnBoundary(g::Point3D(2,   2,   0)));  // inside hole
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

}  // namespace geompp_tests
