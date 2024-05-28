#include "vector2d.hpp"

#include "constants.hpp"
#include "point2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

TEST(Vector2D, Equality) {
  EXPECT_EQ(g::Vector2D(2.56, 748.1203), g::Vector2D(2.56, 748.1203));
  EXPECT_EQ(g::Vector2D(2, 3), g::Vector2D(2, 3));
  EXPECT_EQ(g::Vector2D(-56.682, 30.56), g::Vector2D(-56.682, 30.56));
  EXPECT_EQ(g::Vector2D(-672.6456, -153.516), g::Vector2D(-672.6456, -153.516));

  EXPECT_EQ(g::Vector2D(672.64560944, -153.5166067079), g::Vector2D(672.64560944, -153.5166067079));
}

TEST(Vector2D, AddPoint) {
  auto p = g::Point2D(1, 2);
  auto v = g::Vector2D(1, 1);
  g::Point2D pv = p + v;

  EXPECT_EQ(2, pv.x());
  EXPECT_EQ(3, pv.y());

  EXPECT_EQ(g::Point2D(2, 3), pv);
}

TEST(Vector2D, AddVector) {
  auto v1 = g::Vector2D(1.2, 2.12);
  auto v2 = g::Vector2D(1.85, 1.65);
  g::Vector2D v3 = v1 + v2;

  EXPECT_EQ(3.05, v3.x());
  EXPECT_EQ(3.77, v3.y());

  EXPECT_EQ(g::Vector2D(3.05, 3.77), v3);
}

TEST(Vector2D, PerpDotCross) {
  int prec = 3;
  auto v1 = g::Vector2D(1, 0);
  auto v2 = v1.Perp();
  ASSERT_EQ(0.0, g::round_to(v1.Dot(v2), prec));  // perp is perpendicular
  EXPECT_EQ(1.0, g::round_to(v1.Cross(v2), prec));
  EXPECT_EQ(-1.0, g::round_to(v2.Cross(v1), prec));
}

TEST(Vector2D, Wkt) {
  ASSERT_EQ("VECTOR (0 0)", g::Vector2D().ToWkt());
  ASSERT_EQ("VECTOR (56491.62 -795.97)", g::Vector2D(56491.6164, -795.97416).ToWkt(2));

  EXPECT_EQ(g::Vector2D(256.1343, -684.64971), g::Vector2D::FromWkt("VECTOR (256.1343 -684.64971)"));
  EXPECT_EQ(g::Vector2D(-7.5, -60.7), g::Vector2D::FromWkt("  vector( -7.5    -60.7)"));
  EXPECT_EQ(g::Vector2D(0.645, -1.689741), g::Vector2D::FromWkt("VecTOR   ( 0.645  -1.689741  )"));

  EXPECT_ANY_THROW(g::Vector2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Vector2D::FromWkt("vecto ( -7.5 -60.7)"));
  EXPECT_ANY_THROW(g::Vector2D::FromWkt("vector -7.5 -64.4)"));
  EXPECT_ANY_THROW(g::Vector2D::FromWkt("vector (-7.5 -64.4"));
  EXPECT_ANY_THROW(g::Vector2D::FromWkt("vector ( -7.5 )"));
  EXPECT_ANY_THROW(g::Vector2D::FromWkt("vector ( )"));
  EXPECT_ANY_THROW(g::Vector2D::FromWkt("vector ( -7.5 -64.4 15.5)"));
}