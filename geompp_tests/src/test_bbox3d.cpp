#include "bbox3d.hpp"

#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class BBox3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(BBox3DTest, Constructor) {
  auto bb = g::BBox3D(g::Point3D(0, 0, 0), g::Point3D(3, 4, 5));
  ASSERT_EQ(g::Point3D(0, 0, 0), bb.min());
  ASSERT_EQ(g::Point3D(3, 4, 5), bb.max());
}

TEST_F(BBox3DTest, CopyConstructor) {
  auto bb1 = g::BBox3D(g::Point3D(0, 0, 0), g::Point3D(3, 4, 5));
  g::BBox3D bb2(bb1);
  ASSERT_EQ(g::Point3D(0, 0, 0), bb2.min());
  ASSERT_EQ(g::Point3D(3, 4, 5), bb2.max());
}

TEST_F(BBox3DTest, AlmostEquals) {
  auto bb1 = g::BBox3D(g::Point3D(0, 0, 0), g::Point3D(3, 4, 5));
  auto bb2 = g::BBox3D(g::Point3D(0, 0, 0), g::Point3D(3, 4, 5));
  auto bb3 = g::BBox3D(g::Point3D(1, 1, 1), g::Point3D(5, 6, 7));

  ASSERT_TRUE(bb1.AlmostEquals(bb2));
  ASSERT_FALSE(bb1.AlmostEquals(bb3));
  ASSERT_EQ(bb1, bb2);
  ASSERT_NE(bb1, bb3);
}

TEST_F(BBox3DTest, Assignment) {
  auto bb1 = g::BBox3D(g::Point3D(0, 0, 0), g::Point3D(3, 4, 5));
  auto bb2 = g::BBox3D(g::Point3D(1, 1, 1), g::Point3D(5, 6, 7));
  bb2 = bb1;
  ASSERT_EQ(bb1, bb2);
}

TEST_F(BBox3DTest, ConstructorFromLineSegment) {
  // min/max correctly assigned across all three axes
  auto s = g::LineSegment3D::Make(g::Point3D(-1, -2, -3), g::Point3D(3, 4, 5));
  auto bb = g::BBox3D(s);
  ASSERT_EQ(g::Point3D(-1, -2, -3), bb.min());
  ASSERT_EQ(g::Point3D(3, 4, 5), bb.max());

  // reversed endpoints — same result
  auto s_rev = g::LineSegment3D::Make(g::Point3D(3, 4, 5), g::Point3D(-1, -2, -3));
  auto bb_rev = g::BBox3D(s_rev);
  ASSERT_EQ(bb.min(), bb_rev.min());
  ASSERT_EQ(bb.max(), bb_rev.max());
}

TEST_F(BBox3DTest, ConstructorFromPolyline) {
  auto poly = g::Polyline3D::Make({g::Point3D(-1, 0, 2), g::Point3D(3, 4, -1), g::Point3D(0, -2, 5)});
  auto bb = g::BBox3D(poly);
  ASSERT_EQ(g::Point3D(-1, -2, -1), bb.min());
  ASSERT_EQ(g::Point3D(3, 4, 5), bb.max());
}

TEST_F(BBox3DTest, ConstructorFromPolygon) {
  auto p = g::Polygon3D::Make({g::Point3D(-1, 0, 2), g::Point3D(3, 4, -1), g::Point3D(0, -2, 5)});
  auto bb = g::BBox3D(p);
  ASSERT_EQ(g::Point3D(-1, -2, -1), bb.min());
  ASSERT_EQ(g::Point3D(3, 4, 5), bb.max());
}

TEST_F(BBox3DTest, ConstructorFromTriangle) {
  auto t = g::Triangle3D::Make(g::Point3D(-1, 0, 2), g::Point3D(3, 4, -1), g::Point3D(0, -2, 5));
  auto bb = g::BBox3D(t);
  ASSERT_EQ(g::Point3D(-1, -2, -1), bb.min());
  ASSERT_EQ(g::Point3D(3, 4, 5), bb.max());
}

TEST_F(BBox3DTest, Contains) {
  geompp::DECIMAL_PRECISION = 4;
  auto bb = g::BBox3D(g::Point3D(0, 0, 0), g::Point3D(3, 4, 5));

  // corners
  ASSERT_TRUE(bb.Contains(g::Point3D(0, 0, 0)));
  ASSERT_TRUE(bb.Contains(g::Point3D(3, 4, 5)));
  ASSERT_TRUE(bb.Contains(g::Point3D(3, 0, 0)));
  ASSERT_TRUE(bb.Contains(g::Point3D(0, 4, 5)));

  // interior
  ASSERT_TRUE(bb.Contains(g::Point3D(1.5, 2, 2.5)));

  // outside each axis
  ASSERT_FALSE(bb.Contains(g::Point3D(-1, 2, 2)));
  ASSERT_FALSE(bb.Contains(g::Point3D(4, 2, 2)));
  ASSERT_FALSE(bb.Contains(g::Point3D(1, -1, 2)));
  ASSERT_FALSE(bb.Contains(g::Point3D(1, 5, 2)));
  ASSERT_FALSE(bb.Contains(g::Point3D(1, 2, -1)));
  ASSERT_FALSE(bb.Contains(g::Point3D(1, 2, 6)));
}

}  // namespace geompp_tests
