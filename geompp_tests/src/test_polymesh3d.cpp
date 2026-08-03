#include "polymesh3d.hpp"

#include "mesh3d.hpp"
#include "point3d.hpp"
#include "polygon3d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

namespace g = geompp;

namespace geompp_tests {

class PolyMesh3DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(PolyMesh3DTest, FromPolygons_Empty_Throws) {
  EXPECT_THROW(g::PolyMesh3D::FromPolygons({}), std::invalid_argument);
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

}  // namespace geompp_tests
