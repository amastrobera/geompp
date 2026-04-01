#include "line2d.hpp"

#include "point2d.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
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

  auto v = g::Vector2D(0, 1);
  auto l2 = g::Line2D::Make(p0, v);
  EXPECT_EQ(l2.Origin(), p0);
  EXPECT_EQ(l2.Direction(), v);
  EXPECT_EQ(l2.Last(), p0 + v);
}

TEST_F(Line2DTest, Contains) {
  auto l1 = g::Line2D::Make(g::Point2D(0, -1), g::Point2D(0, 1));
  auto p0 = g::Point2D();
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
  ASSERT_TRUE(std::holds_alternative<g::Point2D>(*inter));

  EXPECT_EQ(g::Point2D(0, 0), std::get<g::Point2D>(*inter));
}

TEST_F(Line2DTest, Wkt) {
  ASSERT_EQ("LINE (0 0, 1 1)", g::Line2D::Make(g::Point2D(), g::Point2D(1, 1)).ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("LINE (56491.62 -795.97, -9137.37 10.36)",
            g::Line2D::Make(g::Point2D(56491.6164, -795.97416), g::Point2D(-9137.3679, 10.35678)).ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Line2D::Make(g::Point2D(256.1343, -684.64971), g::Point2D(-601.674503, 7.361975)),
            g::Line2D::FromWkt("LINE (256.1343 -684.64971, -601.674503 7.361975)"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(-7.5, -60.7), g::Point2D()),
            g::Line2D::FromWkt("  LINE( -7.5    -60.7, 0   0)"));
  EXPECT_EQ(g::Line2D::Make(g::Point2D(0.645, -1.689741), g::Point2D(1, 0)),
            g::Line2D::FromWkt("LINE   ( 0.645  -1.689741  , 1 0  )"));

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
  auto p1 = g::Point2D();
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

}  // namespace geompp_tests
