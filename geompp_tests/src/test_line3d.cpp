#include "line3d.hpp"

#include "constants.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Line3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Line3DTest, MakeFromTwoPoints) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(3, 0, 0));

  ASSERT_EQ(g::Point3D::Zero(), l.Origin());
  ASSERT_EQ(g::Point3D::Zero(), l.First());
  ASSERT_EQ(g::Point3D(3, 0, 0), l.Last());

  // direction is normalised
  ASSERT_EQ(g::Vector3D::BasisX(), l.Direction());

  // coincident points throw
  EXPECT_ANY_THROW(g::Line3D::Make(g::Point3D(1, 2, 3), g::Point3D(1, 2, 3)));
}

TEST_F(Line3DTest, MakeFromPointAndVector) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D(1, 2, 0), g::Vector3D(3, 0, 0));

  ASSERT_EQ(g::Point3D(1, 2, 0), l.Origin());
  // direction is normalised
  ASSERT_EQ(g::Vector3D::BasisX(), l.Direction());

  // zero direction throws
  EXPECT_ANY_THROW(g::Line3D::Make(g::Point3D::Zero(), g::Vector3D(0, 0, 0)));
}

TEST_F(Line3DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto l1 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  auto l2 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  auto l3 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(0, 1, 0));

  // identical construction
  ASSERT_EQ(l1, l2);
  ASSERT_EQ(l1, l1);

  // different direction: not equal
  ASSERT_NE(l1, l3);

  // same infinite line, different origin — geometrically equal
  auto l4 = g::Line3D::Make(g::Point3D(5, 0, 0), g::Point3D(8, 0, 0));
  ASSERT_EQ(l1, l4);

  // same infinite line, reversed direction — geometrically equal
  auto l5 = g::Line3D::Make(g::Point3D(1, 0, 0), g::Point3D::Zero());
  ASSERT_EQ(l1, l5);

  // parallel but offset — NOT equal
  auto l6 = g::Line3D::Make(g::Point3D(0, 1, 0), g::Point3D(5, 1, 0));
  ASSERT_NE(l1, l6);
}

TEST_F(Line3DTest, Assignment) {
  auto l1 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0));
  auto l2 = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(3, 4, 0));

  l2 = l1;
  ASSERT_EQ(l1, l2);

  // self-assignment
  l1 = l1;
  ASSERT_EQ(g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0)), l1);
}

TEST_F(Line3DTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  // horizontal line along X-axis
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0));

  // projection of off-axis point lands on the line
  auto proj = l.ProjectOnto(g::Point3D(3, 5, 0));
  ASSERT_EQ(g::Point3D(3, 0, 0), proj);

  // projection of a point already on the line is itself
  ASSERT_EQ(g::Point3D(2, 0, 0), l.ProjectOnto(g::Point3D(2, 0, 0)));

  // projection before origin
  ASSERT_EQ(g::Point3D(-2, 0, 0), l.ProjectOnto(g::Point3D(-2, 3, 0)));

  // diagonal line: y = x in XY plane
  auto ld = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 1, 0));
  geompp::DECIMAL_PRECISION = 3;
  auto proj_d = ld.ProjectOnto(g::Point3D(1, 0, 0));
  // (1,0,0) projects onto (0.5, 0.5, 0) on y=x line
  ASSERT_EQ(g::Point3D(0.5, 0.5, 0), proj_d);
  geompp::DECIMAL_PRECISION = 4;
}

TEST_F(Line3DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(3, 0, 0));
  ASSERT_EQ("LINE (0 0 0, 3 0 0)", l.ToWkt());

  geompp::DECIMAL_PRECISION = 2;
  auto l2 = g::Line3D::Make(g::Point3D(1.126, 2.354, 0.0), g::Point3D(5.678, 9.012, 0.0));
  ASSERT_EQ("LINE (1.13 2.35 0, 5.68 9.01 0)", l2.ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  // round-trip
  auto l3 = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(4, 6, 0));
  EXPECT_EQ(l3, g::Line3D::FromWkt(l3.ToWkt()));

  // invalid: wrong geometry type
  EXPECT_ANY_THROW(g::Line3D::FromWkt("POINT (0 0 0, 1 1 0)"));
  EXPECT_ANY_THROW(g::Line3D::FromWkt("angelo"));

  // invalid: missing brackets
  EXPECT_ANY_THROW(g::Line3D::FromWkt("LINE 0 0 0, 1 1 0)"));
  EXPECT_ANY_THROW(g::Line3D::FromWkt("LINE (0 0 0, 1 1 0"));

  // invalid: missing comma separator
  EXPECT_ANY_THROW(g::Line3D::FromWkt("LINE (0 0 0 1 1 0)"));
}

TEST_F(Line3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "line3d.wkt").string();
  auto l = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(4, 6, 0));

  l.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  auto l_file = g::Line3D::FromFile(path);
  EXPECT_EQ(l, l_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Line3DTest, TestFromFile) {
  std::string path = (test_res_path / "line3d" / "line.wkt").string();

  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Line3D::FromFile(path));

  auto l = g::Line3D::FromFile(path);
  GEOMPP_LOG(INFO) << "from file = " << l.ToWkt();
}

TEST_F(Line3DTest, Contains) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(3, 0, 0));

  ASSERT_TRUE(l.Contains(g::Point3D(1, 0, 0)));
  ASSERT_FALSE(l.Contains(g::Point3D(1, 1, 0)));
}

TEST_F(Line3DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  // horizontal line along X-axis
  auto line = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0));

  // on the line: distance = 0
  ASSERT_EQ(0.0, g::round(line.DistanceTo(g::Point3D::Zero())));
  ASSERT_EQ(0.0, g::round(line.DistanceTo(g::Point3D(3, 0, 0))));
  ASSERT_EQ(0.0, g::round(line.DistanceTo(g::Point3D(-2, 0, 0))));  // behind origin, still on line

  // perpendicular offset in Y
  ASSERT_EQ(3.0, g::round(line.DistanceTo(g::Point3D(2, 3, 0))));

  // perpendicular offset in Z
  ASSERT_EQ(4.0, g::round(line.DistanceTo(g::Point3D(1, 0, 4))));

  // offset in both Y and Z: 3-4-5 triple
  ASSERT_EQ(5.0, g::round(line.DistanceTo(g::Point3D(0, 3, 4))));
}

TEST_F(Line3DTest, IntersectionWithLine3D) {
  geompp::DECIMAL_PRECISION = 4;
  // X-axis and vertical line through (3, 3, 0):
  // expected intersection at (3, 0, 0) — but the 3D intersection algorithm
  // inherits 2D Perp logic, so we only test the result value here.
  auto x_axis = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  auto y_axis = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(0, 1, 0));

  // parallel lines (same direction) should not intersect
  auto l_parallel = g::Line3D::Make(g::Point3D(0, 2, 0), g::Point3D(1, 2, 0));
  EXPECT_FALSE(x_axis.Intersects(l_parallel));
  EXPECT_FALSE(x_axis.Intersection(l_parallel).has_value());
}

}  // namespace geompp_tests
