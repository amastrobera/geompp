#include "polyline3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <vector>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Polyline3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Polyline3DTest, Constructor) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly =
      g::Polyline3D::Make({g::Point3D(-2, -5, 0), g::Point3D(-2, -3, 0), g::Point3D(2, -3, 0), g::Point3D(2, 2, 0)});
  ASSERT_EQ(4, poly.Size());
  ASSERT_EQ(11, poly.Length());

  // too few unique points
  EXPECT_ANY_THROW(g::Polyline3D::Make({}));
  EXPECT_ANY_THROW(g::Polyline3D::Make({g::Point3D::Zero()}));
  EXPECT_ANY_THROW(g::Polyline3D::Make({g::Point3D::Zero(), g::Point3D::Zero()}));

  // duplicate point removed, leaving 2 unique → ok
  EXPECT_NO_THROW(g::Polyline3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 0, 0)}));
  EXPECT_EQ(2, g::Polyline3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(1, 0, 0)}).Size());

  // collinear middle point removed
  EXPECT_EQ(2, g::Polyline3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(3, 0, 0)}).Size());
}

TEST_F(Polyline3DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 4;
  auto p1 = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)});
  auto p2 = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)});
  auto p3 = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 2, 0)});

  ASSERT_TRUE(p1.AlmostEquals(p2));
  ASSERT_FALSE(p1.AlmostEquals(p3));
  ASSERT_EQ(p1, p2);
  ASSERT_NE(p1, p3);

  // different knot counts → not equal
  auto p4 = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  ASSERT_FALSE(p1.AlmostEquals(p4));
}

TEST_F(Polyline3DTest, ToSegments) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});

  auto segs = poly.ToSegments();
  ASSERT_EQ(3, segs.size());

  ASSERT_EQ(g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0)), segs[0]);
  ASSERT_EQ(g::LineSegment3D::Make(g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)), segs[1]);
  ASSERT_EQ(g::LineSegment3D::Make(g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)), segs[2]);

  // two-knot → one segment
  auto p2 = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(3, 4, 0)});
  ASSERT_EQ(1, p2.ToSegments().size());
}

TEST_F(Polyline3DTest, Contains) {
  geompp::DECIMAL_PRECISION = 4;
  std::vector<g::Point3D> pts{g::Point3D(-2, -5, 0), g::Point3D(-2, -3, 0), g::Point3D(2, -3, 0), g::Point3D(2, 2, 0)};
  auto poly = g::Polyline3D::Make(pts);

  // knots
  for (auto const& p : pts) {
    ASSERT_TRUE(poly.Contains(p));
  }

  // midpoints of each segment
  for (size_t i = 0; i < pts.size() - 1; ++i) {
    auto mid = g::Point3D((pts[i].x() + pts[i + 1].x()) / 2.0, (pts[i].y() + pts[i + 1].y()) / 2.0,
                          (pts[i].z() + pts[i + 1].z()) / 2.0);
    ASSERT_TRUE(poly.Contains(mid));
  }

  // off-polyline point
  ASSERT_FALSE(poly.Contains(g::Point3D(0, 0, 0)));
}

TEST_F(Polyline3DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  std::vector<g::Point3D> pts{g::Point3D(-2, -5, 0), g::Point3D(-2, -3, 0), g::Point3D(2, -3, 0), g::Point3D(2, 2, 0)};
  auto poly = g::Polyline3D::Make(pts);

  // on knots → 0
  for (auto const& p : pts) ASSERT_EQ(0, g::round(poly.DistanceTo(p)));

  // midpoints → 0
  for (size_t i = 0; i < pts.size() - 1; ++i) {
    auto mid = g::Point3D((pts[i].x() + pts[i + 1].x()) / 2.0, (pts[i].y() + pts[i + 1].y()) / 2.0,
                          (pts[i].z() + pts[i + 1].z()) / 2.0);
    ASSERT_EQ(0, g::round(poly.DistanceTo(mid)));
  }

  // 1 unit off a horizontal segment
  ASSERT_EQ(1, g::round(poly.DistanceTo(g::Point3D(-3, -4, 0))));
  ASSERT_EQ(1, g::round(poly.DistanceTo(g::Point3D(0, -2, 0))));
}

TEST_F(Polyline3DTest, Interpolate) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly = g::Polyline3D::FromWkt("LINESTRING (0 0 0, 3 0 0)");

  ASSERT_EQ(g::Point3D(0, 0, 0), poly.Interpolate(0));
  ASSERT_EQ(g::Point3D(3, 0, 0), poly.Interpolate(1));

  // clamped
  ASSERT_EQ(g::Point3D(0, 0, 0), poly.Interpolate(-0.5));
  ASSERT_EQ(g::Point3D(3, 0, 0), poly.Interpolate(1.5));
}

TEST_F(Polyline3DTest, IntersectionWLine) {
  geompp::DECIMAL_PRECISION = 4;
  // L-shaped polyline in XY plane: crosses x-axis twice, y-axis once
  auto poly1 = g::Polyline3D::FromWkt("LINESTRING (-1 2 0, -1 -2 0, 1 -2 0, 1 2 0)");
  auto poly2 = g::Polyline3D::FromWkt("LINESTRING (-1 2 0, -0.5 2 0, 1 2 0, 2 1 0)");
  auto poly3 = g::Polyline3D::FromWkt("LINESTRING (-1 2 0, -1 -1 0, -2 -2 0, -1 -3 0)");

  auto x = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto y = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(0, 1, 0));

  ASSERT_TRUE(poly1.Intersects(x));
  {
    auto inter = poly1.Intersection(x);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline3D::MultiPoint>(*inter));
    auto pts = std::get<g::Polyline3D::MultiPoint>(*inter);
    ASSERT_EQ(g::Point3D(-1, 0, 0), pts[0]);
    ASSERT_EQ(g::Point3D(1, 0, 0), pts[1]);
  }

  ASSERT_TRUE(poly1.Intersects(y));
  {
    auto inter = poly1.Intersection(y);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(0, -2, 0), std::get<g::Point3D>(*inter));
  }

  ASSERT_FALSE(poly2.Intersects(x));

  ASSERT_TRUE(poly3.Intersects(x));
  ASSERT_FALSE(poly3.Intersects(y));
}

TEST_F(Polyline3DTest, IntersectionWRay) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline3D::FromWkt("LINESTRING (-1 2 0, -1 -2 0, 1 -2 0, 1 2 0)");

  auto x_neg = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(-1, 0, 0));
  auto x_pos = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(1, 0, 0));
  auto y_neg = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(0, -1, 0));
  auto y_pos = g::Ray3D::Make(g::Point3D(0, 0, 0), g::Vector3D(0, 1, 0));

  ASSERT_TRUE(poly1.Intersects(x_neg));
  {
    auto inter = poly1.Intersection(x_neg);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(-1, 0, 0), std::get<g::Point3D>(*inter));
  }

  ASSERT_TRUE(poly1.Intersects(x_pos));
  {
    auto inter = poly1.Intersection(x_pos);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(1, 0, 0), std::get<g::Point3D>(*inter));
  }

  ASSERT_TRUE(poly1.Intersects(y_neg));
  ASSERT_FALSE(poly1.Intersects(y_pos));
}

TEST_F(Polyline3DTest, IntersectionWSegment) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline3D::FromWkt("LINESTRING (-1 2 0, -1 -2 0, 1 -2 0, 1 2 0)");
  auto poly2 = g::Polyline3D::FromWkt("LINESTRING (-1 2 0, -0.5 2 0, 1 2 0, 2 1 0)");

  auto s1 = g::LineSegment3D::Make(g::Point3D(-2, 0, 0), g::Point3D(2, 0, 0));
  auto s2 = g::LineSegment3D::Make(g::Point3D(0, -1, 0), g::Point3D(0, 3, 0));

  ASSERT_TRUE(poly1.Intersects(s1));
  {
    auto inter = poly1.Intersection(s1);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline3D::MultiPoint>(*inter));
    auto pts = std::get<g::Polyline3D::MultiPoint>(*inter);
    ASSERT_EQ(g::Point3D(-1, 0, 0), pts[0]);
    ASSERT_EQ(g::Point3D(1, 0, 0), pts[1]);
  }

  ASSERT_FALSE(poly1.Intersects(s2));

  ASSERT_TRUE(poly2.Intersects(s2));
  {
    auto inter = poly2.Intersection(s2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Point3D>(*inter));
    EXPECT_EQ(g::Point3D(0, 2, 0), std::get<g::Point3D>(*inter));
  }
}

TEST_F(Polyline3DTest, Intersection) {
  geompp::DECIMAL_PRECISION = 4;
  auto poly1 = g::Polyline3D::FromWkt("LINESTRING (-1 2 0, -1 -2 0, 1 -2 0, 1 2 0)");
  auto poly2 = g::Polyline3D::FromWkt("LINESTRING (-2 1 0, -0.5 1 0, -0.5 -3 0, 0.5 -3 0, 0.5 1 0, 2 1 0)");
  auto poly3 = g::Polyline3D::FromWkt("LINESTRING (-3 4 0, -2 5 0, 0 5 0, 1 6 0, 2 4 0)");

  ASSERT_TRUE(poly1.Intersects(poly2));
  {
    auto inter = poly1.Intersection(poly2);
    ASSERT_TRUE(inter.has_value());
    ASSERT_TRUE(std::holds_alternative<g::Polyline3D::MultiPoint>(*inter));
    auto pts = std::get<g::Polyline3D::MultiPoint>(*inter);
    ASSERT_EQ(4, pts.size());
    EXPECT_EQ(g::Point3D(-1, 1, 0), pts[0]);
    EXPECT_EQ(g::Point3D(-0.5, -2, 0), pts[1]);
    EXPECT_EQ(g::Point3D(0.5, -2, 0), pts[2]);
    EXPECT_EQ(g::Point3D(1, 1, 0), pts[3]);
  }

  ASSERT_FALSE(poly1.Intersects(poly3));
  ASSERT_FALSE(poly2.Intersects(poly3));
}

TEST_F(Polyline3DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  ASSERT_EQ("LINESTRING (0 0 0, 1 1 0)", g::Polyline3D::Make({g::Point3D::Zero(), g::Point3D(1, 1, 0)}).ToWkt());

  // round-trip
  geompp::DECIMAL_PRECISION = 6;
  EXPECT_EQ(g::Polyline3D::Make({g::Point3D(1.5, -2.0, 3.0), g::Point3D(-1.0, 4.5, 0.0)}),
            g::Polyline3D::FromWkt("LINESTRING (1.5 -2.0 3.0, -1.0 4.5 0.0)"));

  // case-insensitive keyword
  EXPECT_EQ(g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 1, 1)}),
            g::Polyline3D::FromWkt("  linestring( 0 0 0, 1 1 1)"));

  // invalid
  EXPECT_ANY_THROW(g::Polyline3D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Polyline3D::FromWkt("linestrin (0 0 0, 1 1 1)"));
  EXPECT_ANY_THROW(g::Polyline3D::FromWkt("linestring 0 0 0, 1 1 1)"));
  EXPECT_ANY_THROW(g::Polyline3D::FromWkt("linestring (0 0 0, 1 1 1"));
  EXPECT_ANY_THROW(g::Polyline3D::FromWkt("linestring (0 0, 1 1)"));  // 2D coords in 3D parser
  EXPECT_ANY_THROW(g::Polyline3D::FromWkt("linestring ( )"));
}

TEST_F(Polyline3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "polyline3d.wkt").string();
  auto poly = g::Polyline3D::Make({g::Point3D::Zero(), g::Point3D(1, 0, 0)});

  poly.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  auto poly_file = g::Polyline3D::FromFile(path);
  EXPECT_EQ(poly, poly_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Polyline3DTest, TestFromFile) {
  std::string path = (test_res_path / "polyline3d" / "polyline.wkt").string();
  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Polyline3D::FromFile(path));

  auto p = g::Polyline3D::FromFile(path);
  GEOMPP_LOG(INFO) << "from file = " << p.ToWkt();
}

}  // namespace geompp_tests
