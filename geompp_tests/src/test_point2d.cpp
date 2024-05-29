#include "point2d.hpp"

#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(Point2D, Equality) {
  ASSERT_EQ(g::Point2D(2.56, 748.1203), g::Point2D(2.56, 748.1203));
  ASSERT_EQ(g::Point2D(2, 3), g::Point2D(2, 3));
  ASSERT_EQ(g::Point2D(-56.682, 30.56), g::Point2D(-56.682, 30.56));
  ASSERT_EQ(g::Point2D(-672.6456, -153.516), g::Point2D(-672.6456, -153.516));

  ASSERT_EQ(g::Point2D(672.64560944, -153.5166067079), g::Point2D(672.64560944, -153.5166067079));
}  // namespace std::filesystem

TEST(Point2D, SubtractPoint) {
  auto p1 = g::Point2D(1, 2);
  auto p2 = g::Point2D(1, 1);
  g::Vector2D v = p2 - p1;

  ASSERT_EQ(0, v.x());
  ASSERT_EQ(-1, v.y());

  ASSERT_EQ(g::Vector2D(0, -1), v);
}

TEST(Point2D, AddVector) {
  auto p = g::Point2D(1.2, 2.12);
  auto v = g::Vector2D(1.85, 1.65);
  g::Point2D pv = p + v;

  ASSERT_EQ(3.05, pv.x());
  ASSERT_EQ(3.77, pv.y());

  ASSERT_EQ(g::Point2D(3.05, 3.77), pv);
}

TEST(Point2D, Wkt) {
  ASSERT_EQ("POINT (0 0)", g::Point2D().ToWkt());
  ASSERT_EQ("POINT (56491.62 -795.97)", g::Point2D(56491.6164, -795.97416).ToWkt(2));

  EXPECT_EQ(g::Point2D(256.1343, -684.64971), g::Point2D::FromWkt("POINT (256.1343 -684.64971)"));
  EXPECT_EQ(g::Point2D(-7.5, -60.7), g::Point2D::FromWkt("  point( -7.5    -60.7)"));
  EXPECT_EQ(g::Point2D(0.645, -1.689741), g::Point2D::FromWkt("PoInT   ( 0.645  -1.689741  )"));

  EXPECT_ANY_THROW(g::Point2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("poin ( -7.5 -60.7)"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point -7.5 -64.4)"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point (-7.5 -64.4"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( -7.5 )"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( )"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( -7.5 -64.4 15.5)"));
}

TEST(Point2D, ToFile) {
  int prec = 4;
  std::string path = "point.wkt";
  auto p = g::Point2D(15.341, -781.684);

  ASSERT_NO_THROW(p.ToFile(path, prec));

  g::Point2D p_file = g::Point2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(p, p_file);
}

TEST(Point2D, TestFromFile) {
  test_res_path = "/home/angelo/development/geompp/geompp_tests/res";
  std::string path = (test_res_path / "point2d" / "point.wkt").string();

  std::cout << "path = " << path << std::endl;

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Point2D::FromFile(path));

  auto p = g::Point2D::FromFile(path);

  std::cout << "form file = " << p.ToWkt() << std::endl;
}

}  // namespace geompp_tests