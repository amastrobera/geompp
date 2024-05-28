#include "point2d.hpp"

#include "vector2d.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

TEST(Point2D, Equality) {
  ASSERT_EQ(g::Point2D(2.56, 748.1203), g::Point2D(2.56, 748.1203));
  ASSERT_EQ(g::Point2D(2, 3), g::Point2D(2, 3));
  ASSERT_EQ(g::Point2D(-56.682, 30.56), g::Point2D(-56.682, 30.56));
  ASSERT_EQ(g::Point2D(-672.6456, -153.516), g::Point2D(-672.6456, -153.516));

  ASSERT_EQ(g::Point2D(672.64560944, -153.5166067079), g::Point2D(672.64560944, -153.5166067079));
}

TEST(Point2D, SubtractPoint) {
  auto p1 = g::Point2D(1, 2);
  auto p2 = g::Point2D(1, 1);
  g::Vector2D v = p2 - p1;

  ASSERT_EQ(0, v.x());
  ASSERT_EQ(-1, v.y());

  ASSERT_EQ(g::Vector2D(0, -1), v);
}

TEST(Point2D, AddVector) {
  auto p = g::Point2D(1.2, 2.12);
  auto v = g::Vector2D(1.85, 1.65);
  g::Point2D pv = p + v;

  ASSERT_EQ(3.05, pv.x());
  ASSERT_EQ(3.77, pv.y());

  ASSERT_EQ(g::Point2D(3.05, 3.77), pv);
}

TEST(Point2D, Wkt) {
  ASSERT_EQ("POINT (0 0)", g::Point2D().ToWkt());
  ASSERT_EQ("POINT (56491.62 -795.97)", g::Point2D(56491.6164, -795.97416).ToWkt(2));

  EXPECT_EQ(g::Point2D(256.1343, -684.64971), g::Point2D::FromWkt("POINT (256.1343 -684.64971)"));
  EXPECT_EQ(g::Point2D(-7.5, -60.7), g::Point2D::FromWkt("  point( -7.5    -60.7)"));
  EXPECT_EQ(g::Point2D(0.645, -1.689741), g::Point2D::FromWkt("PoInT   ( 0.645  -1.689741  )"));

  EXPECT_ANY_THROW(g::Point2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("poin ( -7.5 -60.7)"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point -7.5 -64.4)"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point (-7.5 -64.4"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( -7.5 )"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( )"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( -7.5 -64.4 15.5)"));
}