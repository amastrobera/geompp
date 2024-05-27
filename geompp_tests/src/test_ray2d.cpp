#include "line2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <optional>
#include <variant>

namespace g = geompp;

TEST(Ray2D, Constructor) {
  auto r1 = g::Ray2D::Make(g::Point2D(), g::Vector2D(1, 0));

  ASSERT_EQ(g::Point2D(), r1.Origin());
  ASSERT_EQ(g::Vector2D(1, 0), r1.Direction());

  EXPECT_ANY_THROW(g::Ray2D::Make(g::Point2D(), g::Vector2D(0, 0)));  // cannot make a ray going no where
}

TEST(Ray2D, Contains) {
  auto r1 = g::Ray2D::Make(g::Point2D(), g::Vector2D(1, 0));
  ASSERT_TRUE(r1.Contains(g::Point2D(0, 0)));
  ASSERT_TRUE(r1.Contains(g::Point2D(1, 0)));
  ASSERT_TRUE(r1.Contains(g::Point2D(30, 0)));

  ASSERT_FALSE(r1.Contains(g::Point2D(1, 1)));
  ASSERT_FALSE(r1.Contains(g::Point2D(1, -1)));
  ASSERT_FALSE(r1.Contains(g::Point2D(-1, 0)));
  ASSERT_FALSE(r1.Contains(g::Point2D(-1, 1)));
  ASSERT_FALSE(r1.Contains(g::Point2D(-1, -1)));
}

TEST(Ray2D, AheadBehind) {
  auto r1 = g::Ray2D::Make(g::Point2D(), g::Vector2D(1, 0));

  ASSERT_TRUE(r1.IsAhead(g::Point2D(1, 0)));
  ASSERT_TRUE(r1.IsAhead(g::Point2D(1, 1)));
  ASSERT_TRUE(r1.IsAhead(g::Point2D(1, -1)));
  ASSERT_TRUE(r1.IsAhead(g::Point2D(0, 1)));
  ASSERT_TRUE(r1.IsAhead(g::Point2D(0, -1)));

  ASSERT_FALSE(r1.IsAhead(g::Point2D(-1, 0)));
  ASSERT_FALSE(r1.IsAhead(g::Point2D(-1, 1)));
  ASSERT_FALSE(r1.IsAhead(g::Point2D(-1, -1)));

  ASSERT_TRUE(r1.IsBehind(g::Point2D(-1, 0)));
  ASSERT_TRUE(r1.IsBehind(g::Point2D(-1, 1)));
  ASSERT_TRUE(r1.IsBehind(g::Point2D(-1, -1)));

  ASSERT_FALSE(r1.IsBehind(g::Point2D(1, 0)));
  ASSERT_FALSE(r1.IsBehind(g::Point2D(1, 1)));
  ASSERT_FALSE(r1.IsBehind(g::Point2D(1, -1)));
  ASSERT_FALSE(r1.IsBehind(g::Point2D(0, 1)));
  ASSERT_FALSE(r1.IsBehind(g::Point2D(0, -1)));
}

TEST(Ray2D, Intersection) {
  int prec = 4;
  auto r1 = g::Ray2D::Make(g::Point2D(-1, 1), g::Vector2D(1, -1), prec);
  auto r2 = g::Ray2D::Make(g::Point2D(-1, -1), g::Vector2D(1, 1), prec);    // intersects r1 in (0,0)
  auto r3 = g::Ray2D::Make(g::Point2D(-0.5, 0), g::Vector2D(0, -1), prec);  // intersects r2 in (-0.5,-0.5)
  auto r4 = g::Ray2D::Make(g::Point2D(1, -0.5), g::Vector2D(0, 1), prec);   // intersects r2 in (1,1)

  ASSERT_TRUE(r1.Intersects(r2, prec));
  {
    auto inter = r1.Intersection(r2, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 0), std::get<g::Point2D>(*inter));
  }

  ASSERT_FALSE(r1.Intersects(r3, prec));
  ASSERT_FALSE(r1.Intersection(r3, prec).has_value());
  ASSERT_FALSE(r1.Intersects(r4, prec));
  ASSERT_FALSE(r1.Intersection(r4, prec).has_value());

  ASSERT_TRUE(r2.Intersects(r3, prec));
  {
    auto inter = r2.Intersection(r3, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-0.5, -0.5), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(r2.Intersects(r4, prec));
  {
    auto inter = r2.Intersection(r4, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(1, 1), std::get<g::Point2D>(*inter));
  }
}

TEST(Ray2D, IntersectionWLine) {
  int prec = 4;
  auto r1 = g::Ray2D::Make(g::Point2D(-1, 1), g::Vector2D(1, -1), prec);
  auto r2 = g::Ray2D::Make(g::Point2D(1, -1), g::Vector2D(1, 1), prec);  // intersects r1 in (0,0)

  auto x = g::Line2D::Make(g::Point2D(), g::Vector2D(1, 0), prec);
  auto y = g::Line2D::Make(g::Point2D(), g::Vector2D(0, 1), prec);

  ASSERT_TRUE(r1.Intersects(x, prec));
  {
    auto inter = r1.Intersection(x, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 0), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(r1.Intersects(y, prec));
  {
    auto inter = r1.Intersection(y, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 0), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(r2.Intersects(x, prec));
  {
    auto inter = r2.Intersection(x, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(2, 0), std::get<g::Point2D>(*inter));
  }

  ASSERT_FALSE(r2.Intersects(y, prec));
}
