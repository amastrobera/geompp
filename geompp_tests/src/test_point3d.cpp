#include "point3d.hpp"

#include "utils.hpp"
#include "vector3d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <vector>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Point3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Point3DTest, Equality) {
  ASSERT_EQ(g::Point3D(2.56, 748.1203, -3.14), g::Point3D(2.56, 748.1203, -3.14));
  ASSERT_EQ(g::Point3D(2, 3, 4), g::Point3D(2, 3, 4));
  ASSERT_EQ(g::Point3D(-56.682, 30.56, 0.0), g::Point3D(-56.682, 30.56, 0.0));

  // Zero() is the origin
  ASSERT_EQ(g::Point3D::Zero(), g::Point3D::Zero());

  // inequality
  ASSERT_NE(g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  ASSERT_NE(g::Point3D::Zero(), g::Point3D(0, 0, 1));
}

TEST_F(Point3DTest, Assignment) {
  g::Point3D p1(1, 2, 3);
  g::Point3D p2(4, 5, 6);

  p2 = p1;
  ASSERT_EQ(g::Point3D(1, 2, 3), p2);

  // self-assignment is a no-op
  p1 = p1;
  ASSERT_EQ(g::Point3D(1, 2, 3), p1);
}

TEST_F(Point3DTest, Getters) {
  auto p = g::Point3D(1.5, -2.3, 7.0);

  ASSERT_EQ(1.5, p.x());
  ASSERT_EQ(-2.3, p.y());
  ASSERT_EQ(7.0, p.z());

  // copy constructor preserves values
  auto p_copy = g::Point3D(p);
  ASSERT_EQ(p, p_copy);
}

TEST_F(Point3DTest, ToVector) {
  auto p = g::Point3D(3.0, -1.5, 2.7);
  auto v = p.ToVector();

  ASSERT_EQ(3.0, v.x());
  ASSERT_EQ(-1.5, v.y());
  ASSERT_EQ(2.7, v.z());

  // round-trip via ToPoint
  ASSERT_EQ(p, v.ToPoint());
}

TEST_F(Point3DTest, SubtractPoint) {
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

TEST_F(Point3DTest, AddVector) {
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

TEST_F(Point3DTest, AddVectorInPlace) {
  auto p = g::Point3D(1.0, 2.0, 3.0);
  auto v = g::Vector3D(0.5, -1.0, 2.0);
  p += v;
  ASSERT_EQ(g::Point3D(1.5, 1.0, 5.0), p);

  // zero vector leaves point unchanged
  p += g::Vector3D(0, 0, 0);
  ASSERT_EQ(g::Point3D(1.5, 1.0, 5.0), p);

  // returns reference to lhs (enables chaining)
  auto p2 = g::Point3D(0.0, 0.0, 0.0);
  auto& ref = (p2 += v);
  ASSERT_EQ(&p2, &ref);
  ASSERT_EQ(g::Point3D(0.5, -1.0, 2.0), p2);
}

TEST_F(Point3DTest, ScalarMultiply) {
  auto p = g::Point3D(2.0, -3.0, 4.0);

  ASSERT_EQ(g::Point3D(4.0, -6.0, 8.0), p * 2.0);
  ASSERT_EQ(g::Point3D(1.0, -1.5, 2.0), p * 0.5);
  ASSERT_EQ(g::Point3D(0.0, 0.0, 0.0), p * 0.0);
  ASSERT_EQ(g::Point3D(-2.0, 3.0, -4.0), p * -1.0);

  // scalar on the left
  ASSERT_EQ(p * 3.0, 3.0 * p);
}

TEST_F(Point3DTest, ScalarDivide) {
  auto p = g::Point3D(4.0, -6.0, 8.0);
  ASSERT_EQ(g::Point3D(2.0, -3.0, 4.0), p / 2.0);
  ASSERT_EQ(g::Point3D(1.0, -1.5, 2.0), p / 4.0);
  ASSERT_EQ(p, p / 1.0);
  ASSERT_EQ(g::Point3D(-4.0, 6.0, -8.0), p / -1.0);
  ASSERT_EQ(p * 0.5, p / 2.0);
}

TEST_F(Point3DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto origin = g::Point3D::Zero();

  // axis-aligned
  ASSERT_EQ(1.0, g::round(origin.DistanceTo(g::Point3D(1, 0, 0))));
  ASSERT_EQ(1.0, g::round(origin.DistanceTo(g::Point3D(0, 1, 0))));
  ASSERT_EQ(1.0, g::round(origin.DistanceTo(g::Point3D(0, 0, 1))));

  // diagonal in XY plane: sqrt(2)
  geompp::DECIMAL_PRECISION = 3;
  ASSERT_EQ(g::round(std::sqrt(2.0)), g::round(origin.DistanceTo(g::Point3D(1, 1, 0))));

  // space diagonal of unit cube: sqrt(3)
  ASSERT_EQ(g::round(std::sqrt(3.0)), g::round(origin.DistanceTo(g::Point3D(1, 1, 1))));

  geompp::DECIMAL_PRECISION = 4;

  // symmetry
  auto p1 = g::Point3D(1, 2, 3);
  auto p2 = g::Point3D(4, 6, 3);
  ASSERT_EQ(g::round(p1.DistanceTo(p2)), g::round(p2.DistanceTo(p1)));

  // distance to self is 0
  ASSERT_EQ(0.0, g::round(p1.DistanceTo(p1)));
}

TEST_F(Point3DTest, AlmostEquals) {
  geompp::DECIMAL_PRECISION = 2;

  auto p = g::Point3D(1.0, 2.0, 3.0);
  ASSERT_TRUE(p.AlmostEquals(g::Point3D(1.004, 2.004, 3.004)));  // within tolerance
  ASSERT_FALSE(p.AlmostEquals(g::Point3D(1.02, 2.0, 3.0)));      // just outside

  // operator== delegates to AlmostEquals
  ASSERT_EQ(p, g::Point3D(1.004, 2.004, 3.004));
  ASSERT_NE(p, g::Point3D(1.02, 2.0, 3.0));

  geompp::DECIMAL_PRECISION = 4;
  ASSERT_FALSE(p.AlmostEquals(g::Point3D(1.004, 2.0, 3.0)));  // was OK at DP=2, not at DP=4
}

TEST_F(Point3DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  ASSERT_EQ("POINT (0 0 0)", g::Point3D::Zero().ToWkt());
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

TEST_F(Point3DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "point3d.wkt").string();
  auto p = g::Point3D(15.341, -781.684, 42.0);

  p.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Point3D p_file = g::Point3D::FromFile(path);

  EXPECT_EQ(p, p_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Point3DTest, TestFromFile) {
  std::string path = (test_res_path / "point3d" / "point.wkt").string();

  ASSERT_TRUE(fs::exists(path));
  ASSERT_NO_THROW(g::Point3D::FromFile(path));

  auto p = g::Point3D::FromFile(path);
  GEOMPP_LOG(INFO) << "from file = " << p.ToWkt();
}

TEST_F(Point3DTest, AreCollinear) {
  // on the X axis
  ASSERT_TRUE(g::are_collinear(g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(2, 0, 0)));
  ASSERT_TRUE(g::are_collinear(g::Point3D::Zero(), g::Point3D(5, 0, 0), g::Point3D(100, 0, 0)));
  // 3D diagonal
  ASSERT_TRUE(g::are_collinear(g::Point3D::Zero(), g::Point3D(1, 1, 1), g::Point3D(2, 2, 2)));

  // non-collinear
  ASSERT_FALSE(g::are_collinear(g::Point3D::Zero(), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0)));
  ASSERT_FALSE(g::are_collinear(g::Point3D::Zero(), g::Point3D(1, 1, 0), g::Point3D(0, 0, 1)));
  // triangle vertices
  ASSERT_FALSE(g::are_collinear(g::Point3D(-1, 1, 2), g::Point3D(0, -1, 2), g::Point3D(1, 1, 2)));
}

TEST_F(Point3DTest, RemoveDuplicates) {
  // clang-format off
  std::vector<g::Point3D> pts{
    g::Point3D::Zero(),
    g::Point3D::Zero(),  // duplicate
    g::Point3D::Zero(),  // duplicate
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
  ASSERT_EQ(g::Point3D::Zero(), unique[0]);
  ASSERT_EQ(g::Point3D(1, 0, 0), unique[1]);
  ASSERT_EQ(g::Point3D(2, 0, 0), unique[2]);
  ASSERT_EQ(g::Point3D(2, 2, 0), unique[3]);
  ASSERT_EQ(g::Point3D(3, 2, 1), unique[4]);

  // empty input → empty output
  ASSERT_EQ(0, g::remove_duplicates({}).size());

  // single point → returned as-is
  ASSERT_EQ(1, g::remove_duplicates({g::Point3D(1, 2, 3)}).size());
}

TEST_F(Point3DTest, RemoveCollinear) {
  // clang-format off
  std::vector<g::Point3D> pts{
    g::Point3D::Zero(),
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

  ASSERT_EQ(4, compressed.size());
  ASSERT_EQ(g::Point3D::Zero(), compressed[0]);
  ASSERT_EQ(g::Point3D(2, 0, 0), compressed[1]);  // farthest on X run
  ASSERT_EQ(g::Point3D(2, 3, 0), compressed[2]);  // farthest on Y run
  ASSERT_EQ(g::Point3D(5, 3, 0), compressed[3]);  // farthest on X run

  // fewer than 3 points → returned unchanged
  ASSERT_EQ(2, g::remove_collinear({g::Point3D::Zero(), g::Point3D(1, 0, 0)}).size());
}

TEST_F(Point3DTest, Average) {
  // clang-format off
  std::vector<g::Point3D> pts{
    g::Point3D::Zero(),
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

TEST_F(Point3DTest, LinearCombination) {
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

TEST_F(Point3DTest, FromVector) {
  // explicit construction from a vector copies x/y/z components
  auto v = g::Vector3D(3.0, -4.5, 6.25);
  auto p = g::Point3D(v);
  ASSERT_EQ(3.0, p.x());
  ASSERT_EQ(-4.5, p.y());
  ASSERT_EQ(6.25, p.z());
  ASSERT_EQ(g::Point3D(3.0, -4.5, 6.25), p);

  // implicit conversion: Point3D parameter accepts a Vector3D
  auto via_implicit = [](g::Point3D const& q) { return q; }(g::Vector3D(7.5, 8.25, -9.5));
  ASSERT_EQ(g::Point3D(7.5, 8.25, -9.5), via_implicit);

  // round-trip: Point3D → Vector3D → Point3D
  auto p0 = g::Point3D(1.25, -2.75, 0.5);
  auto roundtrip = g::Point3D(p0.ToVector());
  ASSERT_EQ(p0, roundtrip);
}

}  // namespace geompp_tests
