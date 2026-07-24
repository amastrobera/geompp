#include "plane.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "ray3d.hpp"
#include "triangle3d.hpp"
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

  // FromOriginAndAxes with non-unit axes: constructor must normalize AxisU and AxisV
  auto xy_scaled = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D(2, 0, 0), g::Vector3D(0, 3, 0));
  ASSERT_TRUE(xy_scaled.normal().AlmostEquals(g::Vector3D::BasisZ()));
  ASSERT_TRUE(xy_scaled.axis_u().AlmostEquals(g::Vector3D::BasisX()));
  ASSERT_TRUE(xy_scaled.axis_v().AlmostEquals(g::Vector3D::BasisY()));
  ASSERT_NEAR(1.0, xy_scaled.axis_u().Length(), 1e-9);
  ASSERT_NEAR(1.0, xy_scaled.axis_v().Length(), 1e-9);

  // From3Points with non-unit span vectors: same normalization applies
  auto xy_3pts_scaled = g::Plane::From3Points(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 3, 0));
  ASSERT_TRUE(xy_3pts_scaled.normal().AlmostEquals(g::Vector3D::BasisZ()));
  ASSERT_TRUE(xy_3pts_scaled.axis_u().AlmostEquals(g::Vector3D::BasisX()));
  ASSERT_TRUE(xy_3pts_scaled.axis_v().AlmostEquals(g::Vector3D::BasisY()));
  ASSERT_NEAR(1.0, xy_3pts_scaled.axis_u().Length(), 1e-9);
  ASSERT_NEAR(1.0, xy_3pts_scaled.axis_v().Length(), 1e-9);

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
  EXPECT_TRUE(g::Plane::XY().AlmostEquals(g::Plane::XY()));
  EXPECT_FALSE(g::Plane::XY().AlmostEquals(g::Plane::YZ()));
  EXPECT_FALSE(g::Plane::XY().AlmostEquals(g::Plane::ZX()));

  // anti-parallel normals are no longer the same plane — opposite winding is a real difference
  EXPECT_FALSE(g::Plane::FromOriginAndNormal(g::Point3D::Zero(), g::Vector3D::BasisZ())
                   .AlmostEquals(g::Plane::FromOriginAndNormal(g::Point3D::Zero(), -g::Vector3D::BasisZ())));

  // same normal, different offset
  EXPECT_FALSE(g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 1), g::Vector3D::BasisZ())
                   .AlmostEquals(g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 2), g::Vector3D::BasisZ())));

  // same normal, different origin point, but on the same actual plane
  EXPECT_TRUE(g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 1), g::Vector3D::BasisZ())
                  .AlmostEquals(g::Plane::FromOriginAndNormal(g::Point3D(5, 3, 1), g::Vector3D::BasisZ())));

  // same plane (coplanar origins, dot product exactly 0), checked with a loose custom epsilon — the
  // coplanarity test must compare the dot product against zero using epsilon as the tolerance, not
  // compare the dot product against epsilon itself
  auto p1 = g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 1), g::Vector3D::BasisZ());
  auto p2 = g::Plane::FromOriginAndNormal(g::Point3D(5, 3, 1), g::Vector3D::BasisZ());
  EXPECT_TRUE(p1.AlmostEquals(p2, 0.5));

  // nearly (but not exactly) coplanar origins: within a loose epsilon, still equal; within a tight one, not
  auto p3 = g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 1.05), g::Vector3D::BasisZ());
  EXPECT_TRUE(p1.AlmostEquals(p3, 0.5));
  EXPECT_FALSE(p1.AlmostEquals(p3, 0.001));
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
  auto xy = g::Plane::XY();  // normal = +Z

  EXPECT_EQ(5.0, g::round(xy.SignedDistanceTo(g::Point3D(0, 0, 5))));
  EXPECT_EQ(-3.0, g::round(xy.SignedDistanceTo(g::Point3D(0, 0, -3))));
  EXPECT_EQ(0.0, g::round(xy.SignedDistanceTo(g::Point3D(5, 3, 0))));

  auto yz = g::Plane::YZ();  // normal = +X

  EXPECT_EQ(5.0, g::round(yz.SignedDistanceTo(g::Point3D(5, 0, 0))));
  EXPECT_EQ(-3.0, g::round(yz.SignedDistanceTo(g::Point3D(-3, 0, 0))));
  EXPECT_EQ(0.0, g::round(yz.SignedDistanceTo(g::Point3D(0, 5, 3))));

  auto zx = g::Plane::ZX();  // normal = +Y

  EXPECT_EQ(5.0, g::round(zx.SignedDistanceTo(g::Point3D(0, 5, 0))));
  EXPECT_EQ(-3.0, g::round(zx.SignedDistanceTo(g::Point3D(0, -3, 0))));
  EXPECT_EQ(0.0, g::round(zx.SignedDistanceTo(g::Point3D(5, 0, 3))));
}

TEST_F(PlaneTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();  // normal = +Z

  EXPECT_EQ(5.0, g::round(xy.DistanceTo(g::Point3D(0, 0, 5))));
  EXPECT_EQ(3.0, g::round(xy.DistanceTo(g::Point3D(0, 0, -3))));
  EXPECT_EQ(0.0, g::round(xy.DistanceTo(g::Point3D(5, 3, 0))));

  auto yz = g::Plane::YZ();  // normal = +X

  EXPECT_EQ(5.0, g::round(yz.DistanceTo(g::Point3D(5, 0, 0))));
  EXPECT_EQ(3.0, g::round(yz.DistanceTo(g::Point3D(-3, 0, 0))));
  EXPECT_EQ(0.0, g::round(yz.DistanceTo(g::Point3D(0, 5, 3))));

  auto zx = g::Plane::ZX();  // normal = +Y

  EXPECT_EQ(5.0, g::round(zx.DistanceTo(g::Point3D(0, 5, 0))));
  EXPECT_EQ(3.0, g::round(zx.DistanceTo(g::Point3D(0, -3, 0))));
  EXPECT_EQ(0.0, g::round(zx.DistanceTo(g::Point3D(5, 0, 3))));
}

TEST_F(PlaneTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();  // normal = +Z, drops Z

  EXPECT_EQ(g::Point3D(5, 3, 0), xy.ProjectOnto(g::Point3D(5, 3, 7)));
  EXPECT_EQ(g::Point3D::Zero(), xy.ProjectOnto(g::Point3D(0, 0, -5)));
  EXPECT_EQ(g::Point3D(1, 2, 0), xy.ProjectOnto(g::Point3D(1, 2, 0)));  // already on plane

  auto yz = g::Plane::YZ();  // normal = +X, drops X

  EXPECT_EQ(g::Point3D(0, 5, 3), yz.ProjectOnto(g::Point3D(7, 5, 3)));
  EXPECT_EQ(g::Point3D::Zero(), yz.ProjectOnto(g::Point3D(-5, 0, 0)));
  EXPECT_EQ(g::Point3D(0, 1, 2), yz.ProjectOnto(g::Point3D(0, 1, 2)));  // already on plane

  auto zx = g::Plane::ZX();  // normal = +Y, drops Y

  EXPECT_EQ(g::Point3D(5, 0, 3), zx.ProjectOnto(g::Point3D(5, 7, 3)));
  EXPECT_EQ(g::Point3D::Zero(), zx.ProjectOnto(g::Point3D(0, -5, 0)));
  EXPECT_EQ(g::Point3D(1, 0, 2), zx.ProjectOnto(g::Point3D(1, 0, 2)));  // already on plane
}

TEST_F(PlaneTest, ProjectInto) {
  geompp::DECIMAL_PRECISION = 4;
  // Use explicit axes so local 2D coordinates are predictable
  auto xy = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisX(), g::Vector3D::BasisY());
  // u=+X, v=+Y → 2D coords are (X, Y)

  EXPECT_EQ(g::Point2D(5, 3), xy.ProjectInto(g::Point3D(5, 3, 7)));
  EXPECT_EQ(g::Point2D::Zero(), xy.ProjectInto(g::Point3D(0, 0, -5)));
  EXPECT_EQ(g::Point2D(1, 2), xy.ProjectInto(g::Point3D(1, 2, 0)));

  auto yz = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisY(), g::Vector3D::BasisZ());
  // u=+Y, v=+Z → 2D coords are (Y, Z)

  EXPECT_EQ(g::Point2D(5, 3), yz.ProjectInto(g::Point3D(7, 5, 3)));
  EXPECT_EQ(g::Point2D::Zero(), yz.ProjectInto(g::Point3D(-5, 0, 0)));
  EXPECT_EQ(g::Point2D(1, 2), yz.ProjectInto(g::Point3D(0, 1, 2)));

  auto zx = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisZ(), g::Vector3D::BasisX());
  // u=+Z, v=+X → 2D coords are (Z, X)

  EXPECT_EQ(g::Point2D(3, 5), zx.ProjectInto(g::Point3D(5, 7, 3)));
  EXPECT_EQ(g::Point2D::Zero(), zx.ProjectInto(g::Point3D(0, -5, 0)));
  EXPECT_EQ(g::Point2D(2, 1), zx.ProjectInto(g::Point3D(1, 0, 2)));
}

TEST_F(PlaneTest, Evaluate) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisX(), g::Vector3D::BasisY());
  // u=+X, v=+Y → Evaluate(a,b) = (a, b, 0)

  EXPECT_EQ(g::Point3D(5, 3, 0), xy.Evaluate(g::Point2D(5, 3)));
  EXPECT_EQ(g::Point3D::Zero(), xy.Evaluate(g::Point2D::Zero()));
  auto p_xy = g::Point3D(4, 7, 3);
  EXPECT_EQ(xy.ProjectOnto(p_xy), xy.Evaluate(xy.ProjectInto(p_xy)));

  auto yz = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisY(), g::Vector3D::BasisZ());
  // u=+Y, v=+Z → Evaluate(a,b) = (0, a, b)

  EXPECT_EQ(g::Point3D(0, 5, 3), yz.Evaluate(g::Point2D(5, 3)));
  EXPECT_EQ(g::Point3D::Zero(), yz.Evaluate(g::Point2D::Zero()));
  auto p_yz = g::Point3D(7, 4, 3);
  EXPECT_EQ(yz.ProjectOnto(p_yz), yz.Evaluate(yz.ProjectInto(p_yz)));

  auto zx = g::Plane::FromOriginAndAxes(g::Point3D::Zero(), g::Vector3D::BasisZ(), g::Vector3D::BasisX());
  // u=+Z, v=+X → Evaluate(a,b) = (b, 0, a)

  EXPECT_EQ(g::Point3D(5, 0, 3), zx.Evaluate(g::Point2D(3, 5)));
  EXPECT_EQ(g::Point3D::Zero(), zx.Evaluate(g::Point2D::Zero()));
  auto p_zx = g::Point3D(4, 7, 3);
  EXPECT_EQ(zx.ProjectOnto(p_zx), zx.Evaluate(zx.ProjectInto(p_zx)));
}

TEST_F(PlaneTest, Contains) {
  auto xy = g::Plane::XY();  // Z = 0

  ASSERT_TRUE(xy.Contains(g::Point3D::Zero()));
  ASSERT_TRUE(xy.Contains(g::Point3D(5, 3, 0)));
  ASSERT_TRUE(xy.Contains(g::Point3D(-100, 200, 0)));
  ASSERT_FALSE(xy.Contains(g::Point3D(0, 0, 1)));
  ASSERT_FALSE(xy.Contains(g::Point3D(5, 3, -1)));

  auto yz = g::Plane::YZ();  // X = 0

  ASSERT_TRUE(yz.Contains(g::Point3D::Zero()));
  ASSERT_TRUE(yz.Contains(g::Point3D(0, 5, 3)));
  ASSERT_TRUE(yz.Contains(g::Point3D(0, -100, 200)));
  ASSERT_FALSE(yz.Contains(g::Point3D(1, 0, 0)));
  ASSERT_FALSE(yz.Contains(g::Point3D(-1, 5, 3)));

  auto zx = g::Plane::ZX();  // Y = 0

  ASSERT_TRUE(zx.Contains(g::Point3D::Zero()));
  ASSERT_TRUE(zx.Contains(g::Point3D(5, 0, 3)));
  ASSERT_TRUE(zx.Contains(g::Point3D(-100, 0, 200)));
  ASSERT_FALSE(zx.Contains(g::Point3D(0, 1, 0)));
  ASSERT_FALSE(zx.Contains(g::Point3D(5, -1, 3)));
}

TEST_F(PlaneTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();  // Z = 0

  // line along Z through (5,3) — intersects XY at (5,3,0)
  auto z_line = g::Line3D::Make(g::Point3D(5, 3, -2), g::Point3D(5, 3, 2));
  ASSERT_TRUE(xy.Intersects(z_line));
  {
    auto inter = xy.Intersection(z_line);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(5, 3, 0), std::get<g::Point3D>(*inter));
  }

  // line lying in XY — the whole line lies in the plane, reported as a Line3D
  auto x_line = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  ASSERT_TRUE(xy.Intersects(x_line));
  {
    auto inter = xy.Intersection(x_line);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    EXPECT_EQ(x_line, std::get<g::Line3D>(*inter));
  }

  // diagonal: (0,0,-1)→(1,0,1) crosses Z=0 at (0.5,0,0)
  auto diag_xy = g::Line3D::Make(g::Point3D(0, 0, -1), g::Point3D(1, 0, 1));
  ASSERT_TRUE(xy.Intersects(diag_xy));
  {
    auto inter = xy.Intersection(diag_xy);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(0.5, 0, 0), std::get<g::Point3D>(*inter));
  }

  auto yz = g::Plane::YZ();  // X = 0

  // line along X through (5,3) in YZ — intersects YZ at (0,5,3)
  auto x_line2 = g::Line3D::Make(g::Point3D(-2, 5, 3), g::Point3D(2, 5, 3));
  ASSERT_TRUE(yz.Intersects(x_line2));
  {
    auto inter = yz.Intersection(x_line2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(0, 5, 3), std::get<g::Point3D>(*inter));
  }

  // line lying in YZ — the whole line lies in the plane, reported as a Line3D
  auto y_line = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(0, 1, 0));
  ASSERT_TRUE(yz.Intersects(y_line));
  {
    auto inter = yz.Intersection(y_line);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    EXPECT_EQ(y_line, std::get<g::Line3D>(*inter));
  }

  // diagonal: (-1,0,0)→(1,0,1) crosses X=0 at (0,0,0.5)
  auto diag_yz = g::Line3D::Make(g::Point3D(-1, 0, 0), g::Point3D(1, 0, 1));
  ASSERT_TRUE(yz.Intersects(diag_yz));
  {
    auto inter = yz.Intersection(diag_yz);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(0, 0, 0.5), std::get<g::Point3D>(*inter));
  }

  auto zx = g::Plane::ZX();  // Y = 0

  // line along Y through (5,3) in ZX — intersects ZX at (5,0,3)
  auto y_line2 = g::Line3D::Make(g::Point3D(5, -2, 3), g::Point3D(5, 2, 3));
  ASSERT_TRUE(zx.Intersects(y_line2));
  {
    auto inter = zx.Intersection(y_line2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(5, 0, 3), std::get<g::Point3D>(*inter));
  }

  // line lying in ZX — the whole line lies in the plane, reported as a Line3D
  auto z_line2 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  ASSERT_TRUE(zx.Intersects(z_line2));
  {
    auto inter = zx.Intersection(z_line2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    EXPECT_EQ(z_line2, std::get<g::Line3D>(*inter));
  }

  // diagonal: (0,-1,0)→(0,1,1) crosses Y=0 at (0,0,0.5)
  auto diag_zx = g::Line3D::Make(g::Point3D(0, -1, 0), g::Point3D(0, 1, 1));
  ASSERT_TRUE(zx.Intersects(diag_zx));
  {
    auto inter = zx.Intersection(diag_zx);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(0, 0, 0.5), std::get<g::Point3D>(*inter));
  }
}

TEST_F(PlaneTest, EqualityOperator) {
  EXPECT_TRUE(g::Plane::XY() == g::Plane::XY());
  EXPECT_FALSE(g::Plane::XY() == g::Plane::YZ());

  // opposite-facing normals are no longer equal — opposite winding is a real difference
  EXPECT_FALSE(g::Plane::FromOriginAndNormal(g::Point3D::Zero(), g::Vector3D::BasisY()) ==
               g::Plane::FromOriginAndNormal(g::Point3D::Zero(), -g::Vector3D::BasisY()));
}

TEST_F(PlaneTest, ClosestWorldPlaneTo) {
  // exact world planes
  EXPECT_TRUE(g::closest_world_plane_to({{0,0,0},{1,0,0},{0,1,0}}).normal().AlmostEquals(g::Vector3D::BasisZ()));
  EXPECT_TRUE(g::closest_world_plane_to({{0,0,0},{0,1,0},{0,0,1}}).normal().AlmostEquals(g::Vector3D::BasisX()));
  // From3Points((0,0,0),(1,0,0),(0,0,1)) yields normal (0,-1,0); abs dominates Y → returns BasisY
  EXPECT_TRUE(g::closest_world_plane_to({{0,0,0},{1,0,0},{0,0,1}}).normal().AlmostEquals(g::Vector3D::BasisY()));

  // tilted planes — dominant axis still selects the right world plane
  // normal ≈ (-0.1,-0.1,1): Z dominant → XY
  EXPECT_TRUE(g::closest_world_plane_to({{0,0,0},{1,0,0.1},{0,1,0.1}}).normal().AlmostEquals(g::Vector3D::BasisZ()));
  // normal ≈ (1,-0.1,-0.1): X dominant → YZ
  EXPECT_TRUE(g::closest_world_plane_to({{0,0,0},{0.1,1,0},{0.1,0,1}}).normal().AlmostEquals(g::Vector3D::BasisX()));
  // normal ≈ (0.1,-1,0.1): Y dominant → ZX
  EXPECT_TRUE(g::closest_world_plane_to({{0,0,0},{1,0.1,0},{0,0.1,1}}).normal().AlmostEquals(g::Vector3D::BasisY()));

  // throws when fewer than 3 non-collinear points remain
  EXPECT_ANY_THROW(g::closest_world_plane_to({{0,0,0},{1,0,0},{2,0,0}}));  // collinear
  EXPECT_ANY_THROW(g::closest_world_plane_to({{0,0,0},{1,0,0}}));           // only 2 points
}

TEST_F(PlaneTest, AreCCW) {
  // XY plane (normal = +Z)
  std::vector<g::Point3D> xy_ccw = {{0,0,0},{1,0,0},{1,1,0},{0,1,0}};
  std::vector<g::Point3D> xy_cw  = {{0,0,0},{0,1,0},{1,1,0},{1,0,0}};
  EXPECT_TRUE(g::are_ccw(xy_ccw));
  EXPECT_FALSE(g::are_ccw(xy_cw));

  // YZ plane (normal = +X); axes U=+Y, V=+Z
  std::vector<g::Point3D> yz_ccw = {{0,0,0},{0,1,0},{0,1,1},{0,0,1}};
  std::vector<g::Point3D> yz_cw  = {{0,0,0},{0,0,1},{0,1,1},{0,1,0}};
  EXPECT_TRUE(g::are_ccw(yz_ccw));
  EXPECT_FALSE(g::are_ccw(yz_cw));

  // ZX plane (normal = +Y); axes U=+Z, V=+X
  std::vector<g::Point3D> zx_ccw = {{0,0,0},{0,0,1},{1,0,1},{1,0,0}};
  std::vector<g::Point3D> zx_cw  = {{0,0,0},{1,0,0},{1,0,1},{0,0,1}};
  EXPECT_TRUE(g::are_ccw(zx_ccw));
  EXPECT_FALSE(g::are_ccw(zx_cw));

  // tilted polygon: auto-detection picks XY (wrong reference) → CW;
  // explicit plane (from the points themselves) → CCW
  std::vector<g::Point3D> tilted = {{0,0,0},{0,1,0},{1,1,1},{1,0,1}};
  auto ref = g::Plane::From3Points(tilted[0], tilted[1], tilted[2]);
  EXPECT_FALSE(g::are_ccw(tilted));        // auto XY ref gives wrong answer
  EXPECT_TRUE(g::are_ccw(tilted, ref));    // correct ref gives right answer
}

TEST_F(PlaneTest, SignedArea_3D_XY) {
  std::vector<g::Point3D> ccw = {{0,0,0},{4,0,0},{4,4,0},{0,4,0}};
  EXPECT_NEAR(16.0, g::signed_area(ccw), 1e-9);
  EXPECT_GT(g::signed_area(ccw), 0.0);

  std::vector<g::Point3D> cw = {{0,0,0},{0,4,0},{4,4,0},{4,0,0}};
  EXPECT_NEAR(-16.0, g::signed_area(cw), 1e-9);
  EXPECT_LT(g::signed_area(cw), 0.0);
}

TEST_F(PlaneTest, SignedArea_3D_YZ) {
  // 4×4 CCW square on YZ plane → +16
  std::vector<g::Point3D> ccw = {{0,0,0},{0,4,0},{0,4,4},{0,0,4}};
  EXPECT_NEAR(16.0, g::signed_area(ccw), 1e-9);
}

TEST_F(PlaneTest, SignedArea_3D_ZX) {
  // 4×4 CCW square on ZX plane → +16
  std::vector<g::Point3D> ccw = {{0,0,0},{0,0,4},{4,0,4},{4,0,0}};
  EXPECT_NEAR(16.0, g::signed_area(ccw), 1e-9);
}

TEST_F(PlaneTest, SignedArea_3D_OppositeNormal) {
  // same CCW points on XY plane: +Z plane → positive, -Z plane → negative
  std::vector<g::Point3D> pts = {{0,0,0},{4,0,0},{4,4,0},{0,4,0}};
  auto plane_pos = g::Plane::FromOriginAndNormal(g::Point3D::Zero(),  g::Vector3D::BasisZ());
  auto plane_neg = g::Plane::FromOriginAndNormal(g::Point3D::Zero(), -g::Vector3D::BasisZ());
  EXPECT_GT(g::signed_area(pts, plane_pos), 0.0);
  EXPECT_LT(g::signed_area(pts, plane_neg), 0.0);
}

TEST_F(PlaneTest, SignedArea_3D_ConsistentWithAreCCW) {
  std::vector<g::Point3D> ccw = {{0,0,0},{4,0,0},{4,4,0},{0,4,0}};
  std::vector<g::Point3D> cw  = {{0,0,0},{0,4,0},{4,4,0},{4,0,0}};
  EXPECT_EQ(g::are_ccw(ccw), g::signed_area(ccw) > 0);
  EXPECT_EQ(g::are_cw(cw),   g::signed_area(cw)  < 0);
}

TEST_F(PlaneTest, Centroid_XYSquare) {
  // 4×4 square on the XY plane at z=0 → centroid = (2,2,0)
  std::vector<g::Point3D> pts = {{0,0,0},{4,0,0},{4,4,0},{0,4,0}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(0.0, c.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_ElevatedXYSquare) {
  // same square lifted to z=7 — the missing z must come from the plane
  std::vector<g::Point3D> pts = {{0,0,7},{4,0,7},{4,4,7},{0,4,7}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(7.0, c.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_YZSquare) {
  // 4×4 square on the YZ plane (x=0) → centroid = (0,2,2)
  std::vector<g::Point3D> pts = {{0,0,0},{0,4,0},{0,4,4},{0,0,4}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(0.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(2.0, c.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_OffOriginYZSquare) {
  // 4×4 square on the plane x=5 → centroid = (5,2,2)
  std::vector<g::Point3D> pts = {{5,0,0},{5,4,0},{5,4,4},{5,0,4}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(5.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
  EXPECT_NEAR(2.0, c.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_ZXSquare) {
  // 4×4 square on the ZX plane (y=0) → centroid = (2,0,2)
  std::vector<g::Point3D> pts = {{0,0,0},{0,0,4},{4,0,4},{4,0,0}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(0.0, c.y(), 1e-9);
  EXPECT_NEAR(2.0, c.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_OffOriginZXSquare) {
  // 4×4 square on the plane y=3 → centroid = (2,3,2)
  std::vector<g::Point3D> pts = {{0,3,0},{0,3,4},{4,3,4},{4,3,0}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(3.0, c.y(), 1e-9);
  EXPECT_NEAR(2.0, c.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_Triangle) {
  // triangle with ≤3 unique points → returns average of vertices
  std::vector<g::Point3D> pts = {{0,0,0},{3,0,0},{0,3,0}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(1.0, c.x(), 1e-9);
  EXPECT_NEAR(1.0, c.y(), 1e-9);
  EXPECT_NEAR(0.0, c.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_WithExplicitPlane) {
  // passing the plane explicitly must give the same result as auto-detection
  std::vector<g::Point3D> pts = {{0,0,5},{4,0,5},{4,4,5},{0,4,5}};
  auto plane = g::Plane::FromOriginAndNormal(g::Point3D(0,0,5), g::Vector3D::BasisZ());
  auto c_auto   = g::centroid(pts);
  auto c_explicit = g::centroid(pts, plane);
  EXPECT_NEAR(c_auto.x(), c_explicit.x(), 1e-9);
  EXPECT_NEAR(c_auto.y(), c_explicit.y(), 1e-9);
  EXPECT_NEAR(c_auto.z(), c_explicit.z(), 1e-9);
}

TEST_F(PlaneTest, Centroid_RightTriangleXY) {
  // right triangle base=4, height=3 → centroid = (4/3, 1, 0)
  std::vector<g::Point3D> pts = {{0,0,0},{4,0,0},{0,3,0}};
  auto c = g::centroid(pts);
  EXPECT_NEAR(4.0/3.0, c.x(), 1e-6);
  EXPECT_NEAR(1.0,     c.y(), 1e-6);
  EXPECT_NEAR(0.0,     c.z(), 1e-9);
}

TEST_F(PlaneTest, AreCW) {
  std::vector<g::Point3D> xy_ccw = {{0,0,0},{1,0,0},{1,1,0},{0,1,0}};
  std::vector<g::Point3D> xy_cw  = {{0,0,0},{0,1,0},{1,1,0},{1,0,0}};
  EXPECT_FALSE(g::are_cw(xy_ccw));
  EXPECT_TRUE(g::are_cw(xy_cw));

  std::vector<g::Point3D> yz_ccw = {{0,0,0},{0,1,0},{0,1,1},{0,0,1}};
  std::vector<g::Point3D> yz_cw  = {{0,0,0},{0,0,1},{0,1,1},{0,1,0}};
  EXPECT_FALSE(g::are_cw(yz_ccw));
  EXPECT_TRUE(g::are_cw(yz_cw));

  std::vector<g::Point3D> zx_ccw = {{0,0,0},{0,0,1},{1,0,1},{1,0,0}};
  std::vector<g::Point3D> zx_cw  = {{0,0,0},{1,0,0},{1,0,1},{0,0,1}};
  EXPECT_FALSE(g::are_cw(zx_ccw));
  EXPECT_TRUE(g::are_cw(zx_cw));

  std::vector<g::Point3D> tilted = {{0,0,0},{0,1,0},{1,1,1},{1,0,1}};
  auto ref = g::Plane::From3Points(tilted[0], tilted[1], tilted[2]);
  EXPECT_TRUE(g::are_cw(tilted));          // auto XY ref gives wrong answer
  EXPECT_FALSE(g::are_cw(tilted, ref));    // correct ref gives right answer
}

TEST_F(PlaneTest, IntersectionWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();  // Z = 0

  // ray pointing toward plane crosses at (5,3,0)
  auto down = g::Ray3D::Make(g::Point3D(5, 3, 4), g::Vector3D(0, 0, -1));
  ASSERT_TRUE(xy.Intersects(down));
  {
    auto inter = xy.Intersection(down);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(5, 3, 0), std::get<g::Point3D>(*inter));
  }

  // ray pointing AWAY from plane — line crosses, but not on the ray side
  auto up_above = g::Ray3D::Make(g::Point3D(5, 3, 4), g::Vector3D(0, 0, 1));
  ASSERT_FALSE(xy.Intersects(up_above));
  ASSERT_FALSE(xy.Intersection(up_above).has_value());

  // ray parallel to plane and above it — no intersection
  auto parallel_above = g::Ray3D::Make(g::Point3D(0, 0, 2), g::Vector3D(1, 0, 0));
  ASSERT_FALSE(xy.Intersects(parallel_above));
  ASSERT_FALSE(xy.Intersection(parallel_above).has_value());

  // ray lying in the plane (coplanar) — its containing line lies in the plane, reported as a Line3D
  auto coplanar = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(1, 0, 0));
  ASSERT_TRUE(xy.Intersects(coplanar));
  {
    auto inter = xy.Intersection(coplanar);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    EXPECT_EQ(coplanar.ToLine(), std::get<g::Line3D>(*inter));
  }

  // ray with origin exactly on plane, pointing away — origin is the intersection
  auto on_pointing_up = g::Ray3D::Make(g::Point3D(5, 3, 0), g::Vector3D(0, 0, 1));
  ASSERT_TRUE(xy.Intersects(on_pointing_up));
  {
    auto inter = xy.Intersection(on_pointing_up);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(5, 3, 0), std::get<g::Point3D>(*inter));
  }

  auto yz = g::Plane::YZ();
  // diagonal ray from x<0 toward yz plane crossing at (0,1,2)
  auto diag = g::Ray3D::Make(g::Point3D(-2, 1, 2), g::Vector3D(1, 0, 0));
  ASSERT_TRUE(yz.Intersects(diag));
  {
    auto inter = yz.Intersection(diag);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point3D(0, 1, 2), std::get<g::Point3D>(*inter));
  }
}

TEST_F(PlaneTest, IntersectionWLineSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();  // Z = 0

  // segment crosses plane at (5,3,0)
  auto crossing = g::LineSegment3D::Make(g::Point3D(5, 3, -2), g::Point3D(5, 3, 4));
  ASSERT_TRUE(xy.Intersects(crossing));
  {
    auto inter = xy.Intersection(crossing);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(5, 3, 0), std::get<g::Point3D>(*inter));
  }

  // segment entirely above plane — line would intersect, segment does not
  auto above = g::LineSegment3D::Make(g::Point3D(0, 0, 1), g::Point3D(1, 1, 2));
  ASSERT_FALSE(xy.Intersects(above));
  ASSERT_FALSE(xy.Intersection(above).has_value());

  // segment entirely below plane
  auto below = g::LineSegment3D::Make(g::Point3D(0, 0, -1), g::Point3D(1, 1, -2));
  ASSERT_FALSE(xy.Intersects(below));
  ASSERT_FALSE(xy.Intersection(below).has_value());

  // segment endpoint exactly on plane — should be reported as the intersection
  auto touching = g::LineSegment3D::Make(g::Point3D(2, 2, 0), g::Point3D(2, 2, 3));
  ASSERT_TRUE(xy.Intersects(touching));
  {
    auto inter = xy.Intersection(touching);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point3D(2, 2, 0), std::get<g::Point3D>(*inter));
  }

  // segment lying in the plane (coplanar) — reported as the segment itself
  auto coplanar = g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0));
  ASSERT_TRUE(xy.Intersects(coplanar));
  {
    auto inter = xy.Intersection(coplanar);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(*inter));
    EXPECT_EQ(coplanar, std::get<g::LineSegment3D>(*inter));
  }

  // segment parallel above plane (not coplanar)
  auto parallel = g::LineSegment3D::Make(g::Point3D(0, 0, 2), g::Point3D(1, 1, 2));
  ASSERT_FALSE(xy.Intersects(parallel));
  ASSERT_FALSE(xy.Intersection(parallel).has_value());

  auto yz = g::Plane::YZ();
  auto crossing_yz = g::LineSegment3D::Make(g::Point3D(-1, 4, 2), g::Point3D(3, 4, 2));
  ASSERT_TRUE(yz.Intersects(crossing_yz));
  {
    auto inter = yz.Intersection(crossing_yz);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point3D(0, 4, 2), std::get<g::Point3D>(*inter));
  }
}

TEST_F(PlaneTest, IntersectionWPlane) {
  geompp::DECIMAL_PRECISION = 4;

  // XY ∩ YZ → Y-axis (direction ±Y, through origin)
  {
    auto inter = g::Plane::XY().Intersection(g::Plane::YZ());
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    auto const& line = std::get<g::Line3D>(*inter);
    // origin must lie on both planes (z=0, x=0) and on the line
    EXPECT_TRUE(g::Plane::XY().Contains(line.First()));
    EXPECT_TRUE(g::Plane::YZ().Contains(line.First()));
    // direction parallel to Y-axis (up to sign)
    EXPECT_TRUE(line.Direction().AlmostEquals(g::Vector3D::BasisY()) ||
                line.Direction().AlmostEquals(-g::Vector3D::BasisY()));
  }

  // XY ∩ ZX → X-axis
  {
    auto inter = g::Plane::XY().Intersection(g::Plane::ZX());
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    auto const& line = std::get<g::Line3D>(*inter);
    EXPECT_TRUE(g::Plane::XY().Contains(line.First()));
    EXPECT_TRUE(g::Plane::ZX().Contains(line.First()));
    EXPECT_TRUE(line.Direction().AlmostEquals(g::Vector3D::BasisX()) ||
                line.Direction().AlmostEquals(-g::Vector3D::BasisX()));
  }

  // YZ ∩ ZX → Z-axis
  {
    auto inter = g::Plane::YZ().Intersection(g::Plane::ZX());
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    auto const& line = std::get<g::Line3D>(*inter);
    EXPECT_TRUE(g::Plane::YZ().Contains(line.First()));
    EXPECT_TRUE(g::Plane::ZX().Contains(line.First()));
    EXPECT_TRUE(line.Direction().AlmostEquals(g::Vector3D::BasisZ()) ||
                line.Direction().AlmostEquals(-g::Vector3D::BasisZ()));
  }

  // z=5 ∩ x=3 → vertical line (x=3, z=5, y free)
  {
    auto p1 = g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 5), g::Vector3D::BasisZ());
    auto p2 = g::Plane::FromOriginAndNormal(g::Point3D(3, 0, 0), g::Vector3D::BasisX());
    auto inter = p1.Intersection(p2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Line3D>(*inter));
    auto const& line = std::get<g::Line3D>(*inter);
    EXPECT_TRUE(p1.Contains(line.First()));
    EXPECT_TRUE(p2.Contains(line.First()));
    EXPECT_TRUE(line.Direction().AlmostEquals(g::Vector3D::BasisY()) ||
                line.Direction().AlmostEquals(-g::Vector3D::BasisY()));
  }

  // parallel distinct planes → nullopt
  {
    auto p1 = g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 0), g::Vector3D::BasisZ());
    auto p2 = g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 5), g::Vector3D::BasisZ());
    ASSERT_FALSE(p1.Intersects(p2));
    ASSERT_FALSE(p1.Intersection(p2).has_value());
  }

  // same plane (different normal sign) → API limitation: nullopt
  {
    auto p1 = g::Plane::FromOriginAndNormal(g::Point3D::Zero(),  g::Vector3D::BasisZ());
    auto p2 = g::Plane::FromOriginAndNormal(g::Point3D::Zero(), -g::Vector3D::BasisZ());
    ASSERT_FALSE(p1.Intersects(p2));
    ASSERT_FALSE(p1.Intersection(p2).has_value());
  }
}

TEST_F(PlaneTest, IntersectionWTriangle) {
  geompp::DECIMAL_PRECISION = 4;
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(4, 0, 0), g::Point3D(0, 4, 0));

  // Plane y=1 cuts the triangle's interior (endpoints (0,1,0) and (3,1,0) are on edge interiors,
  // not vertices, so Triangle2D::Intersection accepts the cut).
  {
    auto y1 = g::Plane::FromOriginAndNormal(g::Point3D(0, 1, 0), g::Vector3D::BasisY());
    ASSERT_TRUE(y1.Intersects(t));
    auto inter = y1.Intersection(t);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(*inter));
  }

  // Plane parallel above the triangle's plane — no intersection
  {
    auto above = g::Plane::FromOriginAndNormal(g::Point3D(0, 0, 1), g::Vector3D::BasisZ());
    ASSERT_FALSE(above.Intersects(t));
    ASSERT_FALSE(above.Intersection(t).has_value());
  }

  // Coplanar plane (same as triangle's plane) — API limitation: nullopt
  {
    auto same = g::Plane::XY();
    ASSERT_FALSE(same.Intersects(t));
    ASSERT_FALSE(same.Intersection(t).has_value());
  }
}

TEST_F(PlaneTest, IsParallelWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  // line along X — parallel to XY
  auto along_x = g::Line3D::Make(g::Point3D(0, 0, 3), g::Vector3D(1, 0, 0));
  EXPECT_TRUE(xy.IsParallel(along_x));

  // line along Y — parallel
  auto along_y = g::Line3D::Make(g::Point3D(0, 0, 3), g::Vector3D(0, 1, 0));
  EXPECT_TRUE(xy.IsParallel(along_y));

  // line lying IN the plane — coplanar lines are also "parallel" by this definition
  auto in_plane = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D(1, 1, 0));
  EXPECT_TRUE(xy.IsParallel(in_plane));

  // perpendicular (along Z) — not parallel
  auto perp = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D(0, 0, 1));
  EXPECT_FALSE(xy.IsParallel(perp));

  // diagonal — not parallel
  auto diag = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D(1, 1, 1));
  EXPECT_FALSE(xy.IsParallel(diag));
}

TEST_F(PlaneTest, IsParallelWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  auto along_x = g::Ray3D::Make(g::Point3D(0, 0, 3), g::Vector3D(1, 0, 0));
  EXPECT_TRUE(xy.IsParallel(along_x));

  auto in_plane = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(1, 0, 0));
  EXPECT_TRUE(xy.IsParallel(in_plane));

  auto perp = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(0, 0, 1));
  EXPECT_FALSE(xy.IsParallel(perp));

  auto diag = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(1, 1, 1));
  EXPECT_FALSE(xy.IsParallel(diag));
}

TEST_F(PlaneTest, IsParallelWLineSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  auto along_x = g::LineSegment3D::Make(g::Point3D(0, 0, 3), g::Point3D(5, 0, 3));
  EXPECT_TRUE(xy.IsParallel(along_x));

  auto in_plane = g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(2, 2, 0));
  EXPECT_TRUE(xy.IsParallel(in_plane));

  auto perp = g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(0, 0, 5));
  EXPECT_FALSE(xy.IsParallel(perp));

  auto diag = g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(1, 1, 1));
  EXPECT_FALSE(xy.IsParallel(diag));
}

TEST_F(PlaneTest, IsCoplanarWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  // line in XY plane → coplanar
  auto in_plane = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D(1, 1, 0));
  EXPECT_TRUE(xy.IsCoplanar(in_plane));

  // line parallel to XY but above → not coplanar
  auto parallel_above = g::Line3D::Make(g::Point3D(0, 0, 2), g::Vector3D(1, 0, 0));
  EXPECT_FALSE(xy.IsCoplanar(parallel_above));

  // line crossing the plane → not coplanar
  auto crossing = g::Line3D::Make(g::Point3D(0, 0, -1), g::Vector3D(1, 0, 1));
  EXPECT_FALSE(xy.IsCoplanar(crossing));

  // line perpendicular to plane → not coplanar
  auto perp = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D(0, 0, 1));
  EXPECT_FALSE(xy.IsCoplanar(perp));
}

TEST_F(PlaneTest, IsCoplanarWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  auto in_plane = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(1, 0, 0));
  EXPECT_TRUE(xy.IsCoplanar(in_plane));

  auto parallel_above = g::Ray3D::Make(g::Point3D(0, 0, 2), g::Vector3D(1, 0, 0));
  EXPECT_FALSE(xy.IsCoplanar(parallel_above));

  auto perp = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(0, 0, 1));
  EXPECT_FALSE(xy.IsCoplanar(perp));

  auto diag = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(1, 1, 1));
  EXPECT_FALSE(xy.IsCoplanar(diag));
}

TEST_F(PlaneTest, IsCoplanarWLineSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto xy = g::Plane::XY();

  auto in_plane = g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(2, 2, 0));
  EXPECT_TRUE(xy.IsCoplanar(in_plane));

  auto parallel_above = g::LineSegment3D::Make(g::Point3D(0, 0, 2), g::Point3D(5, 0, 2));
  EXPECT_FALSE(xy.IsCoplanar(parallel_above));

  auto perp = g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(0, 0, 5));
  EXPECT_FALSE(xy.IsCoplanar(perp));

  auto diag = g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(1, 1, 1));
  EXPECT_FALSE(xy.IsCoplanar(diag));
}

}  // namespace geompp_tests
