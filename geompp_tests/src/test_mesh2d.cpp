#include "mesh2d.hpp"

#include "connected_mesh2d.hpp"
#include "geometry_collection2d.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "point2d.hpp"
#include "polyline2d.hpp"
#include "polymesh2d.hpp"
#include "ray2d.hpp"
#include "triangle2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <optional>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

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

TEST_F(Mesh2DTest, Polygonize_LShape_HertelMehlhorn_DoesNotThrowTJunction) {
  // 2x2 grid, top-left cell skipped: an L-shape. HertelMehlhorn returns 2 convex pieces -- a 2x1
  // rectangle and a 1x1 square -- whose shared corner sits exactly at the midpoint of the rectangle's
  // top edge. Regression test: this used to throw here (though not from the free polygonize() function,
  // which has no mesh-conformity requirement to violate) because Polygonize() packaged each piece via
  // Polygon2D::Make(), which silently drops that midpoint as collinear on the rectangle's own ring alone
  // -- leaving the square's corner touching the middle of a neighbor's edge once PolyMesh2D::FromPolygons()
  // re-welds and validates adjacency. Fixed by having Polygonize() preserve every traced vertex instead
  // (see detail::polygons_from_pieces).
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
      g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1)),
      g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(2, 1), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(1, 1), g::Point2D(2, 1), g::Point2D(2, 2)),
      g::Triangle2D::Make(g::Point2D(1, 1), g::Point2D(2, 2), g::Point2D(1, 2)),
  });

  g::PolygonizationParams params;
  params.strategy = g::PolygonizationParams::Strategy::HertelMehlhorn;

  std::optional<g::PolyMesh2D> poly_mesh;
  EXPECT_NO_THROW(poly_mesh = mesh.Polygonize(params));
  ASSERT_TRUE(poly_mesh.has_value());
  EXPECT_EQ(poly_mesh->Size(), 2u);
  EXPECT_NEAR(poly_mesh->Area(), 3.0, 1e-9);
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

TEST_F(Mesh2DTest, Polygonize_LShape_HertelMehlhorn_OperatorBracketPreservesSharedTJunctionVertex) {
  // Regression test for a bug found alongside Polygonize_LShape_HertelMehlhorn_DoesNotThrowTJunction
  // above: that test only checks Polygonize() doesn't throw at construction time, which is not enough --
  // PolyMesh2D::FromPolygons() validates and welds correctly, but PolyMesh2D::operator[] used to
  // reconstruct each returned Polygon2D via Polygon2D::Make(vertices), whose remove_collinear() pass
  // silently stripped the same load-bearing T-junction vertex right back out on every read (it has no way
  // to know (1,1) here is a genuine corner of the neighboring square, since it only ever sees one facet's
  // ring at a time) -- reintroducing, downstream, the exact defect FromPolygons() had just proved absent.
  // Feeding poly_mesh[i]'s own output back into a fresh PolyMesh2D::FromPolygons() used to throw as a
  // result. Fixed by having operator[] use FromUniquePoints() (no remove_collinear()) instead of Make().
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
      g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1)),
      g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(2, 1), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(1, 1), g::Point2D(2, 1), g::Point2D(2, 2)),
      g::Triangle2D::Make(g::Point2D(1, 1), g::Point2D(2, 2), g::Point2D(1, 2)),
  });
  g::PolygonizationParams params;
  params.strategy = g::PolygonizationParams::Strategy::HertelMehlhorn;
  auto poly_mesh = mesh.Polygonize(params);
  ASSERT_EQ(poly_mesh.Size(), 2u);

  std::vector<g::Polygon2D> extracted;
  bool found_rectangle_with_midpoint = false;
  for (std::size_t i = 0; i < poly_mesh.Size(); ++i) {
    g::Polygon2D piece = poly_mesh[i];
    extracted.push_back(piece);
    if (std::abs(piece.Area() - 2.0) < 1e-9) {
      bool has_midpoint = false;
      for (auto const& v : piece.Perimeter()) {
        if (v.AlmostEquals(g::Point2D(1, 1))) {
          has_midpoint = true;
        }
      }
      EXPECT_TRUE(has_midpoint) << "rectangle piece lost the square's shared T-junction corner (1,1)";
      found_rectangle_with_midpoint = true;
    }
  }
  EXPECT_TRUE(found_rectangle_with_midpoint);

  // Round-trips clean: feeding operator[]'s own output back into a fresh PolyMesh2D shouldn't throw.
  EXPECT_NO_THROW(g::PolyMesh2D::FromPolygons(extracted));
}

TEST_F(Mesh2DTest, ToGeometryCollection_MatchesSizeAndArea) {
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
  });
  auto collection = mesh.ToGeometryCollection();
  ASSERT_EQ(collection.Size(), mesh.Size());
  double total_area = 0.0;
  for (std::size_t i = 0; i < collection.Size(); ++i) {
    auto shape = collection.Get(i);
    ASSERT_TRUE(std::holds_alternative<g::Triangle2D>(shape));
    total_area += std::get<g::Triangle2D>(shape).Area();
  }
  EXPECT_NEAR(total_area, mesh.Area(), 1e-9);
}

TEST_F(Mesh2DTest, ToWkt_FromWkt_RoundTripsExactly) {
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 1), g::Point2D(0, 1)),
  });
  std::string wkt = mesh.ToWkt();
  EXPECT_EQ(wkt, "MESH (((0 0, 1 0, 1 1, 0 0)), ((0 0, 1 1, 0 1, 0 0)))");

  auto roundtrip = g::Mesh2D::FromWkt(wkt);
  EXPECT_EQ(roundtrip.Size(), mesh.Size());
  EXPECT_NEAR(roundtrip.Area(), mesh.Area(), 1e-9);
  EXPECT_EQ(roundtrip.ToWkt(), wkt);
}

TEST_F(Mesh2DTest, FromWkt_WrongGeometryName_Throws) { EXPECT_THROW(g::Mesh2D::FromWkt("POLYGON ((0 0))"), std::exception); }

TEST_F(Mesh2DTest, FromWkt_FacetNotATriangle_Throws) {
  // A facet with 4 vertices -- a mesh facet must always be exactly a triangle.
  EXPECT_THROW(g::Mesh2D::FromWkt("MESH (((0 0, 1 0, 1 1, 0 1, 0 0)))"), std::exception);
}

TEST_F(Mesh2DTest, ToFile_FromFile_RoundTrips) {
  auto mesh = g::Mesh2D::FromTriangles({
      g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)),
  });
  std::string path = (test_res_path / "temp" / "mesh2d_roundtrip.wkt").string();
  mesh.ToFile(path);
  auto from_file = g::Mesh2D::FromFile(path);
  EXPECT_EQ(from_file.Size(), mesh.Size());
  EXPECT_NEAR(from_file.Area(), mesh.Area(), 1e-9);
  EXPECT_NO_THROW(fs::remove(path));
}

}  // namespace geompp_tests
