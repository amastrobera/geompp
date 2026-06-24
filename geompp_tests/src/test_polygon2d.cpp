#include "polygon2d.hpp"

#include "point2d.hpp"
#include "utils.hpp"

#include "geompp_log.hpp"

#include <gtest/gtest.h>
#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Polygon2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Polygon2DTest, Constructor) {
  auto p = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_EQ(4, p.Size());

  ASSERT_EQ(g::Point2D::Zero(), p[0]);
  ASSERT_EQ(g::Point2D(1, 0), p[1]);
  ASSERT_EQ(g::Point2D(1, 1), p[2]);
  ASSERT_EQ(g::Point2D(0, 1), p[3]);

  EXPECT_ANY_THROW(p[4]);  // out of range

  // fewer than 3 unique points throws
  EXPECT_ANY_THROW(g::Polygon2D::Make({}));
  EXPECT_ANY_THROW(g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D::Zero(), g::Point2D::Zero()}));
  EXPECT_ANY_THROW(g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0)}));

  // CW outer ring throws
  EXPECT_ANY_THROW(
      g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(0, 1), g::Point2D(1, 1), g::Point2D(1, 0)}));
}

TEST_F(Polygon2DTest, AlmostEquals) {
  auto p1 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p2 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p3 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(2, 0), g::Point2D(2, 2)});

  ASSERT_TRUE(p1.AlmostEquals(p2));
  ASSERT_FALSE(p1.AlmostEquals(p3));
  ASSERT_EQ(p1, p2);
  ASSERT_NE(p1, p3);

  // different sizes are not equal
  auto p4 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_FALSE(p1.AlmostEquals(p4));
}

TEST_F(Polygon2DTest, Assignment) {
  auto p1 = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto p2 = g::Polygon2D::Make({g::Point2D(5, 5), g::Point2D(6, 5), g::Point2D(6, 6)});
  p2 = p1;
  ASSERT_EQ(p1, p2);
}

TEST_F(Polygon2DTest, Wkt) {
  geompp::DECIMAL_PRECISION = 4;
  auto p = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  ASSERT_EQ("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))", p.ToWkt());

  // CCW triangle at DP=2
  geompp::DECIMAL_PRECISION = 2;
  auto p2 = g::Polygon2D::Make({g::Point2D(0.123, 0.456), g::Point2D(8.789, 0.123), g::Point2D(4.321, 7.654)});
  ASSERT_EQ("POLYGON ((0.12 0.46, 8.79 0.12, 4.32 7.65, 0.12 0.46))", p2.ToWkt());

  // FromWkt round-trips correctly
  geompp::DECIMAL_PRECISION = 4;
  auto q = g::Polygon2D::FromWkt("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))");
  EXPECT_EQ(4, q.Size());
  EXPECT_EQ(g::Point2D(0, 0), q[0]);
  EXPECT_EQ(g::Point2D(1, 0), q[1]);
  // invalid input still throws
  EXPECT_ANY_THROW(g::Polygon2D::FromWkt("anything"));
}

TEST_F(Polygon2DTest, WktWithHoles) {
  geompp::DECIMAL_PRECISION = 4;
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto p = g::Polygon2D::Make(outer, {hole});
  ASSERT_EQ("POLYGON ((0 0, 4 0, 4 4, 0 4, 0 0), (1 1, 1 3, 3 3, 3 1, 1 1))", p.ToWkt());
}

TEST_F(Polygon2DTest, WithHoles_Valid) {
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto p = g::Polygon2D::Make(outer, {hole});
  ASSERT_EQ(4, p.Size());  // outer ring has 4 vertices
}

TEST_F(Polygon2DTest, WithHoles_PerimeterCW_Throws) {
  std::vector<g::Point2D> cw_outer = {
      g::Point2D(0, 0), g::Point2D(0, 4), g::Point2D(4, 4), g::Point2D(4, 0)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  EXPECT_ANY_THROW(g::Polygon2D::Make(cw_outer, {hole}));
}

TEST_F(Polygon2DTest, WithHoles_HoleCCW_Throws) {
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> ccw_hole = {
      g::Point2D(1, 1), g::Point2D(3, 1), g::Point2D(3, 3), g::Point2D(1, 3)};
  EXPECT_ANY_THROW(g::Polygon2D::Make(outer, {ccw_hole}));
}

TEST_F(Polygon2DTest, WithHoles_HoleTooFewPoints_Throws) {
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> bad_hole = {g::Point2D(1, 1), g::Point2D(2, 1)};
  EXPECT_ANY_THROW(g::Polygon2D::Make(outer, {bad_hole}));
}

TEST_F(Polygon2DTest, ToFile) {
  geompp::DECIMAL_PRECISION = 4;
  std::string path = (test_res_path / "temp" / "polygon2d.wkt").string();
  auto p = g::Polygon2D::Make({g::Point2D::Zero(), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});

  p.ToFile(path);
  ASSERT_TRUE(fs::exists(path));
  EXPECT_NO_THROW(fs::remove(path));
}

TEST_F(Polygon2DTest, FromFile) {
  std::string path = (test_res_path / "polygon2d" / "polygon.wkt").string();
  ASSERT_TRUE(fs::exists(path));
  auto p = g::Polygon2D::FromFile(path);
  EXPECT_EQ(4, p.Size());
}

// ---- Area -------------------------------------------------------------------

TEST_F(Polygon2DTest, Area_Square) {
  // 1×1 square → area = 1
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  EXPECT_NEAR(1.0, p.Area(), 1e-9);
}

TEST_F(Polygon2DTest, Area_Rectangle) {
  // 3 wide × 4 tall → area = 12
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(3, 0), g::Point2D(3, 4), g::Point2D(0, 4)});
  EXPECT_NEAR(12.0, p.Area(), 1e-9);
}

TEST_F(Polygon2DTest, Area_RightTriangle) {
  // base=4, height=3 → area = 0.5 × 4 × 3 = 6
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3)});
  EXPECT_NEAR(6.0, p.Area(), 1e-9);
}

TEST_F(Polygon2DTest, Area_SquareWithOneHole) {
  // 4×4 outer (area=16) minus 2×2 CW hole (area=4) = 12
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto p = g::Polygon2D::Make(outer, {hole});
  EXPECT_NEAR(12.0, p.Area(), 1e-9);
}

TEST_F(Polygon2DTest, Area_RectangleWithTwoHoles) {
  // 6×4 outer (area=24) minus two 1×2 CW holes (area=2 each) = 20
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(6, 0), g::Point2D(6, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole1 = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(2, 3), g::Point2D(2, 1)};
  std::vector<g::Point2D> hole2 = {
      g::Point2D(4, 1), g::Point2D(4, 3), g::Point2D(5, 3), g::Point2D(5, 1)};
  auto p = g::Polygon2D::Make(outer, {hole1, hole2});
  EXPECT_NEAR(20.0, p.Area(), 1e-9);
}

// ---- Centroid ---------------------------------------------------------------

TEST_F(Polygon2DTest, Centroid_Square) {
  auto p = g::Polygon2D::Make({
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)});
  auto c = p.Centroid();
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
}

TEST_F(Polygon2DTest, Centroid_Rectangle) {
  auto p = g::Polygon2D::Make({
      g::Point2D(0, 0), g::Point2D(6, 0), g::Point2D(6, 2), g::Point2D(0, 2)});
  auto c = p.Centroid();
  EXPECT_NEAR(3.0, c.x(), 1e-9);
  EXPECT_NEAR(1.0, c.y(), 1e-9);
}

TEST_F(Polygon2DTest, Centroid_Triangle) {
  // right triangle (0,0),(4,0),(0,3) → (4/3, 1)
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3)});
  auto c = p.Centroid();
  EXPECT_NEAR(4.0 / 3.0, c.x(), 1e-9);
  EXPECT_NEAR(1.0,        c.y(), 1e-9);
}

TEST_F(Polygon2DTest, Centroid_SquareWithCenteredHole) {
  // 4×4 outer, 2×2 symmetric CW hole → centroid stays at (2, 2)
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)};
  auto p = g::Polygon2D::Make(outer, {hole});
  auto c = p.Centroid();
  EXPECT_NEAR(2.0, c.x(), 1e-9);
  EXPECT_NEAR(2.0, c.y(), 1e-9);
}

TEST_F(Polygon2DTest, Centroid_SquareWithOffCenterHole) {
  // 4×4 outer (area=16, centroid=(2,2)), 2×2 CW hole in top-right ([2,4]×[2,4])
  // hole centroid=(3,3), hole signed_area≈-4
  // c = (16*(2,2) + (-4)*(3,3)) / (16-4) = (20,20)/12 = (5/3, 5/3)
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)};
  std::vector<g::Point2D> hole = {
      g::Point2D(2, 2), g::Point2D(2, 4), g::Point2D(4, 4), g::Point2D(4, 2)};
  auto p = g::Polygon2D::Make(outer, {hole});
  auto c = p.Centroid();
  EXPECT_NEAR(5.0 / 3.0, c.x(), 1e-9);
  EXPECT_NEAR(5.0 / 3.0, c.y(), 1e-9);
}

// ---- Perimeter --------------------------------------------------------------

TEST_F(Polygon2DTest, Perimeter_Square) {
  // 1×1 square → 4 sides of length 1
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  EXPECT_NEAR(4.0, p.Perimeter(), 1e-9);
}

TEST_F(Polygon2DTest, Perimeter_Rectangle) {
  // 3×4 rectangle → 2*(3+4) = 14
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(3, 0), g::Point2D(3, 4), g::Point2D(0, 4)});
  EXPECT_NEAR(14.0, p.Perimeter(), 1e-9);
}

TEST_F(Polygon2DTest, Perimeter_Triangle) {
  // 3-4-5 right triangle → perimeter = 12
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3)});
  EXPECT_NEAR(12.0, p.Perimeter(), 1e-9);
}

TEST_F(Polygon2DTest, DistanceTo) {
  auto poly = g::Polygon2D::Make({g::Point2D(0,0), g::Point2D(1,0), g::Point2D(1,1), g::Point2D(0,1)});
  EXPECT_ANY_THROW(poly.DistanceTo(g::Point2D(0.5, 0.5)));
}

TEST_F(Polygon2DTest, Contains) {
  // unit square
  auto sq = g::Polygon2D::Make({g::Point2D(0,0), g::Point2D(1,0), g::Point2D(1,1), g::Point2D(0,1)});
  EXPECT_TRUE(sq.Contains(g::Point2D(0.5, 0.5)));    // center
  EXPECT_TRUE(sq.Contains(g::Point2D(0.1, 0.1)));    // near corner
  EXPECT_TRUE(sq.Contains(g::Point2D(0.9, 0.9)));    // near corner
  EXPECT_FALSE(sq.Contains(g::Point2D(-0.1, 0.5)));  // left of square
  EXPECT_FALSE(sq.Contains(g::Point2D(1.1, 0.5)));   // right of square
  EXPECT_FALSE(sq.Contains(g::Point2D(0.5, -0.1)));  // below square
  EXPECT_FALSE(sq.Contains(g::Point2D(0.5, 1.1)));   // above square
  EXPECT_FALSE(sq.Contains(g::Point2D(5, 5)));        // far outside

  // 4x4 square with 2x2 centred hole
  auto outer = std::vector<g::Point2D>{{0,0}, {4,0}, {4,4}, {0,4}};
  auto hole  = std::vector<g::Point2D>{{1,1}, {1,3}, {3,3}, {3,1}};
  auto poly  = g::Polygon2D::Make(outer, {hole});
  EXPECT_TRUE(poly.Contains(g::Point2D(0.5, 0.5)));  // inner ring of outer, outside hole
  EXPECT_TRUE(poly.Contains(g::Point2D(3.5, 3.5)));  // inner ring of outer, outside hole
  EXPECT_FALSE(poly.Contains(g::Point2D(2, 2)));     // inside hole
  EXPECT_FALSE(poly.Contains(g::Point2D(-1, 2)));    // outside outer
}

TEST_F(Polygon2DTest, Contains_OnBoundary) {
  // Boundary is included (matches Triangle behaviour).
  auto sq = g::Polygon2D::Make({g::Point2D(0,0), g::Point2D(1,0), g::Point2D(1,1), g::Point2D(0,1)});

  // vertices
  EXPECT_TRUE(sq.Contains(g::Point2D(0, 0)));    // bottom-left vertex
  EXPECT_TRUE(sq.Contains(g::Point2D(1, 0)));    // bottom-right vertex
  EXPECT_TRUE(sq.Contains(g::Point2D(1, 1)));    // top-right vertex
  EXPECT_TRUE(sq.Contains(g::Point2D(0, 1)));    // top-left vertex

  // edge midpoints (horizontal, vertical)
  EXPECT_TRUE(sq.Contains(g::Point2D(0.5, 0)));  // bottom edge
  EXPECT_TRUE(sq.Contains(g::Point2D(1,   0.5)));// right edge
  EXPECT_TRUE(sq.Contains(g::Point2D(0.5, 1)));  // top edge
  EXPECT_TRUE(sq.Contains(g::Point2D(0,   0.5)));// left edge

  // polygon with hole: boundary of outer ring and boundary of hole both count
  auto outer = std::vector<g::Point2D>{{0,0}, {4,0}, {4,4}, {0,4}};
  auto hole  = std::vector<g::Point2D>{{1,1}, {1,3}, {3,3}, {3,1}};
  auto poly  = g::Polygon2D::Make(outer, {hole});
  EXPECT_TRUE(poly.Contains(g::Point2D(2,   0)));  // outer bottom edge
  EXPECT_TRUE(poly.Contains(g::Point2D(4,   2)));  // outer right edge
  EXPECT_TRUE(poly.Contains(g::Point2D(2,   1)));  // hole bottom edge
  EXPECT_TRUE(poly.Contains(g::Point2D(1,   2)));  // hole left edge
}

TEST_F(Polygon2DTest, IsOnBoundary_True) {
  auto sq = g::Polygon2D::Make({g::Point2D(0,0), g::Point2D(1,0), g::Point2D(1,1), g::Point2D(0,1)});

  // all four vertices
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(0,   0)));
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(1,   0)));
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(1,   1)));
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(0,   1)));

  // edge midpoints
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(0.5, 0)));    // bottom
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(1,   0.5)));  // right
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(0.5, 1)));    // top
  EXPECT_TRUE(sq.IsOnBoundary(g::Point2D(0,   0.5)));  // left

  // hole boundary: both outer and hole edges count
  auto outer = std::vector<g::Point2D>{{0,0}, {4,0}, {4,4}, {0,4}};
  auto hole  = std::vector<g::Point2D>{{1,1}, {1,3}, {3,3}, {3,1}};
  auto poly  = g::Polygon2D::Make(outer, {hole});
  EXPECT_TRUE(poly.IsOnBoundary(g::Point2D(2, 0)));  // outer bottom
  EXPECT_TRUE(poly.IsOnBoundary(g::Point2D(4, 2)));  // outer right
  EXPECT_TRUE(poly.IsOnBoundary(g::Point2D(2, 1)));  // hole bottom
  EXPECT_TRUE(poly.IsOnBoundary(g::Point2D(1, 2)));  // hole left
}

TEST_F(Polygon2DTest, IsOnBoundary_False) {
  auto sq = g::Polygon2D::Make({g::Point2D(0,0), g::Point2D(1,0), g::Point2D(1,1), g::Point2D(0,1)});

  EXPECT_FALSE(sq.IsOnBoundary(g::Point2D(0.5, 0.5)));   // interior
  EXPECT_FALSE(sq.IsOnBoundary(g::Point2D(-0.1, 0.5)));  // outside left
  EXPECT_FALSE(sq.IsOnBoundary(g::Point2D(1.1,  0.5)));  // outside right
  EXPECT_FALSE(sq.IsOnBoundary(g::Point2D(0.5, -0.1)));  // outside below
  EXPECT_FALSE(sq.IsOnBoundary(g::Point2D(0.5,  1.1)));  // outside above

  // interior of polygon with hole is not boundary
  auto outer = std::vector<g::Point2D>{{0,0}, {4,0}, {4,4}, {0,4}};
  auto hole  = std::vector<g::Point2D>{{1,1}, {1,3}, {3,3}, {3,1}};
  auto poly  = g::Polygon2D::Make(outer, {hole});
  EXPECT_FALSE(poly.IsOnBoundary(g::Point2D(0.5, 0.5)));  // interior strip
  EXPECT_FALSE(poly.IsOnBoundary(g::Point2D(2,   2)));    // inside hole
}

TEST_F(Polygon2DTest, ToSegments) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto segs = p.ToSegments();

  // N vertices → N segments (closed ring)
  ASSERT_EQ(4, segs.size());

  // first segment: (0,0) → (1,0)
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0)), segs[0]);

  // last segment wraps back: (0,1) → (0,0)
  EXPECT_EQ(g::LineSegment2D::Make(g::Point2D(0, 1), g::Point2D(0, 0)), segs[3]);

  // iterate via range-for
  int count = 0;
  for (auto const& s : segs) {
    EXPECT_NEAR(1.0, s.Length(), 1e-9);
    ++count;
  }
  EXPECT_EQ(4, count);
}

// NOTE: IsSimple() correctness depends on the (currently provisional) sweep-line comparator; these encode the
// intended behaviour and are expected to be re-verified once the real sweep-status ordering is in place.
TEST_F(Polygon2DTest, IsSimple_Smoke_ReturnsWithoutThrowing) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 2), g::Point2D(0, 2)});
  EXPECT_NO_THROW({
    bool s = p.IsSimple();
    (void)s;
  });
}

TEST_F(Polygon2DTest, IsSimple_ConvexSquareIsSimple) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  EXPECT_TRUE(p.IsSimple());
}

TEST_F(Polygon2DTest, IsSimple_SelfIntersectingIsNotSimple) {
  // CCW (positive signed area) but the (4,0)->(1,3) and (3,3)->(0,0) edges cross at (2,2)
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(1, 3), g::Point2D(3, 3)});
  EXPECT_FALSE(p.IsSimple());
}

// ---- IsConvex ---------------------------------------------------------------

TEST_F(Polygon2DTest, IsConvex_Square_True) {
  // A unit square is convex
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  EXPECT_TRUE(p.IsConvex());
}

TEST_F(Polygon2DTest, IsConvex_Triangle_True) {
  // A triangle is always convex
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3)});
  EXPECT_TRUE(p.IsConvex());
}

TEST_F(Polygon2DTest, IsConvex_ConcavePolygon_False) {
  // L-shaped / arrow polygon — has a dent at (2,2), making it concave
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(2, 2), g::Point2D(0, 4)});
  EXPECT_FALSE(p.IsConvex());
}

TEST_F(Polygon2DTest, IsConvex_WithHole_False) {
  // Any polygon with holes is non-convex by definition
  std::vector<g::Point2D> outer = {
      g::Point2D(0, 0), g::Point2D(3, 0), g::Point2D(3, 3), g::Point2D(0, 3)};
  std::vector<g::Point2D> hole = {
      g::Point2D(1, 1), g::Point2D(1, 2), g::Point2D(2, 2), g::Point2D(2, 1)};  // CW
  auto p = g::Polygon2D::Make(outer, {hole});
  EXPECT_FALSE(p.IsConvex());
}

}  // namespace geompp_tests
