#include "geometry_collection3d.hpp"

#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "ray3d.hpp"
#include "triangle3d.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class GeometryCollection3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(GeometryCollection3DTest, DefaultConstructor) {
  g::GeometryCollection3D gc;
  ASSERT_EQ(0u, gc.Size());
}

TEST_F(GeometryCollection3DTest, AddPoint) {
  g::GeometryCollection3D gc;
  gc.Add(g::Point3D(1, 2, 3));
  ASSERT_EQ(1u, gc.Size());
  auto const& s = gc.Get(0);
  ASSERT_EQ(g::Point3D(1, 2, 3), std::get<g::Point3D>(s));
}

TEST_F(GeometryCollection3DTest, AddMultipleTypes) {
  g::GeometryCollection3D gc;
  gc.Add(g::Point3D(1, 2, 3));
  gc.Add(g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0)));
  gc.Add(g::Triangle3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0)));
  ASSERT_EQ(3u, gc.Size());
}

TEST_F(GeometryCollection3DTest, GetOutOfRange) {
  g::GeometryCollection3D gc;
  EXPECT_ANY_THROW(gc.Get(0));
  gc.Add(g::Point3D(0, 0, 0));
  EXPECT_ANY_THROW(gc.Get(1));
}

TEST_F(GeometryCollection3DTest, GetLineSegment) {
  g::GeometryCollection3D gc;
  auto seg = g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 2, 3));
  gc.Add(seg);
  auto const& s = gc.Get(0);
  ASSERT_TRUE(std::holds_alternative<g::LineSegment3D>(s));
  ASSERT_EQ(seg, std::get<g::LineSegment3D>(s));
}

TEST_F(GeometryCollection3DTest, AlmostEquals_Equal) {
  g::GeometryCollection3D gc1, gc2;
  gc1.Add(g::Point3D(1, 2, 3));
  gc2.Add(g::Point3D(1, 2, 3));
  ASSERT_TRUE(gc1.AlmostEquals(gc2));
  ASSERT_EQ(gc1, gc2);
}

TEST_F(GeometryCollection3DTest, AlmostEquals_DifferentValues) {
  g::GeometryCollection3D gc1, gc2;
  gc1.Add(g::Point3D(1, 2, 3));
  gc2.Add(g::Point3D(4, 5, 6));
  ASSERT_FALSE(gc1.AlmostEquals(gc2));
  ASSERT_NE(gc1, gc2);
}

TEST_F(GeometryCollection3DTest, AlmostEquals_DifferentSizes) {
  g::GeometryCollection3D gc1, gc2;
  gc1.Add(g::Point3D(1, 2, 3));
  gc1.Add(g::Point3D(4, 5, 6));
  gc2.Add(g::Point3D(1, 2, 3));
  ASSERT_FALSE(gc1.AlmostEquals(gc2));
}

TEST_F(GeometryCollection3DTest, AlmostEquals_DifferentTypes) {
  g::GeometryCollection3D gc1, gc2;
  gc1.Add(g::Point3D(1, 2, 3));
  gc2.Add(g::LineSegment3D::Make(g::Point3D::Zero(), g::Point3D(1, 0, 0)));
  ASSERT_FALSE(gc1.AlmostEquals(gc2));
}

TEST_F(GeometryCollection3DTest, AlmostEquals_Empty) {
  g::GeometryCollection3D gc1, gc2;
  ASSERT_TRUE(gc1.AlmostEquals(gc2));
}

TEST_F(GeometryCollection3DTest, Wkt_Empty) {
  g::GeometryCollection3D gc;
  ASSERT_EQ("GEOMETRYCOLLECTION EMPTY", gc.ToWkt());
}

TEST_F(GeometryCollection3DTest, Wkt_WithPoint) {
  g::GeometryCollection3D gc;
  gc.Add(g::Point3D(1, 2, 3));
  auto wkt = gc.ToWkt();
  ASSERT_NE(std::string::npos, wkt.find("GEOMETRYCOLLECTION"));
  ASSERT_NE(std::string::npos, wkt.find("POINT"));
}

TEST_F(GeometryCollection3DTest, NestedCollection) {
  g::GeometryCollection3D inner, outer;
  inner.Add(g::Point3D(0, 0, 0));
  outer.Add(inner);
  ASSERT_EQ(1u, outer.Size());
  auto const& s = outer.Get(0);
  ASSERT_TRUE(std::holds_alternative<g::GeometryCollection3D>(s));
}

TEST_F(GeometryCollection3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "geometry_collection3d.wkt").string();
  g::GeometryCollection3D gc;
  gc.Add(g::Point3D(1, 2, 3));
  gc.ToFile(path);
  ASSERT_TRUE(fs::exists(path));
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(GeometryCollection3DTest, FromWkt_Point) {
  geompp::DECIMAL_PRECISION = 4;
  auto gc = g::GeometryCollection3D::FromWkt("GEOMETRYCOLLECTION (POINT (1 2 3))");
  ASSERT_EQ(1u, gc.Size());
  ASSERT_TRUE(std::holds_alternative<g::Point3D>(gc.Get(0)));
  ASSERT_EQ(g::Point3D(1, 2, 3), std::get<g::Point3D>(gc.Get(0)));
}

TEST_F(GeometryCollection3DTest, FromWkt_Invalid_Throws) {
  EXPECT_ANY_THROW(g::GeometryCollection3D::FromWkt("not a wkt"));
  EXPECT_ANY_THROW(g::GeometryCollection3D::FromWkt("POINT (1 2 3)"));
}

TEST_F(GeometryCollection3DTest, FromWkt_Empty_Throws) {
  // "GEOMETRYCOLLECTION EMPTY" has no '(' so the parser throws
  EXPECT_ANY_THROW(g::GeometryCollection3D::FromWkt("GEOMETRYCOLLECTION EMPTY"));
}

TEST_F(GeometryCollection3DTest, Roundtrip_Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  g::GeometryCollection3D gc;
  gc.Add(g::Point3D(1, 2, 3));
  auto gc2 = g::GeometryCollection3D::FromWkt(gc.ToWkt());
  ASSERT_EQ(gc, gc2);
}

}  // namespace geompp_tests
