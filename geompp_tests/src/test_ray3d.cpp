#include "ray3d.hpp"

#include "constants.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Ray3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Ray3DTest, Make) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D(3, 0, 0));

  ASSERT_EQ(g::Point3D(1, 2, 0), r.Origin());
  // direction is normalised
  ASSERT_EQ(g::Vector3D::BasisX(), r.Direction());

  // zero direction throws
  EXPECT_ANY_THROW(g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(0, 0, 0)));
}

TEST_F(Ray3DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto r2 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto r3 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisY());
  auto r4 = g::Ray3D::Make(g::Point3D(1, 0, 0), g::Vector3D::BasisX());

  ASSERT_EQ(r1, r2);
  ASSERT_NE(r1, r3);  // different direction
  ASSERT_NE(r1, r4);  // different origin

  // self-equality
  ASSERT_EQ(r1, r1);
}

TEST_F(Ray3DTest, Assignment) {
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto r2 = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D::BasisY());

  r2 = r1;
  ASSERT_EQ(r1, r2);

  r1 = r1;
  ASSERT_EQ(g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX()), r1);
}

TEST_F(Ray3DTest, IsAheadIsBehind) {
  geompp::DECIMAL_PRECISION = 4;
  // ray from origin pointing along +X
  auto r = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  // points ahead (same direction as DIR)
  ASSERT_TRUE(r.IsAhead(g::Point3D(1, 0, 0)));
  ASSERT_TRUE(r.IsAhead(g::Point3D(5, 3, 0)));  // off-axis but ahead in X
  ASSERT_TRUE(r.IsAhead(g::Point3D::Zero()));  // at origin: on the boundary (ahead)

  // points behind
  ASSERT_TRUE(r.IsBehind(g::Point3D(-1, 0, 0)));
  ASSERT_TRUE(r.IsBehind(g::Point3D(-1, 5, 0)));

  // IsAhead and IsBehind are complementary (not both true for the same point)
  auto p_ahead = g::Point3D(3, 0, 0);
  auto p_behind = g::Point3D(-3, 0, 0);
  ASSERT_TRUE(r.IsAhead(p_ahead));
  ASSERT_FALSE(r.IsBehind(p_ahead));
  ASSERT_TRUE(r.IsBehind(p_behind));
  ASSERT_FALSE(r.IsAhead(p_behind));
}

TEST_F(Ray3DTest, ToLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D::BasisX());
  auto l = r.ToLine();

  // the resulting Line3D passes through the ray's origin in the same direction
  ASSERT_EQ(g::Point3D(1, 2, 0), l.Origin());
  ASSERT_EQ(g::Vector3D::BasisX(), l.Direction());
}

TEST_F(Ray3DTest, IntersectionWithLine3D) {
  // ray along +X from origin; vertical line at x=3 in XY plane
  auto r = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto l_cross = g::Line3D::Make(g::Point3D(3, -1, 0), g::Point3D(3, 1, 0));

  EXPECT_TRUE(r.Intersects(l_cross));
  auto result = r.Intersection(l_cross);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(g::Point3D(3, 0, 0), *result);

  // line is behind the ray's origin: no intersection
  auto l_behind = g::Line3D::Make(g::Point3D(-3, -1, 0), g::Point3D(-3, 1, 0));
  EXPECT_FALSE(r.Intersects(l_behind));
  EXPECT_FALSE(r.Intersection(l_behind).has_value());

  // parallel lines → no intersection
  auto l_parallel = g::Line3D::Make(g::Point3D(0, 1, 0), g::Point3D(5, 1, 0));
  EXPECT_FALSE(r.Intersects(l_parallel));
  EXPECT_FALSE(r.Intersection(l_parallel).has_value());
}

TEST_F(Ray3DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  ASSERT_EQ("RAY (0 0 0, 1 0 0)", r.ToWkt());

  geompp::DECIMAL_PRECISION = 2;
  auto r2 = g::Ray3D::Make(g::Point3D(1.126, 2.354, 0.0), g::Vector3D::BasisX());
  ASSERT_EQ("RAY (1.13 2.35 0, 1 0 0)", r2.ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  // round-trip
  auto r3 = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D::BasisY());
  EXPECT_EQ(r3, g::Ray3D::FromWkt(r3.ToWkt()));
  // no space after comma
  EXPECT_EQ(g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D::BasisX()),
            g::Ray3D::FromWkt("RAY (0 0 0,1 0 0)"));
  // many spaces everywhere
  EXPECT_EQ(g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D::BasisX()),
            g::Ray3D::FromWkt("RAY (  0 0 0  ,  1  0  0  )"));

  // invalid: wrong geometry type
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("LINE (0 0 0, 1 0 0)"));
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("angelo"));

  // invalid: missing brackets
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("RAY 0 0 0, 1 0 0)"));
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("RAY (0 0 0, 1 0 0"));

  // invalid: missing comma separator
  EXPECT_ANY_THROW(g::Ray3D::FromWkt("RAY (0 0 0 1 0 0)"));
}

TEST_F(Ray3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "ray3d.wkt").string();
  auto r = g::Ray3D::Make(g::Point3D(1, 2, 0), g::Vector3D::BasisX());

  r.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  auto r_file = g::Ray3D::FromFile(path);
  EXPECT_EQ(r, r_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Ray3DTest, TestFromFile) {
  std::string path = (test_res_path / "ray3d" / "ray.wkt").string();

  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Ray3D::FromFile(path));

  auto r = g::Ray3D::FromFile(path);
  GEOMPP_LOG(INFO) << "from file = " << r.ToWkt();
}

TEST_F(Ray3DTest, Contains) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  ASSERT_TRUE(r.Contains(g::Point3D(3, 0, 0)));
  ASSERT_FALSE(r.Contains(g::Point3D(3, 1, 0)));
  ASSERT_FALSE(r.Contains(g::Point3D(-1, 0, 0)));
}

TEST_F(Ray3DTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  ASSERT_EQ(g::Point3D(3, 0, 0), r.ProjectOnto(g::Point3D(3, 5, 0)));
  ASSERT_EQ(g::Point3D::Zero(), r.ProjectOnto(g::Point3D(-2, 3, 0)));
}

TEST_F(Ray3DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  // on the ray: 0
  ASSERT_EQ(0.0, g::round(r.DistanceTo(g::Point3D::Zero())));
  ASSERT_EQ(0.0, g::round(r.DistanceTo(g::Point3D(3, 0, 0))));

  // ahead with perpendicular offset: distance = offset magnitude
  ASSERT_EQ(3.0, g::round(r.DistanceTo(g::Point3D(5, 3, 0))));

  // directly behind origin: distance = distance to origin
  ASSERT_EQ(2.0, g::round(r.DistanceTo(g::Point3D(-2, 0, 0))));

  // behind and offset: distance = distance to origin (not to projection on line)
  ASSERT_EQ(g::round(g::Point3D::Zero().DistanceTo(g::Point3D(-1, 3, 0))),
            g::round(r.DistanceTo(g::Point3D(-1, 3, 0))));
}

TEST_F(Ray3DTest, DistanceToLine3D) {
  geompp::DECIMAL_PRECISION = 4;
  auto ray = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());  // +X from origin

  // line that crosses the ray at origin → 0
  auto line_hit_origin = g::Line3D::Make(g::Point3D(0, -1, 0), g::Point3D(0, 1, 0));
  EXPECT_EQ(0.0, g::round(ray.DistanceTo(line_hit_origin)));

  // line parallel to the ray, offset in Y → perp distance
  auto line_parallel = g::Line3D::Make(g::Point3D(0, 5, 0), g::Point3D(1, 5, 0));
  EXPECT_EQ(5.0, g::round(ray.DistanceTo(line_parallel)));

  // line that lies on the same x-axis (overlap) → 0
  auto line_overlap = g::Line3D::Make(g::Point3D(-2, 0, 0), g::Point3D(7, 0, 0));
  EXPECT_EQ(0.0, g::round(ray.DistanceTo(line_overlap)));
  EXPECT_FALSE(ray.Distance(line_overlap).has_value());

  // line skew to ray and behind the origin: closest point on ray is its origin
  // Line through (-5, 0, 3) along Y → perpendicular foot on ray would have sc<0, clamped to origin.
  // Distance from origin (0,0,0) to line at (-5,0,3) along Y is sqrt(25+9)=sqrt(34)
  auto line_behind_skew = g::Line3D::Make(g::Point3D(-5, 0, 3), g::Point3D(-5, 1, 3));
  EXPECT_EQ(g::round(std::sqrt(34.0)), g::round(ray.DistanceTo(line_behind_skew)));
}

TEST_F(Ray3DTest, DistanceToRay3D) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());  // +X from origin

  // crossing rays meeting at (3,0,0) → 0
  auto r2 = g::Ray3D::Make(g::Point3D(3, 1, 0), g::Vector3D(0, -1, 0));
  EXPECT_EQ(0.0, g::round(r1.DistanceTo(r2)));

  // skew rays
  auto r_skew = g::Ray3D::Make(g::Point3D(0, 0, 4), g::Vector3D(0, 1, 0));
  EXPECT_EQ(4.0, g::round(r1.DistanceTo(r_skew)));
  auto dseg = r1.Distance(r_skew);
  ASSERT_TRUE(dseg.has_value());
  EXPECT_EQ(4.0, g::round(dseg->Length()));

  // overlapping (collinear) rays, same direction → 0
  auto r_overlap = g::Ray3D::Make(g::Point3D(2, 0, 0), g::Vector3D::BasisX());
  EXPECT_EQ(0.0, g::round(r1.DistanceTo(r_overlap)));
  EXPECT_FALSE(r1.Distance(r_overlap).has_value());

  // parallel distinct rays (both +X, offset in Y) → perp distance 3
  auto r_parallel = g::Ray3D::Make(g::Point3D(0, 3, 0), g::Vector3D::BasisX());
  EXPECT_EQ(3.0, g::round(r1.DistanceTo(r_parallel)));

  // back-to-back rays from same origin (opposite directions): closest distance is 0 at origin
  auto r_back = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(-1, 0, 0));
  EXPECT_EQ(0.0, g::round(r1.DistanceTo(r_back)));
}

TEST_F(Ray3DTest, DistanceToLineSegment3D) {
  geompp::DECIMAL_PRECISION = 4;
  auto ray = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());  // +X from origin

  // segment crossing the ray at (2,0,0) → 0
  auto seg_cross = g::LineSegment3D::Make(g::Point3D(2, -1, 0), g::Point3D(2, 1, 0));
  EXPECT_EQ(0.0, g::round(ray.DistanceTo(seg_cross)));

  // segment parallel to the ray, offset by 5 in Z → 5
  auto seg_parallel = g::LineSegment3D::Make(g::Point3D(0, 0, 5), g::Point3D(4, 0, 5));
  EXPECT_EQ(5.0, g::round(ray.DistanceTo(seg_parallel)));

  // segment collinear with the ray (overlap) → 0
  auto seg_overlap = g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(3, 0, 0));
  EXPECT_EQ(0.0, g::round(ray.DistanceTo(seg_overlap)));
  EXPECT_FALSE(ray.Distance(seg_overlap).has_value());

  // skew segment in Z above the ray
  auto seg_skew = g::LineSegment3D::Make(g::Point3D(3, -1, 4), g::Point3D(3, 1, 4));
  EXPECT_EQ(4.0, g::round(ray.DistanceTo(seg_skew)));
}

TEST_F(Ray3DTest, IntersectionWithRay3D) {
  geompp::DECIMAL_PRECISION = 4;
  // r1 along +X; r2 from (3,1,0) pointing -Y → meet at (3,0,0)
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto r2 = g::Ray3D::Make(g::Point3D(3, 1, 0), g::Vector3D(0, -1, 0));

  ASSERT_TRUE(r1.Intersects(r2));
  {
    auto inter = r1.Intersection(r2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_EQ(g::Point3D(3, 0, 0), *inter);
  }

  // r3 from (-3,1,0) pointing -Y — lines would cross at (-3,0,0), behind r1
  auto r3 = g::Ray3D::Make(g::Point3D(-3, 1, 0), g::Vector3D(0, -1, 0));
  ASSERT_FALSE(r1.Intersects(r3));
  ASSERT_FALSE(r1.Intersection(r3).has_value());

  // parallel rays: no intersection
  auto r4 = g::Ray3D::Make(g::Point3D(0, 1, 0), g::Vector3D::BasisX());
  ASSERT_FALSE(r1.Intersects(r4));
  ASSERT_FALSE(r1.Intersection(r4).has_value());
}

TEST_F(Ray3DTest, OverlapWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D(2, 0, 0), g::Vector3D::BasisX());
  auto x = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto y = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisY());

  EXPECT_TRUE(r.Overlaps(x));
  auto ov = r.Overlap(x);
  ASSERT_TRUE(ov.has_value());
  EXPECT_TRUE(ov->AlmostEquals(r));

  EXPECT_FALSE(r.Overlaps(y));
  EXPECT_FALSE(r.Overlap(y).has_value());
}

TEST_F(Ray3DTest, OverlapWRay_SameDir) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto r2 = g::Ray3D::Make(g::Point3D(2, 0, 0), g::Vector3D::BasisX());

  EXPECT_TRUE(r1.Overlaps(r2));
  auto ov = r1.Overlap(r2);
  ASSERT_TRUE(ov.has_value());
  ASSERT_TRUE(std::holds_alternative<g::Ray3D>(*ov));
  EXPECT_TRUE(std::get<g::Ray3D>(*ov).AlmostEquals(r2));
}

TEST_F(Ray3DTest, OverlapWRay_AntiParallel) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto r3 = g::Ray3D::Make(g::Point3D(5, 0, 0), g::Vector3D(-1, 0, 0));

  EXPECT_TRUE(r1.Overlaps(r3));
  auto ov = r1.Overlap(r3);
  ASSERT_TRUE(ov.has_value());
  ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(*ov));
  EXPECT_EQ(g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0)),
            std::get<g::LineSegment3D>(*ov));

  // single-point touch → no overlap
  auto r4 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(-1, 0, 0));
  EXPECT_FALSE(r1.Overlaps(r4));
  EXPECT_FALSE(r1.Overlap(r4).has_value());
}

TEST_F(Ray3DTest, OverlapWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D(1, 0, 0), g::Vector3D::BasisX());
  auto seg_inside = g::LineSegment3D::Make(g::Point3D(2, 0, 0), g::Point3D(4, 0, 0));
  auto seg_half   = g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(3, 0, 0));
  auto seg_before = g::LineSegment3D::Make(g::Point3D(-3, 0, 0), g::Point3D(-1, 0, 0));
  auto seg_touch  = g::LineSegment3D::Make(g::Point3D(-1, 0, 0), g::Point3D(1, 0, 0));

  EXPECT_TRUE(r.Overlaps(seg_inside));
  EXPECT_EQ(seg_inside, *r.Overlap(seg_inside));

  EXPECT_TRUE(r.Overlaps(seg_half));
  EXPECT_EQ(g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(3, 0, 0)), *r.Overlap(seg_half));

  EXPECT_FALSE(r.Overlaps(seg_before));
  EXPECT_FALSE(r.Overlap(seg_before).has_value());

  EXPECT_FALSE(r.Overlaps(seg_touch));
  EXPECT_FALSE(r.Overlap(seg_touch).has_value());
}

TEST_F(Ray3DTest, TouchWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  // origin on line, not collinear → touch
  auto r_perp = g::Ray3D::Make(g::Point3D(2, 0, 0), g::Vector3D::BasisZ());
  EXPECT_TRUE(r_perp.Touches(x));
  auto t = r_perp.Touch(x);
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(g::Point3D(2, 0, 0), *t);

  // collinear → Overlap, not Touch
  auto r_col = g::Ray3D::Make(g::Point3D(1, 0, 0), g::Vector3D::BasisX());
  EXPECT_FALSE(r_col.Touches(x));
  EXPECT_FALSE(r_col.Touch(x).has_value());

  // origin off line → no touch
  auto r_off = g::Ray3D::Make(g::Point3D(0, 1, 0), g::Vector3D::BasisX());
  EXPECT_FALSE(r_off.Touches(x));
  EXPECT_FALSE(r_off.Touch(x).has_value());
}

TEST_F(Ray3DTest, TouchWRay_NonParallel) {
  geompp::DECIMAL_PRECISION = 4;
  // r1 goes +x; r2 goes +z, same origin → touch
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto r2 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisZ());
  EXPECT_TRUE(r1.Touches(r2));
  auto t1 = r1.Touch(r2);
  ASSERT_TRUE(t1.has_value());
  EXPECT_EQ(g::Point3D::Zero(), *t1);

  // r3 origin at (3,0,0) going +z — r3.Origin on r1
  auto r3 = g::Ray3D::Make(g::Point3D(3, 0, 0), g::Vector3D::BasisZ());
  EXPECT_TRUE(r1.Touches(r3));
  auto t2 = r1.Touch(r3);
  ASSERT_TRUE(t2.has_value());
  EXPECT_EQ(g::Point3D(3, 0, 0), *t2);

  // skew, no origin contact → no touch
  auto r4 = g::Ray3D::Make(g::Point3D(2, 1, 0), g::Vector3D::BasisZ());
  EXPECT_FALSE(r1.Touches(r4));
  EXPECT_FALSE(r1.Touch(r4).has_value());
}

TEST_F(Ray3DTest, TouchWRay_AntiParallel) {
  geompp::DECIMAL_PRECISION = 4;
  auto r1 = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  // same origin, anti-parallel → touch at origin
  auto r_same = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D(-1, 0, 0));
  EXPECT_TRUE(r1.Touches(r_same));
  auto t = r1.Touch(r_same);
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(g::Point3D::Zero(), *t);

  // anti-parallel, overlapping region → no touch
  auto r_over = g::Ray3D::Make(g::Point3D(3, 0, 0), g::Vector3D(-1, 0, 0));
  EXPECT_FALSE(r1.Touches(r_over));
  EXPECT_FALSE(r1.Touch(r_over).has_value());
}

TEST_F(Ray3DTest, TouchWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto r = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  // seg First() on ray, Last() off ray → touch at First
  auto seg_f = g::LineSegment3D::Make(g::Point3D(3, 0, 0), g::Point3D(3, 0, 2));
  EXPECT_TRUE(r.Touches(seg_f));
  auto t1 = r.Touch(seg_f);
  ASSERT_TRUE(t1.has_value());
  EXPECT_EQ(g::Point3D(3, 0, 0), *t1);

  // both endpoints on ray → Overlap, no touch
  auto seg_both = g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(4, 0, 0));
  EXPECT_FALSE(r.Touches(seg_both));
  EXPECT_FALSE(r.Touch(seg_both).has_value());

  // no contact → no touch
  auto seg_none = g::LineSegment3D::Make(g::Point3D(1, 1, 0), g::Point3D(3, 1, 0));
  EXPECT_FALSE(r.Touches(seg_none));
  EXPECT_FALSE(r.Touch(seg_none).has_value());
}

}  // namespace geompp_tests
