#include "line_segment2d.hpp"

#include "line2d.hpp"
#include "point2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <limits>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class LineSegment2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(LineSegment2DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto s1 = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(3, 0));
  auto s2 = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(3, 0));
  auto s3 = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(0, 3));

  // forward match
  ASSERT_TRUE(s1.AlmostEquals(s2));
  ASSERT_EQ(s1, s2);
  ASSERT_EQ(s1, s1);

  // different segment: not equal
  ASSERT_FALSE(s1.AlmostEquals(s3));
  ASSERT_NE(s1, s3);

  // reversed segment — should be equal
  auto s4 = g::LineSegment2D::Make(g::Point2D(3, 0), g::Point2D::Zero());
  ASSERT_TRUE(s1.AlmostEquals(s4));
  ASSERT_EQ(s1, s4);
}

TEST_F(LineSegment2DTest, Constructor) {
  auto s1 = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0));

  ASSERT_EQ(g::Point2D::Zero(), s1.First());
  ASSERT_EQ(g::Point2D(1, 0), s1.Last());

  EXPECT_ANY_THROW(g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D::Zero()));  // cannot make a segment in 1 sole point
}

TEST_F(LineSegment2DTest, Contains) {
  auto s1 = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0));
  ASSERT_TRUE(s1.Contains(g::Point2D::Zero()));
  ASSERT_TRUE(s1.Contains(g::Point2D(0.5, 0)));
  ASSERT_TRUE(s1.Contains(g::Point2D(1, 0)));

  ASSERT_FALSE(s1.Contains(g::Point2D(1.1, 0)));
  ASSERT_FALSE(s1.Contains(g::Point2D(1, 1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(1, -1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-1, 0)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-1, 1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-1, -1)));
  ASSERT_FALSE(s1.Contains(g::Point2D(-0.1, 0)));
}

TEST_F(LineSegment2DTest, Location) {
  auto s1 = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0));

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

TEST_F(LineSegment2DTest, Interpolate) {
  geompp::DECIMAL_PRECISION = 4;
  auto seg = g::LineSegment2D::FromWkt("LINESTRING (0 0, 3 0)");

  // on segment
  ASSERT_EQ(seg.First(), seg.Interpolate(0));

  ASSERT_EQ(seg.Last(), seg.Interpolate(1));

  // on segment
  ASSERT_EQ(0.223, g::round(seg.Location(seg.Interpolate(0.223)), 3));

  // on line
  ASSERT_EQ(1.0, g::round(seg.Location(seg.Interpolate(1.2)), 1));

  ASSERT_EQ(0.0, g::round(seg.Location(seg.Interpolate(-0.2)), 1));
}

TEST_F(LineSegment2DTest, Intersection) {
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

TEST_F(LineSegment2DTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  auto s1 = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(2, 1));   // intersects x, y
  auto s2 = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1));     // intersects y
  auto s3 = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(-1, -1));  // intersects x

  auto x = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisX());
  auto y = g::Line2D::Make(g::Point2D::Zero(), g::Vector2D::BasisY());

  ASSERT_TRUE(s1.Intersects(x));
  {
    auto inter = s1.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(1, 0), *inter);
  }

  ASSERT_TRUE(s1.Intersects(y));
  {
    auto inter = s1.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(0, -1), *inter);
  }

  ASSERT_TRUE(s2.Intersects(y));
  {
    auto inter = s2.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(0, 1), *inter);
  }
  ASSERT_FALSE(s2.Intersects(x));

  ASSERT_TRUE(s3.Intersects(x));
  {
    auto inter = s3.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(-1, 0), *inter);
  }
  ASSERT_FALSE(s3.Intersects(y));
}

TEST_F(LineSegment2DTest, IntersectionWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto s1 = g::LineSegment2D::Make(g::Point2D(-1, -2), g::Point2D(2, 1));   // intersects r1, r2
  auto s2 = g::LineSegment2D::Make(g::Point2D(1, 1), g::Point2D(0, 1));     // intersects r1
  auto s3 = g::LineSegment2D::Make(g::Point2D(-1, 0), g::Point2D(-1, -1));  // intersects r2

  auto r1 = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D::BasisY());
  auto r2 = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D::BasisX());
  auto r1_rev = g::Ray2D::Make(g::Point2D(0.5, -2), g::Vector2D(0, -1));   // no intersections
  auto r2_rev = g::Ray2D::Make(g::Point2D(-2, -0.5), g::Vector2D(-1, 0));  // no intersections

  ASSERT_TRUE(s1.Intersects(r1));
  {
    auto inter = s1.Intersection(r1);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(0.5, -0.5), *inter);
  }
  ASSERT_TRUE(s1.Intersects(r2));
  {
    auto inter = s1.Intersection(r2);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(0.5, -0.5), *inter);
  }

  ASSERT_TRUE(s2.Intersects(r1));
  {
    auto inter = s2.Intersection(r1);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(0.5, 1), *inter);
  }
  ASSERT_FALSE(s2.Intersects(r2));

  ASSERT_TRUE(s3.Intersects(r2));
  {
    auto inter = s3.Intersection(r2);
    ASSERT_TRUE(inter.has_value());
    EXPECT_EQ(g::Point2D(-1, -0.5), *inter);
  }
  ASSERT_FALSE(s3.Intersects(r1));

  ASSERT_FALSE(s1.Intersects(r1_rev));
  ASSERT_FALSE(s1.Intersects(r2_rev));
  ASSERT_FALSE(s2.Intersects(r1_rev));
  ASSERT_FALSE(s2.Intersects(r2_rev));
  ASSERT_FALSE(s3.Intersects(r1_rev));
  ASSERT_FALSE(s3.Intersects(r2_rev));
}

TEST_F(LineSegment2DTest, Wkt) {
  ASSERT_EQ("LINESTRING (0 0, 1 1)", g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 1)).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("LINESTRING (56491.62 -795.97, -9137.37 10.36)",
            g::LineSegment2D::Make(g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678)).ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)),
            g::LineSegment2D::FromWkt("LINESTRING (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(-7.5, -60.7), g::Point2D::Zero()),
            g::LineSegment2D::FromWkt("  linestring( -7.5    -60.7, 0   0)"));
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(0.645, -1.689741), g::Point2D(1, 0)),
            g::LineSegment2D::FromWkt("LinESTRing   ( 0.645  -1.689741  , 1 0  )"));
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 1)),
            g::LineSegment2D::FromWkt("LINESTRING (0 0,1 1)"));
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 1)),
            g::LineSegment2D::FromWkt("LINESTRING (  0 0  ,  1  1  )"));

  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestrin ( -7.5 -60.7, 0 0)"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("line string ( -7.5 -60.7, 0 0)"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring -7.5 -64.4, 0 0)"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring (-7.5 -64.4, 0 0"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring (-7.5 -64.4, 0 "));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring (-7.5 -64.4, "));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( -7.5 )"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( )"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( -7.5 -64.4 15.5)"));
  EXPECT_ANY_THROW(g::LineSegment2D::FromWkt("linestring ( -7.5 -64.4 15.5, 0 0 0)"));
}

TEST_F(LineSegment2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "line_segment.wkt").string();
  auto s = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0));

  s.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::LineSegment2D s_file = g::LineSegment2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(s, s_file);

  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(LineSegment2DTest, TestFromFile) {
  std::string path = (test_res_path / "line_segment2d" / "line_segment.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::LineSegment2D::FromFile(path));

  auto p = g::LineSegment2D::FromFile(path);

  GEOMPP_LOG(INFO) << "form file = " << p.ToWkt();
}

TEST_F(LineSegment2DTest, ProjectOnto) {
  geompp::DECIMAL_PRECISION = 4;
  auto seg = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(4, 0));

  ASSERT_EQ(g::Point2D(2, 0), seg.ProjectOnto(g::Point2D(2, 3)));
  ASSERT_EQ(g::Point2D::Zero(), seg.ProjectOnto(g::Point2D(-1, 2)));
  ASSERT_EQ(g::Point2D(4, 0), seg.ProjectOnto(g::Point2D(5, 2)));
}

TEST_F(LineSegment2DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto seg = g::LineSegment2D::FromWkt("LINESTRING (0 0, 3 0)");

  // on segment
  auto p1 = g::Point2D::Zero();
  EXPECT_EQ(0, g::round(seg.DistanceTo(p1)));

  auto p2 = g::Point2D(3, 0);
  EXPECT_EQ(0, g::round(seg.DistanceTo(p2)));

  // on line
  auto p3 = g::Point2D(-20, 0);
  EXPECT_EQ(20, g::round(seg.DistanceTo(p3)));

  auto p4 = g::Point2D(12, 0);
  EXPECT_EQ(9, g::round(seg.DistanceTo(p4)));

  // Q1
  auto p5 = g::Point2D(1.5, 12);
  EXPECT_EQ(12, g::round(seg.DistanceTo(p5)));

  // Q2
  auto p6 = g::Point2D(-5, 10);
  EXPECT_EQ(g::round(p6.DistanceTo(seg.First())), g::round(seg.DistanceTo(p6)));

  // Q3
  auto p7 = g::Point2D(-2, -4);
  EXPECT_EQ(g::round(p7.DistanceTo(seg.First())), g::round(seg.DistanceTo(p7)));

  // Q4
  auto p8 = g::Point2D(3, -7);
  EXPECT_EQ(7, g::round(seg.DistanceTo(p8)));
}

TEST_F(LineSegment2DTest, IsLeft) {
  // IsLeft tests the sign of the cross product (P1-P0) × p, where p is the
  // absolute position vector of the query point (from the world origin).
  // Formula: (P1.x - P0.x)*p.y - (P1.y - P0.y)*p.x > 0

  // rightward segment at origin: formula = p.y
  {
    auto seg = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0));
    ASSERT_TRUE(seg.IsLeft(g::Point2D(0, 1)));    // above → left
    ASSERT_FALSE(seg.IsLeft(g::Point2D(0, -1)));  // below → right
    ASSERT_FALSE(seg.IsLeft(g::Point2D(1, 0)));   // on the direction line → not left
  }

  // upward segment at origin: formula = -p.x
  {
    auto seg = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(0, 1));
    ASSERT_TRUE(seg.IsLeft(g::Point2D(-1, 0)));   // left of upward → left
    ASSERT_FALSE(seg.IsLeft(g::Point2D(1, 0)));   // right of upward → right
    ASSERT_FALSE(seg.IsLeft(g::Point2D(0, 1)));   // collinear from origin → not left
  }

  // diagonal segment (0,0)→(1,1): formula = p.y - p.x
  {
    auto seg = g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 1));
    ASSERT_TRUE(seg.IsLeft(g::Point2D(0, 1)));    // above y=x → left
    ASSERT_FALSE(seg.IsLeft(g::Point2D(1, 0)));   // below y=x → right
    ASSERT_FALSE(seg.IsLeft(g::Point2D(1, 1)));   // on the line → not left
  }

  // reversed rightward (1,0)→(0,0): formula = -p.y
  {
    auto seg = g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D::Zero());
    ASSERT_TRUE(seg.IsLeft(g::Point2D(0, -1)));   // below → left when going leftward
    ASSERT_FALSE(seg.IsLeft(g::Point2D(0, 1)));   // above → right when going leftward
  }
}

TEST_F(LineSegment2DTest, FreeIntersect_Crossing) {
  auto a = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 2));
  auto b = g::LineSegment2D::Make(g::Point2D(0, 2), g::Point2D(2, 0));  // crosses a at (1,1)
  EXPECT_TRUE(g::intersect(a, b));
  EXPECT_TRUE(g::intersect(b, a));  // symmetric
}

TEST_F(LineSegment2DTest, FreeIntersect_TouchingAtEndpoint) {
  auto a = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0));
  auto b = g::LineSegment2D::Make(g::Point2D(1, 0), g::Point2D(1, 2));  // T-junction: b starts on a
  EXPECT_TRUE(g::intersect(a, b));
}

TEST_F(LineSegment2DTest, FreeIntersect_ParallelDisjoint) {
  auto a = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(2, 0));
  auto b = g::LineSegment2D::Make(g::Point2D(0, 1), g::Point2D(2, 1));  // parallel, never meets
  EXPECT_FALSE(g::intersect(a, b));
}

TEST_F(LineSegment2DTest, FreeIntersect_SeparatedDoesNotCross) {
  auto a = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  auto b = g::LineSegment2D::Make(g::Point2D(2, -1), g::Point2D(2, 1));  // vertical at x=2, a ends at x=1
  EXPECT_FALSE(g::intersect(a, b));
}

}  // namespace geompp_tests
