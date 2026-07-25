#include "polymesh2d.hpp"

#include "point2d.hpp"
#include "polygon2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class PolyMesh2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(PolyMesh2DTest, FromPolygons_Empty_Throws) {
  EXPECT_THROW(g::PolyMesh2D::FromPolygons({}), std::invalid_argument);
}

TEST_F(PolyMesh2DTest, FromPolygons_PolygonWithHoles_Throws) {
  auto outer = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)},
                                   {{g::Point2D(1, 1), g::Point2D(1, 2), g::Point2D(2, 2), g::Point2D(2, 1)}});
  EXPECT_THROW(g::PolyMesh2D::FromPolygons({outer}), std::invalid_argument);
}

TEST_F(PolyMesh2DTest, FromPolygons_SingleQuad) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p});

  EXPECT_EQ(1u, mesh.Size());
  EXPECT_NEAR(1.0, mesh.Area(), 1e-9);
}

TEST_F(PolyMesh2DTest, OperatorBracket_OutOfRange_Throws) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p});
  EXPECT_THROW(mesh[1], std::out_of_range);
}

TEST_F(PolyMesh2DTest, OperatorBracket_SingleQuad_MatchesInput) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p});
  EXPECT_TRUE(p.AlmostEquals(mesh[0]));
}

// Regression test: two quads sharing an edge must weld shared vertices, and operator[]
// must return each face's TRUE vertices in the correct order — this exercises the
// PolyMesh2D::FromPolygons index-shift bug fixed alongside these tests, including the
// last-registered vertex (previously one-past-the-end / out of bounds).
TEST_F(PolyMesh2DTest, FromPolygons_SharedEdge_WeldsVerticesAndPreservesFaces) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_NEAR(2.0, mesh.Area(), 1e-9);

  EXPECT_TRUE(p0.AlmostEquals(mesh[0])) << "first face's vertices were not preserved (possible index shift)";
  EXPECT_TRUE(p1.AlmostEquals(mesh[1])) << "second face's vertices were not preserved (possible index shift)";
}

TEST_F(PolyMesh2DTest, FromPolygons_MixedFaceSizes) {
  // A triangle and a quad sharing one edge — exercises the variable-length face buffer.
  auto tri = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0.5, 1)});
  auto quad = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(0.5, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({tri, quad});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_TRUE(tri.AlmostEquals(mesh[0]));
  EXPECT_TRUE(quad.AlmostEquals(mesh[1]));
}

TEST_F(PolyMesh2DTest, Faces_MatchesOperatorBracket) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  std::size_t i = 0;
  for (auto const& face : mesh.Faces()) {
    EXPECT_TRUE(face.AlmostEquals(mesh[i])) << "Faces() view diverges from operator[] at " << i;
    ++i;
  }
  EXPECT_EQ(mesh.Size(), i);
}

}  // namespace geompp_tests
