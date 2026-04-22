#include "polygon3d.hpp"

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

  geompp::DECIMAL_PRECISION = 2;
  auto p2 =
      g::Polygon3D::Make({g::Point3D(1.126, 2.354, 0.5), g::Point3D(5.678, 9.012, 1.0), g::Point3D(3.333, 0.111, 2.0)});
  ASSERT_EQ("POLYGON ((1.13 2.35 0.5, 5.68 9.01 1, 3.33 0.11 2, 1.13 2.35 0.5))", p2.ToWkt());

  // FromWkt is not yet implemented — all calls throw
  EXPECT_ANY_THROW(g::Polygon3D::FromWkt("POLYGON ((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))"));
  EXPECT_ANY_THROW(g::Polygon3D::FromWkt("anything"));
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
