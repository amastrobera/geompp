#include "point2d.hpp"

#include "utils.hpp"
#include "vector2d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <vector>

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
  std::string path = (test_res_path / "temp" / "point.wkt").string();
  auto p = g::Point2D(15.341, -781.684);

  p.ToFile(path, prec);
  ASSERT_TRUE(fs::exists(path));

  g::Point2D p_file = g::Point2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(p, p_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST(Point2D, TestFromFile) {
  std::string path = (test_res_path / "point2d" / "point.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Point2D::FromFile(path));

  auto p = g::Point2D::FromFile(path);

  std::cout << "form file = " << p.ToWkt() << std::endl;
}

TEST(Point2D, DistanceTo) {
  int prec = 4;
  auto p1 = g::Point2D();
  auto p2 = g::Point2D(1, 0);

  ASSERT_EQ(1, p1.DistanceTo(p2));

  auto p3 = g::Point2D(-1, -1);

  EXPECT_EQ(g::round_to(sqrt(2), prec), p1.DistanceTo(p3, prec));
}

TEST(Point2D, RemoveDuplicates) {
  // clang-format off
  std::vector<g::Point2D> pts{g::Point2D(0, 0), 
                              g::Point2D(0, 0), // duplicate
                              g::Point2D(0, 0), // duplicate
                              g::Point2D(1, 0),
                              g::Point2D(2, 0), 
                              g::Point2D(2, 2), 
                              g::Point2D(2, 2), // duplicate
                              g::Point2D(3, 2), 
                              g::Point2D(3, 2) // duplicate
                              };
  // clang-format on
  ASSERT_EQ(9, pts.size());

  auto unique_pts = g::Point2D::remove_duplicates(pts);

  ASSERT_EQ(5, unique_pts.size());
  ASSERT_EQ(g::Point2D(0, 0), unique_pts[0]);
  ASSERT_EQ(g::Point2D(1, 0), unique_pts[1]);
  ASSERT_EQ(g::Point2D(2, 0), unique_pts[2]);
  ASSERT_EQ(g::Point2D(2, 2), unique_pts[3]);
  ASSERT_EQ(g::Point2D(3, 2), unique_pts[4]);
}

TEST(Point2D, RemoveCollinear) {
  // clang-format off
  std::vector<g::Point2D> pts{g::Point2D(0, 0), 
                              g::Point2D(1, 0),  
                              g::Point2D(2, 0), // collinear
                              g::Point2D(2, 2), 
                              g::Point2D(2, 3), // collinear
                              g::Point2D(2, 4), // collinear
                              g::Point2D(2, 5), // collinear
                              g::Point2D(3, 6), 
                              g::Point2D(4, 5), 
                              g::Point2D(5, 4), // collinear 
                              g::Point2D(6, 0), 
                              g::Point2D(7, 0)
                              };
  // clang-format on

  ASSERT_EQ(12, pts.size());

  auto unique_pts = g::Point2D::remove_collinear(pts);

  ASSERT_EQ(g::Point2D(0, 0), unique_pts[0]);
  ASSERT_EQ(g::Point2D(2, 0), unique_pts[1]);
  ASSERT_EQ(g::Point2D(2, 5), unique_pts[2]);
  ASSERT_EQ(g::Point2D(3, 6), unique_pts[3]) << "value=" << unique_pts[3].ToWkt();
  ASSERT_EQ(g::Point2D(5, 4), unique_pts[4]);
  ASSERT_EQ(g::Point2D(6, 0), unique_pts[5]);
  ASSERT_EQ(g::Point2D(7, 0), unique_pts[6]);
}

TEST(Point2D, Average) {
  // clang-format off
  std::vector<g::Point2D> pts{g::Point2D(0, 0),
                              g::Point2D(1, 0),
                              g::Point2D(2, -3),
                              g::Point2D(-5, 6),
                              g::Point2D(7, -1),
                              g::Point2D(-2, -8),
                              g::Point2D(0, 1)
                              };
  // clang-format on
  ASSERT_EQ(g::Point2D(3.0 / pts.size(), -5.0 / pts.size()), g::Point2D::average(pts));
}

}  // namespace geompp_tests