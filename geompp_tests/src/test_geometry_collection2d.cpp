#include "geometry_collection2d.hpp"

#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "polyline2d.hpp"
#include "ray2d.hpp"
#include "triangle2d.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class GeometryCollection2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(GeometryCollection2DTest, DefaultConstructor) {
  g::GeometryCollection2D gc;
  ASSERT_EQ(0u, gc.Size());
}

TEST_F(GeometryCollection2DTest, AddPoint) {
  g::GeometryCollection2D gc;
  gc.Add(g::Point2D(1, 2));
  ASSERT_EQ(1u, gc.Size());
  auto const& s = gc.Get(0);
  ASSERT_EQ(g::Point2D(1, 2), std::get<g::Point2D>(s));
}

TEST_F(GeometryCollection2DTest, AddMultipleTypes) {
  g::GeometryCollection2D gc;
  gc.Add(g::Point2D(1, 2));
  gc.Add(g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0)));
  gc.Add(g::Triangle2D::Make(g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(0, 1)));
  ASSERT_EQ(3u, gc.Size());
}

TEST_F(GeometryCollection2DTest, GetOutOfRange) {
  g::GeometryCollection2D gc;
  EXPECT_ANY_THROW(gc.Get(0));
  gc.Add(g::Point2D(0, 0));
  EXPECT_ANY_THROW(gc.Get(1));
}

TEST_F(GeometryCollection2DTest, GetLineSegment) {
  g::GeometryCollection2D gc;
  auto seg = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(3, 4));
  gc.Add(seg);
  auto const& s = gc.Get(0);
  ASSERT_TRUE(std::holds_alternative<g::LineSegment2D>(s));
  ASSERT_EQ(seg, std::get<g::LineSegment2D>(s));
}

TEST_F(GeometryCollection2DTest, AlmostEquals_Equal) {
  g::GeometryCollection2D gc1, gc2;
  gc1.Add(g::Point2D(1, 2));
  gc2.Add(g::Point2D(1, 2));
  ASSERT_TRUE(gc1.AlmostEquals(gc2));
  ASSERT_EQ(gc1, gc2);
}

TEST_F(GeometryCollection2DTest, AlmostEquals_DifferentValues) {
  g::GeometryCollection2D gc1, gc2;
  gc1.Add(g::Point2D(1, 2));
  gc2.Add(g::Point2D(3, 4));
  ASSERT_FALSE(gc1.AlmostEquals(gc2));
  ASSERT_NE(gc1, gc2);
}

TEST_F(GeometryCollection2DTest, AlmostEquals_DifferentSizes) {
  g::GeometryCollection2D gc1, gc2;
  gc1.Add(g::Point2D(1, 2));
  gc1.Add(g::Point2D(3, 4));
  gc2.Add(g::Point2D(1, 2));
  ASSERT_FALSE(gc1.AlmostEquals(gc2));
}

TEST_F(GeometryCollection2DTest, AlmostEquals_DifferentTypes) {
  g::GeometryCollection2D gc1, gc2;
  gc1.Add(g::Point2D(1, 2));
  gc2.Add(g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0)));
  ASSERT_FALSE(gc1.AlmostEquals(gc2));
}

TEST_F(GeometryCollection2DTest, AlmostEquals_Empty) {
  g::GeometryCollection2D gc1, gc2;
  ASSERT_TRUE(gc1.AlmostEquals(gc2));
}

TEST_F(GeometryCollection2DTest, Wkt_Empty) {
  g::GeometryCollection2D gc;
  ASSERT_EQ("GEOMETRYCOLLECTION EMPTY", gc.ToWkt());
}

TEST_F(GeometryCollection2DTest, Wkt_WithPoint) {
  g::GeometryCollection2D gc;
  gc.Add(g::Point2D(1, 2));
  auto wkt = gc.ToWkt();
  ASSERT_NE(std::string::npos, wkt.find("GEOMETRYCOLLECTION"));
  ASSERT_NE(std::string::npos, wkt.find("POINT"));
}

TEST_F(GeometryCollection2DTest, Wkt_WithMultiple) {
  g::GeometryCollection2D gc;
  gc.Add(g::Point2D(1, 2));
  gc.Add(g::LineSegment2D::Make(g::Point2D::Zero(), g::Point2D(1, 0)));
  auto wkt = gc.ToWkt();
  ASSERT_NE(std::string::npos, wkt.find("POINT"));
  ASSERT_NE(std::string::npos, wkt.find("LINESTRING"));
}

TEST_F(GeometryCollection2DTest, NestedCollection) {
  g::GeometryCollection2D inner, outer;
  inner.Add(g::Point2D(0, 0));
  outer.Add(inner);
  ASSERT_EQ(1u, outer.Size());
  auto const& s = outer.Get(0);
  ASSERT_TRUE(std::holds_alternative<g::GeometryCollection2D>(s));
}

TEST_F(GeometryCollection2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "geometry_collection2d.wkt").string();
  g::GeometryCollection2D gc;
  gc.Add(g::Point2D(1, 2));
  gc.ToFile(path);
  ASSERT_TRUE(fs::exists(path));
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(GeometryCollection2DTest, FromWkt_Point) {
  geompp::DECIMAL_PRECISION = 4;
  auto gc = g::GeometryCollection2D::FromWkt("GEOMETRYCOLLECTION (POINT (1 2))");
  ASSERT_EQ(1u, gc.Size());
  ASSERT_TRUE(std::holds_alternative<g::Point2D>(gc.Get(0)));
  ASSERT_EQ(g::Point2D(1, 2), std::get<g::Point2D>(gc.Get(0)));
}

TEST_F(GeometryCollection2DTest, FromWkt_Invalid_Throws) {
  EXPECT_ANY_THROW(g::GeometryCollection2D::FromWkt("not a wkt"));
  EXPECT_ANY_THROW(g::GeometryCollection2D::FromWkt("POINT (1 2)"));
}

TEST_F(GeometryCollection2DTest, FromWkt_Empty_Throws) {
  // "GEOMETRYCOLLECTION EMPTY" has no '(' so the parser throws
  EXPECT_ANY_THROW(g::GeometryCollection2D::FromWkt("GEOMETRYCOLLECTION EMPTY"));
}

TEST_F(GeometryCollection2DTest, Roundtrip_Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  g::GeometryCollection2D gc;
  gc.Add(g::Point2D(1, 2));
  auto gc2 = g::GeometryCollection2D::FromWkt(gc.ToWkt());
  ASSERT_EQ(gc, gc2);
}

}  // namespace geompp_tests
