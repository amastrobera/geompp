#include "triangle3d.hpp"

#include "line3d.hpp"
//#include "line_segment3d.hpp"
#include "point3d.hpp"
//#include "polygon3d.hpp"
//#include "ray3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>
#include <tuple>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(Triangle3D, Constructor) {
  auto t = g::Triangle3D::Make(g::Point3D(-1, 1, 2), g::Point3D(0, -1, 2), g::Point3D(1, 1, 2));

  ASSERT_EQ(g::Point3D(0, 0.333, 2), t.Centroid());

  EXPECT_ANY_THROW(g::Triangle3D::Make(g::Point3D(-1, 1, 0), g::Point3D(0, -1, 0), g::Point3D(0, -1, 0)));
  EXPECT_ANY_THROW(g::Triangle3D::Make(g::Point3D(-1, 1, 2), g::Point3D(-1, 1, 2), g::Point3D(0, -1, 2)));
  EXPECT_ANY_THROW(g::Triangle3D::Make(g::Point3D(), g::Point3D(), g::Point3D()));
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
////  int prec = 4;
////  auto t_ccw = g::Triangle3D::Make(g::Point3D(-1, 1), g::Point3D(0, -1), g::Point3D(1, 1), prec);
////
////  ASSERT_EQ(6.4721, g::round_to(t_ccw.Perimeter(), prec));
////  double sa_ccw = t_ccw.SignedArea();
////  ASSERT_TRUE(sa_ccw > 0);
////  double a_ccw = t_ccw.Area();
////  ASSERT_EQ(2.0, g::round_to(a_ccw, prec));
////  ASSERT_EQ(a_ccw, sa_ccw);
////
////  auto verts_ccw = t_ccw.Vertices();
////  auto t_cw = g::Triangle3D::Make(std::get<0>(verts_ccw), std::get<2>(verts_ccw), std::get<1>(verts_ccw), prec);
////
////  ASSERT_EQ(6.4721, g::round_to(t_cw.Perimeter(), prec));
////  double sa_cw = t_cw.SignedArea();
////  ASSERT_TRUE(sa_cw < 0);
////  double a_cw = t_cw.Area();
////  ASSERT_EQ(2.0, g::round_to(a_cw, prec));
////  ASSERT_EQ(-a_cw, sa_cw);
////
////  ASSERT_EQ(a_ccw, a_cw);
////  ASSERT_EQ(-sa_ccw, sa_cw);
////}
////
////TEST(Triangle3D, ToPolygon) {
////  int prec = 4;
////  auto t = g::Triangle3D::Make(g::Point3D(-1, 1), g::Point3D(0, -1), g::Point3D(1, 1), prec);
////
////  auto verts = t.Vertices();
////  auto poly = g::Polygon3D::Make({std::get<0>(verts), std::get<1>(verts), std::get<2>(verts)}, prec);
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
////  int prec = 4;
////  auto t = g::Triangle3D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");
////
////  // points
////  auto points = t.Vertices();
////  ASSERT_EQ(0, t.DistanceTo(std::get<0>(points), prec));
////  ASSERT_EQ(0, t.DistanceTo(std::get<1>(points), prec));
////  ASSERT_EQ(0, t.DistanceTo(std::get<2>(points), prec));
////  ASSERT_EQ(0, t.DistanceTo(t.Centroid(), prec));
////
////  // on line
////  ASSERT_EQ(0, t.DistanceTo(g::Point3D::average({std::get<0>(points), std::get<1>(points)}), prec));
////  ASSERT_EQ(0, t.DistanceTo(g::Point3D::average({std::get<1>(points), std::get<2>(points)}), prec));
////  ASSERT_EQ(0, t.DistanceTo(g::Point3D::average({std::get<2>(points), std::get<0>(points)}), prec));
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
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D(0, -2), prec));
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D(2, 0), prec));
////  ASSERT_EQ(1.0, t.DistanceTo(g::Point3D(-2, 0), prec));
////}
////
////TEST(Triangle3D, Interpolate) {
////  int prec = 4;
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
////  auto loc = tri.Location(c, prec);
////  ASSERT_EQ(c, tri.Interpolate(std::get<0>(loc), std::get<1>(loc), prec));
////}
//
// TEST(Triangle3D, IntersectionWLine) {
//  int prec = 4;
//  auto tri = g::Triangle3D::FromWkt("TRIANGLE (0 -1, 1 1, -1 1)");
//
//  //auto x = g::Line3D::Make(g::Point3D(), g::Vector3D(1, 0), prec);
//  //auto y = g::Line3D::Make(g::Point3D(), g::Vector3D(0, 1), prec);
//
//  //auto px = g::Line3D::Make(g::Point3D(0, -1), g::Vector3D(1, -1), prec);
//  //auto py = g::Line3D::Make(g::Point3D(1, 1), g::Vector3D(1, 2), prec);
//
//  //auto pax = g::Line3D::Make(g::Point3D(0, 2), g::Vector3D(1, 2), prec);
//  //auto pay = g::Line3D::Make(g::Point3D(2, 0), g::Vector3D(2, 1), prec);
//
//  //ASSERT_TRUE(tri.Intersects(x, prec));
//  //{
//  //  auto inter = tri.Intersection(x, prec);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(*inter));
//  //  EXPECT_EQ(g::LineSegment3D::Make({-0.5, 0}, {0.5, 0}), std::get<g::LineSegment3D>(*inter));
//  //}
//
//  //ASSERT_TRUE(tri.Intersects(y, prec));
//  //{
//  //  auto inter = tri.Intersection(y, prec);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(*inter));
//  //  EXPECT_EQ(g::LineSegment3D::Make({0, -1}, {0, 1}), std::get<g::LineSegment3D>(*inter));
//  //}
//
//  //ASSERT_TRUE(tri.Intersects(px, prec));
//  //{
//  //  auto inter = tri.Intersection(px, prec);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
//  //  EXPECT_EQ(g::Point3D(0, -1), std::get<g::Point3D>(*inter));
//  //}
//
//  //ASSERT_TRUE(tri.Intersects(py, prec));
//  //{
//  //  auto inter = tri.Intersection(py, prec);
//  //  ASSERT_TRUE(inter.has_value());
//  //  ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
//  //  EXPECT_EQ(g::Point3D(1, 1), std::get<g::Point3D>(*inter));
//  //}
//
//  //ASSERT_FALSE(tri.Intersects(pax, prec));
//  //ASSERT_FALSE(tri.Intersects(pay, prec));
//}
//
//// TEST(Triangle3D, IntersectionWRay) {
////   int prec = 4;
////   auto r1 = g::Ray3D::Make(g::Point3D(-1, 1), g::Vector3D(1, -1), prec);
////   auto r2 = g::Ray3D::Make(g::Point3D(-1, -1), g::Vector3D(1, 1), prec);    // intersects r1 in (0,0)
////   auto r3 = g::Ray3D::Make(g::Point3D(-0.5, 0), g::Vector3D(0, -1), prec);  // intersects r2 in (-0.5,-0.5)
////   auto r4 = g::Ray3D::Make(g::Point3D(1, -0.5), g::Vector3D(0, 1), prec);   // intersects r2 in (1,1)
//
////   ASSERT_TRUE(r1.Intersects(r2, prec));
////   {
////     auto inter = r1.Intersection(r2, prec);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0, 0), std::get<g::Point3D>(*inter));
////   }
//
////   ASSERT_FALSE(r1.Intersects(r3, prec));
////   ASSERT_FALSE(r1.Intersection(r3, prec).has_value());
////   ASSERT_FALSE(r1.Intersects(r4, prec));
////   ASSERT_FALSE(r1.Intersection(r4, prec).has_value());
//
////   ASSERT_TRUE(r2.Intersects(r3, prec));
////   {
////     auto inter = r2.Intersection(r3, prec);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(-0.5, -0.5), std::get<g::Point3D>(*inter));
////   }
//
////   ASSERT_TRUE(r2.Intersects(r4, prec));
////   {
////     auto inter = r2.Intersection(r4, prec);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(1, 1), std::get<g::Point3D>(*inter));
////   }
//// }
//
//// TEST(Triangle3D, IntersectionWRay) {
////   int prec = 4;
////   auto s1 = g::LineSegment3D::Make(g::Point3D(-1, -2), g::Point3D(2, 1), prec);   // intersects r1, r2
////   auto s2 = g::LineSegment3D::Make(g::Point3D(1, 1), g::Point3D(0, 1), prec);     // intersects r1
////   auto s3 = g::LineSegment3D::Make(g::Point3D(-1, 0), g::Point3D(-1, -1), prec);  // intersects r2
//
////   auto r1 = g::Ray3D::Make(g::Point3D(0.5, -2), g::Vector3D(0, 1), prec);
////   auto r2 = g::Ray3D::Make(g::Point3D(-2, -0.5), g::Vector3D(1, 0), prec);
////   auto r1_rev = g::Ray3D::Make(g::Point3D(0.5, -2), g::Vector3D(0, -1), prec);   // no intersections
////   auto r2_rev = g::Ray3D::Make(g::Point3D(-2, -0.5), g::Vector3D(-1, 0), prec);  // no intersections
//
////   ASSERT_TRUE(s1.Intersects(r1, prec));
////   {
////     auto inter = s1.Intersection(r1, prec);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0.5, -0.5), std::get<g::Point3D>(*inter));
////   }
////   ASSERT_TRUE(s1.Intersects(r2, prec));
////   {
////     auto inter = s1.Intersection(r2, prec);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0.5, -0.5), std::get<g::Point3D>(*inter));
////   }
//
////   ASSERT_TRUE(s2.Intersects(r1, prec));
////   {
////     auto inter = s2.Intersection(r1, prec);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(0.5, 1), std::get<g::Point3D>(*inter));
////   }
////   ASSERT_FALSE(s2.Intersects(r2, prec));
//
////   ASSERT_TRUE(s3.Intersects(r2, prec));
////   {
////     auto inter = s3.Intersection(r2, prec);
////     ASSERT_TRUE(inter.has_value());
////     ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
////     EXPECT_EQ(g::Point3D(-1, -0.5), std::get<g::Point3D>(*inter));
////   }
////   ASSERT_FALSE(s3.Intersects(r1, prec));
//
////   ASSERT_FALSE(s1.Intersects(r1_rev, prec));
////   ASSERT_FALSE(s1.Intersects(r2_rev, prec));
////   ASSERT_FALSE(s2.Intersects(r1_rev, prec));
////   ASSERT_FALSE(s2.Intersects(r2_rev, prec));
////   ASSERT_FALSE(s3.Intersects(r1_rev, prec));
////   ASSERT_FALSE(s3.Intersects(r2_rev, prec));
//// }

TEST(Triangle3D, Wkt) {
  ASSERT_EQ("TRIANGLE (0 0, 1 1, 0 2)",
            g::Triangle3D::Make(g::Point3D(0, 0), g::Point3D(1, 1), g::Point3D(0, 2)).ToWkt());
  ASSERT_EQ("TRIANGLE (56491.62 -795.97, -9137.37 10.36, 321.13 206.62)",
            g::Triangle3D::Make(g::Point3D(56491.6164, -795.97416), g::Point3D(-9137.3679, 10.35678),
                                g::Point3D(321.1302, 206.619749))
                .ToWkt(2));

  EXPECT_EQ(g::Triangle3D::Make(g::Point3D(0, 0), g::Point3D(1, 1), g::Point3D(0, 2)),
            g::Triangle3D::FromWkt("TRIANGLE (0 0, 1 1, 0 2)"));
  EXPECT_EQ(g::Triangle3D::Make(g::Point3D(0, 0), g::Point3D(1, 1), g::Point3D(0, 2)),
            g::Triangle3D::FromWkt("  triangle( 0     0 , 1   1  , 0 2   )"));
  EXPECT_EQ(g::Triangle3D::Make(g::Point3D(0, 0), g::Point3D(1, 1), g::Point3D(0, 2)),
            g::Triangle3D::FromWkt("triANGle   ( 0 0  , 1 1 , 0   2    )"));

  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangl ( -7.5 -60.7, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("tri angle ( -7.5 -60.7, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle -7.5 -64.4, 0 0, 1 1)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle (-7.5 -64.4, 0 0, 1 1"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle (-7.5 -64.4, 0 0, 1 "));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle (-7.5 -64.4, 0 0"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( -7.5 )"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( )"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::Triangle3D::FromWkt("triangle ( -7.5 -64.4 15.5, 0 0 0, 1 1 1)"));
}

TEST(Triangle3D, ToFile) {
  int prec = 4;
  std::string path = (test_res_path / "temp" / "triangle.wkt").string();
  auto s = g::Triangle3D::Make(g::Point3D(), g::Point3D(1, 0), g::Point3D(0, 2));

  s.ToFile(path, prec);
  ASSERT_TRUE(fs::exists(path));

  g::Triangle3D s_file = g::Triangle3D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(s, s_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST(Triangle3D, TestFromFile) {
  std::string path = (test_res_path / "triangle3d" / "triangle.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Triangle3D::FromFile(path));

  auto p = g::Triangle3D::FromFile(path);

  std::cout << "form file = " << p.ToWkt() << std::endl;
}

}  // namespace geompp_tests