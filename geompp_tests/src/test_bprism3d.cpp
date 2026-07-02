#include "bprism3d.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <gtest/gtest.h>

#include <cmath>

namespace g = geompp;

namespace geompp_tests {

class BPrism3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

// ── Constructor ──────────────────────────────────────────────────────────────

TEST_F(BPrism3DTest, ConstructorEmpty_Throws) {
  ASSERT_THROW(g::BPrism3D(std::vector<g::Point3D>{}), std::invalid_argument);
}

TEST_F(BPrism3DTest, ConstructorSinglePoint_Throws) {
  ASSERT_THROW(g::BPrism3D(std::vector<g::Point3D>{{3.0, 4.0, 5.0}}), std::invalid_argument);
}

TEST_F(BPrism3DTest, ConstructorTwoPoints_Throws) {
  ASSERT_THROW(g::BPrism3D(std::vector<g::Point3D>{{0.0, 0.0, 0.0}, {4.0, 0.0, 0.0}}),
               std::invalid_argument);
}

TEST_F(BPrism3DTest, ConstructorAxisAlignedBox) {
  // 8 corners of a 4x3x2 box
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  ASSERT_TRUE(p.center().AlmostEquals(g::Point3D(2.0, 1.5, 1.0)));
  ASSERT_NEAR(p.volume(), 24.0, 1e-6);
  // All input points must be contained
  for (auto const& pt : pts) {
    ASSERT_TRUE(p.Contains(pt)) << "prism must contain all input points";
  }
}

TEST_F(BPrism3DTest, ConstructorFlatCloud_WIsEpsilon) {
  // 4 coplanar points in the z=0 plane — HALF_LEN_W should be clamped to DOUBLE_EPSILON
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {2, 0, 0}, {2, 1, 0}, {0, 1, 0},
  };
  auto p = g::BPrism3D(pts);
  double eps = g::DOUBLE_EPSILON;
  ASSERT_NEAR(p.half_len_w(), eps, 1e-12);
}

TEST_F(BPrism3DTest, ConstructorNonConvex_AllPointsContained) {
  // Box corners plus interior points — every input point must be inside
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
    {1, 1, 1}, {2, 0.5, 0.3}, {3.5, 2.5, 1.8},
  };
  auto p = g::BPrism3D(pts);
  for (auto const& pt : pts) {
    ASSERT_TRUE(p.Contains(pt)) << "prism must contain all input points";
  }
}

// ── Accessors ────────────────────────────────────────────────────────────────

TEST_F(BPrism3DTest, Accessors_AxesAreUnitVectors) {
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  ASSERT_NEAR(p.axis_u().Length(), 1.0, 1e-9);
  ASSERT_NEAR(p.axis_v().Length(), 1.0, 1e-9);
  ASSERT_NEAR(p.axis_w().Length(), 1.0, 1e-9);
}

TEST_F(BPrism3DTest, Accessors_AxesOrthogonal) {
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  ASSERT_NEAR(p.axis_u().Dot(p.axis_v()), 0.0, 1e-9);
  ASSERT_NEAR(p.axis_u().Dot(p.axis_w()), 0.0, 1e-9);
  ASSERT_NEAR(p.axis_v().Dot(p.axis_w()), 0.0, 1e-9);
}

TEST_F(BPrism3DTest, Accessors_WidthHeightDepthVolume) {
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  ASSERT_NEAR(p.width(), 2.0 * p.half_len_u(), 1e-9);
  ASSERT_NEAR(p.height(), 2.0 * p.half_len_v(), 1e-9);
  ASSERT_NEAR(p.depth(), 2.0 * p.half_len_w(), 1e-9);
  ASSERT_NEAR(p.volume(), p.width() * p.height() * p.depth(), 1e-9);
}

// ── Corners ──────────────────────────────────────────────────────────────────

TEST_F(BPrism3DTest, Corners_EightDistinctPoints) {
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  auto corners = p.Corners();
  ASSERT_EQ(corners.size(), 8u);
  // All eight corners must be contained in the prism
  for (auto const& c : corners) {
    ASSERT_TRUE(p.Contains(c)) << "prism must contain all its corners";
  }
}

// ── Contains ─────────────────────────────────────────────────────────────────

TEST_F(BPrism3DTest, Contains_Center_True) {
  g::DECIMAL_PRECISION = 6;
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  ASSERT_TRUE(p.Contains(p.center()));
}

TEST_F(BPrism3DTest, Contains_Interior_True) {
  g::DECIMAL_PRECISION = 6;
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  ASSERT_TRUE(p.Contains(g::Point3D(1.0, 1.0, 0.5)));
  ASSERT_TRUE(p.Contains(g::Point3D(3.5, 2.5, 1.8)));
}

TEST_F(BPrism3DTest, Contains_Outside_False) {
  g::DECIMAL_PRECISION = 6;
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  ASSERT_FALSE(p.Contains(g::Point3D(5.0, 1.5, 1.0)));   // beyond u-axis
  ASSERT_FALSE(p.Contains(g::Point3D(2.0, 4.0, 1.0)));   // beyond v-axis
  ASSERT_FALSE(p.Contains(g::Point3D(2.0, 1.5, 3.0)));   // beyond w-axis
}

TEST_F(BPrism3DTest, Contains_Boundary_True) {
  g::DECIMAL_PRECISION = 6;
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p = g::BPrism3D(pts);
  // Input corners lie on the prism boundary
  ASSERT_TRUE(p.Contains(g::Point3D(0.0, 0.0, 0.0)));
  ASSERT_TRUE(p.Contains(g::Point3D(4.0, 3.0, 2.0)));
}

// ── AlmostEquals / operator== ─────────────────────────────────────────────────

TEST_F(BPrism3DTest, AlmostEquals_Same) {
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p1 = g::BPrism3D(pts);
  auto p2 = g::BPrism3D(pts);
  ASSERT_TRUE(p1.AlmostEquals(p2));
  ASSERT_EQ(p1, p2);
}

TEST_F(BPrism3DTest, AlmostEquals_Different) {
  std::vector<g::Point3D> pts1 = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  std::vector<g::Point3D> pts2 = {
    {0, 0, 0}, {6, 0, 0}, {6, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {6, 0, 2}, {6, 3, 2}, {0, 3, 2},
  };
  auto p1 = g::BPrism3D(pts1);
  auto p2 = g::BPrism3D(pts2);
  ASSERT_FALSE(p1.AlmostEquals(p2));
  ASSERT_NE(p1, p2);
}

// ── Copy / Assignment ─────────────────────────────────────────────────────────

TEST_F(BPrism3DTest, CopyConstructor) {
  std::vector<g::Point3D> pts = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  auto p1 = g::BPrism3D(pts);
  auto p2 = p1;
  ASSERT_TRUE(p1.AlmostEquals(p2));
}

TEST_F(BPrism3DTest, Assignment) {
  std::vector<g::Point3D> pts1 = {
    {0, 0, 0}, {4, 0, 0}, {4, 3, 0}, {0, 3, 0},
    {0, 0, 2}, {4, 0, 2}, {4, 3, 2}, {0, 3, 2},
  };
  std::vector<g::Point3D> pts2 = {
    {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
    {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1},
  };
  auto p1 = g::BPrism3D(pts1);
  auto p2 = g::BPrism3D(pts2);
  p2 = p1;
  ASSERT_TRUE(p2.AlmostEquals(p1));
  p1 = p1;  // self-assignment
  ASSERT_TRUE(p1.AlmostEquals(g::BPrism3D(pts1)));
}

}  // namespace geompp_tests
