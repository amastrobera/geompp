#include "line2d.hpp"

#include "line_segment2d.hpp"
#include "ray2d.hpp"

#include "point2d.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Line2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Line2DTest, Constructor) {
  geompp::DECIMAL_PRECISION = 4;
  auto p0 = g::Point2D(1, 2);
  auto p1 = g::Point2D(3, 5);
  auto l1 = g::Line2D::Make(p0, p1);

  EXPECT_ANY_THROW(g::Line2D::Make(p0, p0));  // cannot make a line that starts and finishes at the same point

  EXPECT_EQ(l1.First(), p0);
  EXPECT_EQ(l1.Last(), p1);
  EXPECT_EQ(l1.Direction(), (p1 - p0).Normalize());

  auto v = g::Vector2D::BasisY();
  auto l2 = g::Line2D::Make(p0, v);
  EXPECT_EQ(l2.Origin(), p0);
  EXPECT_EQ(l2.Direction(), v);
  EXPECT_EQ(l2.Last(), p0 + v);
}

TEST_F(Line2DTest, Contains) {
  auto l1 = g::Line2D::Make(g::Point2D(0, -1), g::Point2D(0, 1));
  auto p0 = g::Point2D::Zero();
  auto p1 = g::Point2D(0, 10);
  auto p2 = g::Point2D(-10, 0);

  ASSERT_TRUE(l1.Contains(p0));
  ASSERT_TRUE(l1.Contains(p1));
  ASSERT_TRUE(!l1.Contains(p2));
}

TEST_F(Line2DTest, Intersection) {
  auto l1 = g::Line2D::Make(g::Point2D(0, -1), g::Point2D(0, 1));
  auto l2 = g::Line2D::Make(g::Point2D(-1, 0), g::Point2D(1, 0));

  EXPECT_TRUE(l1.Intersects(l2));
  auto inter = l1.Intersection(l2);

  ASSERT_TRUE(inter.has_value());
  EXPECT_EQ(g::Point2D::Zero(), *inter);
}

TEST_F(Line2DTest, Wkt) {
  ASSERT_EQ("LINE (0 0, 1 1)", g::Line2D::Make(g::Point2D::Zero(), g::Point2D(1, 1)).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("LINE (56491.62 -795.97, -9137.37 10.36)",
            g::Line2D::Make(g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678)).ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Line2D::Make(g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)),
            g::Line2D::FromWkt("LINE (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(-7.5, -60.7), g::Point2D::Zero()),
            g::Line2D::FromWkt("  LINE( -7.5    -60.7, 0   0)"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(0.645, -1.689741), g::Point2D(1, 0)),
            g::Line2D::FromWkt("LINE   ( 0.645  -1.689741  , 1 0  )"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 1)),
            g::Line2D::FromWkt("LINE (0 0,1 1)"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 1)),
            g::Line2D::FromWkt("LINE (  0 0  ,  1  1  )"));

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

TEST_F(Line2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "line.wkt").string();
  auto v = g::Line2D::Make(g::Point2D(12.32, -61.6164), g::Point2D(-14.64661, -9.1641));

  v.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Line2D v_file = g::Line2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(v, v_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Line2DTest, TestFromFile) {
  std::string path = (test_res_path / "line2d" / "line.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Line2D::FromFile(path));

  auto p = g::Line2D::FromFile(path);

  GEOMPP_LOG(INFO) << "form file = " << p.ToWkt();
}

TEST_F(Line2DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto line = g::Line2D::FromWkt("LINE (0 0, 3 0)");

  // on segment
  auto p1 = g::Point2D::Zero();
  EXPECT_EQ(0, g::round(line.DistanceTo(p1)));

  auto p2 = g::Point2D(3, 0);
  EXPECT_EQ(0, g::round(line.DistanceTo(p2)));

  // on line
  auto p3 = g::Point2D(-20, 0);
  EXPECT_EQ(0, g::round(line.DistanceTo(p3)));

  auto p4 = g::Point2D(12, 0);
  EXPECT_EQ(0, g::round(line.DistanceTo(p4)));

  // Q1
  auto p5 = g::Point2D(12, 12);
  EXPECT_EQ(12, g::round(line.DistanceTo(p5)));

  // Q2
  auto p6 = g::Point2D(-5, 10);
  EXPECT_EQ(10, g::round(line.DistanceTo(p6)));

  // Q3
  auto p7 = g::Point2D(-2, -4);
  EXPECT_EQ(4, g::round(line.DistanceTo(p7)));

  // Q4
  auto p8 = g::Point2D(3, -7);
  EXPECT_EQ(7, g::round(line.DistanceTo(p8)));
}

TEST_F(Line2DTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  auto line = g::Line2D::Make(g::Point2D::Zero(), g::Point2D(3, 0));

  // point above: foot on X-axis
  ASSERT_EQ(g::Point2D(2, 0), line.ProjectOnto(g::Point2D(2, 5)));

  // already on line: unchanged
  ASSERT_EQ(g::Point2D(2, 0), line.ProjectOnto(g::Point2D(2, 0)));

  // before P0: foot is behind origin
  ASSERT_EQ(g::Point2D(-2, 0), line.ProjectOnto(g::Point2D(-2, 3)));

  // beyond P1: foot is past P1
  ASSERT_EQ(g::Point2D(5, 0), line.ProjectOnto(g::Point2D(5, -3)));

  // diagonal line y=x: (1,0) projects to (0.5, 0.5)
  geompp::DECIMAL_PRECISION = 3;
  auto ld = g::Line2D::Make(g::Point2D::Zero(), g::Point2D(1, 1));
  ASSERT_EQ(g::Point2D(0.5, 0.5), ld.ProjectOnto(g::Point2D(1, 0)));
}

TEST_F(Line2DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto l1 = g::Line2D::Make(g::Point2D::Zero(), g::Point2D(3, 0));
  auto l2 = g::Line2D::Make(g::Point2D::Zero(), g::Point2D(3, 0));
  auto l3 = g::Line2D::Make(g::Point2D::Zero(), g::Point2D(0, 3));

  // identical construction
  ASSERT_TRUE(l1.AlmostEquals(l2));
  ASSERT_EQ(l1, l2);

  // different direction: not equal
  ASSERT_FALSE(l1.AlmostEquals(l3));
  ASSERT_NE(l1, l3);

  // same infinite line, different origin — geometrically equal
  auto l4 = g::Line2D::Make(g::Point2D(5, 0), g::Point2D(8, 0));
  ASSERT_TRUE(l1.AlmostEquals(l4));
  ASSERT_EQ(l1, l4);

  // same infinite line, reversed direction — geometrically equal
  auto l5 = g::Line2D::Make(g::Point2D(3, 0), g::Point2D::Zero());
  ASSERT_TRUE(l1.AlmostEquals(l5));
  ASSERT_EQ(l1, l5);

  // parallel but offset — NOT equal
  auto l6 = g::Line2D::Make(g::Point2D(0, 1), g::Point2D(3, 1));
  ASSERT_FALSE(l1.AlmostEquals(l6));
  ASSERT_NE(l1, l6);
}

TEST_F(Line2DTest, OverlapWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto x2 = g::Line2D::Make(g::Point2D(5, 0), g::Point2D(8, 0));  // same infinite line
  auto y = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());
  auto x_off = g::Line2D::Make(g::Point2D(0, 1), g::Vector2D::BasisX());  // parallel, offset

  // same line → overlaps
  EXPECT_TRUE(x.Overlaps(x2));
  auto ov = x.Overlap(x2);
  ASSERT_TRUE(ov.has_value());
  EXPECT_TRUE(ov->AlmostEquals(x));

  // crossing (not collinear) → no overlap
  EXPECT_FALSE(x.Overlaps(y));
  EXPECT_FALSE(x.Overlap(y).has_value());

  // parallel but offset → no overlap
  EXPECT_FALSE(x.Overlaps(x_off));
  EXPECT_FALSE(x.Overlap(x_off).has_value());
}

TEST_F(Line2DTest, OverlapWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto rx = g::Ray2D::Make(g::Point2D(2, 0), g::Vector2D::BasisX());  // collinear ray
  auto ry = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());  // perpendicular

  EXPECT_TRUE(x.Overlaps(rx));
  auto ov = x.Overlap(rx);
  ASSERT_TRUE(ov.has_value());
  EXPECT_TRUE(ov->AlmostEquals(rx));

  EXPECT_FALSE(x.Overlaps(ry));
  EXPECT_FALSE(x.Overlap(ry).has_value());
}

TEST_F(Line2DTest, OverlapWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto seg = g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(3, 0));  // collinear
  auto seg_off = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(3, 1));  // parallel, offset

  EXPECT_TRUE(x.Overlaps(seg));
  auto ov = x.Overlap(seg);
  ASSERT_TRUE(ov.has_value());
  EXPECT_EQ(seg, *ov);

  EXPECT_FALSE(x.Overlaps(seg_off));
  EXPECT_FALSE(x.Overlap(seg_off).has_value());
}

TEST_F(Line2DTest, TouchWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

  // ray origin lies on line, non-collinear → touch at origin
  auto r_touch = g::Ray2D::Make(g::Point2D(3, 0), g::Vector2D::BasisY());
  EXPECT_TRUE(x.Touches(r_touch));
  auto t = x.Touch(r_touch);
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(g::Point2D(3, 0), *t);

  // collinear ray → Overlap, not Touch
  auto r_col = g::Ray2D::Make(g::Point2D(1, 0), g::Vector2D::BasisX());
  EXPECT_FALSE(x.Touches(r_col));
  EXPECT_FALSE(x.Touch(r_col).has_value());

  // ray origin off the line → no touch
  auto r_off = g::Ray2D::Make(g::Point2D(0, 1), g::Vector2D::BasisY());
  EXPECT_FALSE(x.Touches(r_off));
  EXPECT_FALSE(x.Touch(r_off).has_value());
}

TEST_F(Line2DTest, TouchWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

  // First() endpoint lies on line → touch at First
  auto seg_first = g::LineSegment2D::Make(g::Point2D(2, 0), g::Point2D(2, 3));
  EXPECT_TRUE(x.Touches(seg_first));
  auto t1 = x.Touch(seg_first);
  ASSERT_TRUE(t1.has_value());
  EXPECT_EQ(g::Point2D(2, 0), *t1);

  // Last() endpoint lies on line → touch at Last
  auto seg_last = g::LineSegment2D::Make(g::Point2D(2, 3), g::Point2D(2, 0));
  EXPECT_TRUE(x.Touches(seg_last));
  auto t2 = x.Touch(seg_last);
  ASSERT_TRUE(t2.has_value());
  EXPECT_EQ(g::Point2D(2, 0), *t2);

  // collinear segment → Overlap, not Touch
  auto seg_col = g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(4, 0));
  EXPECT_FALSE(x.Touches(seg_col));
  EXPECT_FALSE(x.Touch(seg_col).has_value());

  // segment fully above line → no touch
  auto seg_off = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(3, 2));
  EXPECT_FALSE(x.Touches(seg_off));
  EXPECT_FALSE(x.Touch(seg_off).has_value());
}

}  // namespace geompp_tests
