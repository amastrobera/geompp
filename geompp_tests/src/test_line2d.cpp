#include "line2d.hpp"

#include "point2d.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>

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

TEST(Line2D, Wkt) {
  ASSERT_EQ("LINE (0 0, 1 1)", g::Line2D::Make(g::Point2D(), g::Point2D(1, 1)).ToWkt());
  ASSERT_EQ("LINE (56491.62 -795.97, -9137.37 10.36)",
            g::Line2D::Make(g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678)).ToWkt(2));

  EXPECT_EQ(g::Line2D::Make(g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)),
            g::Line2D::FromWkt("LINE (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(-7.5, -60.7), g::Point2D()),
            g::Line2D::FromWkt("  LINE( -7.5    -60.7, 0   0)"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(0.645, -1.689741), g::Point2D(1, 0)),
            g::Line2D::FromWkt("LINE   ( 0.645  -1.689741  , 1 0  )"));

  EXPECT_ANY_THROW(g::Line2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("lin ( -7.5 -60.7, 0 0)"));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE -7.5 -64.4, 0 0)"));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE (-7.5 -64.4, 0 0"));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE (-7.5 -64.4, 0 "));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE (-7.5 -64.4, "));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE ( -7.5 )"));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE ( )"));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE ( -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Line2D::FromWkt("LINE ( -7.5 -64.4 15.5, 0 0 0)"));
}

TEST(Line2D, ToFile) {
    int prec = 4;
    std::string path = "line.wkt";
    auto v = g::Line2D::Make(g::Point2D(12.32, -61.6164), g::Point2D(-14.64661, -9.1641));

    ASSERT_NO_THROW(v.ToFile(path, prec));

    g::Line2D v_file = g::Line2D::FromFile(path);  // TODO make assert no throw for the whole call

    EXPECT_EQ(v, v_file, result);
}