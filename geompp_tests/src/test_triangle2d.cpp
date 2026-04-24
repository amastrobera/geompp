#include "triangle2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

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

class Triangle2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Triangle2DTest, Constructor) {
  auto t = g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(1, 1));

  ASSERT_EQ(g::Point2D(0, 0.333), t.Centroid());

  EXPECT_ANY_THROW(g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(0, -1)));
  EXPECT_ANY_THROW(g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(-1, 1), g::Point2D(0, -1)));
  EXPECT_ANY_THROW(g::Triangle2D::Make(g::Point2D::Zero(), g::Point2D::Zero(), g::Point2D::Zero()));
}

TEST_F(Triangle2DTest, Contains) {
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

TEST_F(Triangle2DTest, Areas) {
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

TEST_F(Triangle2DTest, ToPolygon) {
  geompp::DECIMAL_PRECISION = 4;
  auto t = g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(1, 1));

  auto verts = t.Vertices();
  auto poly = g::Polygon2D::Make({std::get<0>(verts), std::get<1>(verts), std::get<2>(verts)});
  ASSERT_EQ(poly, t.ToPolygon());

  ASSERT_EQ(poly.ToWkt(), t.ToPolygon().ToWkt());
}

TEST_F(Triangle2DTest, IsCCW) {
  // CCW winding → SignedArea > 0 → IsCCW == true
  auto t_ccw = g::Triangle2D::Make(g::Point2D(-1, 1), g::Point2D(0, -1), g::Point2D(1, 1));
  ASSERT_TRUE(t_ccw.SignedArea() > 0);
  ASSERT_TRUE(t_ccw.IsCCW());

  // CW winding → SignedArea < 0 → IsCCW == false
  auto [p0, p1, p2] = t_ccw.Vertices();
  auto t_cw = g::Triangle2D::Make(p0, p2, p1);
  ASSERT_TRUE(t_cw.SignedArea() < 0);
  ASSERT_FALSE(t_cw.IsCCW());

  // IsCCW is consistent with sign of SignedArea
  ASSERT_EQ(t_ccw.IsCCW(), t_ccw.SignedArea() > 0);
  ASSERT_EQ(t_cw.IsCCW(),  t_cw.SignedArea()  > 0);
}

TEST_F(Triangle2DTest, ToAxis) {
  auto t = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");

  auto axis = t.ToAxis();
  ASSERT_EQ(g::Vector2D(1, 1), std::get<0>(axis));
  ASSERT_EQ(g::Vector2D(-1, 1), std::get<1>(axis));
}

TEST_F(Triangle2DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto t = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 0, -1 0)");

  // points
  auto points = t.Vertices();
  ASSERT_EQ(0, g::round(t.DistanceTo(std::get<0>(points))));
  ASSERT_EQ(0, g::round(t.DistanceTo(std::get<1>(points))));
  ASSERT_EQ(0, g::round(t.DistanceTo(std::get<2>(points))));
  ASSERT_EQ(0, g::round(t.DistanceTo(t.Centroid())));

  // on line
  ASSERT_EQ(0, g::round(t.DistanceTo(g::average({std::get<0>(points), std::get<1>(points)}))));
  ASSERT_EQ(0, g::round(t.DistanceTo(g::average({std::get<1>(points), std::get<2>(points)}))));
  ASSERT_EQ(0, g::round(t.DistanceTo(g::average({std::get<2>(points), std::get<0>(points)}))));

  // outside (ahead of segments)
  ASSERT_EQ(1.0, g::round(t.DistanceTo(g::average({std::get<0>(points), std::get<1>(points)}) -
                                       (std::get<1>(points) - std::get<0>(points)).Perp().Normalize())));

  ASSERT_EQ(1.0, g::round(t.DistanceTo(g::average({std::get<1>(points), std::get<2>(points)}) -
                                       (std::get<2>(points) - std::get<1>(points)).Perp().Normalize())));
  ASSERT_EQ(1.0, g::round(t.DistanceTo(g::average({std::get<2>(points), std::get<0>(points)}) -
                                       (std::get<0>(points) - std::get<2>(points)).Perp().Normalize())));

  // outside (ahead of vertices)
  ASSERT_EQ(1.0, g::round(t.DistanceTo(g::Point2D(0, -2))));
  ASSERT_EQ(1.0, g::round(t.DistanceTo(g::Point2D(2, 0))));
  ASSERT_EQ(1.0, g::round(t.DistanceTo(g::Point2D(-2, 0))));
}

TEST_F(Triangle2DTest, Interpolate) {
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

TEST_F(Triangle2DTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto tri = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 1, -1 1)");

  // no intersection: only one point touch
  ASSERT_FALSE(tri.Intersects(g::Line2D::Make(g::Point2D(-1, 0), g::Vector2D::BasisY())));
  ASSERT_FALSE(tri.Intersects(g::Line2D::Make(g::Point2D(1, 0), g::Vector2D::BasisY())));
  ASSERT_FALSE(tri.Intersects(g::Line2D::Make(g::Point2D(0, -1), g::Vector2D::BasisX())));

  // no intersection: edge overlap
  ASSERT_FALSE(tri.Intersects(g::Line2D::Make(g::Point2D(-2, 1), g::Vector2D::BasisX())));
  ASSERT_FALSE(tri.Intersects(g::Line2D::FromWkt("LINE (0 -1, 1 1)")));
  ASSERT_FALSE(tri.Intersects(g::Line2D::FromWkt("LINE (-2 2, 1 1)")));

  // no intersection: outside of the triangle
  ASSERT_FALSE(tri.Intersects(g::Line2D::Make(g::Point2D(-2, 0), g::Vector2D(1, 2))));
  ASSERT_FALSE(tri.Intersects(g::Line2D::Make(g::Point2D(-1, -1), g::Vector2D(1, -1))));
  ASSERT_FALSE(tri.Intersects(g::Line2D::Make(g::Point2D(2, 0), g::Vector2D::BasisY())));

  // actual intersect
  auto line_int = g::Line2D::Make(g::Point2D(-1, -1), g::Vector2D(1, 2));
  ASSERT_TRUE(tri.Intersects(line_int));
  {
    auto inter = tri.Intersection(line_int);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(*inter));
    EXPECT_EQ(g::LineSegment2D::Make({-0.5, 0}, {0, 1}), std::get<g::LineSegment2D>(*inter));
  }

  auto line_int_rev = g::Line2D::Make(line_int.First(), -line_int.Direction());
  ASSERT_TRUE(tri.Intersects(line_int));
  {
    auto inter = tri.Intersection(line_int_rev);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(*inter));
    EXPECT_EQ(g::LineSegment2D::Make({0, 1}, {-0.5, 0}), std::get<g::LineSegment2D>(*inter));
  }

  // actual intersect (reverse order)
}

// Triangle TRIANGLE(0 -1, 1 1, -1 1):
//   P0=(0,-1)  P1=(1,1)  P2=(-1,1)
//   At y=0: left edge P2P0 crosses at (-0.5,0), right edge P0P1 at (0.5,0)
//   Top edge P1P2 is at y=1

TEST_F(Triangle2DTest, IntersectionWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto tri = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 1, -1 1)");

  // ray crosses both sides → segment result
  auto ray_thru = g::Ray2D::Make(g::Point2D(-3, 0), g::Vector2D::BasisX());
  ASSERT_TRUE(tri.Intersects(ray_thru));
  {
    auto inter = tri.Intersection(ray_thru);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(*inter));
    EXPECT_EQ(g::LineSegment2D::Make({-0.5, 0}, {0.5, 0}), std::get<g::LineSegment2D>(*inter));
  }

  // ray pointing away from the triangle: no intersection
  auto ray_away = g::Ray2D::Make(g::Point2D(-3, 0), -g::Vector2D::BasisX());
  ASSERT_FALSE(tri.Intersects(ray_away));
  ASSERT_FALSE(tri.Intersection(ray_away).has_value());

  // ray origin inside, pointing right → exits at right edge only (point result)
  auto ray_from_inside_right = g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D::BasisX());
  ASSERT_TRUE(tri.Intersects(ray_from_inside_right));
  {
    auto inter = tri.Intersection(ray_from_inside_right);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0.5, 0), std::get<g::Point2D>(*inter));
  }

  // ray origin inside, pointing left → exits at left edge only (point result)
  auto ray_from_inside_left = g::Ray2D::Make(g::Point2D(0, 0), -g::Vector2D::BasisX());
  ASSERT_TRUE(tri.Intersects(ray_from_inside_left));
  {
    auto inter = tri.Intersection(ray_from_inside_left);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-0.5, 0), std::get<g::Point2D>(*inter));
  }

  // ray origin inside, pointing up → exits at top edge (point result)
  auto ray_from_inside_up = g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D::BasisY());
  ASSERT_TRUE(tri.Intersects(ray_from_inside_up));
  {
    auto inter = tri.Intersection(ray_from_inside_up);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 1), std::get<g::Point2D>(*inter));
  }

  // ray above triangle pointing up: entirely behind
  auto ray_above = g::Ray2D::Make(g::Point2D(0, 3), g::Vector2D::BasisY());
  ASSERT_FALSE(tri.Intersects(ray_above));
  ASSERT_FALSE(tri.Intersection(ray_above).has_value());
}

TEST_F(Triangle2DTest, IntersectionWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto tri = g::Triangle2D::FromWkt("TRIANGLE (0 -1, 1 1, -1 1)");

  // segment crosses both sides → segment result
  auto seg_thru = g::LineSegment2D::Make(g::Point2D(-3, 0), g::Point2D(3, 0));
  ASSERT_TRUE(tri.Intersects(seg_thru));
  {
    auto inter = tri.Intersection(seg_thru);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(*inter));
    EXPECT_EQ(g::LineSegment2D::Make({-0.5, 0}, {0.5, 0}), std::get<g::LineSegment2D>(*inter));
  }

  // segment enters from the left but stops inside → point at left edge
  auto seg_enter_left = g::LineSegment2D::Make(g::Point2D(-3, 0), g::Point2D(0, 0));
  ASSERT_TRUE(tri.Intersects(seg_enter_left));
  {
    auto inter = tri.Intersection(seg_enter_left);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-0.5, 0), std::get<g::Point2D>(*inter));
  }

  // segment starts inside and exits right → point at right edge
  auto seg_exit_right = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(3, 0));
  ASSERT_TRUE(tri.Intersects(seg_exit_right));
  {
    auto inter = tri.Intersection(seg_exit_right);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0.5, 0), std::get<g::Point2D>(*inter));
  }

  // segment entirely outside: no intersection
  auto seg_outside = g::LineSegment2D::Make(g::Point2D(-3, 0), g::Point2D(-2, 0));
  ASSERT_FALSE(tri.Intersects(seg_outside));
  ASSERT_FALSE(tri.Intersection(seg_outside).has_value());

  // segment entirely inside: no boundary crossing
  auto seg_inside = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(0, 0.5));
  ASSERT_FALSE(tri.Intersects(seg_inside));
  ASSERT_FALSE(tri.Intersection(seg_inside).has_value());
}

TEST_F(Triangle2DTest, Wkt) {
  ASSERT_EQ("TRIANGLE (0 0, 1 1, 0 2)",
            g::Triangle2D::Make(g::Point2D::Zero(), g::Point2D(1, 1), g::Point2D(0, 2)).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("TRIANGLE (56491.62 -795.97, -9137.37 10.36, 321.13 206.62)",
            g::Triangle2D::Make(g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678),
                                g::Point2D(321.1302, 206.619749))
                .ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Triangle2D::Make(g::Point2D::Zero(), g::Point2D(1, 1), g::Point2D(0, 2)),
            g::Triangle2D::FromWkt("TRIANGLE (0 0, 1 1, 0 2)"));
  EXPECT_EQ(g::Triangle2D::Make(g::Point2D::Zero(), g::Point2D(1, 1), g::Point2D(0, 2)),
            g::Triangle2D::FromWkt("  triangle( 0     0 , 1   1  , 0 2   )"));
  EXPECT_EQ(g::Triangle2D::Make(g::Point2D::Zero(), g::Point2D(1, 1), g::Point2D(0, 2)),
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

TEST_F(Triangle2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "triangle.wkt").string();
  auto s = g::Triangle2D::Make(g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(0, 2));

  s.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Triangle2D s_file = g::Triangle2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(s, s_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Triangle2DTest, TestFromFile) {
  std::string path = (test_res_path / "triangle2d" / "triangle.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Triangle2D::FromFile(path));

  auto p = g::Triangle2D::FromFile(path);

  GEOMPP_LOG(INFO) << "form file = " << p.ToWkt();
}

}  // namespace geompp_tests
