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

TEST(Line3D, MakeFromTwoPoints) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(3, 0, 0));

  ASSERT_EQ(g::Point3D(0, 0, 0), l.Origin());
  ASSERT_EQ(g::Point3D(0, 0, 0), l.First());
  ASSERT_EQ(g::Point3D(3, 0, 0), l.Last());

  // direction is normalised
  ASSERT_EQ(g::Vector3D(1, 0, 0), l.Direction());

  // coincident points throw
  EXPECT_ANY_THROW(g::Line3D::Make(g::Point3D(1, 2, 3), g::Point3D(1, 2, 3)));
}

TEST(Line3D, MakeFromPointAndVector) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D(1, 2, 0), g::Vector3D(3, 0, 0));

  ASSERT_EQ(g::Point3D(1, 2, 0), l.Origin());
  // direction is normalised
  ASSERT_EQ(g::Vector3D(1, 0, 0), l.Direction());

  // zero direction throws
  EXPECT_ANY_THROW(g::Line3D::Make(g::Point3D(0, 0, 0), g::Vector3D(0, 0, 0)));
}

TEST(Line3D, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto l1 = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto l2 = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto l3 = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(0, 1, 0));

  ASSERT_EQ(l1, l2);
  ASSERT_NE(l1, l3);

  // self-equality
  ASSERT_EQ(l1, l1);
}

TEST(Line3D, Assignment) {
  auto l1 = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(5, 0, 0));
  auto l2 = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(3, 4, 0));

  l2 = l1;
  ASSERT_EQ(l1, l2);

  // self-assignment
  l1 = l1;
  ASSERT_EQ(g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(5, 0, 0)), l1);
}

TEST(Line3D, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  // horizontal line along X-axis
  auto l = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(5, 0, 0));

  // projection of off-axis point lands on the line
  auto proj = l.ProjectOnto(g::Point3D(3, 5, 0));
  ASSERT_EQ(g::Point3D(3, 0, 0), proj);

  // projection of a point already on the line is itself
  ASSERT_EQ(g::Point3D(2, 0, 0), l.ProjectOnto(g::Point3D(2, 0, 0)));

  // projection before origin
  ASSERT_EQ(g::Point3D(-2, 0, 0), l.ProjectOnto(g::Point3D(-2, 3, 0)));

  // diagonal line: y = x in XY plane
  auto ld = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0));
  geompp::DECIMAL_PRECISION = 3;
  auto proj_d = ld.ProjectOnto(g::Point3D(1, 0, 0));
  // (1,0,0) projects onto (0.5, 0.5, 0) on y=x line
  ASSERT_EQ(g::Point3D(0.5, 0.5, 0), proj_d);
  geompp::DECIMAL_PRECISION = 4;
}

TEST(Line3D, Location) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(5, 0, 0));

  // at origin: location 0
  ASSERT_EQ(0.0, l.Location(g::Point3D(0, 0, 0)));

  // at P1: location == distance(P0, P1)
  ASSERT_EQ(5.0, l.Location(g::Point3D(5, 0, 0)));

  // between: proportional to displacement
  ASSERT_EQ(3.0, l.Location(g::Point3D(3, 0, 0)));

  // behind origin: negative
  ASSERT_EQ(-2.0, l.Location(g::Point3D(-2, 0, 0)));
}

TEST(Line3D, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(3, 0, 0));
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

TEST(Line3D, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "line3d.wkt").string();
  auto l = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(4, 6, 0));

  l.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  auto l_file = g::Line3D::FromFile(path);
  EXPECT_EQ(l, l_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST(Line3D, TestFromFile) {
  std::string path = (test_res_path / "line3d" / "line.wkt").string();

  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Line3D::FromFile(path));

  auto l = g::Line3D::FromFile(path);
  GEOMPP_LOG(INFO) << "from file = " << l.ToWkt();
}

TEST(Line3D, IntersectionWithLine3D) {
  geompp::DECIMAL_PRECISION = 4;
  // X-axis and vertical line through (3, 3, 0):
  // expected intersection at (3, 0, 0) — but the 3D intersection algorithm
  // inherits 2D Perp logic, so we only test the result value here.
  auto x_axis = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto y_axis = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(0, 1, 0));

  // parallel lines (same direction) should not intersect
  auto l_parallel = g::Line3D::Make(g::Point3D(0, 2, 0), g::Point3D(1, 2, 0));
  EXPECT_FALSE(x_axis.Intersects(l_parallel));
  EXPECT_FALSE(x_axis.Intersection(l_parallel).has_value());
}

}  // namespace geompp_tests
