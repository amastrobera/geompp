#include "point3d.hpp"

#include "utils.hpp"
#include "vector3d.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <vector>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

TEST(Point3D, Equality) {
  ASSERT_EQ(g::Point3D(2.56, 748.1203, -3.14), g::Point3D(2.56, 748.1203, -3.14));
  ASSERT_EQ(g::Point3D(2, 3, 4), g::Point3D(2, 3, 4));
  ASSERT_EQ(g::Point3D(-56.682, 30.56, 0.0), g::Point3D(-56.682, 30.56, 0.0));

  // default constructor is the origin
  ASSERT_EQ(g::Point3D(), g::Point3D(0, 0, 0));
  ASSERT_EQ(g::Point3D::Origin(), g::Point3D());

  // inequality
  ASSERT_NE(g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  ASSERT_NE(g::Point3D(0, 0, 0), g::Point3D(0, 0, 1));
}

TEST(Point3D, Assignment) {
  g::Point3D p1(1, 2, 3);
  g::Point3D p2(4, 5, 6);

  p2 = p1;
  ASSERT_EQ(g::Point3D(1, 2, 3), p2);

  // self-assignment is a no-op
  p1 = p1;
  ASSERT_EQ(g::Point3D(1, 2, 3), p1);
}

TEST(Point3D, Getters) {
  auto p = g::Point3D(1.5, -2.3, 7.0);

  ASSERT_EQ(1.5, p.x());
  ASSERT_EQ(-2.3, p.y());
  ASSERT_EQ(7.0, p.z());

  // copy constructor preserves values
  auto p_copy = g::Point3D(p);
  ASSERT_EQ(p, p_copy);
}

TEST(Point3D, ToVector) {
  auto p = g::Point3D(3.0, -1.5, 2.7);
  auto v = p.ToVector();

  ASSERT_EQ(3.0, v.x());
  ASSERT_EQ(-1.5, v.y());
  ASSERT_EQ(2.7, v.z());

  // round-trip via ToPoint
  ASSERT_EQ(p, v.ToPoint());
}

TEST(Point3D, SubtractPoint) {
  auto p1 = g::Point3D(1, 2, 3);
  auto p2 = g::Point3D(4, 6, 9);
  g::Vector3D v = p2 - p1;

  ASSERT_EQ(3.0, v.x());
  ASSERT_EQ(4.0, v.y());
  ASSERT_EQ(6.0, v.z());

  ASSERT_EQ(g::Vector3D(3, 4, 6), v);

  // reversing order negates the vector
  g::Vector3D v_rev = p1 - p2;
  ASSERT_EQ(g::Vector3D(-3, -4, -6), v_rev);
}

TEST(Point3D, AddVector) {
  auto p = g::Point3D(1.0, 2.0, 3.0);
  auto v = g::Vector3D(0.5, -1.0, 2.0);

  g::Point3D sum = p + v;
  ASSERT_EQ(g::Point3D(1.5, 1.0, 5.0), sum);

  g::Point3D diff = p - v;
  ASSERT_EQ(g::Point3D(0.5, 3.0, 1.0), diff);

  // Vector + Point (commutative via operator+(Vector3D, Point3D))
  g::Point3D sum2 = v + p;
  ASSERT_EQ(sum, sum2);
}

TEST(Point3D, ScalarMultiply) {
  auto p = g::Point3D(2.0, -3.0, 4.0);

  ASSERT_EQ(g::Point3D(4.0, -6.0, 8.0), p * 2.0);
  ASSERT_EQ(g::Point3D(1.0, -1.5, 2.0), p * 0.5);
  ASSERT_EQ(g::Point3D(0.0, 0.0, 0.0), p * 0.0);
  ASSERT_EQ(g::Point3D(-2.0, 3.0, -4.0), p * -1.0);

  // scalar on the left
  ASSERT_EQ(p * 3.0, 3.0 * p);
}

TEST(Point3D, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto origin = g::Point3D();

  // axis-aligned
  ASSERT_EQ(1.0, origin.DistanceTo(g::Point3D(1, 0, 0)));
  ASSERT_EQ(1.0, origin.DistanceTo(g::Point3D(0, 1, 0)));
  ASSERT_EQ(1.0, origin.DistanceTo(g::Point3D(0, 0, 1)));

  // diagonal in XY plane: sqrt(2)
  geompp::DECIMAL_PRECISION = 3;
  ASSERT_EQ(g::round(std::sqrt(2.0)), origin.DistanceTo(g::Point3D(1, 1, 0)));

  // space diagonal of unit cube: sqrt(3)
  ASSERT_EQ(g::round(std::sqrt(3.0)), origin.DistanceTo(g::Point3D(1, 1, 1)));

  geompp::DECIMAL_PRECISION = 4;

  // symmetry
  auto p1 = g::Point3D(1, 2, 3);
  auto p2 = g::Point3D(4, 6, 3);
  ASSERT_EQ(p1.DistanceTo(p2), p2.DistanceTo(p1));

  // distance to self is 0
  ASSERT_EQ(0.0, p1.DistanceTo(p1));
}

TEST(Point3D, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 2;  // tolerance ~0.005

  auto p = g::Point3D(1.0, 2.0, 3.0);
  ASSERT_TRUE(p.AlmostEquals(g::Point3D(1.004, 2.004, 3.004)));  // within tolerance
  ASSERT_FALSE(p.AlmostEquals(g::Point3D(1.01, 2.0, 3.0)));      // just outside

  // operator== delegates to AlmostEquals
  ASSERT_EQ(p, g::Point3D(1.004, 2.004, 3.004));
  ASSERT_NE(p, g::Point3D(1.01, 2.0, 3.0));

  geompp::DECIMAL_PRECISION = 4;                              // tighter tolerance
  ASSERT_FALSE(p.AlmostEquals(g::Point3D(1.004, 2.0, 3.0)));  // was OK at DP=2, not at DP=4
}

TEST(Point3D, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  ASSERT_EQ("POINT (0 0 0)", g::Point3D().ToWkt());
  ASSERT_EQ("POINT (1 2 3)", g::Point3D(1, 2, 3).ToWkt());

  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("POINT (56491.62 -795.97 12.34)", g::Point3D(56491.6164, -795.97416, 12.3412).ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Point3D(256.1343, -684.6497, 99.0), g::Point3D::FromWkt("POINT (256.1343 -684.6497 99.0)"));
  EXPECT_EQ(g::Point3D(-7.5, -60.7, 0.0), g::Point3D::FromWkt("  point( -7.5    -60.7  0.0 )"));
  EXPECT_EQ(g::Point3D(0.645, -1.689741, 3.0), g::Point3D::FromWkt("PoInT   ( 0.645  -1.689741  3.0  )"));

  // invalid: wrong geometry type
  EXPECT_ANY_THROW(g::Point3D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Point3D::FromWkt("poin ( 1 2 3)"));

  // invalid: missing brackets
  EXPECT_ANY_THROW(g::Point3D::FromWkt("point 1 2 3)"));
  EXPECT_ANY_THROW(g::Point3D::FromWkt("point (1 2 3"));

  // invalid: wrong number of coordinates (needs exactly 3)
  EXPECT_ANY_THROW(g::Point3D::FromWkt("point ( )"));
  EXPECT_ANY_THROW(g::Point3D::FromWkt("point ( -7.5 )"));
  EXPECT_ANY_THROW(g::Point3D::FromWkt("point (1 2)"));
  EXPECT_ANY_THROW(g::Point3D::FromWkt("point (1 2 3 4)"));
}

TEST(Point3D, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "point3d.wkt").string();
  auto p = g::Point3D(15.341, -781.684, 42.0);

  p.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Point3D p_file = g::Point3D::FromFile(path);

  EXPECT_EQ(p, p_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST(Point3D, TestFromFile) {
  std::string path = (test_res_path / "point3d" / "point.wkt").string();

  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Point3D::FromFile(path));

  auto p = g::Point3D::FromFile(path);
  std::cout << "from file = " << p.ToWkt() << std::endl;
}

TEST(Point3D, AreCollinear) {
  // on the X axis
  ASSERT_TRUE(g::are_collinear(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(2, 0, 0)));
  ASSERT_TRUE(g::are_collinear(g::Point3D(0, 0, 0), g::Point3D(5, 0, 0), g::Point3D(100, 0, 0)));
  // 3D diagonal
  ASSERT_TRUE(g::are_collinear(g::Point3D(0, 0, 0), g::Point3D(1, 1, 1), g::Point3D(2, 2, 2)));

  // non-collinear
  ASSERT_FALSE(g::are_collinear(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0)));
  ASSERT_FALSE(g::are_collinear(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 0, 1)));
  // triangle vertices
  ASSERT_FALSE(g::are_collinear(g::Point3D(-1, 1, 2), g::Point3D(0, -1, 2), g::Point3D(1, 1, 2)));
}

TEST(Point3D, RemoveDuplicates) {
  // clang-format off
  std::vector<g::Point3D> pts{
    g::Point3D(0, 0, 0),
    g::Point3D(0, 0, 0),  // duplicate
    g::Point3D(0, 0, 0),  // duplicate
    g::Point3D(1, 0, 0),
    g::Point3D(2, 0, 0),
    g::Point3D(2, 2, 0),
    g::Point3D(2, 2, 0),  // duplicate
    g::Point3D(3, 2, 1),
    g::Point3D(3, 2, 1),  // duplicate
  };
  // clang-format on
  ASSERT_EQ(9, pts.size());

  auto unique = g::remove_duplicates(pts);

  ASSERT_EQ(5, unique.size());
  ASSERT_EQ(g::Point3D(0, 0, 0), unique[0]);
  ASSERT_EQ(g::Point3D(1, 0, 0), unique[1]);
  ASSERT_EQ(g::Point3D(2, 0, 0), unique[2]);
  ASSERT_EQ(g::Point3D(2, 2, 0), unique[3]);
  ASSERT_EQ(g::Point3D(3, 2, 1), unique[4]);

  // empty input → empty output
  ASSERT_EQ(0, g::remove_duplicates({}).size());

  // single point → returned as-is
  ASSERT_EQ(1, g::remove_duplicates({g::Point3D(1, 2, 3)}).size());
}

TEST(Point3D, RemoveCollinear) {
  // clang-format off
  std::vector<g::Point3D> pts{
    g::Point3D(0, 0, 0),
    g::Point3D(1, 0, 0),
    g::Point3D(2, 0, 0),  // collinear — X axis run
    g::Point3D(2, 1, 0),
    g::Point3D(2, 2, 0),  // collinear — Y axis run
    g::Point3D(2, 3, 0),  // collinear
    g::Point3D(3, 3, 0),
    g::Point3D(4, 3, 0),  // collinear — X axis run
    g::Point3D(5, 3, 0),
  };
  // clang-format on

  auto compressed = g::remove_collinear(pts);

  ASSERT_EQ(5, compressed.size());
  ASSERT_EQ(g::Point3D(0, 0, 0), compressed[0]);
  ASSERT_EQ(g::Point3D(2, 0, 0), compressed[1]);  // farthest on X run
  ASSERT_EQ(g::Point3D(2, 3, 0), compressed[2]);  // farthest on Y run
  ASSERT_EQ(g::Point3D(3, 3, 0), compressed[3]);
  ASSERT_EQ(g::Point3D(5, 3, 0), compressed[4]);  // farthest on X run

  // fewer than 3 points → returned unchanged
  ASSERT_EQ(2, g::remove_collinear({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0)}).size());
}

TEST(Point3D, Average) {
  // clang-format off
  std::vector<g::Point3D> pts{
    g::Point3D(0, 0, 0),
    g::Point3D(1, 0, 0),
    g::Point3D(2, -3, 6),
    g::Point3D(-5, 6, -3),
    g::Point3D(7, -1, 2),
    g::Point3D(-2, -8, 4),
    g::Point3D(0, 1, -1),
  };
  // clang-format on

  int n = pts.size();
  double exp_x = (0.0 + 1 + 2 - 5 + 7 - 2 + 0) / n;  //  3/7
  double exp_y = (0.0 + 0 - 3 + 6 - 1 - 8 + 1) / n;  // -5/7
  double exp_z = (0.0 + 0 + 6 - 3 + 2 + 4 - 1) / n;  //  8/7

  ASSERT_EQ(g::Point3D(exp_x, exp_y, exp_z), g::average(pts));

  // empty → throws
  EXPECT_ANY_THROW(g::average(std::vector<g::Point3D>{}));
}

TEST(Point3D, LinearCombination) {
  std::vector<g::Point3D> pts{g::Point3D(1, 0, 0), g::Point3D(0, 1, 0), g::Point3D(0, 0, 1)};
  std::vector<double> w{0.5, 0.3, 0.2};

  auto result = g::linear_combination(pts, w);
  ASSERT_EQ(g::Point3D(0.5, 0.3, 0.2), result);

  // uniform weights == average
  std::vector<double> uniform{1.0 / 3, 1.0 / 3, 1.0 / 3};
  ASSERT_EQ(g::average(pts), g::linear_combination(pts, uniform));

  // mismatched sizes → throws
  EXPECT_ANY_THROW(g::linear_combination(pts, {0.5, 0.5}));

  // empty → throws
  EXPECT_ANY_THROW(g::linear_combination(std::vector<g::Point3D>{}, std::vector<double>{}));
}

}  // namespace geompp_tests
