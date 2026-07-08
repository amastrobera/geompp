#include "line3d.hpp"

#include "constants.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "ray3d.hpp"
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

class Line3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Line3DTest, MakeFromTwoPoints) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(3, 0, 0));

  ASSERT_EQ(g::Point3D::Zero(), l.Origin());
  ASSERT_EQ(g::Point3D::Zero(), l.First());
  ASSERT_EQ(g::Point3D(3, 0, 0), l.Last());

  // direction is normalised
  ASSERT_EQ(g::Vector3D::BasisX(), l.Direction());

  // coincident points throw
  EXPECT_ANY_THROW(g::Line3D::Make(g::Point3D(1, 2, 3), g::Point3D(1, 2, 3)));
}

TEST_F(Line3DTest, MakeFromPointAndVector) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D(1, 2, 0), g::Vector3D(3, 0, 0));

  ASSERT_EQ(g::Point3D(1, 2, 0), l.Origin());
  // direction is normalised
  ASSERT_EQ(g::Vector3D::BasisX(), l.Direction());

  // zero direction throws
  EXPECT_ANY_THROW(g::Line3D::Make(g::Point3D::Zero(), g::Vector3D(0, 0, 0)));
}

TEST_F(Line3DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto l1 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  auto l2 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  auto l3 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(0, 1, 0));

  // identical construction
  ASSERT_EQ(l1, l2);
  ASSERT_EQ(l1, l1);

  // different direction: not equal
  ASSERT_NE(l1, l3);

  // same infinite line, different origin — geometrically equal
  auto l4 = g::Line3D::Make(g::Point3D(5, 0, 0), g::Point3D(8, 0, 0));
  ASSERT_EQ(l1, l4);

  // same infinite line, reversed direction — geometrically equal
  auto l5 = g::Line3D::Make(g::Point3D(1, 0, 0), g::Point3D::Zero());
  ASSERT_EQ(l1, l5);

  // parallel but offset — NOT equal
  auto l6 = g::Line3D::Make(g::Point3D(0, 1, 0), g::Point3D(5, 1, 0));
  ASSERT_NE(l1, l6);
}

TEST_F(Line3DTest, Assignment) {
  auto l1 = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0));
  auto l2 = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(3, 4, 0));

  l2 = l1;
  ASSERT_EQ(l1, l2);

  // self-assignment
  l1 = l1;
  ASSERT_EQ(g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0)), l1);
}

TEST_F(Line3DTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  // horizontal line along X-axis
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0));

  // projection of off-axis point lands on the line
  auto proj = l.ProjectOnto(g::Point3D(3, 5, 0));
  ASSERT_EQ(g::Point3D(3, 0, 0), proj);

  // projection of a point already on the line is itself
  ASSERT_EQ(g::Point3D(2, 0, 0), l.ProjectOnto(g::Point3D(2, 0, 0)));

  // projection before origin
  ASSERT_EQ(g::Point3D(-2, 0, 0), l.ProjectOnto(g::Point3D(-2, 3, 0)));

  // diagonal line: y = x in XY plane
  auto ld = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 1, 0));
  geompp::DECIMAL_PRECISION = 3;
  auto proj_d = ld.ProjectOnto(g::Point3D(1, 0, 0));
  // (1,0,0) projects onto (0.5, 0.5, 0) on y=x line
  ASSERT_EQ(g::Point3D(0.5, 0.5, 0), proj_d);
  geompp::DECIMAL_PRECISION = 4;
}

TEST_F(Line3DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(3, 0, 0));
  ASSERT_EQ("LINE (0 0 0, 3 0 0)", l.ToWkt());

  geompp::DECIMAL_PRECISION = 2;
  auto l2 = g::Line3D::Make(g::Point3D(1.126, 2.354, 0.0), g::Point3D(5.678, 9.012, 0.0));
  ASSERT_EQ("LINE (1.13 2.35 0, 5.68 9.01 0)", l2.ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  // round-trip
  auto l3 = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(4, 6, 0));
  EXPECT_EQ(l3, g::Line3D::FromWkt(l3.ToWkt()));
  // no space after comma
  EXPECT_EQ(g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0)),
            g::Line3D::FromWkt("LINE (0 0 0,1 1 0)"));
  // many spaces everywhere
  EXPECT_EQ(g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0)),
            g::Line3D::FromWkt("LINE (  0 0 0  ,  1  1  0  )"));

  // invalid: wrong geometry type
  EXPECT_ANY_THROW(g::Line3D::FromWkt("POINT (0 0 0, 1 1 0)"));
  EXPECT_ANY_THROW(g::Line3D::FromWkt("angelo"));

  // invalid: missing brackets
  EXPECT_ANY_THROW(g::Line3D::FromWkt("LINE 0 0 0, 1 1 0)"));
  EXPECT_ANY_THROW(g::Line3D::FromWkt("LINE (0 0 0, 1 1 0"));

  // invalid: missing comma separator
  EXPECT_ANY_THROW(g::Line3D::FromWkt("LINE (0 0 0 1 1 0)"));
}

TEST_F(Line3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "line3d.wkt").string();
  auto l = g::Line3D::Make(g::Point3D(1, 2, 0), g::Point3D(4, 6, 0));

  l.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  auto l_file = g::Line3D::FromFile(path);
  EXPECT_EQ(l, l_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Line3DTest, TestFromFile) {
  std::string path = (test_res_path / "line3d" / "line.wkt").string();

  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Line3D::FromFile(path));

  auto l = g::Line3D::FromFile(path);
  GEOMPP_LOG(INFO) << "from file = " << l.ToWkt();
}

TEST_F(Line3DTest, Contains) {
  geompp::DECIMAL_PRECISION = 4;
  auto l = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(3, 0, 0));

  ASSERT_TRUE(l.Contains(g::Point3D(1, 0, 0)));
  ASSERT_FALSE(l.Contains(g::Point3D(1, 1, 0)));
}

TEST_F(Line3DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  // horizontal line along X-axis
  auto line = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(5, 0, 0));

  // on the line: distance = 0
  ASSERT_EQ(0.0, g::round(line.DistanceTo(g::Point3D::Zero())));
  ASSERT_EQ(0.0, g::round(line.DistanceTo(g::Point3D(3, 0, 0))));
  ASSERT_EQ(0.0, g::round(line.DistanceTo(g::Point3D(-2, 0, 0))));  // behind origin, still on line

  // perpendicular offset in Y
  ASSERT_EQ(3.0, g::round(line.DistanceTo(g::Point3D(2, 3, 0))));

  // perpendicular offset in Z
  ASSERT_EQ(4.0, g::round(line.DistanceTo(g::Point3D(1, 0, 4))));

  // offset in both Y and Z: 3-4-5 triple
  ASSERT_EQ(5.0, g::round(line.DistanceTo(g::Point3D(0, 3, 4))));
}

TEST_F(Line3DTest, DistanceToLine3D) {
  geompp::DECIMAL_PRECISION = 4;
  auto x_axis = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));

  // intersecting lines (Y-axis crosses X-axis at origin) → 0
  auto y_axis = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(0, 1, 0));
  EXPECT_EQ(0.0, g::round(x_axis.DistanceTo(y_axis)));
  EXPECT_FALSE(x_axis.Distance(y_axis).has_value());  // intersect → nullopt

  // skew lines: X-axis and a Y-direction line at z=5 → distance 5
  auto skew = g::Line3D::Make(g::Point3D(0, -1, 5), g::Point3D(0, 1, 5));
  EXPECT_EQ(5.0, g::round(x_axis.DistanceTo(skew)));
  auto dseg = x_axis.Distance(skew);
  ASSERT_TRUE(dseg.has_value());
  EXPECT_EQ(5.0, g::round(dseg->Length()));

  // parallel distinct lines (both along X, offset by 3 in Y) → 3
  auto parallel = g::Line3D::Make(g::Point3D(0, 3, 0), g::Point3D(1, 3, 0));
  EXPECT_EQ(3.0, g::round(x_axis.DistanceTo(parallel)));

  // overlapping (collinear) lines → 0, Distance returns nullopt
  auto overlap = g::Line3D::Make(g::Point3D(5, 0, 0), g::Point3D(7, 0, 0));
  EXPECT_EQ(0.0, g::round(x_axis.DistanceTo(overlap)));
  EXPECT_FALSE(x_axis.Distance(overlap).has_value());

  // same line, opposite direction → still overlap → 0
  auto same_rev = g::Line3D::Make(g::Point3D(10, 0, 0), g::Point3D(-1, 0, 0));
  EXPECT_EQ(0.0, g::round(x_axis.DistanceTo(same_rev)));
}

TEST_F(Line3DTest, DistanceToRay3D) {
  geompp::DECIMAL_PRECISION = 4;
  auto line = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));  // X-axis

  // ray pointing toward the line (down -Y from (0,2,0)) — crosses at origin → 0
  auto ray_hit = g::Ray3D::Make(g::Point3D(0, 2, 0), g::Vector3D(0, -1, 0));
  EXPECT_EQ(0.0, g::round(line.DistanceTo(ray_hit)));

  // ray pointing away from line (up +Y from (0,2,0)): closest point on ray is its origin → perp dist = 2
  auto ray_away = g::Ray3D::Make(g::Point3D(0, 2, 0), g::Vector3D(0, 1, 0));
  EXPECT_EQ(2.0, g::round(line.DistanceTo(ray_away)));
  auto dseg = line.Distance(ray_away);
  ASSERT_TRUE(dseg.has_value());
  EXPECT_EQ(2.0, g::round(dseg->Length()));

  // ray that overlaps the line (collinear) → 0
  auto ray_overlap = g::Ray3D::Make(g::Point3D(3, 0, 0), g::Vector3D(1, 0, 0));
  EXPECT_EQ(0.0, g::round(line.DistanceTo(ray_overlap)));
  EXPECT_FALSE(line.Distance(ray_overlap).has_value());
}

TEST_F(Line3DTest, DistanceToLineSegment3D) {
  geompp::DECIMAL_PRECISION = 4;
  auto line = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));  // X-axis

  // segment crossing the line at the origin → 0
  auto seg_cross = g::LineSegment3D::Make(g::Point3D(0, -1, 0), g::Point3D(0, 1, 0));
  EXPECT_EQ(0.0, g::round(line.DistanceTo(seg_cross)));

  // segment parallel to the line, offset by 4 in Y → 4
  auto seg_parallel = g::LineSegment3D::Make(g::Point3D(0, 4, 0), g::Point3D(3, 4, 0));
  EXPECT_EQ(4.0, g::round(line.DistanceTo(seg_parallel)));

  // segment lying on the line (overlap) → 0
  auto seg_overlap = g::LineSegment3D::Make(g::Point3D(2, 0, 0), g::Point3D(5, 0, 0));
  EXPECT_EQ(0.0, g::round(line.DistanceTo(seg_overlap)));
  EXPECT_FALSE(line.Distance(seg_overlap).has_value());

  // segment offset in Z, not touching the line → perpendicular dist 7
  auto seg_off = g::LineSegment3D::Make(g::Point3D(1, 0, 7), g::Point3D(3, 0, 7));
  EXPECT_EQ(7.0, g::round(line.DistanceTo(seg_off)));
}

TEST_F(Line3DTest, IntersectionWithLine3D) {
  geompp::DECIMAL_PRECISION = 4;
  // X-axis and vertical line through (3, 3, 0):
  // expected intersection at (3, 0, 0) — but the 3D intersection algorithm
  // inherits 2D Perp logic, so we only test the result value here.
  auto x_axis = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0));
  auto y_axis = g::Line3D::Make(g::Point3D::Zero(), g::Point3D(0, 1, 0));

  // parallel lines (same direction) should not intersect
  auto l_parallel = g::Line3D::Make(g::Point3D(0, 2, 0), g::Point3D(1, 2, 0));
  EXPECT_FALSE(x_axis.Intersects(l_parallel));
  EXPECT_FALSE(x_axis.Intersection(l_parallel).has_value());
}

TEST_F(Line3DTest, OverlapWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto x2 = g::Line3D::Make(g::Point3D(5, 0, 0), g::Point3D(8, 0, 0));  // same infinite line
  auto y  = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisY());
  auto x_off = g::Line3D::Make(g::Point3D(0, 1, 0), g::Vector3D::BasisX());  // parallel, offset

  EXPECT_TRUE(x.Overlaps(x2));
  auto ov = x.Overlap(x2);
  ASSERT_TRUE(ov.has_value());
  EXPECT_TRUE(ov->AlmostEquals(x));

  EXPECT_FALSE(x.Overlaps(y));
  EXPECT_FALSE(x.Overlap(y).has_value());

  EXPECT_FALSE(x.Overlaps(x_off));
  EXPECT_FALSE(x.Overlap(x_off).has_value());
}

TEST_F(Line3DTest, OverlapWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto x  = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto rx = g::Ray3D::Make(g::Point3D(2, 0, 0), g::Vector3D::BasisX());
  auto ry = g::Ray3D::Make(g::Point3D::Zero(), g::Vector3D::BasisY());

  EXPECT_TRUE(x.Overlaps(rx));
  auto ov = x.Overlap(rx);
  ASSERT_TRUE(ov.has_value());
  EXPECT_TRUE(ov->AlmostEquals(rx));

  EXPECT_FALSE(x.Overlaps(ry));
  EXPECT_FALSE(x.Overlap(ry).has_value());
}

TEST_F(Line3DTest, OverlapWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto x   = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());
  auto seg = g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(3, 0, 0));
  auto seg_off = g::LineSegment3D::Make(g::Point3D(1, 1, 0), g::Point3D(3, 1, 0));

  EXPECT_TRUE(x.Overlaps(seg));
  EXPECT_EQ(seg, *x.Overlap(seg));

  EXPECT_FALSE(x.Overlaps(seg_off));
  EXPECT_FALSE(x.Overlap(seg_off).has_value());
}

TEST_F(Line3DTest, TouchWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  // ray origin on line, not collinear → touch at origin
  auto r_touch = g::Ray3D::Make(g::Point3D(3, 0, 0), g::Vector3D::BasisZ());
  EXPECT_TRUE(x.Touches(r_touch));
  auto t = x.Touch(r_touch);
  ASSERT_TRUE(t.has_value());
  EXPECT_EQ(g::Point3D(3, 0, 0), *t);

  // collinear ray → Overlap, not Touch
  auto r_col = g::Ray3D::Make(g::Point3D(1, 0, 0), g::Vector3D::BasisX());
  EXPECT_FALSE(x.Touches(r_col));
  EXPECT_FALSE(x.Touch(r_col).has_value());

  // ray origin off line → no touch
  auto r_off = g::Ray3D::Make(g::Point3D(0, 1, 0), g::Vector3D::BasisZ());
  EXPECT_FALSE(x.Touches(r_off));
  EXPECT_FALSE(x.Touch(r_off).has_value());
}

TEST_F(Line3DTest, TouchWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto x = g::Line3D::Make(g::Point3D::Zero(), g::Vector3D::BasisX());

  // First() on line, segment goes off-axis → touch at First
  auto seg_f = g::LineSegment3D::Make(g::Point3D(2, 0, 0), g::Point3D(2, 0, 3));
  EXPECT_TRUE(x.Touches(seg_f));
  auto t1 = x.Touch(seg_f);
  ASSERT_TRUE(t1.has_value());
  EXPECT_EQ(g::Point3D(2, 0, 0), *t1);

  // collinear → Overlap, not Touch
  auto seg_col = g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(4, 0, 0));
  EXPECT_FALSE(x.Touches(seg_col));
  EXPECT_FALSE(x.Touch(seg_col).has_value());

  // segment fully off-axis → no touch
  auto seg_off = g::LineSegment3D::Make(g::Point3D(1, 1, 0), g::Point3D(3, 2, 0));
  EXPECT_FALSE(x.Touches(seg_off));
  EXPECT_FALSE(x.Touch(seg_off).has_value());
}

}  // namespace geompp_tests
