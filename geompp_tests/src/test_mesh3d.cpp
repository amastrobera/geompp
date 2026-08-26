#include "mesh3d.hpp"

#include "connected_mesh3d.hpp"
#include "geometry_collection3d.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "point3d.hpp"
#include "polyline3d.hpp"
#include "polymesh3d.hpp"
#include "ray3d.hpp"
#include "triangle3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <optional>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class Mesh3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(Mesh3DTest, FromTriangles_Empty_Throws) {
  EXPECT_THROW(g::Mesh3D::FromTriangles({}), std::invalid_argument);
}

TEST_F(Mesh3DTest, FromTriangles_NonManifoldEdge_Throws) {
  // Three triangles all sharing the exact same edge (0,0,0)-(1,0,0), fanned out into three different
  // planes -- a full edge with 3 neighbors, not just 1.
  auto a = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0.5, 1, 0));
  auto b = g::Triangle3D::Make(g::Point3D(1, 0, 0), g::Point3D(0, 0, 0), g::Point3D(0.5, 0, 1));
  auto c = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0.5, -1, 0));
  EXPECT_THROW(g::Mesh3D::FromTriangles({a, b, c}), std::invalid_argument);
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

TEST_F(Mesh3DTest, Polygonize_LShape_HertelMehlhorn_DoesNotThrowTJunction) {
  // 3D counterpart of Mesh2DTest.Polygonize_LShape_HertelMehlhorn_DoesNotThrowTJunction, flat on z=0 --
  // see its own comment for the full explanation.
  auto mesh = g::Mesh3D::FromTriangles({
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)),
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)),
      g::Triangle3D::Make(g::Point3D(1, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0)),
      g::Triangle3D::Make(g::Point3D(1, 0, 0), g::Point3D(2, 1, 0), g::Point3D(1, 1, 0)),
      g::Triangle3D::Make(g::Point3D(1, 1, 0), g::Point3D(2, 1, 0), g::Point3D(2, 2, 0)),
      g::Triangle3D::Make(g::Point3D(1, 1, 0), g::Point3D(2, 2, 0), g::Point3D(1, 2, 0)),
  });

  g::PolygonizationParams params;
  params.strategy = g::PolygonizationParams::Strategy::HertelMehlhorn;

  std::optional<g::PolyMesh3D> poly_mesh;
  EXPECT_NO_THROW(poly_mesh = mesh.Polygonize(params));
  ASSERT_TRUE(poly_mesh.has_value());
  EXPECT_EQ(poly_mesh->Size(), 2u);
  EXPECT_NEAR(poly_mesh->Area(), 3.0, 1e-9);
}

TEST_F(Mesh3DTest, Polygonize_MatchesConnectThenPolygonize) {
  // Same reasoning as the Mesh2DTest counterpart: Mesh3D::Polygonize() takes a different (cheaper)
  // internal path than Connect().Polygonize() would, but must agree on the actual result.
  auto mesh = g::Mesh3D::FromTriangles({
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)),
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)),
  });

  g::PolygonizationParams params;
  params.strategy = g::PolygonizationParams::Strategy::PlanarBoundaryExtraction;

  auto direct = mesh.Polygonize(params);
  auto via_connect = mesh.Connect().Polygonize(params);

  ASSERT_EQ(direct.Size(), via_connect.Size());
  EXPECT_NEAR(direct.Area(), via_connect.Area(), 1e-9);
  for (std::size_t i = 0; i < direct.Size(); ++i) {
    EXPECT_TRUE(direct[i].AlmostEquals(via_connect[i]))
        << "piece " << i << " differs between Mesh3D::Polygonize() and Connect().Polygonize()";
  }
}

TEST_F(Mesh3DTest, ToGeometryCollection_MatchesSizeAndArea) {
  auto mesh = g::Mesh3D::FromTriangles({
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)),
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)),
  });
  auto collection = mesh.ToGeometryCollection();
  ASSERT_EQ(collection.Size(), mesh.Size());
  double total_area = 0.0;
  for (std::size_t i = 0; i < collection.Size(); ++i) {
    auto shape = collection.Get(i);
    ASSERT_TRUE(std::holds_alternative<g::Triangle3D>(shape));
    total_area += std::get<g::Triangle3D>(shape).Area();
  }
  EXPECT_NEAR(total_area, mesh.Area(), 1e-9);
}

TEST_F(Mesh3DTest, ToWkt_FromWkt_RoundTripsExactly) {
  auto mesh = g::Mesh3D::FromTriangles({
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)),
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)),
  });
  std::string wkt = mesh.ToWkt();
  EXPECT_EQ(wkt, "MESH (((0 0 0, 1 0 0, 1 1 0, 0 0 0)), ((0 0 0, 1 1 0, 0 1 0, 0 0 0)))");

  auto roundtrip = g::Mesh3D::FromWkt(wkt);
  EXPECT_EQ(roundtrip.Size(), mesh.Size());
  EXPECT_NEAR(roundtrip.Area(), mesh.Area(), 1e-9);
  EXPECT_EQ(roundtrip.ToWkt(), wkt);
}

TEST_F(Mesh3DTest, FromWkt_WrongGeometryName_Throws) { EXPECT_THROW(g::Mesh3D::FromWkt("POLYGON ((0 0 0))"), std::exception); }

TEST_F(Mesh3DTest, FromWkt_FacetNotATriangle_Throws) {
  EXPECT_THROW(g::Mesh3D::FromWkt("MESH (((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0)))"), std::exception);
}

TEST_F(Mesh3DTest, ToFile_FromFile_RoundTrips) {
  auto mesh = g::Mesh3D::FromTriangles({
      g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)),
  });
  std::string path = (test_res_path / "temp" / "mesh3d_roundtrip.wkt").string();
  mesh.ToFile(path);
  auto from_file = g::Mesh3D::FromFile(path);
  EXPECT_EQ(from_file.Size(), mesh.Size());
  EXPECT_NEAR(from_file.Area(), mesh.Area(), 1e-9);
  EXPECT_NO_THROW(fs::remove(path));
}

}  // namespace geompp_tests
