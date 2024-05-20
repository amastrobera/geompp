#include <gtest/gtest.h>

#include "point2d.hpp"
#include "vector2d.hpp"


using namespace geompp;

TEST(Vector2D, Equality) {
  EXPECT_EQ(Vector2D(2.56, 748.1203), Vector2D(2.56, 748.1203));
  EXPECT_EQ(Vector2D(2, 3), Vector2D(2, 3));
  EXPECT_EQ(Vector2D(-56.682, 30.56), Vector2D(-56.682, 30.56));
  EXPECT_EQ(Vector2D(-672.6456, -153.516), Vector2D(-672.6456, -153.516));

  EXPECT_EQ(Vector2D(672.64560944, -153.5166067079),
            Vector2D(672.64560944, -153.5166067079));
}

TEST(Vector2D, AddPoint) {

  auto p = Point2D(1, 2);
  auto v = Vector2D(1, 1);
  Point2D pv = p + v;

  EXPECT_EQ(2, pv.x());
  EXPECT_EQ(3, pv.y());

  EXPECT_EQ(Point2D(2, 3), pv);
}

TEST(Vector2D, AddVector) {

  auto v1 = Vector2D(1.2, 2.12);
  auto v2 = Vector2D(1.85, 1.65);
  Vector2D v3 = v1 + v2;

  EXPECT_EQ(3.05, v3.x());
  EXPECT_EQ(3.77, v3.y());

  EXPECT_EQ(Vector2D(3.05, 3.77), v3);
}
