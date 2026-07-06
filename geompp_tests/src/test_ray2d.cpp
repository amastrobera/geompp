#include "ray2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Ray2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Ray2DTest, Constructor) {
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

  ASSERT_EQ(g::Point2D::Zero(), r1.Origin());
  ASSERT_EQ(g::Vector2D::BasisX(), r1.Direction());

  EXPECT_ANY_THROW(g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D(0, 0)));  // cannot make a ray going no where
}

TEST_F(Ray2DTest, Contains) {
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  ASSERT_TRUE(r1.Contains(g::Point2D::Zero()));
  ASSERT_TRUE(r1.Contains(g::Point2D(1, 0)));
  ASSERT_TRUE(r1.Contains(g::Point2D(30, 0)));

  ASSERT_FALSE(r1.Contains(g::Point2D(1, 1)));
  ASSERT_FALSE(r1.Contains(g::Point2D(1, -1)));
  ASSERT_FALSE(r1.Contains(g::Point2D(-1, 0)));
  ASSERT_FALSE(r1.Contains(g::Point2D(-1, 1)));
  ASSERT_FALSE(r1.Contains(g::Point2D(-1, -1)));
}

TEST_F(Ray2DTest, AheadBehind) {
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

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

TEST_F(Ray2DTest, Intersection) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray2D::Make(g::Point2D(-1, 1), g::Vector2D(1, -1));
  auto r2 = g::Ray2D::Make(g::Point2D(-1, -1), g::Vector2D(1, 1));    // intersects r1 in (0,0)
  auto r3 = g::Ray2D::Make(g::Point2D(-0.5, 0), g::Vector2D(0, -1));  // intersects r2 in (-0.5,-0.5)
  auto r4 = g::Ray2D::Make(g::Point2D(1, -0.5), g::Vector2D::BasisY());   // intersects r2 in (1,1)

  ASSERT_TRUE(r1.Intersects(r2));
  {
    auto inter = r1.Intersection(r2);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D::Zero(), *inter);
  }

  ASSERT_FALSE(r1.Intersects(r3));
  ASSERT_FALSE(r1.Intersection(r3).has_value());
  ASSERT_FALSE(r1.Intersects(r4));
  ASSERT_FALSE(r1.Intersection(r4).has_value());

  ASSERT_TRUE(r2.Intersects(r3));
  {
    auto inter = r2.Intersection(r3);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(-0.5, -0.5), *inter);
  }

  ASSERT_TRUE(r2.Intersects(r4));
  {
    auto inter = r2.Intersection(r4);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(1, 1), *inter);
  }
}

TEST_F(Ray2DTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray2D::Make(g::Point2D(-1, 1), g::Vector2D(1, -1));
  auto r2 = g::Ray2D::Make(g::Point2D(1, -1), g::Vector2D(1, 1));  // intersects r1 in (0,0)

  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto y = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());

  ASSERT_TRUE(r1.Intersects(x));
  {
    auto inter = r1.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D::Zero(), *inter);
  }

  ASSERT_TRUE(r1.Intersects(y));
  {
    auto inter = r1.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D::Zero(), *inter);
  }

  ASSERT_TRUE(r2.Intersects(x));
  {
    auto inter = r2.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(2, 0), *inter);
  }

  ASSERT_FALSE(r2.Intersects(y));
}

TEST_F(Ray2DTest, Wkt) {
  ASSERT_EQ("RAY (0 0, 0.707 0.707)", g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D(1, 1)).ToWkt());  // normalized vector!
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("RAY (56491.62 -795.97, -1 0)",
            g::Ray2D::Make(g::Point2D(56491.6164, -795.97416), g::Vector2D(-9137.3679, 10.35678))
                .ToWkt());  // normalized vector!

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Ray2D::Make(g::Point2D(256.1343, -684.64971), g::Vector2D(-601.674503, 7.361975)),
            g::Ray2D::FromWkt("RAY (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Ray2D::Make(g::Point2D(-7.5, -60.7), g::Vector2D::BasisX()),
            g::Ray2D::FromWkt("  ray( -7.5    -60.7, 1   0)"));
  EXPECT_EQ(g::Ray2D::Make(g::Point2D(0.645, -1.689741), g::Vector2D::BasisX()),
            g::Ray2D::FromWkt("ray   ( 0.645  -1.689741  , 1 0  )"));
  EXPECT_EQ(g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D::BasisX()),
            g::Ray2D::FromWkt("RAY (0 0,1 0)"));
  EXPECT_EQ(g::Ray2D::Make(g::Point2D(0, 0), g::Vector2D::BasisX()),
            g::Ray2D::FromWkt("RAY (  0 0  ,  1  0  )"));

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

TEST_F(Ray2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "ray.wkt").string();
  auto v = g::Ray2D::Make(g::Point2D(12.32, -61.6164), g::Vector2D(1, 1));

  v.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Ray2D v_file = g::Ray2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(v, v_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Ray2DTest, TestFromFile) {
  std::string path = (test_res_path / "ray2d" / "ray.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Ray2D::FromFile(path));

  auto p = g::Ray2D::FromFile(path);

  GEOMPP_LOG(INFO) << "form file = " << p.ToWkt();
}

TEST_F(Ray2DTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  auto ray = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

  ASSERT_EQ(g::Point2D(3, 0), ray.ProjectOnto(g::Point2D(3, 5)));
  ASSERT_EQ(g::Point2D::Zero(), ray.ProjectOnto(g::Point2D(-2, 3)));
}

TEST_F(Ray2DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto ray = g::Ray2D::FromWkt("RAY (0 0, 1 0)");

  // on segment
  auto p1 = g::Point2D::Zero();
  EXPECT_EQ(0, g::round(ray.DistanceTo(p1)));

  auto p2 = g::Point2D(3, 0);
  EXPECT_EQ(0, g::round(ray.DistanceTo(p2)));

  // on line
  auto p3 = g::Point2D(-20, 0);
  EXPECT_EQ(20, g::round(ray.DistanceTo(p3)));

  auto p4 = g::Point2D(12, 0);
  EXPECT_EQ(0, g::round(ray.DistanceTo(p4)));

  // Q1
  auto p5 = g::Point2D(12, 12);
  EXPECT_EQ(12, g::round(ray.DistanceTo(p5)));

  // Q2
  auto p6 = g::Point2D(-5, 10);
  EXPECT_EQ(g::round(p6.DistanceTo(ray.Origin())), g::round(ray.DistanceTo(p6)));

  // Q3
  auto p7 = g::Point2D(-2, -4);
  EXPECT_EQ(g::round(p7.DistanceTo(ray.Origin())), g::round(ray.DistanceTo(p7)));

  // Q4
  auto p8 = g::Point2D(3, -7);
  EXPECT_EQ(7, g::round(ray.DistanceTo(p8)));
}

TEST_F(Ray2DTest, OverlapWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray2D::Make(g::Point2D(2, 0), g::Vector2D::BasisX());
  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());  // same infinite line
  auto y = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());

  EXPECT_TRUE(r.Overlaps(x));
  auto ov = r.Overlap(x);
  ASSERT_TRUE(ov.has_value());
  EXPECT_TRUE(ov->AlmostEquals(r));

  EXPECT_FALSE(r.Overlaps(y));
  EXPECT_FALSE(r.Overlap(y).has_value());
}

TEST_F(Ray2DTest, OverlapWRay_SameDir) {
  geompp::DECIMAL_PRECISION = 4;
  // r1 starts at (0,0) going +x; r2 starts at (2,0) going +x — r2 is inside r1
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto r2 = g::Ray2D::Make(g::Point2D(2, 0), g::Vector2D::BasisX());

  EXPECT_TRUE(r1.Overlaps(r2));
  auto ov = r1.Overlap(r2);
  ASSERT_TRUE(ov.has_value());
  ASSERT_TRUE(std::holds_alternative<g::Ray2D>(*ov));
  EXPECT_TRUE(std::get<g::Ray2D>(*ov).AlmostEquals(r2));

  // Symmetric: r2 overlaps r1 → result is r2 (the later origin)
  EXPECT_TRUE(r2.Overlaps(r1));
  auto ov2 = r2.Overlap(r1);
  ASSERT_TRUE(ov2.has_value());
  ASSERT_TRUE(std::holds_alternative<g::Ray2D>(*ov2));
  EXPECT_TRUE(std::get<g::Ray2D>(*ov2).AlmostEquals(r2));

  // Identical rays — same origin, same direction: full overlap, return the ray itself
  auto ov3 = r1.Overlap(r1);
  ASSERT_TRUE(ov3.has_value());
  ASSERT_TRUE(std::holds_alternative<g::Ray2D>(*ov3));
  EXPECT_TRUE(std::get<g::Ray2D>(*ov3).AlmostEquals(r1));
}

TEST_F(Ray2DTest, OverlapWRay_AntiParallel) {
  geompp::DECIMAL_PRECISION = 4;
  // r1 at (0,0) going +x; r3 at (5,0) going -x — anti-parallel, overlap is segment [0,5]
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto r3 = g::Ray2D::Make(g::Point2D(5, 0), g::Vector2D(-1, 0));

  EXPECT_TRUE(r1.Overlaps(r3));
  auto ov = r1.Overlap(r3);
  ASSERT_TRUE(ov.has_value());
  ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(*ov));
  auto seg = std::get<g::LineSegment2D>(*ov);
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(5, 0)), seg);

  // touching at a single point → no overlap
  auto r4 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D(-1, 0));  // goes -x from origin
  EXPECT_FALSE(r1.Overlaps(r4));
  EXPECT_FALSE(r1.Overlap(r4).has_value());
}

TEST_F(Ray2DTest, OverlapWRay_Disjoint) {
  geompp::DECIMAL_PRECISION = 4;
  // r1 goes +x from (0,0); r2 goes +y from (0,0) — not collinear
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto r2 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());
  EXPECT_FALSE(r1.Overlaps(r2));
  EXPECT_FALSE(r1.Overlap(r2).has_value());
}

TEST_F(Ray2DTest, OverlapWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray2D::Make(g::Point2D(1, 0), g::Vector2D::BasisX());  // starts at x=1
  auto seg_full = g::LineSegment2D::Make(g::Point2D(2, 0), g::Point2D(4, 0));   // fully inside ray
  auto seg_half = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(3, 0));   // starts before ray origin
  auto seg_none = g::LineSegment2D::Make(g::Point2D(-3, 0), g::Point2D(-1, 0)); // entirely before ray
  auto seg_touch = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(1, 0)); // touches at origin only

  EXPECT_TRUE(r.Overlaps(seg_full));
  EXPECT_EQ(seg_full, *r.Overlap(seg_full));

  EXPECT_TRUE(r.Overlaps(seg_half));
  auto ov_half = r.Overlap(seg_half);
  ASSERT_TRUE(ov_half.has_value());
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(3, 0)), *ov_half);

  EXPECT_FALSE(r.Overlaps(seg_none));
  EXPECT_FALSE(r.Overlap(seg_none).has_value());

  EXPECT_FALSE(r.Overlaps(seg_touch));
  EXPECT_FALSE(r.Overlap(seg_touch).has_value());
}

TEST_F(Ray2DTest, TouchWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

  // origin on the line, perpendicular ray → touch
  auto r_perp = g::Ray2D::Make(g::Point2D(2, 0), g::Vector2D::BasisY());
  EXPECT_TRUE(r_perp.Touches(x));
  auto t = r_perp.Touch(x);
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(g::Point2D(2, 0), *t);

  // collinear ray (Overlap case) → no touch
  auto r_col = g::Ray2D::Make(g::Point2D(1, 0), g::Vector2D::BasisX());
  EXPECT_FALSE(r_col.Touches(x));
  EXPECT_FALSE(r_col.Touch(x).has_value());

  // origin off the line → no touch
  auto r_off = g::Ray2D::Make(g::Point2D(0, 1), g::Vector2D::BasisX());
  EXPECT_FALSE(r_off.Touches(x));
  EXPECT_FALSE(r_off.Touch(x).has_value());
}

TEST_F(Ray2DTest, TouchWRay_NonParallel) {
  geompp::DECIMAL_PRECISION = 4;
  // r1 goes +x from origin; r2 goes +y from origin — same origin
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto r2 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());
  EXPECT_TRUE(r1.Touches(r2));
  auto t1 = r1.Touch(r2);
  ASSERT_TRUE(t1.has_value());
  EXPECT_EQ(g::Point2D::Zero(), *t1);

  // r3 origin at (3,0) going +y — r3.Origin() is on r1
  auto r3 = g::Ray2D::Make(g::Point2D(3, 0), g::Vector2D::BasisY());
  EXPECT_TRUE(r1.Touches(r3));
  auto t2 = r1.Touch(r3);
  ASSERT_TRUE(t2.has_value());
  EXPECT_EQ(g::Point2D(3, 0), *t2);

  // r4 and r1 cross at interior of both — not a Touch (no origin on the other)
  auto r4 = g::Ray2D::Make(g::Point2D(2, -1), g::Vector2D::BasisY());
  EXPECT_FALSE(r1.Touches(r4));
  EXPECT_FALSE(r1.Touch(r4).has_value());
}

TEST_F(Ray2DTest, TouchWRay_AntiParallel) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

  // same origin, anti-parallel → touch at origin
  auto r_same = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D(-1, 0));
  EXPECT_TRUE(r1.Touches(r_same));
  auto t = r1.Touch(r_same);
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(g::Point2D::Zero(), *t);

  // anti-parallel, overlapping region → no touch (Overlap handles it)
  auto r_over = g::Ray2D::Make(g::Point2D(3, 0), g::Vector2D(-1, 0));
  EXPECT_FALSE(r1.Touches(r_over));
  EXPECT_FALSE(r1.Touch(r_over).has_value());

  // same-direction collinear → no touch (Overlap handles it)
  auto r_same_dir = g::Ray2D::Make(g::Point2D(2, 0), g::Vector2D::BasisX());
  EXPECT_FALSE(r1.Touches(r_same_dir));
  EXPECT_FALSE(r1.Touch(r_same_dir).has_value());
}

TEST_F(Ray2DTest, TouchWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  // Ray goes +x from (0,0)
  auto r = g::Ray2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());

  // seg.First() = (3,0) on ray, seg.Last() = (3,2) not on ray → touch at First
  auto seg_f = g::LineSegment2D::Make(g::Point2D(3, 0), g::Point2D(3, 2));
  EXPECT_TRUE(r.Touches(seg_f));
  auto t1 = r.Touch(seg_f);
  ASSERT_TRUE(t1.has_value());
  EXPECT_EQ(g::Point2D(3, 0), *t1);

  // seg.Last() on ray, seg.First() not → touch at Last
  auto seg_l = g::LineSegment2D::Make(g::Point2D(3, 2), g::Point2D(3, 0));
  EXPECT_TRUE(r.Touches(seg_l));
  auto t2 = r.Touch(seg_l);
  ASSERT_TRUE(t2.has_value());
  EXPECT_EQ(g::Point2D(3, 0), *t2);

  // both endpoints on ray → Overlap (no touch)
  auto seg_both = g::LineSegment2D::Make(g::Point2D(2, 0), g::Point2D(4, 0));
  EXPECT_FALSE(r.Touches(seg_both));
  EXPECT_FALSE(r.Touch(seg_both).has_value());

  // no contact → no touch
  auto seg_none = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(3, 1));
  EXPECT_FALSE(r.Touches(seg_none));
  EXPECT_FALSE(r.Touch(seg_none).has_value());
}

}  // namespace geompp_tests
