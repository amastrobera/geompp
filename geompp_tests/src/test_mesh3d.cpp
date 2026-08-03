#include "mesh3d.hpp"

#include "connected_mesh3d.hpp"
#include "point3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class Mesh3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Mesh3DTest, FromTriangles_Empty_Throws) {
  EXPECT_THROW(g::Mesh3D::FromTriangles({}), std::invalid_argument);
}

TEST_F(Mesh3DTest, FromTriangles_SingleTriangle) {
  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto mesh = g::Mesh3D::FromTriangles({t});

  EXPECT_EQ(1u, mesh.Size());
  EXPECT_NEAR(0.5, mesh.Area(), 1e-9);
}

TEST_F(Mesh3DTest, OperatorBracket_OutOfRange_Throws) {
  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto mesh = g::Mesh3D::FromTriangles({t});
  EXPECT_THROW(mesh[1], std::out_of_range);
}

TEST_F(Mesh3DTest, OperatorBracket_SingleTriangle_MatchesInput) {
  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 3, 0));
  auto mesh = g::Mesh3D::FromTriangles({t});
  auto out = mesh[0];
  EXPECT_TRUE(t.AlmostEquals(out));
}

// Regression test: two triangles sharing an edge must weld to exactly 4 unique vertices
// (not 6), and operator[] must return each triangle's TRUE vertices, not shifted by one
// (see the grid_cell add_point/add_triangle off-by-one bug fixed alongside these tests).
TEST_F(Mesh3DTest, FromTriangles_SharedEdge_WeldsToFourVertices) {
  // Two triangles forming a unit square in the XY plane, split along the diagonal
  auto t0 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0));
  auto t1 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::Mesh3D::FromTriangles({t0, t1});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_NEAR(1.0, mesh.Area(), 1e-9);

  auto out0 = mesh[0];
  auto out1 = mesh[1];
  EXPECT_TRUE(t0.AlmostEquals(out0)) << "first face's vertices were not preserved (possible index shift)";
  EXPECT_TRUE(t1.AlmostEquals(out1)) << "second face's vertices were not preserved (possible index shift)";
}

TEST_F(Mesh3DTest, FromTriangles_ManyTriangles_LastVertexInBoundsAndCorrect) {
  // A fan of 4 triangles sharing the origin — exercises the LAST unique vertex registered,
  // which is exactly the index that was previously one-past-the-end (out of bounds).
  std::vector<g::Triangle3D> triangles;
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(-1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(-1, 1, 0), g::Point3D(-1, 0, 0)));

  auto mesh = g::Mesh3D::FromTriangles(triangles);
  ASSERT_EQ(4u, mesh.Size());
  for (std::size_t i = 0; i < triangles.size(); ++i) {
    EXPECT_TRUE(triangles[i].AlmostEquals(mesh[i])) << "face " << i << " mismatch";
  }
}

TEST_F(Mesh3DTest, Faces_MatchesOperatorBracket) {
  auto t0 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0));
  auto t1 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::Mesh3D::FromTriangles({t0, t1});

  std::size_t i = 0;
  for (auto const& face : mesh.Faces()) {
    EXPECT_TRUE(face.AlmostEquals(mesh[i])) << "Faces() view diverges from operator[] at " << i;
    ++i;
  }
  EXPECT_EQ(mesh.Size(), i);
}

TEST_F(Mesh3DTest, Connect_PreservesSizeAreaAndFaces) {
  auto t0 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0));
  auto t1 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::Mesh3D::FromTriangles({t0, t1});

  auto connected = mesh.Connect();
  EXPECT_EQ(mesh.Size(), connected.Size());
  EXPECT_NEAR(mesh.Area(), connected.Area(), 1e-9);
  for (std::size_t i = 0; i < mesh.Size(); ++i) {
    EXPECT_TRUE(mesh[i].AlmostEquals(connected[i].Geometry())) << "face " << i << " mismatch after Connect()";
  }
}

TEST_F(Mesh3DTest, Connect_SharedEdge_ExposesAdjacency) {
  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;
  auto mesh = g::Mesh3D::FromTriangles({
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)),
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)),
  });

  auto connected = mesh.Connect();
  auto neighbor = connected[0].Neighbor(Edge::THIRD);
  ASSERT_TRUE(neighbor.has_value());
  EXPECT_EQ(1u, neighbor->ID());
}

}  // namespace geompp_tests
