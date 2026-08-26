#include "polymesh2d.hpp"

#include "geometry_collection2d.hpp"
#include "line2d.hpp"
#include "line_segment2d.hpp"
#include "mesh2d.hpp"
#include "point2d.hpp"
#include "polygon2d.hpp"
#include "polyline2d.hpp"
#include "ray2d.hpp"
#include "utils.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

namespace g = geompp;
namespace fs = std::filesystem;

namespace geompp_tests {

extern fs::path test_res_path;

class PolyMesh2DTest : public ::testing::Test {
 protected:
  void SetUp() override { g::DECIMAL_PRECISION = g::DP_THREE; }
  void TearDown() override { g::DECIMAL_PRECISION = g::DP_THREE; }
};

TEST_F(PolyMesh2DTest, FromPolygons_Empty_Throws) {
  EXPECT_THROW(g::PolyMesh2D::FromPolygons({}), std::invalid_argument);
}

TEST_F(PolyMesh2DTest, FromPolygons_TJunction_Throws) {
  // Two unit squares side by side, plus a roof triangle spanning both squares' top -- its base edge
  // (0,1)-(2,1) passes straight through the squares' shared vertex (1,1) without that vertex being one
  // of the roof's own endpoints. Classic T-junction.
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto roof = g::Polygon2D::Make({g::Point2D(0, 1), g::Point2D(2, 1), g::Point2D(1, 2)});
  EXPECT_THROW(g::PolyMesh2D::FromPolygons({p0, p1, roof}), std::invalid_argument);
}

TEST_F(PolyMesh2DTest, FromPolygons_PolygonWithHoles_Throws) {
  auto outer = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)},
                                   {{g::Point2D(1, 1), g::Point2D(1, 2), g::Point2D(2, 2), g::Point2D(2, 1)}});
  EXPECT_THROW(g::PolyMesh2D::FromPolygons({outer}), std::invalid_argument);
}

TEST_F(PolyMesh2DTest, FromPolygons_SingleQuad) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p});

  EXPECT_EQ(1u, mesh.Size());
  EXPECT_NEAR(1.0, mesh.Area(), 1e-9);
}

TEST_F(PolyMesh2DTest, OperatorBracket_OutOfRange_Throws) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p});
  EXPECT_THROW(mesh[1], std::out_of_range);
}

TEST_F(PolyMesh2DTest, OperatorBracket_SingleQuad_MatchesInput) {
  auto p = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p});
  EXPECT_TRUE(p.AlmostEquals(mesh[0]));
}

// Regression test: two quads sharing an edge must weld shared vertices, and operator[]
// must return each face's TRUE vertices in the correct order — this exercises the
// PolyMesh2D::FromPolygons index-shift bug fixed alongside these tests, including the
// last-registered vertex (previously one-past-the-end / out of bounds).
TEST_F(PolyMesh2DTest, FromPolygons_SharedEdge_WeldsVerticesAndPreservesFaces) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_NEAR(2.0, mesh.Area(), 1e-9);

  EXPECT_TRUE(p0.AlmostEquals(mesh[0])) << "first face's vertices were not preserved (possible index shift)";
  EXPECT_TRUE(p1.AlmostEquals(mesh[1])) << "second face's vertices were not preserved (possible index shift)";
}

TEST_F(PolyMesh2DTest, FromPolygons_MixedFaceSizes) {
  // A triangle and a quad sharing one edge — exercises the variable-length face buffer.
  auto tri = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0.5, 1)});
  auto quad = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(0.5, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({tri, quad});

  ASSERT_EQ(2u, mesh.Size());
  EXPECT_TRUE(tri.AlmostEquals(mesh[0]));
  EXPECT_TRUE(quad.AlmostEquals(mesh[1]));
}

TEST_F(PolyMesh2DTest, Faces_MatchesOperatorBracket) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  std::size_t i = 0;
  for (auto const& face : mesh.Faces()) {
    EXPECT_TRUE(face.AlmostEquals(mesh[i])) << "Faces() view diverges from operator[] at " << i;
    ++i;
  }
  EXPECT_EQ(mesh.Size(), i);
}

TEST_F(PolyMesh2DTest, Triangulate_TwoFacetsSharedEdge_PreservesTotalArea) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  auto tri_mesh = mesh.Triangulate();
  EXPECT_EQ(4u, tri_mesh.Size());
  EXPECT_NEAR(mesh.Area(), tri_mesh.Area(), 1e-9);
}

// Regression test: PolyMesh2D::Triangulate() used to triangulate the whole (shared, deduplicated)
// VERTICES buffer as if it were a single ring, which only "worked" by coincidence for facets that
// happened to share welded edges. Two disjoint facets — no welding to paper over the bug — exposes it
// directly: the old code produced a wrong triangle count and a wrong total area.
TEST_F(PolyMesh2DTest, Triangulate_TwoDisjointFacets_PreservesTotalArea) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(5, 5), g::Point2D(6, 5), g::Point2D(6, 6), g::Point2D(5, 6)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  auto tri_mesh = mesh.Triangulate();
  EXPECT_EQ(4u, tri_mesh.Size());
  std::cout << "PolyMesh2D::Triangulate() on 2 disjoint quads: " << tri_mesh.Size() << " triangles, area="
            << tri_mesh.Area() << " (mesh.Area()=" << mesh.Area() << ")\n";
  EXPECT_NEAR(mesh.Area(), tri_mesh.Area(), 1e-9);
}

TEST_F(PolyMesh2DTest, ToGeometryCollection_MatchesSizeAndArea) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  auto collection = mesh.ToGeometryCollection();
  ASSERT_EQ(collection.Size(), mesh.Size());
  double total_area = 0.0;
  for (std::size_t i = 0; i < collection.Size(); ++i) {
    auto shape = collection.Get(i);
    ASSERT_TRUE(std::holds_alternative<g::Polygon2D>(shape));
    total_area += std::get<g::Polygon2D>(shape).Area();
  }
  EXPECT_NEAR(total_area, mesh.Area(), 1e-9);
}

TEST_F(PolyMesh2DTest, ToWkt_FromWkt_RoundTripsExactly) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});

  std::string wkt = mesh.ToWkt();
  EXPECT_EQ(wkt, "POLYMESH (((0 0, 1 0, 1 1, 0 1, 0 0)), ((1 0, 2 0, 2 1, 1 1, 1 0)))");

  auto roundtrip = g::PolyMesh2D::FromWkt(wkt);
  EXPECT_EQ(roundtrip.Size(), mesh.Size());
  EXPECT_NEAR(roundtrip.Area(), mesh.Area(), 1e-9);
  EXPECT_EQ(roundtrip.ToWkt(), wkt);
}

TEST_F(PolyMesh2DTest, FromWkt_WrongGeometryName_Throws) {
  EXPECT_THROW(g::PolyMesh2D::FromWkt("MESH (((0 0, 1 0, 1 1, 0 0)))"), std::exception);
}

TEST_F(PolyMesh2DTest, FromWkt_FacetTooFewVertices_Throws) {
  EXPECT_THROW(g::PolyMesh2D::FromWkt("POLYMESH (((0 0, 1 0, 0 0)))"), std::exception);
}

TEST_F(PolyMesh2DTest, ToFile_FromFile_RoundTrips) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0});

  std::string path = (test_res_path / "temp" / "polymesh2d_roundtrip.wkt").string();
  mesh.ToFile(path);
  auto from_file = g::PolyMesh2D::FromFile(path);
  EXPECT_EQ(from_file.Size(), mesh.Size());
  EXPECT_NEAR(from_file.Area(), mesh.Area(), 1e-9);
  EXPECT_NO_THROW(fs::remove(path));
}

}  // namespace geompp_tests
