#include <gtest/gtest.h>

#include "line2d.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"


using namespace geompp;





TEST(Line2D, Constructor) {

  int prec = 4;
  auto p0 = Point2D(1, 2);
  auto p1 = Point2D(3, 5);
  auto l1 = Line2D::Make(p0, p1, prec);

  EXPECT_EQ(l1.First(), p0);
  EXPECT_EQ(l1.Last(), p1);
  EXPECT_EQ(l1.Direction(), p1 - p0);
  

  auto v = Vector2D(0, 1);
  auto l2 = Line2D::Make(p0, v, prec);
  EXPECT_EQ(l2.Origin(), p0);
  EXPECT_EQ(l2.Direction(), v);
  EXPECT_EQ(l2.Last(), p0 + v);

}
