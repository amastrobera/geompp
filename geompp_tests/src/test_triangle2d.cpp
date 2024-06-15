#include "triangle2d.hpp"

#include "line2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(Triangle2D, Constructor) {
  // auto s1 = g::Triangle2D::Make(g::Point2D(), g::Point2D(1, 0));

  // ASSERT_EQ(g::Point2D(), s1.First());
  // ASSERT_EQ(g::Point2D(1, 0), s1.Last());

  // EXPECT_ANY_THROW(g::LineSegment2D::Make(g::Point2D(), g::Point2D()));  // cannot make a segment in 1 sole point
}

// TEST(Triangle2D, Contains) {
//   auto s1 = g::LineSegment2D::Make(g::Point2D(), g::Point2D(1, 0));
//   ASSERT_TRUE(s1.Contains(g::Point2D(0, 0)));
//   ASSERT_TRUE(s1.Contains(g::Point2D(0.5, 0)));
//   ASSERT_TRUE(s1.Contains(g::Point2D(1, 0)));

//   ASSERT_FALSE(s1.Contains(g::Point2D(1.1, 0)));
//   ASSERT_FALSE(s1.Contains(g::Point2D(1, 1)));
//   ASSERT_FALSE(s1.Contains(g::Point2D(1, -1)));
//   ASSERT_FALSE(s1.Contains(g::Point2D(-1, 0)));
//   ASSERT_FALSE(s1.Contains(g::Point2D(-1, 1)));
//   ASSERT_FALSE(s1.Contains(g::Point2D(-1, -1)));
//   ASSERT_FALSE(s1.Contains(g::Point2D(-0.1, 0)));
// }

// TEST(Triangle2D, Location) {
//   int prec = 3;
//   auto s1 = g::LineSegment2D::Make(g::Point2D(), g::Point2D(1, 0));

//   ASSERT_EQ(0.2, g::round_to(s1.Location(g::Point2D(0.2, 0), prec), prec));
//   ASSERT_EQ(0.5, g::round_to(s1.Location(g::Point2D(0.5, 0), prec), prec));
//   ASSERT_EQ(0.75, g::round_to(s1.Location(g::Point2D(0.75, 0), prec), prec));

//   ASSERT_EQ(-1, g::round_to(s1.Location(g::Point2D(-1, 0), prec), prec));
//   ASSERT_EQ(-0.1, g::round_to(s1.Location(g::Point2D(-0.1, 0), prec), prec));
//   ASSERT_EQ(1.1, g::round_to(s1.Location(g::Point2D(1.1, 0), prec), prec));

//   ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, 1), prec)));
//   ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(0, -1), prec)));
//   ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(-1, -1), prec)));
//   ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, -1), prec)));
// }

// TEST(Triangle2D, Interpolate) {
//   int prec = 4;
//   auto seg = g::LineSegment2D::FromWkt("LINESTRING (0 0, 3 0)");

//   // on segment
//   ASSERT_EQ(seg.First(), seg.Interpolate(0));

//   ASSERT_EQ(seg.Last(), seg.Interpolate(1));

//   // on segment
//   ASSERT_EQ(0.223, g::round_to(seg.Location(seg.Interpolate(0.223), prec), 3));

//   // on line
//   ASSERT_EQ(1.0, g::round_to(seg.Location(seg.Interpolate(1.2), prec), 1));

//   ASSERT_EQ(0.0, g::round_to(seg.Location(seg.Interpolate(-0.2), prec), 1));
// }

// TEST(Triangle2D, Intersection) {
//   int prec = 4;
//   auto r1 = g::Ray2D::Make(g::Point2D(-1, 1), g::Vector2D(1, -1), prec);
//   auto r2 = g::Ray2D::Make(g::Point2D(-1, -1), g::Vector2D(1, 1), prec);    // intersects r1 in (0,0)
//   auto r3 = g::Ray2D::Make(g::Point2D(-0.5, 0), g::Vector2D(0, -1), prec);  // intersects r2 in (-0.5,-0.5)
//   auto r4 = g::Ray2D::Make(g::Point2D(1, -0.5), g::Vector2D(0, 1), prec);   // intersects r2 in (1,1)

//   ASSERT_TRUE(r1.Intersects(r2, prec));
//   {
//     auto inter = r1.Intersection(r2, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0, 0), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_FALSE(r1.Intersects(r3, prec));
//   ASSERT_FALSE(r1.Intersection(r3, prec).has_value());
//   ASSERT_FALSE(r1.Intersects(r4, prec));
//   ASSERT_FALSE(r1.Intersection(r4, prec).has_value());

//   ASSERT_TRUE(r2.Intersects(r3, prec));
//   {
//     auto inter = r2.Intersection(r3, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(-0.5, -0.5), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_TRUE(r2.Intersects(r4, prec));
//   {
//     auto inter = r2.Intersection(r4, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(1, 1), std::get<g::Point2D>(*inter));
//   }
// }

// TEST(Triangle2D, IntersectionWLine) {
//   int prec = 4;
//   auto s1 = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(2, 1), prec);   // intersects x, y
//   auto s2 = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1), prec);     // intersects y
//   auto s3 = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(-1, -1), prec);  // intersects x

//   auto x = g::Line2D::Make(g::Point2D(), g::Vector2D(1, 0), prec);
//   auto y = g::Line2D::Make(g::Point2D(), g::Vector2D(0, 1), prec);

//   ASSERT_TRUE(s1.Intersects(x, prec));
//   {
//     auto inter = s1.Intersection(x, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(1, 0), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_TRUE(s1.Intersects(y, prec));
//   {
//     auto inter = s1.Intersection(y, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0, -1), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_TRUE(s2.Intersects(y, prec));
//   {
//     auto inter = s2.Intersection(y, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0, 1), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_FALSE(s2.Intersects(x, prec));

//   ASSERT_TRUE(s3.Intersects(x, prec));
//   {
//     auto inter = s3.Intersection(x, prec);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(-1, 0), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_FALSE(s3.Intersects(y, prec));
// }

// TEST(Triangle2D, IntersectionWRay) {
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

TEST(Triangle2D, Wkt) {
  // ASSERT_EQ("LINESTRING (0 0, 1 1)", g::LineSegment2D::Make(g::Point2D(), g::Point2D(1, 1)).ToWkt());
  // ASSERT_EQ("LINESTRING (56491.62 -795.97, -9137.37 10.36)",
  //           g::LineSegment2D::Make(g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678)).ToWkt(2));

  // EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)),
  //           g::LineSegment2D::FromWkt("LINESTRING (256.1343 -684.64971, -601.674503 7.361975)"));
  // EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(-7.5, -60.7), g::Point2D()),
  //           g::LineSegment2D::FromWkt("  linestring( -7.5    -60.7, 0   0)"));
  // EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(0.645, -1.689741), g::Point2D(1, 0)),
  //           g::LineSegment2D::FromWkt("LinESTRing   ( 0.645  -1.689741  , 1 0  )"));

  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("angelo"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestrin ( -7.5 -60.7, 0 0)"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("line string ( -7.5 -60.7, 0 0)"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring -7.5 -64.4, 0 0)"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring (-7.5 -64.4, 0 0"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring (-7.5 -64.4, 0 "));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring (-7.5 -64.4, "));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( -7.5 )"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( )"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( -7.5 -64.4 15.5)"));
  // EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( -7.5 -64.4 15.5, 0 0 0)"));
}

TEST(Triangle2D, ToFile) {
  int prec = 4;
  std::string path = (test_res_path / "temp" / "triangle.wkt").string();
  // auto s = g::LineSegment2D::Make(g::Point2D(), g::Point2D(1, 0));

  // s.ToFile(path, prec);
  // ASSERT_TRUE(fs::exists(path));

  // g::LineSegment2D s_file = g::LineSegment2D::FromFile(path);  // TODO make assert no throw for the whole call

  // EXPECT_EQ(s, s_file);

  // EXPECT_NO_THROW(fs::remove(path));
}

TEST(Triangle2D, TestFromFile) {
  std::string path = (test_res_path / "triangle2d" / "triangle.wkt").string();

  // ASSERT_TRUE(fs::exists(path));

  // ASSERT_NO_THROW(g::LineSegment2D::FromFile(path));

  // auto p = g::LineSegment2D::FromFile(path);

  // std::cout << "form file = " << p.ToWkt() << std::endl;
}

// TEST(Triangle2D, DistanceTo) {
//   int prec = 4;
//   auto seg = g::LineSegment2D::FromWkt("LINESTRING (0 0, 3 0)");

//   // on segment
//   auto p1 = g::Point2D();
//   EXPECT_EQ(0, seg.DistanceTo(p1, prec));

//   auto p2 = g::Point2D(3, 0);
//   EXPECT_EQ(0, seg.DistanceTo(p2, prec));

//   // on line
//   auto p3 = g::Point2D(-20, 0);
//   EXPECT_EQ(20, seg.DistanceTo(p3, prec));

//   auto p4 = g::Point2D(12, 0);
//   EXPECT_EQ(9, seg.DistanceTo(p4, prec));

//   // Q1
//   auto p5 = g::Point2D(1.5, 12);
//   EXPECT_EQ(12, seg.DistanceTo(p5, prec));

//   // Q2
//   auto p6 = g::Point2D(-5, 10);
//   EXPECT_EQ(p6.DistanceTo(seg.First(), prec), seg.DistanceTo(p6, prec));

//   // Q3
//   auto p7 = g::Point2D(-2, -4);
//   EXPECT_EQ(p7.DistanceTo(seg.First(), prec), seg.DistanceTo(p7, prec));

//   // Q4
//   auto p8 = g::Point2D(3, -7);
//   EXPECT_EQ(7, seg.DistanceTo(p8, prec));
// }

}  // namespace geompp_tests