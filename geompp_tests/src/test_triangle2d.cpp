#include "triangle2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>
#include <tuple>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(Triangle2D, Constructor) {
  auto t = g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(1, 1));

  ASSERT_EQ(g::Point2D(0, 0.333), t.Centroid());

  EXPECT_ANY_THROW(g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(0, -1)));
  EXPECT_ANY_THROW(g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(-1, 1), g::Point2D(0, -1)));
  EXPECT_ANY_THROW(g::Triangle2D::Make(g::Point2D(), g::Point2D(), g::Point2D()));
}

TEST(Triangle2D, Contains) {
  std::tuple<g::Point2D, g::Point2D, g::Point2D> points = {g::Point2D(-1, 0), g::Point2D(1, 1), g::Point2D(-1, 1)};

  auto t = g::Triangle2D::Make(std::get<0>(points), std::get<1>(points), std::get<2>(points));

  // contains its corner points
  ASSERT_TRUE(t.Contains(std::get<0>(points)));
  ASSERT_TRUE(t.Contains(std::get<1>(points)));
  ASSERT_TRUE(t.Contains(std::get<2>(points)));

  // contains the centroid
  auto c = t.Centroid();
  ASSERT_TRUE(t.Contains(c));

  // contains points between the centroid and the corners
  ASSERT_TRUE(t.Contains(c + 0.5 * (std::get<0>(points) - c)));
  ASSERT_TRUE(t.Contains(c + 0.5 * (std::get<1>(points) - c)));
  ASSERT_TRUE(t.Contains(c + 0.5 * (std::get<2>(points) - c)));

  // contains the points along the borders
  ASSERT_TRUE(t.Contains(((std::get<0>(points).ToVector() + std::get<1>(points).ToVector()) / 2.0).ToPoint()));
  ASSERT_TRUE(t.Contains(((std::get<1>(points).ToVector() + std::get<2>(points).ToVector()) / 2.0).ToPoint()));
  ASSERT_TRUE(t.Contains(((std::get<2>(points).ToVector() + std::get<0>(points).ToVector()) / 2.0).ToPoint()));

  // does not contain external points (extension of the corner points along the centroid->corner line)
  ASSERT_FALSE(t.Contains(c + 2 * (std::get<0>(points) - c)));
  ASSERT_FALSE(t.Contains(c + 2 * (std::get<1>(points) - c)));
  ASSERT_FALSE(t.Contains(c + 2 * (std::get<2>(points) - c)));

  // outside (ahead of segments)
  ASSERT_FALSE(t.Contains(g::average({std::get<0>(points), std::get<1>(points)}) -
                          (std::get<1>(points) - std::get<0>(points)).Perp().Normalize()));
  ASSERT_FALSE(t.Contains(g::average({std::get<1>(points), std::get<2>(points)}) -
                          (std::get<2>(points) - std::get<1>(points)).Perp().Normalize()));
  ASSERT_FALSE(t.Contains(g::average({std::get<2>(points), std::get<0>(points)}) -
                          (std::get<0>(points) - std::get<2>(points)).Perp().Normalize()));
}

TEST(Triangle2D, Areas) {
  geompp::DECIMAL_PRECISION = 4;
  auto t_ccw = g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(1, 1));

  ASSERT_EQ(6.4721, g::round(t_ccw.Perimeter()));
  double sa_ccw = t_ccw.SignedArea();
  ASSERT_TRUE(sa_ccw > 0);
  double a_ccw = t_ccw.Area();
  ASSERT_EQ(2.0, g::round(a_ccw));
  ASSERT_EQ(a_ccw, sa_ccw);

  auto verts_ccw = t_ccw.Vertices();
  auto t_cw = g::Triangle2D::Make(std::get<0>(verts_ccw), std::get<2>(verts_ccw), std::get<1>(verts_ccw));

  ASSERT_EQ(6.4721, g::round(t_cw.Perimeter()));
  double sa_cw = t_cw.SignedArea();
  ASSERT_TRUE(sa_cw < 0);
  double a_cw = t_cw.Area();
  ASSERT_EQ(2.0, g::round(a_cw));
  ASSERT_EQ(-a_cw, sa_cw);

  ASSERT_EQ(a_ccw, a_cw);
  ASSERT_EQ(-sa_ccw, sa_cw);
}

TEST(Triangle2D, ToPolygon) {
  geompp::DECIMAL_PRECISION = 4;
  auto t = g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(1, 1));

  auto verts = t.Vertices();
  auto poly = g::Polygon2D::Make({std::get<0>(verts), std::get<1>(verts), std::get<2>(verts)});
  ASSERT_EQ(poly, t.ToPolygon());

  ASSERT_EQ(poly.ToWkt(), t.ToPolygon().ToWkt());
}

TEST(Triangle2D, ToAxis) {
  auto t = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");

  auto axis = t.ToAxis();
  ASSERT_EQ(g::Vector2D(1, 1), std::get<0>(axis));
  ASSERT_EQ(g::Vector2D(-1, 1), std::get<1>(axis));
}

TEST(Triangle2D, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto t = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");

  // points
  auto points = t.Vertices();
  ASSERT_EQ(0, t.DistanceTo(std::get<0>(points)));
  ASSERT_EQ(0, t.DistanceTo(std::get<1>(points)));
  ASSERT_EQ(0, t.DistanceTo(std::get<2>(points)));
  ASSERT_EQ(0, t.DistanceTo(t.Centroid()));

  // on line
  ASSERT_EQ(0, t.DistanceTo(g::average({std::get<0>(points), std::get<1>(points)})));
  ASSERT_EQ(0, t.DistanceTo(g::average({std::get<1>(points), std::get<2>(points)})));
  ASSERT_EQ(0, t.DistanceTo(g::average({std::get<2>(points), std::get<0>(points)})));

  // outside (ahead of segments)
  ASSERT_EQ(1.0, t.DistanceTo(g::average({std::get<0>(points), std::get<1>(points)}) -
                              (std::get<1>(points) - std::get<0>(points)).Perp().Normalize()));

  ASSERT_EQ(1.0, t.DistanceTo(g::average({std::get<1>(points), std::get<2>(points)}) -
                              (std::get<2>(points) - std::get<1>(points)).Perp().Normalize()));
  ASSERT_EQ(1.0, t.DistanceTo(g::average({std::get<2>(points), std::get<0>(points)}) -
                              (std::get<0>(points) - std::get<2>(points)).Perp().Normalize()));

  // outside (ahead of vertices)
  ASSERT_EQ(1.0, t.DistanceTo(g::Point2D(0, -2)));
  ASSERT_EQ(1.0, t.DistanceTo(g::Point2D(2, 0)));
  ASSERT_EQ(1.0, t.DistanceTo(g::Point2D(-2, 0)));
}

TEST(Triangle2D, Interpolate) {
  geompp::DECIMAL_PRECISION = 4;
  auto tri = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");

  auto points = tri.Vertices();

  // triangle points
  ASSERT_EQ(std::get<0>(points), tri.Interpolate(0, 0));
  ASSERT_EQ(std::get<1>(points), tri.Interpolate(1, 0));
  ASSERT_EQ(std::get<2>(points), tri.Interpolate(0, 1));

  // centroid
  auto c = tri.Centroid();
  auto loc = tri.Location(c);
  ASSERT_EQ(c, tri.Interpolate(std::get<0>(loc), std::get<1>(loc)));
}

TEST(Triangle2D, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto tri = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 1, -1 1)");

  auto x = g::Line2D::Make(g::Point2D(), g::Vector2D(1, 0));
  auto y = g::Line2D::Make(g::Point2D(), g::Vector2D(0, 1));

  auto px = g::Line2D::Make(g::Point2D(0, -1), g::Vector2D(1, -1));
  auto py = g::Line2D::Make(g::Point2D(1, 1), g::Vector2D(1, 2));

  auto pax = g::Line2D::Make(g::Point2D(0, 2), g::Vector2D(1, 2));
  auto pay = g::Line2D::Make(g::Point2D(2, 0), g::Vector2D(2, 1));

  ASSERT_TRUE(tri.Intersects(x));
  {
    auto inter = tri.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(*inter));
    EXPECT_EQ(g::LineSegment2D::Make({-0.5, 0}, {0.5, 0}), std::get<g::LineSegment2D>(*inter));
  }

  ASSERT_TRUE(tri.Intersects(y));
  {
    auto inter = tri.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(*inter));
    EXPECT_EQ(g::LineSegment2D::Make({0, -1}, {0, 1}), std::get<g::LineSegment2D>(*inter));
  }

  ASSERT_TRUE(tri.Intersects(px));
  {
    auto inter = tri.Intersection(px);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, -1), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(tri.Intersects(py));
  {
    auto inter = tri.Intersection(py);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(1, 1), std::get<g::Point2D>(*inter));
  }

  ASSERT_FALSE(tri.Intersects(pax));
  ASSERT_FALSE(tri.Intersects(pay));
}

// TEST(Triangle2D, IntersectionWRay) {
//   geompp::DECIMAL_PRECISION =  4;
//   auto r1 = g::Ray2D::Make(g::Point2D(-1, 1), g::Vector2D(1, -1));
//   auto r2 = g::Ray2D::Make(g::Point2D(-1, -1), g::Vector2D(1, 1));    // intersects r1 in (0,0)
//   auto r3 = g::Ray2D::Make(g::Point2D(-0.5, 0), g::Vector2D(0, -1));  // intersects r2 in (-0.5,-0.5)
//   auto r4 = g::Ray2D::Make(g::Point2D(1, -0.5), g::Vector2D(0, 1));   // intersects r2 in (1,1)

//   ASSERT_TRUE(r1.Intersects(r2));
//   {
//     auto inter = r1.Intersection(r2);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0, 0), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_FALSE(r1.Intersects(r3));
//   ASSERT_FALSE(r1.Intersection(r3).has_value());
//   ASSERT_FALSE(r1.Intersects(r4));
//   ASSERT_FALSE(r1.Intersection(r4).has_value());

//   ASSERT_TRUE(r2.Intersects(r3));
//   {
//     auto inter = r2.Intersection(r3);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(-0.5, -0.5), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_TRUE(r2.Intersects(r4));
//   {
//     auto inter = r2.Intersection(r4);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(1, 1), std::get<g::Point2D>(*inter));
//   }
// }

// TEST(Triangle2D, IntersectionWRay) {
//   geompp::DECIMAL_PRECISION =  4;
//   auto s1 = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(2, 1));   // intersects r1, r2
//   auto s2 = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1));     // intersects r1
//   auto s3 = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(-1, -1));  // intersects r2

//   auto r1 = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D(0, 1));
//   auto r2 = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D(1, 0));
//   auto r1_rev = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D(0, -1));   // no intersections
//   auto r2_rev = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D(-1, 0));  // no intersections

//   ASSERT_TRUE(s1.Intersects(r1));
//   {
//     auto inter = s1.Intersection(r1);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0.5, -0.5), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_TRUE(s1.Intersects(r2));
//   {
//     auto inter = s1.Intersection(r2);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0.5, -0.5), std::get<g::Point2D>(*inter));
//   }

//   ASSERT_TRUE(s2.Intersects(r1));
//   {
//     auto inter = s2.Intersection(r1);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(0.5, 1), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_FALSE(s2.Intersects(r2));

//   ASSERT_TRUE(s3.Intersects(r2));
//   {
//     auto inter = s3.Intersection(r2);
//     ASSERT_TRUE(inter.has_value());
//     ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
//     EXPECT_EQ(g::Point2D(-1, -0.5), std::get<g::Point2D>(*inter));
//   }
//   ASSERT_FALSE(s3.Intersects(r1));

//   ASSERT_FALSE(s1.Intersects(r1_rev));
//   ASSERT_FALSE(s1.Intersects(r2_rev));
//   ASSERT_FALSE(s2.Intersects(r1_rev));
//   ASSERT_FALSE(s2.Intersects(r2_rev));
//   ASSERT_FALSE(s3.Intersects(r1_rev));
//   ASSERT_FALSE(s3.Intersects(r2_rev));
// }

TEST(Triangle2D, Wkt) {
  ASSERT_EQ("TRIANGLE (0 0, 1 1, 0 2)",
            g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 2)).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("TRIANGLE (56491.62 -795.97, -9137.37 10.36, 321.13 206.62)",
            g::Triangle2D::Make(g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678),
                                g::Point2D(321.1302, 206.619749))
                .ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 2)),
            g::Triangle2D::FromWkt("TRIANGLE (0 0, 1 1, 0 2)"));
  EXPECT_EQ(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 2)),
            g::Triangle2D::FromWkt("  triangle( 0     0 , 1   1  , 0 2   )"));
  EXPECT_EQ(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 2)),
            g::Triangle2D::FromWkt("triANGle   ( 0 0  , 1 1 , 0   2    )"));

  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangl ( -7.5 -60.7, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("tri angle ( -7.5 -60.7, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle -7.5 -64.4, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle (-7.5 -64.4, 0 0, 1 1"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle (-7.5 -64.4, 0 0, 1 "));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle (-7.5 -64.4, 0 0"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle ( -7.5 )"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle ( )"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle ( -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Triangle2D::FromWkt("triangle ( -7.5 -64.4 15.5, 0 0 0, 1 1 1)"));
}

TEST(Triangle2D, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "triangle.wkt").string();
  auto s = g::Triangle2D::Make(g::Point2D(), g::Point2D(1, 0), g::Point2D(0, 2));

  s.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Triangle2D s_file = g::Triangle2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(s, s_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST(Triangle2D, TestFromFile) {
  std::string path = (test_res_path / "triangle2d" / "triangle.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Triangle2D::FromFile(path));

  auto p = g::Triangle2D::FromFile(path);

  std::cout << "form file = " << p.ToWkt() << std::endl;
}

}  // namespace geompp_tests
