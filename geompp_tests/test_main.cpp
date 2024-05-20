#include <gtest/gtest.h>

#include "geomcpp/include/point2d.hpp"
#include "geomcpp/include/vector2d.hpp"

// Test case for the add function
TEST(Vector, AddPoint) {

  auto p = Point2D(1, 2);
  auto v = VectorD(1, 1);
  auto pv = p + v;

  EXPECT_EQ(2, pv.x());
  EXPECT_EQ(3, pv.y());

  EXPECT_EQ(Point(2, 3), pv);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}