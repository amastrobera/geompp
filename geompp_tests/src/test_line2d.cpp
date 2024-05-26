#include <gtest/gtest.h>

#include "line2d.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

namespace g = geompp;

TEST(Line2D, Constructor) {
  int prec = 4;
  auto p0 = g::Point2D(1, 2);
  auto p1 = g::Point2D(3, 5);
  auto l1 = g::Line2D::Make(p0, p1, prec);

  EXPECT_ANY_THROW(g::Line2D::Make(p0, p0, prec));  // cannot make a line that starts and finishes at the same point

  EXPECT_EQ(l1.First(), p0);
  EXPECT_EQ(l1.Last(), p1);
  EXPECT_EQ(l1.Direction(), (p1 - p0).Normalize());

  auto v = g::Vector2D(0, 1);
  auto l2 = g::Line2D::Make(p0, v, prec);
  EXPECT_EQ(l2.Origin(), p0);
  EXPECT_EQ(l2.Direction(), v);
  EXPECT_EQ(l2.Last(), p0 + v);
}

TEST(Line2D, Contains) {
  int prec = 3;
  auto l1 = g::Line2D::Make(g::Point2D(0, -1), g::Point2D(0, 1), prec);
  auto p0 = g::Point2D();
  auto p1 = g::Point2D(0, 10);
  auto p2 = g::Point2D(-10, 0);

  ASSERT_TRUE(l1.Contains(p0, prec));
  ASSERT_TRUE(l1.Contains(p1, prec));
  ASSERT_TRUE(!l1.Contains(p2, prec));
}

TEST(Line2D, Intersection) {
  int prec = 3;
  auto l1 = g::Line2D::Make(g::Point2D(0, -1), g::Point2D(0, 1), prec);
  auto l2 = g::Line2D::Make(g::Point2D(-1, 0), g::Point2D(1, 0), prec);

  EXPECT_TRUE(l1.Intersects(l2, prec));
  auto inter = l1.Intersection(l2, prec);

  ASSERT_TRUE(inter.has_value());
  ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));

  EXPECT_EQ(g::Point2D(0, 0), std::get<g::Point2D>(*inter));
}
