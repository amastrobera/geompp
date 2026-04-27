#include "polygon2d.hpp"

#include "point2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Polygon2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Polygon2DTest, Constructor) {
  auto p = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_EQ(4, p.Size());

  ASSERT_EQ(g::Point2D::Zero(), p[0]);
  ASSERT_EQ(g::Point2D(1, 0), p[1]);
  ASSERT_EQ(g::Point2D(1, 1), p[2]);
  ASSERT_EQ(g::Point2D(0, 1), p[3]);

  EXPECT_ANY_THROW(p[4]);  // out of range

  // fewer than 3 unique points throws
  EXPECT_ANY_THROW(g::Polygon2D::Make({}));
  EXPECT_ANY_THROW(g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D::Zero(), g::Point2D::Zero()}));
  EXPECT_ANY_THROW(g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0)}));

  // CW outer ring throws
  EXPECT_ANY_THROW(
      g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(0, 1), g::Point2D(1, 1), g::Point2D(1, 0)}));
}

TEST_F(Polygon2DTest, AlmostEquals) {
  auto p1 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p2 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p3 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(2, 0), g::Point2D(2, 2)});

  ASSERT_TRUE(p1.AlmostEquals(p2));
  ASSERT_FALSE(p1.AlmostEquals(p3));
  ASSERT_EQ(p1, p2);
  ASSERT_NE(p1, p3);

  // different sizes are not equal
  auto p4 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_FALSE(p1.AlmostEquals(p4));
}

TEST_F(Polygon2DTest, Assignment) {
  auto p1 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p2 = g::Polygon2D::Make({g::Point2D(5, 5), g::Point2D(6, 5), g::Point2D(6, 6)});
  p2 = p1;
  ASSERT_EQ(p1, p2);
}

TEST_F(Polygon2DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto p = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_EQ("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))", p.ToWkt());

  // CCW triangle at DP=2
  geompp::DECIMAL_PRECISION = 2;
  auto p2 = g::Polygon2D::Make({g::Point2D(0.123, 0.456), g::Point2D(8.789, 0.123), g::Point2D(4.321, 7.654)});
  ASSERT_EQ("POLYGON ((0.12 0.46, 8.79 0.12, 4.32 7.65, 0.12 0.46))", p2.ToWkt());

  // FromWkt is not yet implemented — all calls throw
  EXPECT_ANY_THROW(g::Polygon2D::FromWkt("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))"));
  EXPECT_ANY_THROW(g::Polygon2D::FromWkt("anything"));
}

TEST_F(Polygon2DTest, WktWithHoles) {
  geompp::DECIMAL_PRECISION = 4;
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto p = g::Polygon2D::Make(outer, {hole});
  ASSERT_EQ("POLYGON ((0 0, 4 0, 4 4, 0 4, 0 0), (1 1, 1 3, 3 3, 3 1, 1 1))", p.ToWkt());
}

TEST_F(Polygon2DTest, WithHoles_Valid) {
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto p = g::Polygon2D::Make(outer, {hole});
  ASSERT_EQ(4, p.Size());  // outer ring has 4 vertices
}

TEST_F(Polygon2DTest, WithHoles_PerimeterCW_Throws) {
  std::vector<g::Point2D> cw_outer = {
      g::Point2D(0, 0), g::Point2D(0, 4), g::Point2D(4, 4), g::Point2D(4, 0)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  EXPECT_ANY_THROW(g::Polygon2D::Make(cw_outer, {hole}));
}

TEST_F(Polygon2DTest, WithHoles_HoleCCW_Throws) {
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> ccw_hole = {
      g::Point2D(1, 1), g::Point2D(3, 1), g::Point2D(3, 3), g::Point2D(1, 3)};
  EXPECT_ANY_THROW(g::Polygon2D::Make(outer, {ccw_hole}));
}

TEST_F(Polygon2DTest, WithHoles_HoleTooFewPoints_Throws) {
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> bad_hole = {g::Point2D(1, 1), g::Point2D(2, 1)};
  EXPECT_ANY_THROW(g::Polygon2D::Make(outer, {bad_hole}));
}

TEST_F(Polygon2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "polygon2d.wkt").string();
  auto p = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});

  p.ToFile(path);
  ASSERT_TRUE(fs::exists(path));
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Polygon2DTest, FromFile) {
  // FromFile calls FromWkt which is not yet implemented
  std::string path = (test_res_path / "polygon2d" / "polygon.wkt").string();
  ASSERT_TRUE(fs::exists(path));
  EXPECT_ANY_THROW(g::Polygon2D::FromFile(path));
}

}  // namespace geompp_tests
