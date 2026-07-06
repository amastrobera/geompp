#include "polyline2d.hpp"

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
  EXPECT_ANY_THROW(g::Polyline2D::Make({}));                    // cannot make a segment in 0 points
  EXPECT_ANY_THROW(g::Polyline2D::Make({g::Point2D::Zero()}));  // cannot make a segment in 1 sole point
  EXPECT_ANY_THROW(
      g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D::Zero()}));  // cannot make a segment in 1 sole unique point

  EXPECT_NO_THROW(g::Polyline2D::Make(
      {g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 0)}));  // cannot make a segment in 1 sole unique point
  EXPECT_EQ(2, g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 0)})
                   .Size());  // removed non-unique point

  EXPECT_EQ(
      2,
      g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(3, 0)}).Size());  // removed collinear point
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
  auto s1 = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0)});

  ASSERT_EQ(0.2, g::round(s1.Location(g::Point2D(0.2, 0))));
  ASSERT_EQ(0.5, g::round(s1.Location(g::Point2D(0.5, 0))));
  ASSERT_EQ(0.75, g::round(s1.Location(g::Point2D(0.75, 0))));

  ASSERT_EQ(std::numeric_limits<double>::infinity(), g::round(s1.Location(g::Point2D(-1, 0))));
  ASSERT_EQ(std::numeric_limits<double>::infinity(), g::round(s1.Location(g::Point2D(-0.1, 0))));
  ASSERT_EQ(std::numeric_limits<double>::infinity(), g::round(s1.Location(g::Point2D(1.1, 0))));

  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, 1))));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(0, -1))));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(-1, -1))));
  ASSERT_TRUE(std::isinf(s1.Location(g::Point2D(1, -1))));
}

TEST_F(Polyline2DTest, Interpolate) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly = g::Polyline2D::FromWkt("LINESTRING (0 0, 3 0)");

  // on segment
  ASSERT_EQ(g::Point2D::Zero(), poly.Interpolate(0));

  ASSERT_EQ(g::Point2D(3, 0), poly.Interpolate(1));

  // on segment
  ASSERT_EQ(0.223, g::round(poly.Location(poly.Interpolate(0.223)), 3));

  // out of range throws
  EXPECT_ANY_THROW(poly.Interpolate(1.2));
  EXPECT_ANY_THROW(poly.Interpolate(-0.2));
}

TEST_F(Polyline2DTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -2, 1 -2, 1 2)");  // intersects x (-1 0, 1 0) and y (0 -2)
  auto poly2 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -0.5 2, 1 2, 2 1)");  // intersects y (0 2)
  auto poly3 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -1, -2 -2, -1 -3)");  // intersects x (-1 0)

  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto y = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());

  ASSERT_TRUE(poly1.Intersects(x));
  {
    auto inter = poly1.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(2u, inter->size());
    ASSERT_EQ(g::Point2D(-1, 0), (*inter)[0]);
    ASSERT_EQ(g::Point2D(1, 0), (*inter)[1]);
  }

  ASSERT_TRUE(poly1.Intersects(y));
  {
    auto inter = poly1.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    EXPECT_EQ(g::Point2D(0, -2), inter->front());
  }

  ASSERT_TRUE(poly2.Intersects(y));
  {
    auto inter = poly2.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    EXPECT_EQ(g::Point2D(0, 2), inter->front());
  }
  ASSERT_FALSE(poly2.Intersects(x));

  ASSERT_TRUE(poly3.Intersects(x));
  {
    auto inter = poly3.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    EXPECT_EQ(g::Point2D(-1, 0), inter->front());
  }
  ASSERT_FALSE(poly3.Intersects(y));
}

TEST_F(Polyline2DTest, IntersectionWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline2D::FromWkt(
      "LINESTRING (-1 2, -1 -2, 1 -2, 1 2)");  // intersects x_neg (-1 0), x_pos (1 0) and y_neg (0 -2)
  auto poly2 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -0.5 2, 1 2, 2 1)");     // intersects y_pos (0 2)
  auto poly3 = g::Polyline2D::FromWkt("LINESTRING (-1 2, -1 -1, -2 -2, -1 -3)");  // intersects x_neg (-1 0)

  auto x_pos = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto x_neg = g::Ray2D::Make(g::Point2D::Zero(), -g::Vector2D::BasisX());
  auto y_pos = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());
  auto y_neg = g::Ray2D::Make(g::Point2D::Zero(), -g::Vector2D::BasisY());

  ASSERT_TRUE(poly1.Intersects(x_neg));
  {
    auto inter = poly1.Intersection(x_neg);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    ASSERT_EQ(g::Point2D(-1, 0), inter->front());
  }
  ASSERT_TRUE(poly1.Intersects(x_pos));
  {
    auto inter = poly1.Intersection(x_pos);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    ASSERT_EQ(g::Point2D(1, 0), inter->front());
  }
  ASSERT_TRUE(poly1.Intersects(y_neg));
  {
    auto inter = poly1.Intersection(y_neg);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    ASSERT_EQ(g::Point2D(0, -2), inter->front());
  }
  ASSERT_FALSE(poly1.Intersects(y_pos));

  ASSERT_TRUE(poly2.Intersects(y_pos));
  {
    auto inter = poly2.Intersection(y_pos);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    EXPECT_EQ(g::Point2D(0, 2), inter->front());
  }
  ASSERT_FALSE(poly2.Intersects(y_neg));
  ASSERT_FALSE(poly2.Intersects(x_pos));
  ASSERT_FALSE(poly2.Intersects(x_neg));

  ASSERT_TRUE(poly3.Intersects(x_neg));
  {
    auto inter = poly3.Intersection(x_neg);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    EXPECT_EQ(g::Point2D(-1, 0), inter->front());
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
    ASSERT_EQ(2u, inter->size());
    ASSERT_EQ(g::Point2D(-1, 0), (*inter)[0]);
    ASSERT_EQ(g::Point2D(1, 0), (*inter)[1]);
  }
  ASSERT_FALSE(poly1.Intersects(s2));
  ASSERT_FALSE(poly1.Intersects(s2));

  ASSERT_TRUE(poly2.Intersects(s2));
  {
    auto inter = poly2.Intersection(s2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    EXPECT_EQ(g::Point2D(0, 2), inter->front());
  }
  ASSERT_FALSE(poly2.Intersects(s1));
  ASSERT_FALSE(poly2.Intersects(s3));

  ASSERT_TRUE(poly3.Intersects(s1));
  {
    auto inter = poly3.Intersection(s1);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(1u, inter->size());
    EXPECT_EQ(g::Point2D(-1, 0), inter->front());
  }
  ASSERT_FALSE(poly3.Intersects(s2));
  ASSERT_TRUE(poly3.Intersects(s3));
  {
    auto inter = poly3.Intersection(s3);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(2u, inter->size());
    ASSERT_EQ(g::Point2D(-1.5, -1.5), (*inter)[0]);
    ASSERT_EQ(g::Point2D(-1.5, -2.5), (*inter)[1]);
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
    ASSERT_EQ(4u, inter->size());
    ASSERT_EQ(g::Point2D(-1, 1), (*inter)[0]);
    ASSERT_EQ(g::Point2D(-0.5, -2), (*inter)[1]);
    ASSERT_EQ(g::Point2D(0.5, -2), (*inter)[2]);
    ASSERT_EQ(g::Point2D(1, 1), (*inter)[3]);
  }
  ASSERT_FALSE(poly1.Intersects(poly3));
  ASSERT_FALSE(poly2.Intersects(poly3));
}

TEST_F(Polyline2DTest, Wkt) {
  ASSERT_EQ("LINESTRING (0 0, 1 1)", g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 1)}).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("LINESTRING (0.12 0.57, 4.57 3.23)",
            g::Polyline2D::Make({g::Point2D(0.1234, 0.5678),
                                 g::Point2D(2.3456, 1.9012),  // collinear point: expected to be removed
                                 g::Point2D(4.5678, 3.2346)})
                .ToWkt());

  geompp::DECIMAL_PRECISION = 6;
  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)}),
            g::Polyline2D::FromWkt("LINESTRING (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Polyline2D::Make({g::Point2D(-7.5, -60.7), g::Point2D::Zero()}),
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
  auto s = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0)});

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

TEST_F(Polyline2DTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(4, 0), g::Point2D(4, 4)});

  ASSERT_EQ(g::Point2D(4, 3), poly.ProjectOnto(g::Point2D(2, 3)));
  ASSERT_EQ(g::Point2D(4, 2), poly.ProjectOnto(g::Point2D(6, 2)));
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
  for (int i = 0; i < static_cast<int>(points.size()) - 1; ++i) {
    auto avg = ((points[i].ToVector() + points[i + 1].ToVector()) / 2.0).ToPoint();
    ASSERT_EQ(0, g::round(polyline.DistanceTo(avg)));
  }

  // before the polyline
  ASSERT_EQ(1, g::round(polyline.DistanceTo(g::Point2D(-3, -4))));
  ASSERT_EQ(1, g::round(polyline.DistanceTo(g::Point2D(0, -2))));
}

TEST_F(Polyline2DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto p1 = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p2 = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p3 = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(2, 0), g::Point2D(2, 2)});

  ASSERT_TRUE(p1.AlmostEquals(p2));
  ASSERT_FALSE(p1.AlmostEquals(p3));
  ASSERT_EQ(p1, p2);
  ASSERT_NE(p1, p3);

  // different number of knots → not equal
  auto p4 = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_FALSE(p1.AlmostEquals(p4));
}

TEST_F(Polyline2DTest, ToSegments) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});

  auto segs = poly.ToSegments();
  ASSERT_EQ(3, segs.size());

  ASSERT_EQ(g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0)), segs[0]);
  ASSERT_EQ(g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(1, 1)), segs[1]);
  ASSERT_EQ(g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1)), segs[2]);

  // two-knot polyline → one segment
  auto p2 = g::Polyline2D::Make({g::Point2D::Zero(), g::Point2D(3, 4)});
  auto segs2 = p2.ToSegments();
  ASSERT_EQ(1, segs2.size());
  ASSERT_EQ(g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(3, 4)), segs2[0]);
}

TEST_F(Polyline2DTest, ConvexHull_TooFewPoints_Throws) {
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(1, 0)});
  EXPECT_THROW(pl.ConvexHull(), std::invalid_argument);
}

TEST_F(Polyline2DTest, ConvexHull_ThreePoints_ReturnsTriangle) {
  auto pl = g::Polyline2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(2, 3)});
  auto hull = pl.ConvexHull();
  EXPECT_EQ(3, hull.Size());
}

TEST_F(Polyline2DTest, ConvexHull_ConcavePath_InnerPointExcluded) {
  // simple path: outer square corners with one inner dip at (2,1)
  // hull should be the 4 outer corners; (2,1) excluded
  auto pl = g::Polyline2D::Make(
      {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4),
       g::Point2D(2, 1), g::Point2D(0, 4)});
  auto hull = pl.ConvexHull();
  ASSERT_EQ(4, hull.Size());

  std::vector<g::Point2D> expected{
      g::Point2D(0, 0), g::Point2D(4, 0),
      g::Point2D(4, 4), g::Point2D(0, 4)};
  for (auto const& e : expected) {
    bool found = false;
    for (int i = 0; i < (int)hull.Size(); ++i) {
      if (hull[i].AlmostEquals(e)) { found = true; break; }
    }
    EXPECT_TRUE(found) << "expected " << e.ToWkt() << " on hull";
  }
}

TEST_F(Polyline2DTest, WktFromWkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)});
  ASSERT_EQ("LINESTRING (0 0, 1 0, 1 1)", pl.ToWkt());

  auto pl2 = g::Polyline2D::FromWkt("LINESTRING (0 0, 1 0, 1 1)");
  EXPECT_EQ(pl, pl2);
  EXPECT_EQ(pl, g::Polyline2D::FromWkt("LINESTRING (  0 0  ,  1 0  ,  1 1  )"));
}

// ── Overlap tests ─────────────────────────────────────────────────────────────

TEST_F(Polyline2DTest, OverlapWLine_CollinearSegment_ReturnsSegment) {
  // L-shaped polyline: horizontal (0,0)-(4,0) then vertical (4,0)-(4,3)
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 3)});
  // x-axis line overlaps the first horizontal segment
  auto line = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  EXPECT_TRUE(pl.Overlaps(line));
  auto ov = pl.Overlap(line);
  ASSERT_TRUE(ov.has_value());
  ASSERT_EQ(1u, ov->size());
  EXPECT_TRUE((*ov)[0].AlmostEquals(g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(4, 0))));
}

TEST_F(Polyline2DTest, OverlapWLine_Perpendicular_NoOverlap) {
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 3)});
  auto line = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(0, 1));
  EXPECT_FALSE(pl.Overlaps(line));
  EXPECT_FALSE(pl.Overlap(line).has_value());
}

TEST_F(Polyline2DTest, OverlapWRay_CollinearPartial_ReturnsSegment) {
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(6, 0)});
  // Ray from (2,0) in +x direction overlaps the [2,6] portion
  auto ray = g::Ray2D::Make(g::Point2D(2, 0), g::Vector2D(1, 0));
  EXPECT_TRUE(pl.Overlaps(ray));
  auto ov = pl.Overlap(ray);
  ASSERT_TRUE(ov.has_value());
  ASSERT_EQ(1u, ov->size());
  EXPECT_TRUE((*ov)[0].AlmostEquals(g::LineSegment2D::Make(g::Point2D(2, 0), g::Point2D(6, 0))));
}

TEST_F(Polyline2DTest, OverlapWSegment_PartialOverlap_ReturnsSegment) {
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(5, 0)});
  auto seg = g::LineSegment2D::Make(g::Point2D(3, 0), g::Point2D(7, 0));
  EXPECT_TRUE(pl.Overlaps(seg));
  auto ov = pl.Overlap(seg);
  ASSERT_TRUE(ov.has_value());
  ASSERT_EQ(1u, ov->size());
  EXPECT_TRUE((*ov)[0].AlmostEquals(g::LineSegment2D::Make(g::Point2D(3, 0), g::Point2D(5, 0))));
}

TEST_F(Polyline2DTest, OverlapWPolyline_SharedSegment_ReturnsThem) {
  auto pl1 = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 3)});
  auto pl2 = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(4, 0)});
  EXPECT_TRUE(pl1.Overlaps(pl2));
  auto ov = pl1.Overlap(pl2);
  ASSERT_TRUE(ov.has_value());
  ASSERT_EQ(1u, ov->size());
}

TEST_F(Polyline2DTest, OverlapWPolyline_NoSharedSegment_NullOpt) {
  auto pl1 = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(4, 0)});
  auto pl2 = g::Polyline2D::Make({g::Point2D(0, 1), g::Point2D(4, 1)});
  EXPECT_FALSE(pl1.Overlaps(pl2));
  EXPECT_FALSE(pl1.Overlap(pl2).has_value());
}

// ── Touch tests ───────────────────────────────────────────────────────────────

TEST_F(Polyline2DTest, TouchWLine_SegmentEndpointOnLine_ReturnsPoint) {
  // Polyline has a vertical segment endpoint at (2,0) which lies on the x-axis line
  auto pl = g::Polyline2D::Make({g::Point2D(2, 0), g::Point2D(2, 3)});
  auto line = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  EXPECT_TRUE(pl.Touches(line));
  auto tp = pl.Touch(line);
  ASSERT_TRUE(tp.has_value());
  ASSERT_EQ(1u, tp->size());
  EXPECT_TRUE((*tp)[0].AlmostEquals(g::Point2D(2, 0)));
}

TEST_F(Polyline2DTest, TouchWLine_CollinearSegment_NotTouch) {
  auto pl = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(4, 0)});
  auto line = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  EXPECT_FALSE(pl.Touches(line));
  EXPECT_FALSE(pl.Touch(line).has_value());
}

TEST_F(Polyline2DTest, TouchWRay_EndpointOnRay_ReturnsPoint) {
  auto pl = g::Polyline2D::Make({g::Point2D(3, 0), g::Point2D(3, 2)});
  auto ray = g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D(1, 0));
  EXPECT_TRUE(pl.Touches(ray));
  auto tp = pl.Touch(ray);
  ASSERT_TRUE(tp.has_value());
  ASSERT_EQ(1u, tp->size());
  EXPECT_TRUE((*tp)[0].AlmostEquals(g::Point2D(3, 0)));
}

TEST_F(Polyline2DTest, TouchWSegment_TJunction_ReturnsPoint) {
  // Polyline L-shape; bottom of the vertical arm touches horizontal segment
  auto pl = g::Polyline2D::Make({g::Point2D(3, 0), g::Point2D(3, 3)});
  auto seg = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(5, 0));
  EXPECT_TRUE(pl.Touches(seg));
  auto tp = pl.Touch(seg);
  ASSERT_TRUE(tp.has_value());
  ASSERT_EQ(1u, tp->size());
  EXPECT_TRUE((*tp)[0].AlmostEquals(g::Point2D(3, 0)));
}

TEST_F(Polyline2DTest, TouchWPolyline_SharedEndpoint_ReturnsPoint) {
  auto pl1 = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(3, 0)});
  auto pl2 = g::Polyline2D::Make({g::Point2D(3, 0), g::Point2D(3, 3)});
  EXPECT_TRUE(pl1.Touches(pl2));
  auto tp = pl1.Touch(pl2);
  ASSERT_TRUE(tp.has_value());
  ASSERT_EQ(1u, tp->size());
  EXPECT_TRUE((*tp)[0].AlmostEquals(g::Point2D(3, 0)));
}

TEST_F(Polyline2DTest, TouchWPolyline_Disjoint_NullOpt) {
  auto pl1 = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(2, 0)});
  auto pl2 = g::Polyline2D::Make({g::Point2D(5, 0), g::Point2D(5, 3)});
  EXPECT_FALSE(pl1.Touches(pl2));
  EXPECT_FALSE(pl1.Touch(pl2).has_value());
}

}  // namespace geompp_tests
