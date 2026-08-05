#include "grid_cell2d.hpp"

#include "point2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class GridCell2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(GridCell2DTest, FromPoint_QuantizesByFloorDivision) {
  auto cell = g::GridCell2D::FromPoint(g::Point2D(2.5, 3.5), 1.0);
  EXPECT_EQ(2, cell.x);
  EXPECT_EQ(3, cell.y);
}

TEST_F(GridCell2DTest, FromPoint_NegativeCoordinates_FloorsTowardNegativeInfinity) {
  auto cell = g::GridCell2D::FromPoint(g::Point2D(-0.5, -1.5), 1.0);
  EXPECT_EQ(-1, cell.x);
  EXPECT_EQ(-2, cell.y);
}

TEST_F(GridCell2DTest, FromPoint_SameCell_IdenticalPoints) {
  auto c1 = g::GridCell2D::FromPoint(g::Point2D(1.0, 1.0), 0.1);
  auto c2 = g::GridCell2D::FromPoint(g::Point2D(1.0, 1.0), 0.1);
  EXPECT_TRUE(c1 == c2);
}

TEST_F(GridCell2DTest, FromPoint_NearbyPointsWithinSameCell_Merge) {
  // Both points fall inside cell [10, 11) x [10, 11) at epsilon=1.0
  auto c1 = g::GridCell2D::FromPoint(g::Point2D(10.1, 10.1), 1.0);
  auto c2 = g::GridCell2D::FromPoint(g::Point2D(10.9, 10.9), 1.0);
  EXPECT_TRUE(c1 == c2);
}

TEST_F(GridCell2DTest, FromPoint_DistantPoints_DontMerge) {
  auto c1 = g::GridCell2D::FromPoint(g::Point2D(0.0, 0.0), 1.0);
  auto c2 = g::GridCell2D::FromPoint(g::Point2D(100.0, 100.0), 1.0);
  EXPECT_FALSE(c1 == c2);
}

TEST_F(GridCell2DTest, FromPoint_BoundaryStraddle_KnownLimitation) {
  // Documented limitation: two points closer together than epsilon can still land in
  // different cells if they straddle a cell boundary — grid-cell bucketing is not the
  // same comparison as AlmostEquals(epsilon).
  auto c1 = g::GridCell2D::FromPoint(g::Point2D(0.99, 0.0), 1.0);
  auto c2 = g::GridCell2D::FromPoint(g::Point2D(1.01, 0.0), 1.0);
  EXPECT_FALSE(c1 == c2) << "points 0.02 apart straddle the x=1.0 cell boundary at epsilon=1.0";
}

TEST_F(GridCell2DTest, FromPoint_DefaultEpsilon_TracksDecimalPrecision) {
  g::DECIMAL_PRECISION = 1;  // epsilon = 0.1
  auto c1 = g::GridCell2D::FromPoint(g::Point2D(0.0, 0.0));
  auto c2 = g::GridCell2D::FromPoint(g::Point2D(0.05, 0.0));
  EXPECT_TRUE(c1 == c2);  // both within the same 0.1-wide cell
}

// GridCellMapForConnectedMesh2D computes per-facet edge adjacency (detail::TriangleCompactNeighborRef)
// that ConnectedMesh2D stores but does not (yet) expose publicly, so it must be validated directly here
// rather than through ConnectedMesh2D's own tests.
class GridCellMapForConnectedMesh2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(GridCellMapForConnectedMesh2DTest, Make_Empty_Throws) {
  EXPECT_THROW(g::detail::GridCellMapForConnectedMesh2D::Make({}), std::invalid_argument);
}

// Regression test for a bug where the flat triangle_indices offset (0, 3, 6, ...) was used
// directly as the compact per-triangle index into neighbour_refs (sized n_triangles), instead
// of being divided by 3 first. That indexed and encoded out of bounds for any triangle after
// the first, corrupting memory / crashing on a Debug-checked std::vector::operator[].
TEST_F(GridCellMapForConnectedMesh2DTest, Make_SharedEdge_WiresMutualAdjacency) {
  // Unit square split along the diagonal (0,0)-(1,1).
  auto t0 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1));
  auto t1 = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1));

  auto mesh_maker = g::detail::GridCellMapForConnectedMesh2D::Make({t0, t1});
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

TEST_F(GridCellMapForConnectedMesh2DTest, Make_SingleTriangle_AllEdgesAreBoundary) {
  auto t = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0, 1));
  auto mesh_maker = g::detail::GridCellMapForConnectedMesh2D::Make({t});
  auto neighbors = mesh_maker.GetNeighborRefs();
  ASSERT_EQ(1u, neighbors->size());
  for (auto const& ref : (*neighbors)[0]) {
    EXPECT_TRUE(ref.is_boundary());
  }
}

// A fan of 4 triangles sharing the origin: exercises triangle indices beyond the first, which
// is exactly the case that indexed neighbour_refs out of bounds under the bug described above.
TEST_F(GridCellMapForConnectedMesh2DTest, Make_Fan_LastTriangleAdjacencyInBoundsAndCorrect) {
  std::vector<g::Triangle2D> triangles;
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)));
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)));
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(0, 1), g::Point2D(-1, 1)));
  triangles.push_back(g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(-1, 1), g::Point2D(-1, 0)));

  auto mesh_maker = g::detail::GridCellMapForConnectedMesh2D::Make(triangles);
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
