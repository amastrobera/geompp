#include "polygon3d.hpp"

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

  // FromWkt is not yet implemented — all calls throw
  EXPECT_ANY_THROW(g::Polygon3D::FromWkt("POLYGON ((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))"));
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
  // FromFile calls FromWkt which is not yet implemented
  std::string path = (test_res_path / "polygon3d" / "polygon.wkt").string();
  ASSERT_TRUE(fs::exists(path));
  EXPECT_ANY_THROW(g::Polygon3D::FromFile(path));
}

}  // namespace geompp_tests
