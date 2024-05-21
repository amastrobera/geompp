#include <gtest/gtest.h>

#include "point2d.hpp"
#include "vector2d.hpp"

using namespace geompp;

TEST(Point2D, Equality) {
  EXPECT_EQ(Point2D(2.56, 748.1203), Point2D(2.56, 748.1203));
  EXPECT_EQ(Point2D(2, 3), Point2D(2, 3));
  EXPECT_EQ(Point2D(-56.682, 30.56), Point2D(-56.682, 30.56));
  EXPECT_EQ(Point2D(-672.6456, -153.516), Point2D(-672.6456, -153.516));

  EXPECT_EQ(Point2D(672.64560944, -153.5166067079), Point2D(672.64560944, -153.5166067079));
}

TEST(Point2D, SubtractPoint) {
  auto p1 = Point2D(1, 2);
  auto p2 = Point2D(1, 1);
  Vector2D v = p2 - p1;

  EXPECT_EQ(0, v.x());
  EXPECT_EQ(-1, v.y());

  EXPECT_EQ(Vector2D(0, -1), v);
}

TEST(Point2D, AddVector) {
  auto p = Point2D(1.2, 2.12);
  auto v = Vector2D(1.85, 1.65);
  Point2D pv = p + v;

  EXPECT_EQ(3.05, pv.x());
  EXPECT_EQ(3.77, pv.y());

  EXPECT_EQ(Point2D(3.05, 3.77), pv);
}
