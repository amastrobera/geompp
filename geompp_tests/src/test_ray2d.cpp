#include "ray2d.hpp"

#include "line2d.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

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

TEST(Ray2D, Wkt) {
  ASSERT_EQ("RAY (0 0, 0.707 0.707)", g::Ray2D::Make(g::Point2D(), g::Vector2D(1, 1)).ToWkt());  // normalized vector!
  ASSERT_EQ("RAY (56491.62 -795.97, -1 0)",
            g::Ray2D::Make(g::Point2D(56491.6164, -795.97416), g::Vector2D(-9137.3679, 10.35678))
                .ToWkt(2));  // normalized vector!

  EXPECT_EQ(g::Ray2D::Make(g::Point2D(256.1343, -684.64971), g::Vector2D(-601.674503, 7.361975)),
            g::Ray2D::FromWkt("RAY (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Ray2D::Make(g::Point2D(-7.5, -60.7), g::Vector2D(1, 0)),
            g::Ray2D::FromWkt("  ray( -7.5    -60.7, 1   0)"));
  EXPECT_EQ(g::Ray2D::Make(g::Point2D(0.645, -1.689741), g::Vector2D(1, 0)),
            g::Ray2D::FromWkt("ray   ( 0.645  -1.689741  , 1 0  )"));

  EXPECT_ANY_THROW(g::Ray2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ra ( -7.5 -60.7, 0 0)"));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray -7.5 -64.4, 0 0)"));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray (-7.5 -64.4, 0 0"));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray (-7.5 -64.4, 0 "));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray (-7.5 -64.4, "));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray ( -7.5 )"));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray ( )"));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray ( -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Ray2D::FromWkt("ray ( -7.5 -64.4 15.5, 0 0 0)"));
}

TEST(Ray2D, ToFile) {
  int prec = 4;
  std::string path = (test_res_path / "temp" / "ray.wkt").string();
  auto v = g::Ray2D::Make(g::Point2D(12.32, -61.6164), g::Vector2D(1, 1));

  v.ToFile(path, prec);
  ASSERT_TRUE(fs::exists(path));

  g::Ray2D v_file = g::Ray2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(v, v_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST(Ray2D, TestFromFile) {
  std::string path = (test_res_path / "ray2d" / "ray.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Ray2D::FromFile(path));

  auto p = g::Ray2D::FromFile(path);

  std::cout << "form file = " << p.ToWkt() << std::endl;
}

TEST(Ray2D, DistanceTo) {
  int prec = 4;
  auto ray = g::Ray2D::FromWkt("RAY (0 0, 1 0)");

  // on segment
  auto p1 = g::Point2D();
  EXPECT_EQ(0, ray.DistanceTo(p1, prec));

  auto p2 = g::Point2D(3, 0);
  EXPECT_EQ(0, ray.DistanceTo(p2, prec));

  // on line
  auto p3 = g::Point2D(-20, 0);
  EXPECT_EQ(20, ray.DistanceTo(p3, prec));

  auto p4 = g::Point2D(12, 0);
  EXPECT_EQ(0, ray.DistanceTo(p4, prec));

  // Q1
  auto p5 = g::Point2D(12, 12);
  EXPECT_EQ(12, ray.DistanceTo(p5, prec));

  // Q2
  auto p6 = g::Point2D(-5, 10);
  EXPECT_EQ(p6.DistanceTo(ray.Origin(), prec), ray.DistanceTo(p6, prec));

  // Q3
  auto p7 = g::Point2D(-2, -4);
  EXPECT_EQ(p7.DistanceTo(ray.Origin(), prec), ray.DistanceTo(p7, prec));

  // Q4
  auto p8 = g::Point2D(3, -7);
  EXPECT_EQ(7, ray.DistanceTo(p8, prec));
}

}  // namespace geompp_tests