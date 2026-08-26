#include "polymesh3d.hpp"

#include "geometry_collection3d.hpp"
#include "line3d.hpp"
#include "line_segment3d.hpp"
#include "mesh3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "polyline3d.hpp"
#include "ray3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

#include <filesystem>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class PolyMesh3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(PolyMesh3DTest, FromPolygons_Empty_Throws) {
  EXPECT_THROW(g::PolyMesh3D::FromPolygons({}), std::invalid_argument);
}

TEST_F(PolyMesh3DTest, FromPolygons_TJunction_Throws) {
  // Two unit squares side by side (in the y=0 plane), plus a roof triangle spanning both squares' top
  // -- its base edge passes straight through the squares' shared vertex without that vertex being one
  // of the roof's own endpoints. Classic T-junction.
  auto p0 = g::Polygon3D::Make({g::Point3D(0, 0, 1), g::Point3D(1, 0, 1), g::Point3D(1, 0, 0), g::Point3D(0, 0, 0)});
  auto p1 = g::Polygon3D::Make({g::Point3D(1, 0, 1), g::Point3D(2, 0, 1), g::Point3D(2, 0, 0), g::Point3D(1, 0, 0)});
  auto roof = g::Polygon3D::Make({g::Point3D(1, 0, 2), g::Point3D(2, 0, 1), g::Point3D(0, 0, 1)});
  EXPECT_THROW(g::PolyMesh3D::FromPolygons({p0, p1, roof}), std::invalid_argument);
}

TEST_F(PolyMesh3DTest, FromPolygons_PolygonWithHoles_Throws) {
  auto outer = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)},
      {{g::Point3D(1, 1, 0), g::Point3D(1, 2, 0), g::Point3D(2, 2, 0), g::Point3D(2, 1, 0)}});
  EXPECT_THROW(g::PolyMesh3D::FromPolygons({outer}), std::invalid_argument);
}

TEST_F(PolyMesh3DTest, FromPolygons_SingleQuad) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p});

  EXPECT_EQ(1u, mesh.Size());
  EXPECT_NEAR(1.0, mesh.Area(), 1e-9);
}

TEST_F(PolyMesh3DTest, OperatorBracket_OutOfRange_Throws) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p});
  EXPECT_THROW(mesh[1], std::out_of_range);
}

TEST_F(PolyMesh3DTest, OperatorBracket_SingleQuad_MatchesInput) {
  auto p = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0), g::Point3D(0, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p});
  EXPECT_TRUE(p.AlmostEquals(mesh[0]));
}

// Regression test: two quads sharing an edge must weld shared vertices, and operator[]
// must return each face's TRUE vertices in the correct order — this exercises the
// PolyMesh3D::FromPolygons index-shift bug fixed alongside these tests, including the
// last-registered vertex (previously one-past-the-end / out of bounds).
TEST_F(PolyMesh3DTest, FromPolygons_SharedEdge_WeldsVerticesAndPreservesFaces) {
  auto p0 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto p1 = g::Polygon3D::Make(
      {g::Point3D(1, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0), g::Point3D(1, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0, p1});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_NEAR(2.0, mesh.Area(), 1e-9);

  EXPECT_TRUE(p0.AlmostEquals(mesh[0])) << "first face's vertices were not preserved (possible index shift)";
  EXPECT_TRUE(p1.AlmostEquals(mesh[1])) << "second face's vertices were not preserved (possible index shift)";
}

TEST_F(PolyMesh3DTest, FromPolygons_MixedFaceSizes) {
  auto tri = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0.5, 1, 0)});
  auto quad = g::Polygon3D::Make(
      {g::Point3D(1, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0), g::Point3D(0.5, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({tri, quad});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_TRUE(tri.AlmostEquals(mesh[0]));
  EXPECT_TRUE(quad.AlmostEquals(mesh[1]));
}

TEST_F(PolyMesh3DTest, Faces_MatchesOperatorBracket) {
  auto p0 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto p1 = g::Polygon3D::Make(
      {g::Point3D(1, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0), g::Point3D(1, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0, p1});

  std::size_t i = 0;
  for (auto const& face : mesh.Faces()) {
    EXPECT_TRUE(face.AlmostEquals(mesh[i])) << "Faces() view diverges from operator[] at " << i;
    ++i;
  }
  EXPECT_EQ(mesh.Size(), i);
}

TEST_F(PolyMesh3DTest, Triangulate_TwoFacetsSharedEdge_PreservesTotalArea) {
  auto p0 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto p1 = g::Polygon3D::Make(
      {g::Point3D(1, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0), g::Point3D(1, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0, p1});

  auto tri_mesh = mesh.Triangulate();
  EXPECT_EQ(4u, tri_mesh.Size());
  EXPECT_NEAR(mesh.Area(), tri_mesh.Area(), 1e-9);
}

// Regression test: PolyMesh3D::Triangulate() used to triangulate the whole (shared, deduplicated)
// VERTICES buffer as if it were a single ring, which only "worked" by coincidence for facets that
// happened to share welded edges. Two disjoint facets — no welding to paper over the bug — exposes it
// directly: the old code produced a wrong triangle count and a wrong total area.
TEST_F(PolyMesh3DTest, Triangulate_TwoDisjointFacets_PreservesTotalArea) {
  auto p0 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto p1 = g::Polygon3D::Make(
      {g::Point3D(5, 5, 0), g::Point3D(6, 5, 0), g::Point3D(6, 6, 0), g::Point3D(5, 6, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0, p1});

  auto tri_mesh = mesh.Triangulate();
  EXPECT_EQ(4u, tri_mesh.Size());
  EXPECT_NEAR(mesh.Area(), tri_mesh.Area(), 1e-9);
}

TEST_F(PolyMesh3DTest, ToGeometryCollection_MatchesSizeAndArea) {
  auto p0 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto p1 = g::Polygon3D::Make(
      {g::Point3D(1, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0), g::Point3D(1, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0, p1});

  auto collection = mesh.ToGeometryCollection();
  ASSERT_EQ(collection.Size(), mesh.Size());
  double total_area = 0.0;
  for (std::size_t i = 0; i < collection.Size(); ++i) {
    auto shape = collection.Get(i);
    ASSERT_TRUE(std::holds_alternative<g::Polygon3D>(shape));
    total_area += std::get<g::Polygon3D>(shape).Area();
  }
  EXPECT_NEAR(total_area, mesh.Area(), 1e-9);
}

TEST_F(PolyMesh3DTest, ToWkt_FromWkt_RoundTripsExactly) {
  auto p0 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0});

  std::string wkt = mesh.ToWkt();
  EXPECT_EQ(wkt, "POLYMESH (((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0)))");

  auto roundtrip = g::PolyMesh3D::FromWkt(wkt);
  EXPECT_EQ(roundtrip.Size(), mesh.Size());
  EXPECT_NEAR(roundtrip.Area(), mesh.Area(), 1e-9);
  EXPECT_EQ(roundtrip.ToWkt(), wkt);
}

TEST_F(PolyMesh3DTest, FromWkt_WrongGeometryName_Throws) {
  EXPECT_THROW(g::PolyMesh3D::FromWkt("MESH (((0 0 0, 1 0 0, 1 1 0, 0 0 0)))"), std::exception);
}

TEST_F(PolyMesh3DTest, FromWkt_FacetTooFewVertices_Throws) {
  EXPECT_THROW(g::PolyMesh3D::FromWkt("POLYMESH (((0 0 0, 1 0 0, 0 0 0)))"), std::exception);
}

TEST_F(PolyMesh3DTest, ToFile_FromFile_RoundTrips) {
  auto p0 = g::Polygon3D::Make(
      {g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0});

  std::string path = (test_res_path / "temp" / "polymesh3d_roundtrip.wkt").string();
  mesh.ToFile(path);
  auto from_file = g::PolyMesh3D::FromFile(path);
  EXPECT_EQ(from_file.Size(), mesh.Size());
  EXPECT_NEAR(from_file.Area(), mesh.Area(), 1e-9);
  EXPECT_NO_THROW(fs::remove(path));
}

}  // namespace geompp_tests
