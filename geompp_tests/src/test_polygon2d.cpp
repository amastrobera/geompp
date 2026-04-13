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
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_EQ(4, p.Size());

  ASSERT_EQ(g::Point2D(0, 0), p[0]);
  ASSERT_EQ(g::Point2D(1, 0), p[1]);
  ASSERT_EQ(g::Point2D(1, 1), p[2]);
  ASSERT_EQ(g::Point2D(0, 1), p[3]);

  EXPECT_ANY_THROW(p[4]);  // out of range

  // fewer than 3 unique points throws
  EXPECT_ANY_THROW(g::Polygon2D::Make({}));
  EXPECT_ANY_THROW(g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(0, 0), g::Point2D(0, 0)}));
  EXPECT_ANY_THROW(g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0)}));
}

TEST_F(Polygon2DTest, AlmostEquals) {
  auto p1 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p2 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p3 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 2)});

  ASSERT_TRUE(p1.AlmostEquals(p2));
  ASSERT_FALSE(p1.AlmostEquals(p3));
  ASSERT_EQ(p1, p2);
  ASSERT_NE(p1, p3);

  // different sizes are not equal
  auto p4 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_FALSE(p1.AlmostEquals(p4));
}

TEST_F(Polygon2DTest, Assignment) {
  auto p1 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p2 = g::Polygon2D::Make({g::Point2D(5, 5), g::Point2D(6, 5), g::Point2D(6, 6)});
  p2 = p1;
  ASSERT_EQ(p1, p2);
}

TEST_F(Polygon2DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_EQ("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))", p.ToWkt());

  geompp::DECIMAL_PRECISION = 2;
  auto p2 = g::Polygon2D::Make({g::Point2D(1.126, 2.354), g::Point2D(5.678, 9.012), g::Point2D(3.333, 0.111)});
  ASSERT_EQ("POLYGON ((1.13 2.35, 5.68 9.01, 3.33 0.11, 1.13 2.35))", p2.ToWkt());

  // FromWkt is not yet implemented — all calls throw
  EXPECT_ANY_THROW(g::Polygon2D::FromWkt("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))"));
  EXPECT_ANY_THROW(g::Polygon2D::FromWkt("anything"));
}

TEST_F(Polygon2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "polygon2d.wkt").string();
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});

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
