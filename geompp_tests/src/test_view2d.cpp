#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"
#include "vector3d.hpp"
#include "view2d.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class View2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

// ── Type ──────────────────────────────────────────────────────────────────────

TEST_F(View2DTest, TypeXY) { ASSERT_EQ(g::View2D::XY().type(), g::ProjectionType::XY); }
TEST_F(View2DTest, TypeYZ) { ASSERT_EQ(g::View2D::YZ().type(), g::ProjectionType::YZ); }
TEST_F(View2DTest, TypeZX) { ASSERT_EQ(g::View2D::ZX().type(), g::ProjectionType::ZX); }
TEST_F(View2DTest, TypeCustom) {
  auto v = g::View2D::OnPlane(g::Plane::XY());
  ASSERT_EQ(v.type(), g::ProjectionType::Custom);
}

// ── Point2D pass-through ──────────────────────────────────────────────────────

TEST_F(View2DTest, Point2DPassThrough) {
  g::Point2D p{3.0, 7.0};
  auto v = g::View2D::XY();
  ASSERT_NEAR(v.x(p), 3.0, 1e-9);
  ASSERT_NEAR(v.y(p), 7.0, 1e-9);
}

// ── XY projection (drops Z) ───────────────────────────────────────────────────

TEST_F(View2DTest, XYProjectionX) {
  g::Point3D p{1.0, 2.0, 99.0};
  ASSERT_NEAR(g::View2D::XY().x(p), 1.0, 1e-9);
}

TEST_F(View2DTest, XYProjectionY) {
  g::Point3D p{1.0, 2.0, 99.0};
  ASSERT_NEAR(g::View2D::XY().y(p), 2.0, 1e-9);
}

// ── YZ projection (y→x, z→y, drops X) ────────────────────────────────────────

TEST_F(View2DTest, YZProjectionX) {
  g::Point3D p{99.0, 3.0, 4.0};
  ASSERT_NEAR(g::View2D::YZ().x(p), 3.0, 1e-9);
}

TEST_F(View2DTest, YZProjectionY) {
  g::Point3D p{99.0, 3.0, 4.0};
  ASSERT_NEAR(g::View2D::YZ().y(p), 4.0, 1e-9);
}

// ── ZX projection (z→x, x→y, drops Y) ────────────────────────────────────────

TEST_F(View2DTest, ZXProjectionX) {
  g::Point3D p{5.0, 99.0, 6.0};
  ASSERT_NEAR(g::View2D::ZX().x(p), 6.0, 1e-9);
}

TEST_F(View2DTest, ZXProjectionY) {
  g::Point3D p{5.0, 99.0, 6.0};
  ASSERT_NEAR(g::View2D::ZX().y(p), 5.0, 1e-9);
}

// ── Custom (OnPlane) ──────────────────────────────────────────────────────────

TEST_F(View2DTest, CustomOnXYPlane) {
  // Plane::XY() has origin (0,0,0), axis_u=(1,0,0), axis_v=(0,1,0)
  // so it should behave identically to View2D::XY() but via dot-product path
  auto v = g::View2D::OnPlane(g::Plane::XY());
  g::Point3D p{2.0, 5.0, 0.0};
  ASSERT_NEAR(v.x(p), 2.0, 1e-9);
  ASSERT_NEAR(v.y(p), 5.0, 1e-9);
}

TEST_F(View2DTest, CustomOnPlaneWithOffset) {
  // Plane at origin (1,1,1), normal (0,0,1) → axis_u=(1,0,0), axis_v=(0,1,0)
  auto plane = g::Plane::FromOriginAndNormal(g::Point3D{1.0, 1.0, 1.0}, g::Vector3D{0.0, 0.0, 1.0});
  auto v = g::View2D::OnPlane(plane);
  g::Point3D p{3.0, 4.0, 1.0};  // offset from plane origin: (2, 3, 0)
  ASSERT_NEAR(v.x(p), 2.0, 1e-9);
  ASSERT_NEAR(v.y(p), 3.0, 1e-9);
}

TEST_F(View2DTest, CustomOnYZPlaneMatchesYZ) {
  auto custom = g::View2D::OnPlane(g::Plane::YZ());
  auto builtin = g::View2D::YZ();
  g::Point3D p{99.0, 7.0, 8.0};
  ASSERT_NEAR(custom.x(p), builtin.x(p), 1e-9);
  ASSERT_NEAR(custom.y(p), builtin.y(p), 1e-9);
}

// ── Copy ─────────────────────────────────────────────────────────────────────

TEST_F(View2DTest, CopyConstructor) {
  auto v1 = g::View2D::YZ();
  auto v2 = v1;
  g::Point3D p{1.0, 2.0, 3.0};
  ASSERT_NEAR(v2.x(p), v1.x(p), 1e-9);
  ASSERT_NEAR(v2.y(p), v1.y(p), 1e-9);
}

// ── Bulk projection (the main use-case: streaming 3D points without allocating 2D points) ──

TEST_F(View2DTest, BulkProjection3DTo2D) {
  std::vector<g::Point3D> pts3d = {{1.0, 2.0, 10.0}, {3.0, 4.0, 20.0}, {5.0, 6.0, 30.0}};
  auto v = g::View2D::XY();

  std::vector<double> xs, ys;
  for (auto const& p : pts3d) {
    xs.push_back(v.x(p));
    ys.push_back(v.y(p));
  }

  ASSERT_NEAR(xs[0], 1.0, 1e-9);
  ASSERT_NEAR(xs[1], 3.0, 1e-9);
  ASSERT_NEAR(xs[2], 5.0, 1e-9);
  ASSERT_NEAR(ys[0], 2.0, 1e-9);
  ASSERT_NEAR(ys[1], 4.0, 1e-9);
  ASSERT_NEAR(ys[2], 6.0, 1e-9);
}

}  // namespace geompp_tests
