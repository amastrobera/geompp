#include "grid_cell3d.hpp"

#include "point3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class GridCell3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(GridCell3DTest, FromPoint_QuantizesByFloorDivision) {
  auto cell = g::GridCell3D::FromPoint(g::Point3D(2.5, 3.5, 4.5), 1.0);
  EXPECT_EQ(2, cell.x);
  EXPECT_EQ(3, cell.y);
  EXPECT_EQ(4, cell.z);
}

TEST_F(GridCell3DTest, FromPoint_NegativeCoordinates_FloorsTowardNegativeInfinity) {
  auto cell = g::GridCell3D::FromPoint(g::Point3D(-0.5, -1.5, -2.5), 1.0);
  EXPECT_EQ(-1, cell.x);
  EXPECT_EQ(-2, cell.y);
  EXPECT_EQ(-3, cell.z);
}

TEST_F(GridCell3DTest, FromPoint_SameCell_IdenticalPoints) {
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(1.0, 1.0, 1.0), 0.1);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(1.0, 1.0, 1.0), 0.1);
  EXPECT_TRUE(c1 == c2);
}

TEST_F(GridCell3DTest, FromPoint_NearbyPointsWithinSameCell_Merge) {
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(10.1, 10.1, 10.1), 1.0);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(10.9, 10.9, 10.9), 1.0);
  EXPECT_TRUE(c1 == c2);
}

TEST_F(GridCell3DTest, FromPoint_DistantPoints_DontMerge) {
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(0.0, 0.0, 0.0), 1.0);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(100.0, 100.0, 100.0), 1.0);
  EXPECT_FALSE(c1 == c2);
}

TEST_F(GridCell3DTest, FromPoint_BoundaryStraddle_KnownLimitation) {
  // Documented limitation: two points closer together than epsilon can still land in
  // different cells if they straddle a cell boundary along any axis.
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(0.99, 0.0, 0.0), 1.0);
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(1.01, 0.0, 0.0), 1.0);
  EXPECT_FALSE(c1 == c2) << "points 0.02 apart straddle the x=1.0 cell boundary at epsilon=1.0";
}

TEST_F(GridCell3DTest, FromPoint_DefaultEpsilon_TracksDecimalPrecision) {
  g::DECIMAL_PRECISION = 1;  // epsilon = 0.1
  auto c1 = g::GridCell3D::FromPoint(g::Point3D(0.0, 0.0, 0.0));
  auto c2 = g::GridCell3D::FromPoint(g::Point3D(0.05, 0.0, 0.0));
  EXPECT_TRUE(c1 == c2);  // both within the same 0.1-wide cell
}

// GridCellMapForConnectedMesh3D computes per-facet edge adjacency (detail::TriangleCompactNeighborRef)
// that ConnectedMesh3D stores and exposes via FaceView3D, so it must be validated directly here rather
// than through ConnectedMesh3D's own tests (mirrors GridCellMapForConnectedMesh2DTest in test_grid_cell2d.cpp).
class GridCellMapForConnectedMesh3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(GridCellMapForConnectedMesh3DTest, Make_Empty_Throws) {
  EXPECT_THROW(g::detail::GridCellMapForConnectedMesh3D::Make({}), std::invalid_argument);
}

TEST_F(GridCellMapForConnectedMesh3DTest, Make_SharedEdge_WiresMutualAdjacency) {
  // Unit square split along the diagonal (0,0)-(1,1), in the z=0 plane.
  auto t0 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0));
  auto t1 = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));

  auto mesh_maker = g::detail::GridCellMapForConnectedMesh3D::Make({t0, t1});
  auto neighbors = mesh_maker.GetNeighborRefs();
  ASSERT_EQ(2u, neighbors->size());

  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;

  // t0's THIRD edge (v2 -> v0) is shared with t1's FIRST edge (v0 -> v2).
  auto const& t0_shared = (*neighbors)[0][static_cast<std::size_t>(Edge::THIRD) - 1];
  EXPECT_FALSE(t0_shared.is_boundary());
  EXPECT_EQ(1u, t0_shared.triangle_id());
  EXPECT_EQ(Edge::FIRST, t0_shared.edge_id());

  auto const& t1_shared = (*neighbors)[1][static_cast<std::size_t>(Edge::FIRST) - 1];
  EXPECT_FALSE(t1_shared.is_boundary());
  EXPECT_EQ(0u, t1_shared.triangle_id());
  EXPECT_EQ(Edge::THIRD, t1_shared.edge_id());

  // Every other edge is a boundary edge (no twin).
  EXPECT_TRUE((*neighbors)[0][static_cast<std::size_t>(Edge::FIRST) - 1].is_boundary());
  EXPECT_TRUE((*neighbors)[0][static_cast<std::size_t>(Edge::SECOND) - 1].is_boundary());
  EXPECT_TRUE((*neighbors)[1][static_cast<std::size_t>(Edge::SECOND) - 1].is_boundary());
  EXPECT_TRUE((*neighbors)[1][static_cast<std::size_t>(Edge::THIRD) - 1].is_boundary());
}

TEST_F(GridCellMapForConnectedMesh3DTest, Make_SingleTriangle_AllEdgesAreBoundary) {
  auto t = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto mesh_maker = g::detail::GridCellMapForConnectedMesh3D::Make({t});
  auto neighbors = mesh_maker.GetNeighborRefs();
  ASSERT_EQ(1u, neighbors->size());
  for (auto const& ref : (*neighbors)[0]) {
    EXPECT_TRUE(ref.is_boundary());
  }
}

// A fan of 4 triangles sharing the origin: exercises triangle indices beyond the first, which is
// exactly the case that indexed neighbour_refs out of bounds under the analogous 2D bug.
TEST_F(GridCellMapForConnectedMesh3DTest, Make_Fan_LastTriangleAdjacencyInBoundsAndCorrect) {
  std::vector<g::Triangle3D> triangles;
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(0, 1, 0), g::Point3D(-1, 1, 0)));
  triangles.push_back(g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(-1, 1, 0), g::Point3D(-1, 0, 0)));

  auto mesh_maker = g::detail::GridCellMapForConnectedMesh3D::Make(triangles);
  auto neighbors = mesh_maker.GetNeighborRefs();
  ASSERT_EQ(4u, neighbors->size());

  using Edge = g::detail::TriangleCompactNeighborRef::TriangleEdge;

  // Triangle i's THIRD edge (last vertex -> origin) is shared with triangle i+1's FIRST edge
  // (origin -> its first vertex), for i = 0, 1, 2.
  for (std::size_t i = 0; i + 1 < triangles.size(); ++i) {
    auto const& fwd = (*neighbors)[i][static_cast<std::size_t>(Edge::THIRD) - 1];
    ASSERT_FALSE(fwd.is_boundary()) << "triangle " << i << "'s THIRD edge should be shared";
    EXPECT_EQ(i + 1, fwd.triangle_id());
    EXPECT_EQ(Edge::FIRST, fwd.edge_id());

    auto const& bwd = (*neighbors)[i + 1][static_cast<std::size_t>(Edge::FIRST) - 1];
    ASSERT_FALSE(bwd.is_boundary());
    EXPECT_EQ(i, bwd.triangle_id());
    EXPECT_EQ(Edge::THIRD, bwd.edge_id());
  }
}

}  // namespace geompp_tests
