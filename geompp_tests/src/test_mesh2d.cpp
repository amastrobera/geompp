#include "mesh2d.hpp"

#include "point2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class Mesh2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Mesh2DTest, FromTriangles_Empty_Throws) {
  EXPECT_THROW(g::Mesh2D::FromTriangles({}), std::invalid_argument);
}

TEST_F(Mesh2DTest, FromTriangles_SingleTriangle) {
  auto t = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0, 1));
  auto mesh = g::Mesh2D::FromTriangles({t});

  EXPECT_EQ(1u, mesh.Size());
  EXPECT_NEAR(0.5, mesh.Area(), 1e-9);
}

TEST_F(Mesh2DTest, OperatorBracket_OutOfRange_Throws) {
  auto t = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0, 1));
  auto mesh = g::Mesh2D::FromTriangles({t});
  EXPECT_THROW(mesh[1], std::out_of_range);
}

TEST_F(Mesh2DTest, OperatorBracket_SingleTriangle_MatchesInput) {
  auto t = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3));
  auto mesh = g::Mesh2D::FromTriangles({t});
  auto out = mesh[0];
  EXPECT_TRUE(t.AlmostEquals(out));
}

// Regression test: two triangles sharing an edge must weld to exactly 4 unique vertices
// (not 6), and operator[] must return each triangle's TRUE vertices, not shifted by one
// (see the grid_cell add_point/add_triangle off-by-one bug fixed alongside these tests).
TEST_F(Mesh2DTest, FromTriangles_SharedEdge_WeldsToFourVertices) {
  // Two triangles forming a unit square, split along the diagonal (0,0)-(1,1)
  auto t0 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1));
  auto t1 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1));
  auto mesh = g::Mesh2D::FromTriangles({t0, t1});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_NEAR(1.0, mesh.Area(), 1e-9);  // 0.5 + 0.5

  auto out0 = mesh[0];
  auto out1 = mesh[1];
  EXPECT_TRUE(t0.AlmostEquals(out0)) << "first face's vertices were not preserved (possible index shift)";
  EXPECT_TRUE(t1.AlmostEquals(out1)) << "second face's vertices were not preserved (possible index shift)";
}

TEST_F(Mesh2DTest, FromTriangles_ManyTriangles_LastVertexInBoundsAndCorrect) {
  // A fan of 4 triangles sharing the origin — exercises the LAST unique vertex registered,
  // which is exactly the index that was previously one-past-the-end (out of bounds).
  std::vector<g::Triangle2D> triangles;
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)));
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)));
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(0, 1), g::Point2D(-1, 1)));
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(-1, 1), g::Point2D(-1, 0)));

  auto mesh = g::Mesh2D::FromTriangles(triangles);
  ASSERT_EQ(4u, mesh.Size());
  for (std::size_t i = 0; i < triangles.size(); ++i) {
    EXPECT_TRUE(triangles[i].AlmostEquals(mesh[i])) << "face " << i << " mismatch";
  }
}

TEST_F(Mesh2DTest, Faces_MatchesOperatorBracket) {
  auto t0 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1));
  auto t1 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1));
  auto mesh = g::Mesh2D::FromTriangles({t0, t1});

  std::size_t i = 0;
  for (auto const& face : mesh.Faces()) {
    EXPECT_TRUE(face.AlmostEquals(mesh[i])) << "Faces() view diverges from operator[] at " << i;
    ++i;
  }
  EXPECT_EQ(mesh.Size(), i);
}

}  // namespace geompp_tests
