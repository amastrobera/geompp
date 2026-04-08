#include "polyline2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>
#include <vector>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Polyline2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Polyline2DTest, Constructor) {
  geompp::DECIMAL_PRECISION = 4;
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

TEST_F(Polyline2DTest, Contains) {
  geompp::DECIMAL_PRECISION = 4;
  std::vector<g::Point2D> points{g::Point2D(-2, -5), g::Point2D(-2, -3), g::Point2D(2, -3), g::Point2D(2, 2)};
  auto polyline = g::Polyline2D::Make(points);

  // knots
  for (auto const& p : points) {
    ASSERT_TRUE(polyline.Contains(p));
  }

  // mid points
  for (int i = 0; i < points.size() - 1; ++i) {
    auto avg = ((points[i].ToVector() + points[i + 1].ToVector()) / 2.0).ToPoint();
    ASSERT_TRUE(polyline.Contains(avg));
  }

  // before the polyline
  ASSERT_FALSE(polyline.Contains(points[0] - (points[1] - points[0])));
  // after the polyline
  ASSERT_FALSE(polyline.Contains(points[points.size() - 1] + (points[points.size() - 1] - points[points.size() - 2])));
}

TEST_F(Polyline2DTest, Location) {
  auto s1 = g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 0)});

  ASSERT_EQ(0.2, g::round(s1.Location(g::Point2D(0.2, 0))));
  ASSERT_EQ(0.5, g::round(s1.Location(g::Point2D(0.5, 0))));
  ASSERT_EQ(0.75, g::round(s1.Location(g::Point2D(0.75, 0))));

  ASSERT_EQ(-1, g::round(s1.Location(g::Point2D(-1, 0))));
  ASSERT_EQ(-0.1, g::round(s1.Location(g::Point2D(-0.1, 0))));
  ASSERT_EQ(1.1, g::round(s1.Location(g::Point2D(1.1, 0))));

  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, 1))));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(0, -1))));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(-1, -1))));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, -1))));
}

TEST_F(Polyline2DTest, Interpolate) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly = g::Polyline2D::FromWkt("LINESTRING (0 0, 3 0)");

  // on segment
  ASSERT_EQ(g::Point2D(0, 0), poly.Interpolate(0));

  ASSERT_EQ(g::Point2D(3, 0), poly.Interpolate(1));

  // on segment
  ASSERT_EQ(0.223, g::round(poly.Location(poly.Interpolate(0.223)), 3));

  // on line
  ASSERT_EQ(1.0, g::round(poly.Location(poly.Interpolate(1.2)), 1));

  ASSERT_EQ(0.0, g::round(poly.Location(poly.Interpolate(-0.2)), 1));
}

TEST_F(Polyline2DTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -2, 1 -2, 1 2)");  // intersects x (-1 0, 1 0) and y (0 -2)
  auto poly2 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -0.5 2, 1 2, 2 1)");  // intersects y (0 2)
  auto poly3 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -1, -2 -2, -1 -3)");  // intersects x (-1 0)

  auto x = g::Line2D::Make(g::Point2D(), g::Vector2D(1, 0));
  auto y = g::Line2D::Make(g::Point2D(), g::Vector2D(0, 1));

  ASSERT_TRUE(poly1.Intersects(x));
  {
    auto inter = poly1.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline2D::MultiPoint>(*inter));
    auto multi_point = std::get<g::Polyline2D::MultiPoint>(*inter);
    ASSERT_EQ(g::Point2D(-1, 0), multi_point[0]);
    ASSERT_EQ(g::Point2D(1, 0), multi_point[1]);
  }

  ASSERT_TRUE(poly1.Intersects(y));
  {
    auto inter = poly1.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, -2), std::get<g::Point2D>(*inter));
  }

  ASSERT_TRUE(poly2.Intersects(y));
  {
    auto inter = poly2.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 2), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly2.Intersects(x));

  ASSERT_TRUE(poly3.Intersects(x));
  {
    auto inter = poly3.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-1, 0), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly3.Intersects(y));
}

TEST_F(Polyline2DTest, IntersectionWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline2D::FromWkt(
      "LINESTRING (-1 2, -1 -2, 1 -2, 1 2)");  // intersects x_neg (-1 0), x_pos (1 0) and y_neg (0 -2)
  auto poly2 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -0.5 2, 1 2, 2 1)");     // intersects y_pos (0 2)
  auto poly3 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -1, -2 -2, -1 -3)");  // intersects x_neg (-1 0)

  auto x_pos = g::Ray2D::Make(g::Point2D::Origin(), g::Vector2D::BasisX());
  auto x_neg = g::Ray2D::Make(g::Point2D::Origin(), -g::Vector2D::BasisX());
  auto y_pos = g::Ray2D::Make(g::Point2D::Origin(), g::Vector2D::BasisY());
  auto y_neg = g::Ray2D::Make(g::Point2D::Origin(), -g::Vector2D::BasisY());

  ASSERT_TRUE(poly1.Intersects(x_neg));
  {
    auto inter = poly1.Intersection(x_neg);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    auto p = std::get<g::Point2D>(*inter);
    ASSERT_EQ(g::Point2D(-1, 0), p);
  }
  ASSERT_TRUE(poly1.Intersects(x_pos));
  {
    auto inter = poly1.Intersection(x_pos);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    auto p = std::get<g::Point2D>(*inter);
    ASSERT_EQ(g::Point2D(1, 0), p);
  }
  ASSERT_TRUE(poly1.Intersects(y_neg));
  {
    auto inter = poly1.Intersection(y_neg);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    auto p = std::get<g::Point2D>(*inter);
    ASSERT_EQ(g::Point2D(0, -2), p);
  }
  ASSERT_FALSE(poly1.Intersects(y_pos));

  ASSERT_TRUE(poly2.Intersects(y_pos));
  {
    auto inter = poly2.Intersection(y_pos);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 2), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly2.Intersects(y_neg));
  ASSERT_FALSE(poly2.Intersects(x_pos));
  ASSERT_FALSE(poly2.Intersects(x_neg));

  ASSERT_TRUE(poly3.Intersects(x_neg));
  {
    auto inter = poly3.Intersection(x_neg);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-1, 0), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly3.Intersects(x_pos));
  ASSERT_FALSE(poly3.Intersects(y_pos));
  ASSERT_FALSE(poly3.Intersects(y_neg));
}

TEST_F(Polyline2DTest, IntersectionWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -2, 1 -2, 1 2)");     // intersects s1 (-1 0, 1 0)
  auto poly2 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -0.5 2, 1 2, 2 1)");     // intersects s2 (0 2)
  auto poly3 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -1, -2 -2, -1 -3)");  // intersects s1 (-1 0), s3 ()

  auto s1 = g::LineSegment2D::FromWkt("LINESTRING (-2 0, 2 0)");
  auto s2 = g::LineSegment2D::FromWkt("LINESTRING (0 -1, 0 3)");
  auto s3 = g::LineSegment2D::FromWkt("LINESTRING (-1.5 -1, -1.5 -3)");

  ASSERT_TRUE(poly1.Intersects(s1));
  {
    auto inter = poly1.Intersection(s1);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline2D::MultiPoint>(*inter));
    auto mpoint = std::get<g::Polyline2D::MultiPoint>(*inter);
    ASSERT_EQ(g::Point2D(-1, 0), mpoint[0]);
    ASSERT_EQ(g::Point2D(1, 0), mpoint[1]);
  }
  ASSERT_FALSE(poly1.Intersects(s2));
  ASSERT_FALSE(poly1.Intersects(s2));

  ASSERT_TRUE(poly2.Intersects(s2));
  {
    auto inter = poly2.Intersection(s2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(0, 2), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly2.Intersects(s1));
  ASSERT_FALSE(poly2.Intersects(s3));

  ASSERT_TRUE(poly3.Intersects(s1));
  {
    auto inter = poly3.Intersection(s1);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));
    EXPECT_EQ(g::Point2D(-1, 0), std::get<g::Point2D>(*inter));
  }
  ASSERT_FALSE(poly3.Intersects(s2));
  ASSERT_TRUE(poly3.Intersects(s3));
  {
    auto inter = poly3.Intersection(s3);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline2D::MultiPoint>(*inter));
    auto mpoint = std::get<g::Polyline2D::MultiPoint>(*inter);
    ASSERT_EQ(g::Point2D(-1.5, -1.5), mpoint[0]);
    ASSERT_EQ(g::Point2D(-1.5, -2.5), mpoint[1]);
  }
}

TEST_F(Polyline2DTest, Intersection) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 =
      g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -2, 1 -2, 1 2)");  // intersects poly2 (-1 1, -0.5 -2, 0.5 -2, 1 1)
  auto poly2 = g::Polyline2D::FromWkt("LINESTRING (-2 1, -0.5 1, -0.5 -3, 0.5 -3, 0.5 1, 2 1)");
  auto poly3 = g::Polyline2D::FromWkt("LINESTRING (-3 4, -2 5, 0 5, 1 6, 2 4)");  // no intersection

  ASSERT_TRUE(poly1.Intersects(poly2));
  {
    auto inter = poly1.Intersection(poly2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline2D::MultiPoint>(*inter));
    auto mpoint = std::get<g::Polyline2D::MultiPoint>(*inter);
    ASSERT_EQ(4, mpoint.size());
    ASSERT_EQ(g::Point2D(-1, 1), mpoint[0]);
    ASSERT_EQ(g::Point2D(-0.5, -2), mpoint[1]);
    ASSERT_EQ(g::Point2D(0.5, -2), mpoint[2]);
    ASSERT_EQ(g::Point2D(1, 1), mpoint[3]);
  }
  ASSERT_FALSE(poly1.Intersects(poly3));
  ASSERT_FALSE(poly2.Intersects(poly3));
}

TEST_F(Polyline2DTest, Wkt) {
  ASSERT_EQ("LINESTRING (0 0, 1 1)", g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 1)}).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("LINESTRING (56491.62 -795.97, -10351.52 7.61)",
            g::Polyline2D::Make({g::Point2D(56491.6164, -795.97416),
                                 g::Point2D(-9137.3679, 10.35678),  // collinear point: expected to be removed
                                 g::Point2D(-10351.516, 7.61)})
                .ToWkt());

  geompp::DECIMAL_PRECISION = 6;
  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)}),
            g::Polyline2D::FromWkt("LINESTRING (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(-7.5, -60.7), g::Point2D()}),
            g::Polyline2D::FromWkt("  linestring( -7.5    -60.7, 0   0)"));
  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(0.645, -1.689741), g::Point2D(1, 0), g::Point2D(7, 41.365197)}),
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

TEST_F(Polyline2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "polyline.wkt").string();
  auto s = g::Polyline2D::Make({g::Point2D(), g::Point2D(1, 0)});

  s.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Polyline2D s_file = g::Polyline2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(s, s_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Polyline2DTest, TestFromFile) {
  std::string path = (test_res_path / "polyline2d" / "polyline.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Polyline2D::FromFile(path));

  auto p = g::Polyline2D::FromFile(path);

  GEOMPP_LOG(INFO) << "form file = " << p.ToWkt();
}

TEST_F(Polyline2DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  std::vector<g::Point2D> points{g::Point2D(-2, -5), g::Point2D(-2, -3), g::Point2D(2, -3), g::Point2D(2, 2)};
  auto polyline = g::Polyline2D::Make(points);

  // knots
  for (auto const& p : points) {
    ASSERT_EQ(0, g::round(polyline.DistanceTo(p)));
  }

  // mid points
  for (int i = 0; i < points.size() - 1; ++i) {
    auto avg = ((points[i].ToVector() + points[i + 1].ToVector()) / 2.0).ToPoint();
    ASSERT_EQ(0, g::round(polyline.DistanceTo(avg)));
  }

  // before the polyline
  ASSERT_EQ(1, g::round(polyline.DistanceTo(g::Point2D(-3, -4))));
  ASSERT_EQ(1, g::round(polyline.DistanceTo(g::Point2D(0, -2))));
}

}  // namespace geompp_tests
