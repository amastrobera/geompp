#include "line_segment2d.hpp"

#include "line2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <limits>
#include <optional>

namespace g = geompp;

TEST(LineSegment2D, Constructor) {
  auto s1 = g::LineSegment2D::Make(g::Point2D(), g::Point2D(1, 0));

  ASSERT_EQ(g::Point2D(), s1.First());
  ASSERT_EQ(g::Point2D(1, 0), s1.Last());

  EXPECT_ANY_THROW(g::LineSegment2D::Make(g::Point2D(), g::Point2D()));  // cannot make a segment in 1 sole point
}

TEST(LineSegment2D, Contains) {
  auto s1 = g::LineSegment2D::Make(g::Point2D(), g::Point2D(1, 0));
  ASSERT_TRUE(s1.Contains(g::Point2D(0, 0)));
  ASSERT_TRUE(s1.Contains(g::Point2D(0.5, 0)));
  ASSERT_TRUE(s1.Contains(g::Point2D(1, 0)));

  ASSERT_FALSE(s1.Contains(g::Point2D(1.1, 0)));
  ASSERT_FALSE(s1.Contains(g::Point2D(1, 1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(1, -1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-1, 0)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-1, 1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-1, -1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-0.1, 0)));
}

TEST(LineSegment2D, Location) {
  int prec = 3;
  auto s1 = g::LineSegment2D::Make(g::Point2D(), g::Point2D(1, 0));

  ASSERT_EQ(0.2, g::round_to(s1.Location(g::Point2D(0.2, 0), prec), prec));
  ASSERT_EQ(0.5, g::round_to(s1.Location(g::Point2D(0.5, 0), prec), prec));
  ASSERT_EQ(0.75, g::round_to(s1.Location(g::Point2D(0.75, 0), prec), prec));

  ASSERT_EQ(-1, g::round_to(s1.Location(g::Point2D(-1, 0), prec), prec));
  ASSERT_EQ(-0.1, g::round_to(s1.Location(g::Point2D(-0.1, 0), prec), prec));
  ASSERT_EQ(1.1, g::round_to(s1.Location(g::Point2D(1.1, 0), prec), prec));

  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, 1), prec)));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(0, -1), prec)));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(-1, -1), prec)));
}

TEST(LineSegment2D, Intersection) {
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

TEST(LineSegment2D, IntersectionWLine) {
  int prec = 4;
  auto s1 = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(2, 1), prec);   // intersects x, y
  auto s2 = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1), prec);     // intersects y
  auto s3 = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(-1, -1), prec);  // intersects x

  auto x = g::Line2D::Make(g::Point2D(), g::Vector2D(1, 0), prec);
  auto y = g::Line2D::Make(g::Point2D(), g::Vector2D(0, 1), prec);

  ASSERT_TRUE(s1.Intersects(x, prec));
  {
    auto inter = s1.Intersection(x, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(1, 0), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(s1.Intersects(y, prec));
  {
    auto inter = s1.Intersection(y, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, -1), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(s2.Intersects(y, prec));
  {
    auto inter = s2.Intersection(y, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 1), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(s2.Intersects(x, prec));

  ASSERT_TRUE(s3.Intersects(x, prec));
  {
    auto inter = s3.Intersection(x, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-1, 0), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(s3.Intersects(y, prec));
}

TEST(LineSegment2D, IntersectionWRay) {
  int prec = 4;
  auto s1 = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(2, 1), prec);   // intersects r1, r2
  auto s2 = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1), prec);     // intersects r1
  auto s3 = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(-1, -1), prec);  // intersects r2

  auto r1 = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D(0, 1), prec);
  auto r2 = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D(1, 0), prec);
  auto r1_rev = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D(0, -1), prec);   // no intersections
  auto r2_rev = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D(-1, 0), prec);  // no intersections

  ASSERT_TRUE(s1.Intersects(r1, prec));
  {
    auto inter = s1.Intersection(r1, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0.5, -0.5), std::get<g::Point2D>(*inter));
  }
  ASSERT_TRUE(s1.Intersects(r2, prec));
  {
    auto inter = s1.Intersection(r2, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0.5, -0.5), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(s2.Intersects(r1, prec));
  {
    auto inter = s2.Intersection(r1, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0.5, 1), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(s2.Intersects(r2, prec));

  ASSERT_TRUE(s3.Intersects(r2, prec));
  {
    auto inter = s3.Intersection(r2, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-1, -0.5), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(s3.Intersects(r1, prec));

  ASSERT_FALSE(s1.Intersects(r1_rev, prec));
  ASSERT_FALSE(s1.Intersects(r2_rev, prec));
  ASSERT_FALSE(s2.Intersects(r1_rev, prec));
  ASSERT_FALSE(s2.Intersects(r2_rev, prec));
  ASSERT_FALSE(s3.Intersects(r1_rev, prec));
  ASSERT_FALSE(s3.Intersects(r2_rev, prec));
}
