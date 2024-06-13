#include "polyline2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>
#include <vector>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(Polyline2D, Constructor) {
  int prec = 4;
  auto polyline = g::Polyline2D::Make({g::Point2D(-2, -5), g::Point2D(-2, -3), g::Point2D(2, -3), g::Point2D(2, 2)});
  ASSERT_EQ(4, polyline.Size());
  ASSERT_EQ(11, polyline.Length());

  // static assert (won't compile)
  EXPECT_ANY_THROW(g::Polyline2D::Make({}));                            // cannot make a segment in 0 points
  EXPECT_ANY_THROW(g::Polyline2D::Make({g::Point2D()}));                // cannot make a segment in 1 sole point
  EXPECT_ANY_THROW(g::Polyline2D::Make({g::Point2D(), g::Point2D()}));  // cannot make a segment in 1 sole unique point

  EXPECT_NO_THROW(g::Polyline2D::Make(
      {g::Point2D(), g::Point2D(1, 0), g::Point2D(1, 0)}));  // cannot make a segment in 1 sole unique point
  EXPECT_EQ(
      2, g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 0), g::Point2D(1, 0)}).Size());  // removed non-unique point

  EXPECT_EQ(2,
            g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 0), g::Point2D(3, 0)}).Size());  // removed collinear point
}

TEST(Polyline2D, Contains) {
  int prec = 4;
  std::vector<g::Point2D> points{g::Point2D(-2, -5), g::Point2D(-2, -3), g::Point2D(2, -3), g::Point2D(2, 2)};
  auto polyline = g::Polyline2D::Make(points, prec);

  // knots
  for (auto const& p : points) {
    ASSERT_TRUE(polyline.Contains(p, prec));
  }

  // mid points
  for (int i = 0; i < points.size() - 1; ++i) {
    auto avg = ((points[i].ToVector() + points[i + 1].ToVector()) / 2.0).ToPoint();
    ASSERT_TRUE(polyline.Contains(avg, prec));
  }

  // before the polyline
  ASSERT_FALSE(polyline.Contains(points[0] - (points[1] - points[0]), prec));
  // after the polyline
  ASSERT_FALSE(
      polyline.Contains(points[points.size() - 1] + (points[points.size() - 1] - points[points.size() - 2]), prec));
}

TEST(Polyline2D, Location) {
  int prec = 3;
  auto s1 = g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 0)});

  ASSERT_EQ(0.2, g::round_to(s1.Location(g::Point2D(0.2, 0), prec), prec));
  ASSERT_EQ(0.5, g::round_to(s1.Location(g::Point2D(0.5, 0), prec), prec));
  ASSERT_EQ(0.75, g::round_to(s1.Location(g::Point2D(0.75, 0), prec), prec));

  ASSERT_EQ(-1, g::round_to(s1.Location(g::Point2D(-1, 0), prec), prec));
  ASSERT_EQ(-0.1, g::round_to(s1.Location(g::Point2D(-0.1, 0), prec), prec));
  ASSERT_EQ(1.1, g::round_to(s1.Location(g::Point2D(1.1, 0), prec), prec));

  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, 1), prec)));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(0, -1), prec)));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(-1, -1), prec)));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, -1), prec)));
}

TEST(Polyline2D, Interpolate) {
  int prec = 4;
  auto poly = g::Polyline2D::FromWkt("LINESTRING (0 0, 3 0)");

  // on segment
  ASSERT_EQ(g::Point2D(0, 0), poly.Interpolate(0));

  ASSERT_EQ(g::Point2D(3, 0), poly.Interpolate(1));

  // on segment
  ASSERT_EQ(0.223, g::round_to(poly.Location(poly.Interpolate(0.223), prec), 3));

  // on line
  ASSERT_EQ(1.0, g::round_to(poly.Location(poly.Interpolate(1.2), prec), 1));

  ASSERT_EQ(0.0, g::round_to(poly.Location(poly.Interpolate(-0.2), prec), 1));
}

TEST(Polyline2D, IntersectionWLine) {
  int prec = 4;
  auto poly1 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -2, 1 -2, 1 2)");  // intersects x (-1 0, 1 0) and y (0 -2)
  auto poly2 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -0.5 2, 1 2, 2 1)");  // intersects y (0 2)
  auto poly3 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -1, -2 -2, -1 -3)");  // intersects x (-1 0)

  auto x = g::Line2D::Make(g::Point2D(), g::Vector2D(1, 0), prec);
  auto y = g::Line2D::Make(g::Point2D(), g::Vector2D(0, 1), prec);

  ASSERT_TRUE(poly1.Intersects(x, prec));
  {
    auto inter = poly1.Intersection(x, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline2D::MultiPoint>(*inter));
    auto multi_point = std::get<g::Polyline2D::MultiPoint>(*inter);
    ASSERT_EQ(g::Point2D(-1, 0), multi_point[0]);
    ASSERT_EQ(g::Point2D(1, 0), multi_point[1]);
  }

  ASSERT_TRUE(poly1.Intersects(y, prec));
  {
    auto inter = poly1.Intersection(y, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, -2), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(poly2.Intersects(y, prec));
  {
    auto inter = poly2.Intersection(y, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 2), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly2.Intersects(x, prec));

  ASSERT_TRUE(poly3.Intersects(x, prec));
  {
    auto inter = poly3.Intersection(x, prec);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-1, 0), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly3.Intersects(y, prec));
}

// TEST(LineSegment2D, IntersectionWRay) {
//   int prec = 4;
//   auto s1 = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(2, 1), prec);   // intersects r1, r2
//   auto s2 = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1), prec);     // intersects r1
//   auto s3 = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(-1, -1), prec);  // intersects r2

//   auto r1 = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D(0, 1), prec);
//   auto r2 = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D(1, 0), prec);
//   auto r1_rev = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D(0, -1), prec);   // no intersections
//   auto r2_rev = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D(-1, 0), prec);  // no intersections

//   ASSERT_TRUE(s1.Intersects(r1, prec));
//   {
//     auto inter = s1.Intersection(r1, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0.5, -0.5), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_TRUE(s1.Intersects(r2, prec));
//   {
//     auto inter = s1.Intersection(r2, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0.5, -0.5), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_TRUE(s2.Intersects(r1, prec));
//   {
//     auto inter = s2.Intersection(r1, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0.5, 1), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_FALSE(s2.Intersects(r2, prec));

//   ASSERT_TRUE(s3.Intersects(r2, prec));
//   {
//     auto inter = s3.Intersection(r2, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(-1, -0.5), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_FALSE(s3.Intersects(r1, prec));

//   ASSERT_FALSE(s1.Intersects(r1_rev, prec));
//   ASSERT_FALSE(s1.Intersects(r2_rev, prec));
//   ASSERT_FALSE(s2.Intersects(r1_rev, prec));
//   ASSERT_FALSE(s2.Intersects(r2_rev, prec));
//   ASSERT_FALSE(s3.Intersects(r1_rev, prec));
//   ASSERT_FALSE(s3.Intersects(r2_rev, prec));
// }

TEST(Polyline2D, Wkt) {
  ASSERT_EQ("LINESTRING (0 0, 1 1)", g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 1)}).ToWkt());
  ASSERT_EQ("LINESTRING (56491.62 -795.97, -9137.37 10.36, -10351.52 7.61)",
            g::Polyline2D::Make(
                {g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678), g::Point2D(-10351.516, 7.61)})
                .ToWkt(2));

  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)}, 6),
            g::Polyline2D::FromWkt("LINESTRING (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(-7.5, -60.7), g::Point2D()}),
            g::Polyline2D::FromWkt("  linestring( -7.5    -60.7, 0   0)"));
  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(0.645, -1.689741), g::Point2D(1, 0), g::Point2D(7, 41.365197)}, 6),
            g::Polyline2D::FromWkt("LinESTRing   ( 0.645  -1.689741  , 1 0 , 7 41.365197 )"));

  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestrin ( -7.5 -60.7, 0 0)"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("line string ( -7.5 -60.7, 0 0)"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring -7.5 -64.4, 0 0)"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring (-7.5 -64.4, 0 0"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring (-7.5 -64.4, 0 "));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring (-7.5 -64.4, "));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring ( -7.5 )"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring ( )"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring ( -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Polyline2D::FromWkt("linestring ( -7.5 -64.4 15.5, 0 0 0)"));
}

TEST(Polyline2D, ToFile) {
  int prec = 4;
  std::string path = (test_res_path / "temp" / "polyline.wkt").string();
  auto s = g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 0)}, prec);

  s.ToFile(path, prec);
  ASSERT_TRUE(fs::exists(path));

  g::Polyline2D s_file = g::Polyline2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(s, s_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST(Polyline2D, TestFromFile) {
  std::string path = (test_res_path / "polyline2d" / "polyline.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Polyline2D::FromFile(path));

  auto p = g::Polyline2D::FromFile(path);

  std::cout << "form file = " << p.ToWkt() << std::endl;
}

TEST(Polyline2D, DistanceTo) {
  int prec = 4;
  std::vector<g::Point2D> points{g::Point2D(-2, -5), g::Point2D(-2, -3), g::Point2D(2, -3), g::Point2D(2, 2)};
  auto polyline = g::Polyline2D::Make(points, prec);

  // knots
  for (auto const& p : points) {
    ASSERT_EQ(0, polyline.DistanceTo(p, prec));
  }

  // mid points
  for (int i = 0; i < points.size() - 1; ++i) {
    auto avg = ((points[i].ToVector() + points[i + 1].ToVector()) / 2.0).ToPoint();
    ASSERT_EQ(0, polyline.DistanceTo(avg, prec));
  }

  // before the polyline
  ASSERT_EQ(1, polyline.DistanceTo(g::Point2D(-3, -4), prec));
  ASSERT_EQ(1, polyline.DistanceTo(g::Point2D(0, -2), prec));
}

}  // namespace geompp_tests