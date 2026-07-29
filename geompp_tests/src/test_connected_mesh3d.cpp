#include "connected_mesh3d.hpp"

#include "point3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class ConnectedMesh3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(ConnectedMesh3DTest, FromTriangles_Empty_Throws) {
  EXPECT_THROW(g::ConnectedMesh3D::FromTriangles({}), std::invalid_argument);
}

TEST_F(ConnectedMesh3DTest, FromTriangles_SingleTriangle) {
  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({t});

  EXPECT_EQ(1u, mesh.Size());
  EXPECT_NEAR(0.5, mesh.Area(), 1e-9);
}

TEST_F(ConnectedMesh3DTest, OperatorBracket_OutOfRange_Throws) {
  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({t});
  EXPECT_THROW(mesh[1], std::out_of_range);
}

TEST_F(ConnectedMesh3DTest, OperatorBracket_SingleTriangle_MatchesInput) {
  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 3, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({t});
  auto out = mesh[0];
  EXPECT_EQ(0u, out.ID());
  EXPECT_TRUE(t.AlmostEquals(out.Geometry()));
}

TEST_F(ConnectedMesh3DTest, FromTriangles_SharedEdge_WeldsToFourVertices) {
  // Two triangles forming a unit square, split along the diagonal (0,0)-(1,1)
  auto t0 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0));
  auto t1 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({t0, t1});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_NEAR(1.0, mesh.Area(), 1e-9);  // 0.5 + 0.5

  auto out0 = mesh[0];
  auto out1 = mesh[1];
  EXPECT_TRUE(t0.AlmostEquals(out0.Geometry()))
      << "first face's vertices were not preserved (possible index shift)";
  EXPECT_TRUE(t1.AlmostEquals(out1.Geometry()))
      << "second face's vertices were not preserved (possible index shift)";
}

// Regression coverage for a fix in GridCellMapForConnectedMesh3D::Make() where the flat
// triangle_indices offset (a multiple of 3) was used directly as the per-triangle adjacency
// index instead of being divided by 3 first — this indexed and encoded out of bounds for any
// triangle after the first, corrupting memory. A fan of 4 triangles sharing an edge each
// exercises exactly that path; see GridCellMapForConnectedMesh3DTest in test_grid_cell3d.cpp
// for direct assertions on the computed adjacency values.
TEST_F(ConnectedMesh3DTest, FromTriangles_ManyTriangles_LastVertexInBoundsAndCorrect) {
  std::vector<g::Triangle3D> triangles;
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(-1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(-1, 1, 0), g::Point3D(-1, 0, 0)));

  auto mesh = g::ConnectedMesh3D::FromTriangles(triangles);
  ASSERT_EQ(4u, mesh.Size());
  for (std::size_t i = 0; i < triangles.size(); ++i) {
    EXPECT_TRUE(triangles[i].AlmostEquals(mesh[i].Geometry())) << "face " << i << " mismatch";
  }
}

TEST_F(ConnectedMesh3DTest, Faces_MatchesOperatorBracket) {
  auto t0 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0));
  auto t1 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({t0, t1});

  std::size_t i = 0;
  for (auto const& face : mesh.Faces()) {
    EXPECT_TRUE(face.Geometry().AlmostEquals(mesh[i].Geometry()))
        << "Faces() view diverges from operator[] at " << i;
    ++i;
  }
  EXPECT_EQ(mesh.Size(), i);
}

// Fan of 4 triangles sharing the origin, laid out left-to-right (triangle i's THIRD edge is welded
// to triangle i+1's FIRST edge — see the matching GridCellMapForConnectedMesh3DTest.Make_Fan_* case
// in test_grid_cell3d.cpp for the raw adjacency assertions this walk relies on):
//
//   face 0: (0,0,0)-(1,0,0)-(1,1,0)       face 1: (0,0,0)-(1,1,0)-(0,1,0)
//   face 2: (0,0,0)-(0,1,0)-(-1,1,0)      face 3: (0,0,0)-(-1,1,0)-(-1,0,0)
//
// Starting at face 0 and always crossing THIRD, the walk visits 0 -> 1 -> 2 -> 3, always entering
// the next face through its FIRST edge, and face 3's THIRD edge is a boundary (end of the fan).
TEST_F(ConnectedMesh3DTest, FaceView_Neighbor_WalksKnownAdjacencyPatternToExpectedTarget) {
  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;

  std::vector<g::Triangle3D> triangles;
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(-1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(-1, 1, 0), g::Point3D(-1, 0, 0)));

  auto mesh = g::ConnectedMesh3D::FromTriangles(triangles);
  ASSERT_EQ(4u, mesh.Size());

  auto face = mesh[0];
  EXPECT_EQ(0u, face.ID());

  for (std::size_t expected_target = 1; expected_target < triangles.size(); ++expected_target) {
    EXPECT_EQ(Edge::FIRST, face.NeighborEntryEdge(Edge::THIRD))
        << "face " << face.ID() << " should be entered through its FIRST edge";

    auto next = face.Neighbor(Edge::THIRD);
    ASSERT_TRUE(next.has_value()) << "face " << face.ID() << "'s THIRD edge should not be a boundary";

    face = *next;
    EXPECT_EQ(expected_target, face.ID());
    EXPECT_TRUE(triangles[expected_target].AlmostEquals(face.Geometry()));
  }

  // face 3 is the end of the fan: its THIRD edge has no twin.
  EXPECT_FALSE(face.Neighbor(Edge::THIRD).has_value());
  EXPECT_EQ(Edge::INVALID, face.NeighborEntryEdge(Edge::THIRD));
}

TEST_F(ConnectedMesh3DTest, FaceView_Neighbor_SingleTriangle_AllEdgesAreBoundary) {
  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;

  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({t});
  auto face = mesh[0];

  for (Edge edge : {Edge::FIRST, Edge::SECOND, Edge::THIRD}) {
    EXPECT_FALSE(face.Neighbor(edge).has_value()) << "edge " << static_cast<int>(edge);
    EXPECT_EQ(Edge::INVALID, face.NeighborEntryEdge(edge)) << "edge " << static_cast<int>(edge);
  }
}

// Crossing to a neighbor and immediately crossing back through the edge NeighborEntryEdge()
// reported must land back on the face we started from — the adjacency link is symmetric.
TEST_F(ConnectedMesh3DTest, FaceView_Neighbor_CrossingBackViaEntryEdge_ReturnsToOrigin) {
  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;

  auto t0 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0));
  auto t1 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({t0, t1});

  auto face0 = mesh[0];
  Edge entry_edge = face0.NeighborEntryEdge(Edge::THIRD);
  auto face1 = face0.Neighbor(Edge::THIRD);
  ASSERT_TRUE(face1.has_value());
  EXPECT_EQ(1u, face1->ID());

  auto back = face1->Neighbor(entry_edge);
  ASSERT_TRUE(back.has_value());
  EXPECT_EQ(face0.ID(), back->ID());
  EXPECT_EQ(Edge::THIRD, face1->NeighborEntryEdge(entry_edge));
}

}  // namespace geompp_tests
