#include "triangle3d.hpp"

#include "line3d.hpp"
// #include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
// #include "ray3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>
#include <tuple>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Triangle3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Triangle3DTest, Constructor) {
  auto t = g::Triangle3D::Make(g::Point3D(-1, 1, 2), g::Point3D(0, -1, 2), g::Point3D(1, 1, 2));

  ASSERT_EQ(g::Point3D(0, 0.333, 2), t.Centroid());

  EXPECT_ANY_THROW(g::Triangle3D::Make(g::Point3D(-1, 1, 0), g::Point3D(0, -1, 0), g::Point3D(0, -1, 0)));
  EXPECT_ANY_THROW(g::Triangle3D::Make(g::Point3D(-1, 1, 2), g::Point3D(-1, 1, 2), g::Point3D(0, -1, 2)));
  EXPECT_ANY_THROW(g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D::Zero(), g::Point3D::Zero()));
}

////TEST(Triangle3D, Contains) {
////  std::tuple<g::Point3D, g::Point3D, g::Point3D> points = {g::Point3D(-1, 0), g::Point3D(1, 1), g::Point3D(-1, 1)};
////
////  auto t = g::Triangle3D::Make(std::get<0>(points), std::get<1>(points), std::get<2>(points));
////
////  // contains its corner points
////  ASSERT_TRUE(t.Contains(std::get<0>(points)));
////  ASSERT_TRUE(t.Contains(std::get<1>(points)));
////  ASSERT_TRUE(t.Contains(std::get<2>(points)));
////
////  // contains the centroid
////  auto c = t.Centroid();
////  ASSERT_TRUE(t.Contains(c));
////
////  // contains points between the centroid and the corners
////  ASSERT_TRUE(t.Contains(c + 0.5 * (std::get<0>(points) - c)));
////  ASSERT_TRUE(t.Contains(c + 0.5 * (std::get<1>(points) - c)));
////  ASSERT_TRUE(t.Contains(c + 0.5 * (std::get<2>(points) - c)));
////
////  // contains the points along the borders
////  ASSERT_TRUE(t.Contains(((std::get<0>(points).ToVector() + std::get<1>(points).ToVector()) / 2.0).ToPoint()));
////  ASSERT_TRUE(t.Contains(((std::get<1>(points).ToVector() + std::get<2>(points).ToVector()) / 2.0).ToPoint()));
////  ASSERT_TRUE(t.Contains(((std::get<2>(points).ToVector() + std::get<0>(points).ToVector()) / 2.0).ToPoint()));
////
////  // does not contain external points (extension of the corner points along the centroid->corner line)
////  ASSERT_FALSE(t.Contains(c + 2 * (std::get<0>(points) - c)));
////  ASSERT_FALSE(t.Contains(c + 2 * (std::get<1>(points) - c)));
////  ASSERT_FALSE(t.Contains(c + 2 * (std::get<2>(points) - c)));
////
////  // outside (ahead of segments)
////  ASSERT_FALSE(t.Contains(g::Point3D::average({std::get<0>(points), std::get<1>(points)}) -
////                          (std::get<1>(points) - std::get<0>(points)).Perp().Normalize()));
////  ASSERT_FALSE(t.Contains(g::Point3D::average({std::get<1>(points), std::get<2>(points)}) -
////                          (std::get<2>(points) - std::get<1>(points)).Perp().Normalize()));
////  ASSERT_FALSE(t.Contains(g::Point3D::average({std::get<2>(points), std::get<0>(points)}) -
////                          (std::get<0>(points) - std::get<2>(points)).Perp().Normalize()));
////}
//
////TEST(Triangle3D, Areas) {
////  geompp::DECIMAL_PRECISION =  4;
////  auto t_ccw = g::Triangle3D::Make(g::Point3D(-1, 1), g::Point3D(0, -1), g::Point3D(1, 1));
////
////  ASSERT_EQ(6.4721, g::round(t_ccw.Perimeter()));
////  double sa_ccw = t_ccw.SignedArea();
////  ASSERT_TRUE(sa_ccw > 0);
////  double a_ccw = t_ccw.Area();
////  ASSERT_EQ(2.0, g::round(a_ccw));
////  ASSERT_EQ(a_ccw, sa_ccw);
////
////  auto verts_ccw = t_ccw.Vertices();
////  auto t_cw = g::Triangle3D::Make(std::get<0>(verts_ccw), std::get<2>(verts_ccw), std::get<1>(verts_ccw));
////
////  ASSERT_EQ(6.4721, g::round(t_cw.Perimeter()));
////  double sa_cw = t_cw.SignedArea();
////  ASSERT_TRUE(sa_cw < 0);
////  double a_cw = t_cw.Area();
////  ASSERT_EQ(2.0, g::round(a_cw));
////  ASSERT_EQ(-a_cw, sa_cw);
////
////  ASSERT_EQ(a_ccw, a_cw);
////  ASSERT_EQ(-sa_ccw, sa_cw);
////}
////
////TEST(Triangle3D, ToPolygon) {
////  geompp::DECIMAL_PRECISION =  4;
////  auto t = g::Triangle3D::Make(g::Point3D(-1, 1), g::Point3D(0, -1), g::Point3D(1, 1));
////
////  auto verts = t.Vertices();
////  auto poly = g::Polygon3D::Make({std::get<0>(verts), std::get<1>(verts), std::get<2>(verts)});
////  ASSERT_EQ(poly, t.ToPolygon(prec));
////
////  ASSERT_EQ(poly.ToWkt(prec), t.ToPolygon(prec).ToWkt(prec));
////}
////
////TEST(Triangle3D, ToAxis) {
////  auto t = g::Triangle3D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");
////
////  auto axis = t.ToAxis();
////  ASSERT_EQ(g::Vector3D(1, 1), std::get<0>(axis));
////  ASSERT_EQ(g::Vector3D(-1, 1), std::get<1>(axis));
////}
////
////TEST(Triangle3D, DistanceTo) {
////  geompp::DECIMAL_PRECISION =  4;
////  auto t = g::Triangle3D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");
////
////  // points
////  auto points = t.Vertices();
////  ASSERT_EQ(0, t.DistanceTo(std::get<0>(points)));
////  ASSERT_EQ(0, t.DistanceTo(std::get<1>(points)));
////  ASSERT_EQ(0, t.DistanceTo(std::get<2>(points)));
////  ASSERT_EQ(0, t.DistanceTo(t.Centroid()));
////
////  // on line
////  ASSERT_EQ(0, t.DistanceTo(g::Point3D::average({std::get<0>(points), std::get<1>(points)})));
////  ASSERT_EQ(0, t.DistanceTo(g::Point3D::average({std::get<1>(points), std::get<2>(points)})));
////  ASSERT_EQ(0, t.DistanceTo(g::Point3D::average({std::get<2>(points), std::get<0>(points)})));
////
////  // outside (ahead of segments)
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D::average({std::get<0>(points), std::get<1>(points)}) -
////                                  (std::get<1>(points) - std::get<0>(points)).Perp().Normalize(),
////                              prec));
////
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D::average({std::get<1>(points), std::get<2>(points)}) -
////                                  (std::get<2>(points) - std::get<1>(points)).Perp().Normalize(),
////                              prec));
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D::average({std::get<2>(points), std::get<0>(points)}) -
////                                  (std::get<0>(points) - std::get<2>(points)).Perp().Normalize(),
////                              prec));
////
////  // outside (ahead of vertices)
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D(0, -2)));
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D(2, 0)));
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D(-2, 0)));
////}
////
////TEST(Triangle3D, Interpolate) {
////  geompp::DECIMAL_PRECISION =  4;
////  auto tri = g::Triangle3D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");
////
////  auto points = tri.Vertices();
////
////  // triangle points
////  ASSERT_EQ(std::get<0>(points), tri.Interpolate(0, 0));
////  ASSERT_EQ(std::get<1>(points), tri.Interpolate(1, 0));
////  ASSERT_EQ(std::get<2>(points), tri.Interpolate(0, 1));
////
////  // centroid
////  auto c = tri.Centroid();
////  auto loc = tri.Location(c);
////  ASSERT_EQ(c, tri.Interpolate(std::get<0>(loc), std::get<1>(loc)));
////}
//
// TEST(Triangle3D, IntersectionWLine) {
//  geompp::DECIMAL_PRECISION =  4;
//  auto tri = g::Triangle3D::FromWkt("TRIANGLE (0 -1, 1 1, -1 1)");
//
//  //auto x = g::Line3D::Make(g::Point3D(), g::Vector3D(1, 0));
//  //auto y = g::Line3D::Make(g::Point3D(), g::Vector3D(0, 1));
//
//  //auto px = g::Line3D::Make(g::Point3D(0, -1), g::Vector3D(1, -1));
//  //auto py = g::Line3D::Make(g::Point3D(1, 1), g::Vector3D(1, 2));
//
//  //auto pax = g::Line3D::Make(g::Point3D(0, 2), g::Vector3D(1, 2));
//  //auto pay = g::Line3D::Make(g::Point3D(2, 0), g::Vector3D(2, 1));
//
//  //ASSERT_TRUE(tri.Intersects(x));
//  //{
//  //  auto inter = tri.Intersection(x);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(*inter));
//  //  EXPECT_EQ(g::LineSegment3D::Make({-0.5, 0}, {0.5, 0}), std::get<g::LineSegment3D>(*inter));
//  //}
//
//  //ASSERT_TRUE(tri.Intersects(y));
//  //{
//  //  auto inter = tri.Intersection(y);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(*inter));
//  //  EXPECT_EQ(g::LineSegment3D::Make({0, -1}, {0, 1}), std::get<g::LineSegment3D>(*inter));
//  //}
//
//  //ASSERT_TRUE(tri.Intersects(px));
//  //{
//  //  auto inter = tri.Intersection(px);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
//  //  EXPECT_EQ(g::Point3D(0, -1), std::get<g::Point3D>(*inter));
//  //}
//
//  //ASSERT_TRUE(tri.Intersects(py));
//  //{
//  //  auto inter = tri.Intersection(py);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
//  //  EXPECT_EQ(g::Point3D(1, 1), std::get<g::Point3D>(*inter));
//  //}
//
//  //ASSERT_FALSE(tri.Intersects(pax));
//  //ASSERT_FALSE(tri.Intersects(pay));
//}
//
//// TEST(Triangle3D, IntersectionWRay) {
////   geompp::DECIMAL_PRECISION =  4;
////   auto r1 = g::Ray3D::Make(g::Point3D(-1, 1), g::Vector3D(1, -1));
////   auto r2 = g::Ray3D::Make(g::Point3D(-1, -1), g::Vector3D(1, 1));    // intersects r1 in (0,0)
////   auto r3 = g::Ray3D::Make(g::Point3D(-0.5, 0), g::Vector3D(0, -1));  // intersects r2 in (-0.5,-0.5)
////   auto r4 = g::Ray3D::Make(g::Point3D(1, -0.5), g::Vector3D(0, 1));   // intersects r2 in (1,1)
//
////   ASSERT_TRUE(r1.Intersects(r2));
////   {
////     auto inter = r1.Intersection(r2);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0, 0), std::get<g::Point3D>(*inter));
////   }
//
////   ASSERT_FALSE(r1.Intersects(r3));
////   ASSERT_FALSE(r1.Intersection(r3).has_value());
////   ASSERT_FALSE(r1.Intersects(r4));
////   ASSERT_FALSE(r1.Intersection(r4).has_value());
//
////   ASSERT_TRUE(r2.Intersects(r3));
////   {
////     auto inter = r2.Intersection(r3);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(-0.5, -0.5), std::get<g::Point3D>(*inter));
////   }
//
////   ASSERT_TRUE(r2.Intersects(r4));
////   {
////     auto inter = r2.Intersection(r4);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(1, 1), std::get<g::Point3D>(*inter));
////   }
//// }
//
//// TEST(Triangle3D, IntersectionWRay) {
////   geompp::DECIMAL_PRECISION =  4;
////   auto s1 = g::LineSegment3D::Make(g::Point3D(-1, -2), g::Point3D(2, 1));   // intersects r1, r2
////   auto s2 = g::LineSegment3D::Make(g::Point3D(1, 1), g::Point3D(0, 1));     // intersects r1
////   auto s3 = g::LineSegment3D::Make(g::Point3D(-1, 0), g::Point3D(-1, -1));  // intersects r2
//
////   auto r1 = g::Ray3D::Make(g::Point3D(0.5, -2), g::Vector3D(0, 1));
////   auto r2 = g::Ray3D::Make(g::Point3D(-2, -0.5), g::Vector3D(1, 0));
////   auto r1_rev = g::Ray3D::Make(g::Point3D(0.5, -2), g::Vector3D(0, -1));   // no intersections
////   auto r2_rev = g::Ray3D::Make(g::Point3D(-2, -0.5), g::Vector3D(-1, 0));  // no intersections
//
////   ASSERT_TRUE(s1.Intersects(r1));
////   {
////     auto inter = s1.Intersection(r1);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0.5, -0.5), std::get<g::Point3D>(*inter));
////   }
////   ASSERT_TRUE(s1.Intersects(r2));
////   {
////     auto inter = s1.Intersection(r2);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0.5, -0.5), std::get<g::Point3D>(*inter));
////   }
//
////   ASSERT_TRUE(s2.Intersects(r1));
////   {
////     auto inter = s2.Intersection(r1);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0.5, 1), std::get<g::Point3D>(*inter));
////   }
////   ASSERT_FALSE(s2.Intersects(r2));
//
////   ASSERT_TRUE(s3.Intersects(r2));
////   {
////     auto inter = s3.Intersection(r2);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(-1, -0.5), std::get<g::Point3D>(*inter));
////   }
////   ASSERT_FALSE(s3.Intersects(r1));
//
////   ASSERT_FALSE(s1.Intersects(r1_rev));
////   ASSERT_FALSE(s1.Intersects(r2_rev));
////   ASSERT_FALSE(s2.Intersects(r1_rev));
////   ASSERT_FALSE(s2.Intersects(r2_rev));
////   ASSERT_FALSE(s3.Intersects(r1_rev));
////   ASSERT_FALSE(s3.Intersects(r2_rev));
//// }

TEST_F(Triangle3DTest, Wkt) {
  ASSERT_EQ("TRIANGLE (0 0 1, 1 1 1, 0 2 1)",
            g::Triangle3D::Make(g::Point3D(0, 0, 1), g::Point3D(1, 1, 1), g::Point3D(0, 2, 1)).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("TRIANGLE (56491.62 -795.97 1, -9137.37 10.36 2, 321.13 206.62 3)",
            g::Triangle3D::Make(g::Point3D(56491.6164, -795.97416, 1), g::Point3D(-9137.3679, 10.35678, 2),
                                g::Point3D(321.1302, 206.619749, 3))
                .ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(1, 1, 1), g::Point3D(0, 2, 0)),
            g::Triangle3D::FromWkt("TRIANGLE (0 0 0, 1 1 1, 0 2 0)"));
  EXPECT_EQ(g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(1, 1, 0), g::Point3D(0, 2, 0)),
            g::Triangle3D::FromWkt("  triangle( 0     0 0 , 1   1  0, 0 2 0  )"));
  EXPECT_EQ(g::Triangle3D::Make(g::Point3D(0, 0, 1), g::Point3D(1, 1, 2), g::Point3D(0, 2, 3)),
            g::Triangle3D::FromWkt("triANGle   ( 0 0  1, 1 1  2, 0   2  3    )"));

  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangl ( -7.5 -60.7, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("tri angle ( -7.5 -60.7, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle -7.5 -64.4 1, 0 0, 1 1 3)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle (-7.5 -64.4, 0 0 2, 1 1 2"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle (-7.5 -64.4, 0 0, 1 1 2"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle (-7.5 -64.4 3, 0 0 3, 1 3"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle (-7.5 -64.4 1, 0 0 2"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( -7.5 )"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( )"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( , -7.5 -64.4 15.5, )"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( , ,-7.5 -64.4 15.5)"));
}

TEST_F(Triangle3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "triangle.wkt").string();
  auto s = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(0, 2, 0));

  s.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Triangle3D s_file = g::Triangle3D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(s, s_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Triangle3DTest, TestFromFile) {
  std::string path = (test_res_path / "triangle3d" / "triangle.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Triangle3D::FromFile(path));

  auto p = g::Triangle3D::FromFile(path);

  GEOMPP_LOG(INFO) << "form file = " << p.ToWkt();
}

TEST_F(Triangle3DTest, Vertices) {
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto [p0, p1, p2] = t.Vertices();
  ASSERT_EQ(g::Point3D::Zero(), p0);
  ASSERT_EQ(g::Point3D(2, 0, 0), p1);
  ASSERT_EQ(g::Point3D(0, 2, 0), p2);
}

TEST_F(Triangle3DTest, AlmostEquals) {
  auto t1 = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto t2 = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto t3 = g::Triangle3D::Make(g::Point3D(0, 0, 1), g::Point3D(2, 0, 1), g::Point3D(0, 2, 1));

  ASSERT_TRUE(t1.AlmostEquals(t2));
  ASSERT_FALSE(t1.AlmostEquals(t3));
  ASSERT_EQ(t1, t2);
  ASSERT_NE(t1, t3);

  // epsilon: tiny offset on all three vertices, within a wider tolerance
  double tiny = 1e-6;
  auto t_near = g::Triangle3D::Make(g::Point3D(tiny, 0, 0), g::Point3D(2 + tiny, 0, 0), g::Point3D(0, 2 + tiny, 0));
  ASSERT_TRUE(t1.AlmostEquals(t_near, 1e-4));
  ASSERT_FALSE(t1.AlmostEquals(t_near, 1e-8));  // tighter than the offset → not equal

  // a difference of 1 on the Z axis is always outside any reasonable epsilon
  ASSERT_FALSE(t1.AlmostEquals(t3, 1e-4));
}

TEST_F(Triangle3DTest, Perimeter) {
  geompp::DECIMAL_PRECISION = 4;
  // right-isosceles triangle with legs of length 2: perimeter = 2 + 2 + 2*sqrt(2)
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  ASSERT_EQ(6.8284, g::round(t.Perimeter()));

  // equilateral triangle with side 2: perimeter = 6
  auto t2 = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(1, 0, 0));
  // degenerate — use a proper equilateral
  auto t3 = g::Triangle3D::Make(g::Point3D(-1, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 0, 2));
  ASSERT_TRUE(t3.Perimeter() > 0);
}

TEST_F(Triangle3DTest, ToAxis) {
  geompp::DECIMAL_PRECISION = 4;
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto [u, v] = t.ToAxis();
  ASSERT_EQ(g::Vector3D(2, 0, 0), u);  // P1 - P0
  ASSERT_EQ(g::Vector3D(0, 2, 0), v);  // P2 - P0
}

TEST_F(Triangle3DTest, Interpolate) {
  geompp::DECIMAL_PRECISION = 4;
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));

  // vertices
  ASSERT_EQ(g::Point3D::Zero(), t.Interpolate(0, 0));
  ASSERT_EQ(g::Point3D(2, 0, 0), t.Interpolate(1, 0));
  ASSERT_EQ(g::Point3D(0, 2, 0), t.Interpolate(0, 1));

  // centroid at (s=1/3, t=1/3)
  auto c = t.Interpolate(1.0 / 3.0, 1.0 / 3.0);
  ASSERT_TRUE(c.has_value());
  ASSERT_EQ(g::Point3D(0.6667, 0.6667, 0), *c);

  // outside (s+t > 1) returns nullopt
  ASSERT_FALSE(t.Interpolate(0.8, 0.8).has_value());
  ASSERT_FALSE(t.Interpolate(1, 1).has_value());
}

TEST_F(Triangle3DTest, SignedArea) {
  geompp::DECIMAL_PRECISION = 4;

  // right-isosceles triangle in XY plane, legs = 2  →  area = 2.0
  auto t_ccw = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  double sa_ccw = t_ccw.SignedArea(g::Vector3D::BasisZ());
  ASSERT_TRUE(sa_ccw > 0);  // CCW winding → positive
  ASSERT_EQ(2.0, g::round(t_ccw.Area()));
  ASSERT_EQ(sa_ccw, t_ccw.Area());  // Area() == |SignedArea()| for CCW

  // reversed winding (CW) → same geometry, opposite sign
  auto [p0, p1, p2] = t_ccw.Vertices();
  auto t_cw = g::Triangle3D::Make(p0, p2, p1);
  double sa_cw = t_cw.SignedArea(g::Vector3D::BasisZ());
  ASSERT_TRUE(sa_cw < 0);                 // CW winding → negative
  ASSERT_EQ(2.0, g::round(t_cw.Area()));  // Area() is always positive
  ASSERT_EQ(-sa_ccw, sa_cw);              // symmetric

  // flipping ref_normal reverses the sign
  ASSERT_EQ(-sa_ccw, t_ccw.SignedArea(-g::Vector3D::BasisZ()));

  // equilateral triangle with side 2 in XY plane  →  area = sqrt(3) ≈ 1.7321
  auto t_eq = g::Triangle3D::Make(g::Point3D(-1, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, std::sqrt(3.0), 0));
  ASSERT_EQ(1.7321, g::round(t_eq.Area()));
  ASSERT_GT(t_eq.SignedArea(g::Vector3D::BasisZ()), 0.0);  // CCW as constructed

  // tilted triangle — use its own normal as ref so result is positive and |result| == Area()
  auto t_3d = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(1, 1, 1));
  ASSERT_GT(t_3d.Area(), 0.0);
  ASSERT_TRUE(geompp::compare(t_3d.Area(), std::abs(t_3d.SignedArea(t_3d.Normal()))) == 0);
}

TEST_F(Triangle3DTest, AreaVector) {
  geompp::DECIMAL_PRECISION = 4;

  // right-isosceles in XY plane → cross/2 = (0,0,2), length = area
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto av = t.AreaVector();
  ASSERT_EQ(g::Vector3D(0, 0, 2), av);  // perpendicular to XY plane, pointing +Z
  ASSERT_EQ(t.Area(), av.Length());     // |AreaVector| == area

  // reversed winding (CW) → flipped direction, same length
  auto [p0, p1, p2] = t.Vertices();
  auto t_cw = g::Triangle3D::Make(p0, p2, p1);
  auto av_cw = t_cw.AreaVector();
  ASSERT_EQ(g::Vector3D(0, 0, -2), av_cw);  // pointing −Z
  ASSERT_EQ(t_cw.Area(), av_cw.Length());

  // tilted triangle in XZ plane: (0,0,0),(2,0,0),(0,0,2) → cross/2 = (0,-2,0)
  auto t_xz = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 0, 2));
  ASSERT_EQ(g::Vector3D(0, -2, 0), t_xz.AreaVector());
  ASSERT_EQ(t_xz.Area(), t_xz.AreaVector().Length());
}

TEST_F(Triangle3DTest, IsCCW) {
  auto ref = g::Vector3D::BasisZ();

  // CCW winding in XY plane → SignedArea > 0 → IsCCW == true
  auto t_ccw = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  ASSERT_TRUE(t_ccw.SignedArea(ref) > 0);
  ASSERT_TRUE(t_ccw.IsCCW(ref));

  // CW winding → SignedArea < 0 → IsCCW == false
  auto [p0, p1, p2] = t_ccw.Vertices();
  auto t_cw = g::Triangle3D::Make(p0, p2, p1);
  ASSERT_TRUE(t_cw.SignedArea(ref) < 0);
  ASSERT_FALSE(t_cw.IsCCW(ref));

  // IsCCW is consistent with sign of SignedArea
  ASSERT_EQ(t_ccw.IsCCW(ref), t_ccw.SignedArea(ref) > 0);
  ASSERT_EQ(t_cw.IsCCW(ref), t_cw.SignedArea(ref) > 0);

  // flipping ref_normal flips the result
  ASSERT_FALSE(t_ccw.IsCCW(-ref));
  ASSERT_TRUE(t_cw.IsCCW(-ref));
}

TEST_F(Triangle3DTest, Normal) {
  // XY-plane triangle — normal must point +Z
  auto t_xy = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  ASSERT_TRUE(t_xy.Normal().AlmostEquals(g::Vector3D::BasisZ()));

  // Normal must be a unit vector
  ASSERT_EQ(1.0, g::round(t_xy.Normal().Length()));

  // Normal must equal ToPlane().normal()
  ASSERT_TRUE(t_xy.Normal().AlmostEquals(t_xy.ToPlane().normal()));

  // XZ-plane triangle — normal must point -Y (CCW winding: (0,0,0),(2,0,0),(0,0,2))
  auto t_xz = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 0, 2));
  ASSERT_TRUE(t_xz.Normal().AlmostEquals(t_xz.ToPlane().normal()));
  ASSERT_EQ(1.0, g::round(t_xz.Normal().Length()));

  // tilted triangle — normal still equals ToPlane().normal()
  auto t_3d = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(1, 1, 1));
  ASSERT_TRUE(t_3d.Normal().AlmostEquals(t_3d.ToPlane().normal()));
  ASSERT_EQ(1.0, g::round(t_3d.Normal().Length()));
}

TEST_F(Triangle3DTest, ToPolygon) {
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto poly = t.ToPolygon();

  ASSERT_EQ(3u, poly.Size());
  ASSERT_EQ(g::Point3D::Zero(), poly[0]);
  ASSERT_EQ(g::Point3D(2, 0, 0), poly[1]);
  ASSERT_EQ(g::Point3D(0, 2, 0), poly[2]);

  // matches a polygon built from the same vertices
  auto [p0, p1, p2] = t.Vertices();
  ASSERT_EQ(g::Polygon3D::Make({p0, p1, p2}), poly);

  // different triangle → different polygon
  auto t2 = g::Triangle3D::Make(g::Point3D(0, 0, 1), g::Point3D(2, 0, 1), g::Point3D(0, 2, 1));
  ASSERT_NE(t2.ToPolygon(), poly);
}

TEST_F(Triangle3DTest, DistanceTo) {
  // DistanceTo is not yet implemented — throws
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  EXPECT_ANY_THROW(t.DistanceTo(g::Point3D(0.5, 0.5, 0)));
}

TEST_F(Triangle3DTest, Location) {
  // P0=(0,0,0), P1=(2,0,0), P2=(0,2,0)
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));

  // inside points: Location returns (s,t) AND Contains agrees
  auto check_inside = [&](g::Point3D const& p, double exp_s, double exp_t) {
    auto st = t.Location(p);
    ASSERT_TRUE(st.has_value());
    EXPECT_NEAR(exp_s, std::get<0>(*st), 1e-9);
    EXPECT_NEAR(exp_t, std::get<1>(*st), 1e-9);
    EXPECT_TRUE(t.Contains(p));   // Location non-null ↔ Contains true
  };

  check_inside(g::Point3D::Zero(),    0.0,       0.0);       // P0
  check_inside(g::Point3D(2, 0, 0),  1.0,       0.0);       // P1
  check_inside(g::Point3D(0, 2, 0),  0.0,       1.0);       // P2
  check_inside(t.Centroid(),          1.0 / 3.0, 1.0 / 3.0);

  // outside points: Location is nullopt AND Contains agrees
  auto check_outside = [&](g::Point3D const& p) {
    EXPECT_FALSE(t.Location(p).has_value());
    EXPECT_FALSE(t.Contains(p));  // Location null ↔ Contains false
  };

  check_outside(g::Point3D(0.5, 0.5,  1));   // off-plane above
  check_outside(g::Point3D(0.5, 0.5, -1));   // off-plane below
  check_outside(g::Point3D(-1, 0, 0));        // outside in-plane
  check_outside(g::Point3D(2, 2, 0));         // past hypotenuse

  // round-trip A: Interpolate(Location(p)) == p
  auto p = g::Point3D(0.5, 0.5, 0);
  auto st_p = t.Location(p);
  ASSERT_TRUE(st_p.has_value());
  auto p_back = t.Interpolate(std::get<0>(*st_p), std::get<1>(*st_p));
  ASSERT_TRUE(p_back.has_value());
  EXPECT_TRUE(p.AlmostEquals(*p_back));

  // round-trip B: Location(Interpolate(s,t)) == (s,t)
  double s_in = 0.25, t_in = 0.25;
  auto q = t.Interpolate(s_in, t_in);
  ASSERT_TRUE(q.has_value());
  EXPECT_TRUE(t.Contains(*q));          // Interpolate result is always inside
  auto st_q = t.Location(*q);
  ASSERT_TRUE(st_q.has_value());
  EXPECT_NEAR(s_in, std::get<0>(*st_q), 1e-9);
  EXPECT_NEAR(t_in, std::get<1>(*st_q), 1e-9);
}

TEST_F(Triangle3DTest, Contains) {
  // XY-plane triangle: P0=(0,0,0), P1=(2,0,0), P2=(0,2,0)
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto c = t.Centroid();

  // corner points are on the boundary
  ASSERT_TRUE(t.Contains(g::Point3D::Zero()));
  ASSERT_TRUE(t.Contains(g::Point3D(2, 0, 0)));
  ASSERT_TRUE(t.Contains(g::Point3D(0, 2, 0)));

  // centroid is inside
  ASSERT_TRUE(t.Contains(c));

  // midpoints of edges (on boundary)
  ASSERT_TRUE(t.Contains(g::Point3D(1, 0, 0)));    // midpoint P0-P1
  ASSERT_TRUE(t.Contains(g::Point3D(0, 1, 0)));    // midpoint P0-P2
  ASSERT_TRUE(t.Contains(g::Point3D(1, 1, 0)));    // midpoint P1-P2

  // points outside in the plane
  ASSERT_FALSE(t.Contains(g::Point3D(-0.5, 0, 0)));
  ASSERT_FALSE(t.Contains(g::Point3D(0, -0.5, 0)));
  ASSERT_FALSE(t.Contains(g::Point3D(1.5, 1.5, 0)));  // beyond hypotenuse

  // point off the plane → outside regardless of XY position
  ASSERT_FALSE(t.Contains(g::Point3D(0.5, 0.5, 1)));
  ASSERT_FALSE(t.Contains(g::Point3D(0.5, 0.5, -1)));
}

TEST_F(Triangle3DTest, IntersectionWLine) {
  // Intersection(Line3D) is not yet implemented — throws
  auto t = g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(2, 0, 0), g::Point3D(0, 2, 0));
  auto line = g::Line3D::Make(g::Point3D(0.5, 0.5, -1), g::Point3D(0.5, 0.5, 1));
  EXPECT_ANY_THROW(t.Intersection(line));
  EXPECT_ANY_THROW(t.Intersects(line));
}

}  // namespace geompp_tests
