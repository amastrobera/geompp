#include "point2d.hpp"

#include "utils.hpp"
#include "vector2d.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <cmath>
#include <filesystem>
#include <vector>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Point2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Point2DTest, Equality) {
  ASSERT_EQ(g::Point2D(2.56, 748.1203), g::Point2D(2.56, 748.1203));
  ASSERT_EQ(g::Point2D(2, 3), g::Point2D(2, 3));
  ASSERT_EQ(g::Point2D(-56.682, 30.56), g::Point2D(-56.682, 30.56));
  ASSERT_EQ(g::Point2D(-672.6456, -153.516), g::Point2D(-672.6456, -153.516));

  ASSERT_EQ(g::Point2D(672.64560944, -153.5166067079), g::Point2D(672.64560944, -153.5166067079));
}  // namespace std::filesystem

TEST_F(Point2DTest, SubtractPoint) {
  auto p1 = g::Point2D(1, 2);
  auto p2 = g::Point2D(1, 1);
  g::Vector2D v = p2 - p1;

  ASSERT_EQ(0, v.x());
  ASSERT_EQ(-1, v.y());

  ASSERT_EQ(g::Vector2D(0, -1), v);
}

TEST_F(Point2DTest, AddVector) {
  auto p = g::Point2D(1.2, 2.12);
  auto v = g::Vector2D(1.85, 1.65);
  g::Point2D pv = p + v;

  ASSERT_EQ(3.05, pv.x());
  ASSERT_EQ(3.77, pv.y());

  ASSERT_EQ(g::Point2D(3.05, 3.77), pv);
}

TEST_F(Point2DTest, AddVectorInPlace) {
  auto p = g::Point2D(1.0, 2.0);
  auto v = g::Vector2D(0.5, -1.0);
  p += v;
  ASSERT_EQ(g::Point2D(1.5, 1.0), p);

  // zero vector leaves point unchanged
  p += g::Vector2D(0, 0);
  ASSERT_EQ(g::Point2D(1.5, 1.0), p);

  // returns reference to lhs (enables chaining)
  auto p2 = g::Point2D(0.0, 0.0);
  auto& ref = (p2 += v);
  ASSERT_EQ(&p2, &ref);
  ASSERT_EQ(g::Point2D(0.5, -1.0), p2);
}

TEST_F(Point2DTest, ScalarDivide) {
  auto p = g::Point2D(4.0, -6.0);
  ASSERT_EQ(g::Point2D(2.0, -3.0), p / 2.0);
  ASSERT_EQ(g::Point2D(1.0, -1.5), p / 4.0);
  ASSERT_EQ(p, p / 1.0);
  ASSERT_EQ(g::Point2D(-4.0, 6.0), p / -1.0);
  ASSERT_EQ(p * 0.5, p / 2.0);
}

TEST_F(Point2DTest, Wkt) {
  ASSERT_EQ("POINT (0 0)", g::Point2D::Zero().ToWkt());
  geompp::DECIMAL_PRECISION = 2;
  ASSERT_EQ("POINT (56491.62 -795.97)", g::Point2D(56491.6164, -795.97416).ToWkt());

  geompp::DECIMAL_PRECISION = 4;
  EXPECT_EQ(g::Point2D(256.1343, -684.64971), g::Point2D::FromWkt("POINT (256.1343 -684.64971)"));
  EXPECT_EQ(g::Point2D(-7.5, -60.7), g::Point2D::FromWkt("  point( -7.5    -60.7)"));
  EXPECT_EQ(g::Point2D(0.645, -1.689741), g::Point2D::FromWkt("PoInT   ( 0.645  -1.689741  )"));
  EXPECT_EQ(g::Point2D(0, 1), g::Point2D::FromWkt("POINT (  0  1  )"));

  EXPECT_ANY_THROW(g::Point2D::FromWkt("angelo"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("poin ( -7.5 -60.7)"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point -7.5 -64.4)"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point (-7.5 -64.4"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( -7.5 )"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( )"));
  EXPECT_ANY_THROW(g::Point2D::FromWkt("point ( -7.5 -64.4 15.5)"));
}

TEST_F(Point2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "point.wkt").string();
  auto p = g::Point2D(15.341, -781.684);

  p.ToFile(path);
  ASSERT_TRUE(fs::exists(path));

  g::Point2D p_file = g::Point2D::FromFile(path);  // TODO make assert no throw for the whole call

  EXPECT_EQ(p, p_file);
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Point2DTest, TestFromFile) {
  std::string path = (test_res_path / "point2d" / "point.wkt").string();

  ASSERT_TRUE(fs::exists(path));

  ASSERT_NO_THROW(g::Point2D::FromFile(path));

  auto p = g::Point2D::FromFile(path);

  GEOMPP_LOG(INFO) << "form file = " << p.ToWkt();
}

TEST_F(Point2DTest, DistanceTo) {
  geompp::DECIMAL_PRECISION = 4;
  auto p1 = g::Point2D::Zero();
  auto p2 = g::Point2D(1, 0);

  ASSERT_EQ(1, g::round(p1.DistanceTo(p2)));
  ASSERT_EQ(1, g::round(p2.DistanceTo(p1)));

  auto p3 = g::Point2D(-1, -1);

  EXPECT_EQ(1.4142, g::round(p1.DistanceTo(p3)));
}

TEST_F(Point2DTest, RemoveDuplicates) {
  // clang-format off
  std::vector<g::Point2D> pts{g::Point2D::Zero(),
                              g::Point2D::Zero(), // duplicate
                              g::Point2D::Zero(), // duplicate
                              g::Point2D(1, 0),
                              g::Point2D(2, 0),
                              g::Point2D(2, 2),
                              g::Point2D(2, 2), // duplicate
                              g::Point2D(3, 2),
                              g::Point2D(3, 2) // duplicate
                              };
  // clang-format on
  ASSERT_EQ(9, pts.size());

  auto unique_pts = g::remove_duplicates(pts);

  ASSERT_EQ(5, unique_pts.size());
  ASSERT_EQ(g::Point2D::Zero(), unique_pts[0]);
  ASSERT_EQ(g::Point2D(1, 0), unique_pts[1]);
  ASSERT_EQ(g::Point2D(2, 0), unique_pts[2]);
  ASSERT_EQ(g::Point2D(2, 2), unique_pts[3]);
  ASSERT_EQ(g::Point2D(3, 2), unique_pts[4]);
}

TEST_F(Point2DTest, RemoveCollinear) {
  // clang-format off
  std::vector<g::Point2D> pts{g::Point2D::Zero(),
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

  auto unique_pts = g::remove_collinear(pts);

  ASSERT_EQ(g::Point2D::Zero(), unique_pts[0]);
  ASSERT_EQ(g::Point2D(2, 0), unique_pts[1]);
  ASSERT_EQ(g::Point2D(2, 5), unique_pts[2]);
  ASSERT_EQ(g::Point2D(3, 6), unique_pts[3]) << "value=" << unique_pts[3].ToWkt();
  ASSERT_EQ(g::Point2D(5, 4), unique_pts[4]);
  ASSERT_EQ(g::Point2D(6, 0), unique_pts[5]);
  ASSERT_EQ(g::Point2D(7, 0), unique_pts[6]);
}

TEST_F(Point2DTest, Centroid_Square) {
  // 4×4 CCW square → centroid at (2, 2)
  std::vector<g::Point2D> pts = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  auto c = g::centroid(pts);
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
}

TEST_F(Point2DTest, Centroid_Triangle) {
  // right triangle (0,0),(4,0),(0,3) → centroid = (4/3, 1)
  std::vector<g::Point2D> pts = {g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3)};
  auto c = g::centroid(pts);
  EXPECT_NEAR(4.0 / 3.0, c.x(), 1e-9);
  EXPECT_NEAR(1.0,        c.y(), 1e-9);
}

TEST_F(Point2DTest, Centroid_Rectangle) {
  // 6×2 rectangle → centroid at (3, 1)
  std::vector<g::Point2D> pts = {
      g::Point2D(0, 0), g::Point2D(6, 0), g::Point2D(6, 2), g::Point2D(0, 2)};
  auto c = g::centroid(pts);
  EXPECT_NEAR(3.0, c.x(), 1e-9);
  EXPECT_NEAR(1.0, c.y(), 1e-9);
}

TEST_F(Point2DTest, Centroid_ZeroArea_Throws) {
  // collinear points → signed_area == 0 → throws
  std::vector<g::Point2D> pts = {g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(2, 0)};
  EXPECT_ANY_THROW(g::centroid(pts));
}

TEST_F(Point2DTest, Average) {
  // clang-format off
  std::vector<g::Point2D> pts{g::Point2D::Zero(),
                              g::Point2D(1, 0),
                              g::Point2D(2, -3),
                              g::Point2D(-5, 6),
                              g::Point2D(7, -1),
                              g::Point2D(-2, -8),
                              g::Point2D(0, 1)
                              };
  // clang-format on
  ASSERT_EQ(g::Point2D(3.0 / pts.size(), -5.0 / pts.size()), g::average(pts));
}

TEST_F(Point2DTest, FromVector) {
  // explicit construction from a vector copies x/y components
  auto v = g::Vector2D(3.0, -4.5);
  auto p = g::Point2D(v);
  ASSERT_EQ(3.0, p.x());
  ASSERT_EQ(-4.5, p.y());
  ASSERT_EQ(g::Point2D(3.0, -4.5), p);

  // implicit conversion: Point2D parameter accepts a Vector2D
  auto via_implicit = [](g::Point2D const& q) { return q; }(g::Vector2D(7.5, 8.25));
  ASSERT_EQ(g::Point2D(7.5, 8.25), via_implicit);

  // round-trip: Point2D → Vector2D → Point2D
  auto p0 = g::Point2D(1.25, -2.75);
  auto roundtrip = g::Point2D(p0.ToVector());
  ASSERT_EQ(p0, roundtrip);
}

TEST_F(Point2DTest, IsLeft) {
  // directed edge v1->v2 pointing right along +x; cross = (v2-v1) x (p-v1) = 4 * p.y
  auto v1 = g::Point2D(0, 0);
  auto v2 = g::Point2D(4, 0);

  EXPECT_TRUE(g::is_left(v1, v2, g::Point2D(2, 1)));    // above the rightward edge -> left
  EXPECT_FALSE(g::is_left(v1, v2, g::Point2D(2, -1)));  // below -> not left
  EXPECT_FALSE(g::is_left(v1, v2, g::Point2D(2, 0)));   // on the edge -> not left (strict)

  // reversed (leftward) edge flips the sense
  EXPECT_TRUE(g::is_left(v2, v1, g::Point2D(2, -1)));   // below a leftward edge -> left
  EXPECT_FALSE(g::is_left(v2, v1, g::Point2D(2, 1)));
}

TEST_F(Point2DTest, IsRight) {
  auto v1 = g::Point2D(0, 0);
  auto v2 = g::Point2D(4, 0);

  EXPECT_TRUE(g::is_right(v1, v2, g::Point2D(2, -1)));  // below the rightward edge -> right
  EXPECT_FALSE(g::is_right(v1, v2, g::Point2D(2, 1)));  // above -> not right
  EXPECT_FALSE(g::is_right(v1, v2, g::Point2D(2, 0)));  // on the edge -> not right (strict)
}

TEST_F(Point2DTest, IsLeftIsRightAreMutuallyExclusiveOffTheLine) {
  auto v1 = g::Point2D(0, 0);
  auto v2 = g::Point2D(1, 1);
  auto p = g::Point2D(0, 1);  // above the diagonal y = x

  EXPECT_TRUE(g::is_left(v1, v2, p));
  EXPECT_FALSE(g::is_right(v1, v2, p));
}

// --------------------------------------------------------------------------------------------------
// convex_hull
// --------------------------------------------------------------------------------------------------

TEST_F(Point2DTest, ConvexHull_TooFewPoints_ReturnsAsIs) {
  std::vector<g::Point2D> two{g::Point2D(0, 0), g::Point2D(1, 1)};
  auto hull = g::convex_hull(two);
  ASSERT_EQ(hull.size(), 2u);
}

TEST_F(Point2DTest, ConvexHull_ConvexSquare_ReturnsSamePoints) {
  std::vector<g::Point2D> pts{
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  auto hull = g::convex_hull(pts);
  ASSERT_EQ(hull.size(), 4u);
  for (auto const& p : pts) {
    bool found = false;
    for (auto const& h : hull) {
      if (h.AlmostEquals(p)) { found = true; break; }
    }
    EXPECT_TRUE(found) << "square corner " << p.ToWkt() << " should be on the hull";
  }
}

// Asymmetric 5-pointed star: outer tips at different radii, inner concave vertices near center.
// The convex hull must contain exactly the 5 outer tips and exclude all inner vertices.
TEST_F(Point2DTest, ConvexHull_AsymmetricStar_HullIsPentagon) {
  // outer tips (unequal distances from origin)
  g::Point2D tip0( 0,  5);
  g::Point2D tip1( 4,  2);
  g::Point2D tip2( 3, -3);
  g::Point2D tip3(-2, -4);
  g::Point2D tip4(-3,  1);

  // inner concave vertices (close to origin — well inside the outer tips' pentagon)
  g::Point2D inner0( 2,  1);
  g::Point2D inner1( 2, -1);
  g::Point2D inner2( 0, -1);
  g::Point2D inner3(-1, -1);
  g::Point2D inner4(-1,  2);

  std::vector<g::Point2D> star{
      tip0, inner0, tip1, inner1, tip2, inner2, tip3, inner3, tip4, inner4};

  auto hull = g::convex_hull(star);

  ASSERT_EQ(hull.size(), 5u) << "expected the 5 outer tips as the convex hull";

  std::vector<g::Point2D> outer_tips{tip0, tip1, tip2, tip3, tip4};
  for (auto const& tip : outer_tips) {
    bool found = false;
    for (auto const& h : hull) {
      if (h.AlmostEquals(tip)) { found = true; break; }
    }
    EXPECT_TRUE(found) << "outer tip " << tip.ToWkt() << " should be on the hull";
  }

  std::vector<g::Point2D> inner_pts{inner0, inner1, inner2, inner3, inner4};
  for (auto const& ip : inner_pts) {
    bool found = false;
    for (auto const& h : hull) {
      if (h.AlmostEquals(ip)) { found = true; break; }
    }
    EXPECT_FALSE(found) << "inner point " << ip.ToWkt() << " should NOT be on the hull";
  }
}

}  // namespace geompp_tests
