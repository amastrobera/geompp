#include "mesh2d.hpp"

#include "connected_mesh2d.hpp"
#include "point2d.hpp"
#include "polymesh2d.hpp"
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

TEST_F(Mesh2DTest, FromTriangles_NonManifoldEdge_Throws) {
  // Three triangles all sharing the exact same edge (0,0)-(1,0) -- a full edge with 3 neighbors.
  auto a = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0.5, 1));
  auto b = g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(0, 0), g::Point2D(0.5, -1));
  auto c = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0.5, -2));
  EXPECT_THROW(g::Mesh2D::FromTriangles({a, b, c}), std::invalid_argument);
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

TEST_F(Mesh2DTest, Connect_PreservesSizeAreaAndFaces) {
  auto t0 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1));
  auto t1 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1));
  auto mesh = g::Mesh2D::FromTriangles({t0, t1});

  auto connected = mesh.Connect();
  EXPECT_EQ(mesh.Size(), connected.Size());
  EXPECT_NEAR(mesh.Area(), connected.Area(), 1e-9);
  for (std::size_t i = 0; i < mesh.Size(); ++i) {
    EXPECT_TRUE(mesh[i].AlmostEquals(connected[i].Geometry())) << "face " << i << " mismatch after Connect()";
  }
}

TEST_F(Mesh2DTest, Connect_SharedEdge_ExposesAdjacency) {
  // Same fan-of-4 pattern used in the ConnectedMesh2D suite: adjacent facets should now be
  // queryable across their shared THIRD edge, which a bare Mesh2D cannot do.
  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
  });

  auto connected = mesh.Connect();
  auto neighbor = connected[0].Neighbor(Edge::THIRD);
  ASSERT_TRUE(neighbor.has_value());
  EXPECT_EQ(1u, neighbor->ID());
}

TEST_F(Mesh2DTest, Polygonize_UnitSquareFromTwoTriangles_ReturnsSingleQuad) {
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
  });

  g::PolygonizationParams params;
  params.strategy = g::PolygonizationParams::Strategy::PlanarBoundaryExtraction;
  auto poly_mesh = mesh.Polygonize(params);

  ASSERT_EQ(poly_mesh.Size(), 1u);
  EXPECT_NEAR(poly_mesh.Area(), 1.0, 1e-9);
  EXPECT_EQ(poly_mesh[0].Size(), 4u);
}

TEST_F(Mesh2DTest, Polygonize_MatchesConnectThenPolygonize) {
  // Mesh2D::Polygonize() takes a different (cheaper) internal path than Connect().Polygonize() would --
  // no ConnectedMesh2D is ever constructed -- but must agree on the actual result.
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
      g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(1, 1)),
  });

  g::PolygonizationParams params;
  params.strategy = g::PolygonizationParams::Strategy::HertelMehlhorn;

  auto direct = mesh.Polygonize(params);
  auto via_connect = mesh.Connect().Polygonize(params);

  ASSERT_EQ(direct.Size(), via_connect.Size());
  EXPECT_NEAR(direct.Area(), via_connect.Area(), 1e-9);
  for (std::size_t i = 0; i < direct.Size(); ++i) {
    EXPECT_TRUE(direct[i].AlmostEquals(via_connect[i]))
        << "piece " << i << " differs between Mesh2D::Polygonize() and Connect().Polygonize()";
  }
}

}  // namespace geompp_tests
