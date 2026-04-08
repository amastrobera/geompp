#include "ray3d.hpp"

#include "constants.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Ray3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Ray3DTest, Make) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D(3, 0, 0));

  ASSERT_EQ(g::Point3D(1, 2, 0), r.Origin());
  // direction is normalised
  ASSERT_EQ(g::Vector3D(1, 0, 0), r.Direction());

  // zero direction throws
  EXPECT_ANY_THROW(g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(0, 0, 0)));
}

TEST_F(Ray3DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));
  auto r2 = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));
  auto r3 = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(0, 1, 0));
  auto r4 = g::Ray3D::Make(g::Point3D(1, 0, 0), g::Vector3D(1, 0, 0));

  ASSERT_EQ(r1, r2);
  ASSERT_NE(r1, r3);  // different direction
  ASSERT_NE(r1, r4);  // different origin

  // self-equality
  ASSERT_EQ(r1, r1);
}

TEST_F(Ray3DTest, Assignment) {
  auto r1 = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));
  auto r2 = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D(0, 1, 0));

  r2 = r1;
  ASSERT_EQ(r1, r2);

  r1 = r1;
  ASSERT_EQ(g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0)), r1);
}

TEST_F(Ray3DTest, IsAheadIsBehind) {
  geompp::DECIMAL_PRECISION = 4;
  // ray from origin pointing along +X
  auto r = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));

  // points ahead (same direction as DIR)
  ASSERT_TRUE(r.IsAhead(g::Point3D(1, 0, 0)));
  ASSERT_TRUE(r.IsAhead(g::Point3D(5, 3, 0)));  // off-axis but ahead in X
  ASSERT_TRUE(r.IsAhead(g::Point3D(0, 0, 0)));  // at origin: on the boundary (ahead)

  // points behind
  ASSERT_TRUE(r.IsBehind(g::Point3D(-1, 0, 0)));
  ASSERT_TRUE(r.IsBehind(g::Point3D(-1, 5, 0)));

  // IsAhead and IsBehind are complementary (not both true for the same point)
  auto p_ahead = g::Point3D(3, 0, 0);
  auto p_behind = g::Point3D(-3, 0, 0);
  ASSERT_TRUE(r.IsAhead(p_ahead));
  ASSERT_FALSE(r.IsBehind(p_ahead));
  ASSERT_TRUE(r.IsBehind(p_behind));
  ASSERT_FALSE(r.IsAhead(p_behind));
}

TEST_F(Ray3DTest, ToLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D(1, 0, 0));
  auto l = r.ToLine();

  // the resulting Line3D passes through the ray's origin in the same direction
  ASSERT_EQ(g::Point3D(1, 2, 0), l.Origin());
  ASSERT_EQ(g::Vector3D(1, 0, 0), l.Direction());
}

TEST_F(Ray3DTest, IntersectionWithLine3D) {
  // ray along +X from origin; vertical line at x=3 in XY plane
  auto r = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));
  auto l_cross = g::Line3D::Make(g::Point3D(3, -1, 0), g::Point3D(3, 1, 0));

  EXPECT_TRUE(r.Intersects(l_cross));
  auto result = r.Intersection(l_cross);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(g::Point3D(3, 0, 0), std::get<g::Point3D>(*result));

  // line is behind the ray's origin: no intersection
  auto l_behind = g::Line3D::Make(g::Point3D(-3, -1, 0), g::Point3D(-3, 1, 0));
  EXPECT_FALSE(r.Intersects(l_behind));
  EXPECT_FALSE(r.Intersection(l_behind).has_value());

  // parallel lines → no intersection
  auto l_parallel = g::Line3D::Make(g::Point3D(0, 1, 0), g::Point3D(5, 1, 0));
  EXPECT_FALSE(r.Intersects(l_parallel));
  EXPECT_FALSE(r.Intersection(l_parallel).has_value());
}

TEST_F(Ray3DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));
  ASSERT_EQ("RAY (0 0 0, 1 0 0)", r.ToWkt());

  geompp::DECIMAL_PRECISION = 2;
  auto r2 = g::Ray3D::Make(g::Point3D(1.126, 2.354, 0.0), g::Vector3D(1, 0, 0));
  ASSERT_EQ("RAY (1.13 2.35 0, 1 0 0)", r2.ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  // round-trip
  auto r3 = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D(0, 1, 0));
  EXPECT_EQ(r3, g::Ray3D::FromWkt(r3.ToWkt()));

  // invalid: wrong geometry type
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("LINE (0 0 0, 1 0 0)"));
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("angelo"));

  // invalid: missing brackets
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("RAY 0 0 0, 1 0 0)"));
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("RAY (0 0 0, 1 0 0"));

  // invalid: missing comma separator
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("RAY (0 0 0 1 0 0)"));
}

TEST_F(Ray3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "ray3d.wkt").string();
  auto r = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D(1, 0, 0));

  r.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  auto r_file = g::Ray3D::FromFile(path);
  EXPECT_EQ(r, r_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Ray3DTest, TestFromFile) {
  std::string path = (test_res_path / "ray3d" / "ray.wkt").string();

  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Ray3D::FromFile(path));

  auto r = g::Ray3D::FromFile(path);
  GEOMPP_LOG(INFO) << "from file = " << r.ToWkt();
}

}  // namespace geompp_tests
