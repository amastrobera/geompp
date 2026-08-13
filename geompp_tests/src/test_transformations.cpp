#include "transformations.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numbers>

namespace g = geompp;
namespace gt = geompp::transformations;
namespace gm = geompp::maths;

#pragma region 2D fast-path (Point2D translate/rotate/scale, no matrix)

TEST(Transformations2DTest, Translate_Point_AddsOffset) {
  auto p = gt::translate(g::Point2D(1, 2), gm::Vector2(10, 20));
  EXPECT_DOUBLE_EQ(p.x(), 11.0);
  EXPECT_DOUBLE_EQ(p.y(), 22.0);
}

TEST(Transformations2DTest, Rotate_Point_QuarterTurn_RotatesXOntoY) {
  auto p = gt::rotate(g::Point2D(1, 0), std::numbers::pi / 2.0);
  EXPECT_NEAR(p.x(), 0.0, 1e-9);
  EXPECT_NEAR(p.y(), 1.0, 1e-9);
}

TEST(Transformations2DTest, Scale_Point_UniformAndNonUniform) {
  auto uniform = gt::scale(g::Point2D(2, 3), 2.0);
  EXPECT_DOUBLE_EQ(uniform.x(), 4.0);
  EXPECT_DOUBLE_EQ(uniform.y(), 6.0);

  auto non_uniform = gt::scale(g::Point2D(2, 3), 2.0, 5.0);
  EXPECT_DOUBLE_EQ(non_uniform.x(), 4.0);
  EXPECT_DOUBLE_EQ(non_uniform.y(), 15.0);
}

#pragma endregion

#pragma region 2D matrix-based transform()

TEST(Transformations2DTest, TransformPoint_TranslationMatrix_MatchesFastPath) {
  auto m = gm::Matrix3::Translation(gm::Vector2(5, -3));
  auto p = gt::transform(g::Point2D(1, 1), m);
  EXPECT_DOUBLE_EQ(p.x(), 6.0);
  EXPECT_DOUBLE_EQ(p.y(), -2.0);
}

TEST(Transformations2DTest, TransformVector_IgnoresTranslation) {
  auto m = gm::Matrix3::Translation(gm::Vector2(100, 100));
  auto v = gt::transform(g::Vector2D(1, 0), m);
  EXPECT_DOUBLE_EQ(v.x(), 1.0);
  EXPECT_DOUBLE_EQ(v.y(), 0.0);
}

TEST(Transformations2DTest, TransformVector_AppliesRotation) {
  auto m = gm::Matrix3::Rotation(std::numbers::pi / 2.0);
  auto v = gt::transform(g::Vector2D(1, 0), m);
  EXPECT_NEAR(v.x(), 0.0, 1e-9);
  EXPECT_NEAR(v.y(), 1.0, 1e-9);
}

TEST(Transformations2DTest, TransformLineSegment_TransformsBothEndpoints) {
  auto seg = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  auto m = gm::Matrix3::Translation(gm::Vector2(0, 5));
  auto moved = gt::transform(seg, m);
  EXPECT_DOUBLE_EQ(moved.First().y(), 5.0);
  EXPECT_DOUBLE_EQ(moved.Last().y(), 5.0);
  EXPECT_NEAR(moved.Length(), seg.Length(), 1e-9);
}

TEST(Transformations2DTest, TransformPolyline_PreservesLengthUnderRigidTransform) {
  auto polyline = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto m = gm::Matrix3::Rotation(1.0) * gm::Matrix3::Translation(gm::Vector2(3, -4));
  auto moved = gt::transform(polyline, m);
  EXPECT_NEAR(moved.Length(), polyline.Length(), 1e-9);
}

TEST(Transformations2DTest, TransformTriangle_PreservesAreaUnderRigidTransform) {
  auto tri = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3));
  auto m = gm::Matrix3::Rotation(0.7) * gm::Matrix3::Translation(gm::Vector2(2, 2));
  auto moved = gt::transform(tri, m);
  EXPECT_NEAR(moved.Area(), tri.Area(), 1e-9);
}

TEST(Transformations2DTest, TransformTriangle_ScaleMultipliesAreaBySquare) {
  auto tri = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3));
  auto moved = gt::transform(tri, gm::Matrix3::Scale(2.0));
  EXPECT_NEAR(moved.Area(), tri.Area() * 4.0, 1e-9);
}

TEST(Transformations2DTest, TransformPolygon_WithHoles_PreservesAreaUnderRigidTransform) {
  auto poly = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(10, 0), g::Point2D(10, 10), g::Point2D(0, 10)},
                                 {{g::Point2D(4, 4), g::Point2D(4, 6), g::Point2D(6, 6), g::Point2D(6, 4)}});
  auto m = gm::Matrix3::Rotation(0.4) * gm::Matrix3::Translation(gm::Vector2(-5, 8));
  auto moved = gt::transform(poly, m);
  EXPECT_TRUE(moved.HasHoles());
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);
}

TEST(Transformations2DTest, TransformMesh_PreservesTotalAreaUnderRigidTransform) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::Mesh2D::FromTriangles({g::Triangle2D::Make(p0[0], p0[1], p0[2]), g::Triangle2D::Make(p0[0], p0[2], p0[3]),
                                        g::Triangle2D::Make(p1[0], p1[1], p1[2]), g::Triangle2D::Make(p1[0], p1[2], p1[3])});
  auto moved = gt::transform(mesh, gm::Matrix3::Rotation(1.2) * gm::Matrix3::Translation(gm::Vector2(1, 1)));
  EXPECT_EQ(moved.Size(), mesh.Size());
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
}

TEST(Transformations2DTest, TransformPolyMesh_PreservesTotalAreaUnderRigidTransform) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto p1 = g::Polygon2D::Make({g::Point2D(1, 0), g::Point2D(2, 0), g::Point2D(2, 1), g::Point2D(1, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0, p1});
  auto moved = gt::transform(mesh, gm::Matrix3::Rotation(0.3) * gm::Matrix3::Translation(gm::Vector2(-1, 2)));
  EXPECT_EQ(moved.Size(), mesh.Size());
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
}

#pragma endregion

#pragma region 3D fast-path (Point3D translate/rotate/scale, no matrix)

TEST(Transformations3DTest, Translate_Point_AddsOffset) {
  auto p = gt::translate(g::Point3D(1, 2, 3), gm::Vector3(10, 20, 30));
  EXPECT_DOUBLE_EQ(p.x(), 11.0);
  EXPECT_DOUBLE_EQ(p.y(), 22.0);
  EXPECT_DOUBLE_EQ(p.z(), 33.0);
}

TEST(Transformations3DTest, Rotate_Point_QuarterTurnAboutZ_RotatesXOntoY) {
  auto p = gt::rotate(g::Point3D(1, 0, 0), std::numbers::pi / 2.0, gm::Vector3(0, 0, 1));
  EXPECT_NEAR(p.x(), 0.0, 1e-9);
  EXPECT_NEAR(p.y(), 1.0, 1e-9);
  EXPECT_NEAR(p.z(), 0.0, 1e-9);
}

TEST(Transformations3DTest, Rotate_Point_ZeroLengthAxis_Throws) {
  EXPECT_THROW(gt::rotate(g::Point3D(1, 0, 0), 1.0, gm::Vector3(0, 0, 0)), std::invalid_argument);
}

TEST(Transformations3DTest, Scale_Point_UniformAndNonUniform) {
  auto uniform = gt::scale(g::Point3D(2, 3, 4), 2.0);
  EXPECT_DOUBLE_EQ(uniform.x(), 4.0);
  EXPECT_DOUBLE_EQ(uniform.z(), 8.0);

  auto non_uniform = gt::scale(g::Point3D(2, 3, 4), 2.0, 5.0, 0.5);
  EXPECT_DOUBLE_EQ(non_uniform.x(), 4.0);
  EXPECT_DOUBLE_EQ(non_uniform.y(), 15.0);
  EXPECT_DOUBLE_EQ(non_uniform.z(), 2.0);
}

#pragma endregion

#pragma region 3D matrix-based transform()

TEST(Transformations3DTest, TransformVector_IgnoresTranslation) {
  auto m = gm::Matrix4::Translation(gm::Vector3(100, 100, 100));
  auto v = gt::transform(g::Vector3D(1, 0, 0), m);
  EXPECT_DOUBLE_EQ(v.x(), 1.0);
  EXPECT_DOUBLE_EQ(v.y(), 0.0);
  EXPECT_DOUBLE_EQ(v.z(), 0.0);
}

TEST(Transformations3DTest, TransformLineSegment_PreservesLengthUnderRigidTransform) {
  auto seg = g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto m = gm::Matrix4::Rotation(0.9, gm::Vector3(0, 1, 0)) * gm::Matrix4::Translation(gm::Vector3(3, -4, 2));
  auto moved = gt::transform(seg, m);
  EXPECT_NEAR(moved.Length(), seg.Length(), 1e-9);
}

TEST(Transformations3DTest, TransformTriangle_PreservesAreaUnderRigidTransform) {
  auto tri = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 3, 0));
  auto m = gm::Matrix4::Rotation(0.5, gm::Vector3(1, 1, 1)) * gm::Matrix4::Translation(gm::Vector3(1, 2, 3));
  auto moved = gt::transform(tri, m);
  EXPECT_NEAR(moved.Area(), tri.Area(), 1e-9);
}

TEST(Transformations3DTest, TransformPolygon_WithHoles_PreservesAreaUnderRigidTransform) {
  auto poly =
      g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(10, 0, 0), g::Point3D(10, 10, 0), g::Point3D(0, 10, 0)},
                         {{g::Point3D(4, 4, 0), g::Point3D(4, 6, 0), g::Point3D(6, 6, 0), g::Point3D(6, 4, 0)}});
  auto m = gm::Matrix4::Rotation(0.4, gm::Vector3(0, 0, 1)) * gm::Matrix4::Translation(gm::Vector3(-5, 8, 1));
  auto moved = gt::transform(poly, m);
  EXPECT_TRUE(moved.HasHoles());
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);
}

TEST(Transformations3DTest, TransformMesh_PreservesTotalAreaUnderRigidTransform) {
  auto a = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto b = g::Triangle3D::Make(g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::Mesh3D::FromTriangles({a, b});
  auto moved = gt::transform(mesh, gm::Matrix4::Rotation(1.1, gm::Vector3(0, 1, 0)) * gm::Matrix4::Translation(gm::Vector3(1, 1, 1)));
  EXPECT_EQ(moved.Size(), mesh.Size());
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
}

TEST(Transformations3DTest, TransformPolyMesh_PreservesTotalAreaUnderRigidTransform) {
  auto p0 = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto p1 = g::Polygon3D::Make({g::Point3D(1, 0, 0), g::Point3D(2, 0, 0), g::Point3D(2, 1, 0), g::Point3D(1, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0, p1});
  auto moved = gt::transform(mesh, gm::Matrix4::Rotation(0.2, gm::Vector3(1, 0, 0)) * gm::Matrix4::Translation(gm::Vector3(-1, 2, 0)));
  EXPECT_EQ(moved.Size(), mesh.Size());
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
}

#pragma endregion

#pragma region TransformBuilder

TEST(TransformBuilderTest, DefaultConstructed_IsIdentity) {
  gt::TransformBuilder builder;
  EXPECT_TRUE(builder.Get() == gm::Matrix4::Identity());
}

TEST(TransformBuilderTest, Translate_MovesPoint) {
  gt::TransformBuilder builder;
  builder.Translate(gm::Vector3(5, 0, 0));
  auto p = gt::transform(g::Point3D(0, 0, 0), builder.Get());
  EXPECT_DOUBLE_EQ(p.x(), 5.0);
}

TEST(TransformBuilderTest, ChainedOps_ApplyInCallOrder) {
  // translate then rotate: (1,0,0) -> translate(+5,0,0) -> (6,0,0) -> rotate 90deg about Z -> (0,6,0)
  gt::TransformBuilder builder;
  builder.Translate(gm::Vector3(5, 0, 0)).Rotate(std::numbers::pi / 2.0, gm::Vector3(0, 0, 1));
  auto p = gt::transform(g::Point3D(1, 0, 0), builder.Get());
  EXPECT_NEAR(p.x(), 0.0, 1e-9);
  EXPECT_NEAR(p.y(), 6.0, 1e-9);
}

TEST(TransformBuilderTest, ReversedChainOrder_ProducesDifferentResult) {
  // rotate then translate: (1,0,0) -> rotate 90deg about Z -> (0,1,0) -> translate(+5,0,0) -> (5,1,0)
  gt::TransformBuilder builder;
  builder.Rotate(std::numbers::pi / 2.0, gm::Vector3(0, 0, 1)).Translate(gm::Vector3(5, 0, 0));
  auto p = gt::transform(g::Point3D(1, 0, 0), builder.Get());
  EXPECT_NEAR(p.x(), 5.0, 1e-9);
  EXPECT_NEAR(p.y(), 1.0, 1e-9);
}

TEST(TransformBuilderTest, Scale_UniformAndNonUniform) {
  gt::TransformBuilder uniform;
  uniform.Scale(2.0);
  auto up = gt::transform(g::Point3D(1, 2, 3), uniform.Get());
  EXPECT_TRUE(up == g::Point3D(2, 4, 6));

  gt::TransformBuilder non_uniform;
  non_uniform.Scale(2.0, 3.0, 4.0);
  auto np = gt::transform(g::Point3D(1, 1, 1), non_uniform.Get());
  EXPECT_TRUE(np == g::Point3D(2, 3, 4));
}

TEST(TransformBuilderTest, Combine_AppliesArbitraryMatrix) {
  gt::TransformBuilder builder;
  builder.Combine(gm::Matrix4::Translation(gm::Vector3(1, 2, 3)));
  auto p = gt::transform(g::Point3D(0, 0, 0), builder.Get());
  EXPECT_TRUE(p == g::Point3D(1, 2, 3));
}

TEST(TransformBuilderTest, Build_ReturnsIndependentCopy) {
  gt::TransformBuilder builder;
  builder.Translate(gm::Vector3(1, 0, 0));
  auto snapshot = builder.Build();
  builder.Translate(gm::Vector3(0, 1, 0));  // further chaining must not retroactively change `snapshot`
  EXPECT_TRUE(snapshot == gm::Matrix4::Translation(gm::Vector3(1, 0, 0)));
}

#pragma endregion
