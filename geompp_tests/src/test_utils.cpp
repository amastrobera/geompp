#include "utils.hpp"

#include "plane.hpp"
#include "point2d.hpp"
#include "point3d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <limits>

namespace g = geompp;

namespace geompp_tests {

class UtilsTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(UtilsTest, Round) {
  geompp::DECIMAL_PRECISION = 3;
  EXPECT_EQ(1.235, g::round(1.2345));
  EXPECT_EQ(-1.235, g::round(-1.2345));
  EXPECT_EQ(0.0, g::round(0.0));

  // explicit precision override
  EXPECT_EQ(1.23, g::round(1.2345, 2));
  EXPECT_EQ(1.2345, g::round(1.2345, 4));
  EXPECT_EQ(1.0, g::round(1.2345, 0));
}

TEST_F(UtilsTest, Compare) {
  // equal within epsilon
  EXPECT_EQ(std::partial_ordering::equivalent, g::compare(1.0, 1.0));
  EXPECT_EQ(std::partial_ordering::equivalent, g::compare(1.0, 1.0 + 1e-10));

  // less / greater
  EXPECT_EQ(std::partial_ordering::less, g::compare(1.0, 2.0));
  EXPECT_EQ(std::partial_ordering::greater, g::compare(2.0, 1.0));

  // boundary: exactly epsilon apart
  EXPECT_EQ(std::partial_ordering::equivalent, g::compare(0.0, g::DOUBLE_EPSILON / 2.0));
}

TEST_F(UtilsTest, IsInRange) {
  EXPECT_TRUE(g::is_in_range(0.5, 0.0, 1.0));
  EXPECT_TRUE(g::is_in_range(0.0, 0.0, 1.0));  // on min boundary
  EXPECT_TRUE(g::is_in_range(1.0, 0.0, 1.0));  // on max boundary
  EXPECT_FALSE(g::is_in_range(-0.1, 0.0, 1.0));
  EXPECT_FALSE(g::is_in_range(1.1, 0.0, 1.0));
}

TEST_F(UtilsTest, IsGreaterOrEqual) {
  EXPECT_TRUE(g::is_greater_or_equal(1.0, 1.0));
  EXPECT_TRUE(g::is_greater_or_equal(1.1, 1.0));
  EXPECT_FALSE(g::is_greater_or_equal(0.9, 1.0));
}

TEST_F(UtilsTest, Sign) {
  EXPECT_EQ(1, g::sign(3.5));
  EXPECT_EQ(-1, g::sign(-2.0));
  EXPECT_EQ(1, g::sign(0.0));
  // EXPECT_EQ(1, g::sign(std::numeric_limits<double>::epsilon()));
  // EXPECT_EQ(-1, g::sign(-std::numeric_limits<double>::epsilon())); // will not work, not cohere with our
  // DECIMAL_PRECISION-based comparison logic
}

TEST_F(UtilsTest, Trim) {
  EXPECT_EQ("hello", g::trim("  hello  "));
  EXPECT_EQ("hello", g::trim("hello  "));
  EXPECT_EQ("hello", g::trim("  hello"));
  EXPECT_EQ("hello", g::trim("hello"));
  EXPECT_EQ("", g::trim("   "));
  EXPECT_EQ("", g::trim(""));
  EXPECT_EQ("hello world", g::trim("  hello world  "));  // inner spaces preserved
}

TEST_F(UtilsTest, ToUpper) {
  EXPECT_EQ("HELLO", g::to_upper("hello"));
  EXPECT_EQ("HELLO", g::to_upper("HeLLo"));
  EXPECT_EQ("HELLO", g::to_upper("HELLO"));
  EXPECT_EQ("", g::to_upper(""));
  EXPECT_EQ("LINESTRING", g::to_upper("linestring"));
}

TEST_F(UtilsTest, TokenizeToDoubles) {
  auto v = g::tokenize_to_doubles("1.5 -3.0 0");
  ASSERT_EQ(3, v.size());
  EXPECT_EQ(1.5, v[0]);
  EXPECT_EQ(-3.0, v[1]);
  EXPECT_EQ(0.0, v[2]);

  // extra spaces
  auto v2 = g::tokenize_to_doubles("  1.0   2.0  ");
  ASSERT_EQ(2, v2.size());
  EXPECT_EQ(1.0, v2[0]);
  EXPECT_EQ(2.0, v2[1]);

  // single value
  auto v3 = g::tokenize_to_doubles("42.5");
  ASSERT_EQ(1, v3.size());
  EXPECT_EQ(42.5, v3[0]);

  // custom delimiter
  auto v4 = g::tokenize_to_doubles("1.0,2.0,3.0", ',');
  ASSERT_EQ(3, v4.size());
  EXPECT_EQ(1.0, v4[0]);
}

TEST_F(UtilsTest, TokenizeString) {
  auto v = g::tokenize_string("a,b,c", ',');
  ASSERT_EQ(3, v.size());
  EXPECT_EQ("a", v[0]);
  EXPECT_EQ("b", v[1]);
  EXPECT_EQ("c", v[2]);

  // single token (no delimiter)
  auto v2 = g::tokenize_string("hello", ',');
  ASSERT_EQ(1, v2.size());
  EXPECT_EQ("hello", v2[0]);

  // real use-case: WKT point list
  auto v3 = g::tokenize_string("0 0 0, 1 1 1, 2 2 2", ',');
  ASSERT_EQ(3, v3.size());
  EXPECT_EQ("0 0 0", g::trim(v3[0]));
  EXPECT_EQ("1 1 1", g::trim(v3[1]));
  EXPECT_EQ("2 2 2", g::trim(v3[2]));
}

TEST_F(UtilsTest, CountDecimalPlaces) {
  EXPECT_EQ(0, g::count_decimal_places(1.0));
  EXPECT_EQ(0, g::count_decimal_places(42.0));
  EXPECT_EQ(1, g::count_decimal_places(1.5));
  EXPECT_EQ(2, g::count_decimal_places(1.23));
  EXPECT_EQ(3, g::count_decimal_places(1.234));
  EXPECT_EQ(2, g::count_decimal_places(-3.14));
}

TEST_F(UtilsTest, StringJoin) {
  std::vector<std::string> v{"a", "b", "c"};
  EXPECT_EQ("a, b, c", g::string_join(v, ", "));
  EXPECT_EQ("a b c", g::string_join(v, " "));
  EXPECT_EQ("abc", g::string_join(v, ""));

  // single element
  std::vector<std::string> single{"only"};
  EXPECT_EQ("only", g::string_join(single, ", "));

  // empty vector
  std::vector<std::string> empty{};
  EXPECT_EQ("", g::string_join(empty, ", "));
}

TEST_F(UtilsTest, ToWktCollection) {
  std::vector<g::Point3D> pts{g::Point3D::Zero(), g::Point3D(1, 1, 1)};
  std::string wkt = g::ToWkt(pts);
  EXPECT_NE(std::string::npos, wkt.find("GEOMETRYCOLLECTION"));
  EXPECT_NE(std::string::npos, wkt.find("0 0 0"));
  EXPECT_NE(std::string::npos, wkt.find("1 1 1"));

  // works for Point2D too
  std::vector<g::Point2D> pts2d{g::Point2D::Zero(), g::Point2D(1, 1)};
  EXPECT_NE(std::string::npos, g::ToWkt(pts2d).find("GEOMETRYCOLLECTION"));
}

TEST_F(UtilsTest, AreCCW_2D) {
  // CCW square
  std::vector<g::Point2D> ccw = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  EXPECT_TRUE(g::are_ccw(ccw));
  EXPECT_FALSE(g::are_cw(ccw));

  // CW square
  std::vector<g::Point2D> cw = {
      g::Point2D(0, 0), g::Point2D(0, 4), g::Point2D(4, 4), g::Point2D(4, 0)};
  EXPECT_FALSE(g::are_ccw(cw));
  EXPECT_TRUE(g::are_cw(cw));

  // CCW triangle
  std::vector<g::Point2D> tri_ccw = {
      g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0, 1)};
  EXPECT_TRUE(g::are_ccw(tri_ccw));
}

TEST_F(UtilsTest, AreCoplanar_3D) {
  // all on XY plane
  std::vector<g::Point3D> planar = {
      g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0), g::Point3D(1, 1, 0)};
  EXPECT_TRUE(g::are_coplanar(planar));

  // not coplanar
  std::vector<g::Point3D> non_planar = {
      g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0), g::Point3D(1, 1, 1)};
  EXPECT_FALSE(g::are_coplanar(non_planar));
}

TEST_F(UtilsTest, AreCCW_3D) {
  // CCW square on XY plane (viewed from +Z)
  std::vector<g::Point3D> ccw = {
      g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)};
  EXPECT_TRUE(g::are_ccw(ccw));
  EXPECT_FALSE(g::are_cw(ccw));

  // CW square on XY plane
  std::vector<g::Point3D> cw = {
      g::Point3D(0, 0, 0), g::Point3D(0, 4, 0), g::Point3D(4, 4, 0), g::Point3D(4, 0, 0)};
  EXPECT_FALSE(g::are_ccw(cw));
  EXPECT_TRUE(g::are_cw(cw));
}

}  // namespace geompp_tests
