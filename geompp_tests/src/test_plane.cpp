#include "plane.hpp"

#include "line3d.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <tuple>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

class PlaneTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(PlaneTest, Constructor) {
  // From3Points: XY plane (CCW → normal = +Z)
  auto xy_3pts = g::Plane::From3Points(g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  ASSERT_EQ(g::Point3D::Zero(), xy_3pts.origin());
  ASSERT_TRUE(xy_3pts.normal().AlmostEquals(g::Vector3D::BasisZ()));

  // FromOriginAndNormal
  auto xy_norm = g::Plane::FromOriginAndNormal(g::Point3D::Zero(), g::Vector3D::BasisZ());
  ASSERT_EQ(g::Point3D::Zero(), xy_norm.origin());
  ASSERT_TRUE(xy_norm.normal().AlmostEquals(g::Vector3D::BasisZ()));

  // FromOriginAndAxes: explicit u=(1,0,0), v=(0,1,0) → normal = u×v = (0,0,1)
  auto xy_axes = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisX(), g::Vector3D::BasisY());
  ASSERT_EQ(g::Point3D::Zero(), xy_axes.origin());
  ASSERT_TRUE(xy_axes.normal().AlmostEquals(g::Vector3D::BasisZ()));
  ASSERT_TRUE(xy_axes.axis_u().AlmostEquals(g::Vector3D::BasisX()));
  ASSERT_TRUE(xy_axes.axis_v().AlmostEquals(g::Vector3D::BasisY()));

  // Static convenience planes
  ASSERT_EQ(g::Point3D::Zero(), g::Plane::XY().origin());
  ASSERT_TRUE(g::Plane::XY().normal().AlmostEquals(g::Vector3D::BasisZ()));
  ASSERT_TRUE(g::Plane::YZ().normal().AlmostEquals(g::Vector3D::BasisX()));
  ASSERT_TRUE(g::Plane::ZX().normal().AlmostEquals(g::Vector3D::BasisY()));

  // zero-length inputs throw
  EXPECT_ANY_THROW(g::Plane::FromOriginAndNormal(g::Point3D::Zero(), g::Vector3D(0, 0, 0)));
  EXPECT_ANY_THROW(g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D(0, 0, 0), g::Vector3D::BasisY()));
  EXPECT_ANY_THROW(g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisX(), g::Vector3D(0, 0, 0)));
}

TEST_F(PlaneTest, AlmostEquals) {
  // AlmostEquals is not yet implemented — it always throws
  EXPECT_ANY_THROW(g::Plane::XY().AlmostEquals(g::Plane::XY()));
}

TEST_F(PlaneTest, Assignment) {
  auto xy = g::Plane::XY();
  auto yz = g::Plane::YZ();
  yz = xy;
  ASSERT_EQ(g::Point3D::Zero(), yz.origin());
  ASSERT_TRUE(yz.normal().AlmostEquals(g::Vector3D::BasisZ()));
}

TEST_F(PlaneTest, SignedDistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  EXPECT_EQ(5.0, g::round(xy.SignedDistanceTo(g::Point3D(0, 0, 5))));
  EXPECT_EQ(-3.0, g::round(xy.SignedDistanceTo(g::Point3D(0, 0, -3))));
  EXPECT_EQ(0.0, g::round(xy.SignedDistanceTo(g::Point3D(5, 3, 0))));
}

TEST_F(PlaneTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  EXPECT_EQ(5.0, g::round(xy.DistanceTo(g::Point3D(0, 0, 5))));
  EXPECT_EQ(3.0, g::round(xy.DistanceTo(g::Point3D(0, 0, -3))));
  EXPECT_EQ(0.0, g::round(xy.DistanceTo(g::Point3D(5, 3, 0))));
}

TEST_F(PlaneTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  EXPECT_EQ(g::Point3D(5, 3, 0), xy.ProjectOnto(g::Point3D(5, 3, 7)));
  EXPECT_EQ(g::Point3D::Zero(), xy.ProjectOnto(g::Point3D(0, 0, -5)));
  EXPECT_EQ(g::Point3D(1, 2, 0), xy.ProjectOnto(g::Point3D(1, 2, 0)));  // already on plane
}

TEST_F(PlaneTest, ProjectInto) {
  geompp::DECIMAL_PRECISION = 4;
  // Use explicit axes so local 2D coordinates are predictable
  auto xy = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisX(), g::Vector3D::BasisY());

  EXPECT_EQ(g::Point2D(5, 3), xy.ProjectInto(g::Point3D(5, 3, 7)));
  EXPECT_EQ(g::Point2D::Zero(), xy.ProjectInto(g::Point3D(0, 0, -5)));
  EXPECT_EQ(g::Point2D(1, 2), xy.ProjectInto(g::Point3D(1, 2, 0)));
}

TEST_F(PlaneTest, Evaluate) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisX(), g::Vector3D::BasisY());

  EXPECT_EQ(g::Point3D(5, 3, 0), xy.Evaluate(g::Point2D(5, 3)));
  EXPECT_EQ(g::Point3D::Zero(), xy.Evaluate(g::Point2D::Zero()));

  // round-trip: Evaluate(ProjectInto(p)) == ProjectOnto(p)
  auto p = g::Point3D(4, 7, 3);
  EXPECT_EQ(xy.ProjectOnto(p), xy.Evaluate(xy.ProjectInto(p)));
}

TEST_F(PlaneTest, Contains) {
  auto xy = g::Plane::XY();

  ASSERT_TRUE(xy.Contains(g::Point3D::Zero()));
  ASSERT_TRUE(xy.Contains(g::Point3D(5, 3, 0)));
  ASSERT_TRUE(xy.Contains(g::Point3D(-100, 200, 0)));

  ASSERT_FALSE(xy.Contains(g::Point3D(0, 0, 1)));
  ASSERT_FALSE(xy.Contains(g::Point3D(5, 3, -1)));
}

TEST_F(PlaneTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  // vertical line through (5,3) — intersects XY at (5,3,0)
  auto z_line = g::Line3D::Make(g::Point3D(5, 3, -2), g::Point3D(5, 3, 2));
  ASSERT_TRUE(xy.Intersects(z_line));
  {
    auto inter = xy.Intersection(z_line);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(5, 3, 0), std::get<g::Point3D>(*inter));
  }

  // line in the XY plane (parallel) — no intersection
  auto x_line = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  ASSERT_FALSE(xy.Intersects(x_line));
  ASSERT_FALSE(xy.Intersection(x_line).has_value());

  // diagonal line: Make((0,0,-1),(1,0,1)) intersects XY at (0.5,0,0)
  auto diag = g::Line3D::Make(g::Point3D(0, 0, -1), g::Point3D(1, 0, 1));
  ASSERT_TRUE(xy.Intersects(diag));
  {
    auto inter = xy.Intersection(diag);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(0.5, 0, 0), std::get<g::Point3D>(*inter));
  }
}

TEST_F(PlaneTest, EqualityOperator) {
  // operator== delegates to AlmostEquals, which is not yet implemented — verify it throws
  EXPECT_ANY_THROW(g::Plane::XY() == g::Plane::XY());
  EXPECT_ANY_THROW(g::Plane::XY() == g::Plane::YZ());
}

}  // namespace geompp_tests
