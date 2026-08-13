#include "transformations.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <numbers>
#include <variant>

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

TEST(Transformations2DTest, Shear_Point_OffsetsAxisByMultipleOfOther) {
  auto p = gt::shear(g::Point2D(1, 3), 2.0, 0.0);
  EXPECT_DOUBLE_EQ(p.x(), 1.0 + 2.0 * 3.0);
  EXPECT_DOUBLE_EQ(p.y(), 3.0);
}

TEST(Transformations2DTest, Reflect_Point_AboutXAxisNormal_FlipsY) {
  auto p = gt::reflect(g::Point2D(3, 4), gm::Vector2(0, 1));
  EXPECT_NEAR(p.x(), 3.0, 1e-9);
  EXPECT_NEAR(p.y(), -4.0, 1e-9);
}

TEST(Transformations2DTest, Reflect_Point_ZeroLengthNormal_Throws) {
  EXPECT_THROW(gt::reflect(g::Point2D(1, 1), gm::Vector2(0, 0)), std::invalid_argument);
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

// Regression: transform() used to hand the raw (now CW) reflected ring straight to Make(), which throws
// on non-CCW input -- reflecting a Polygon2D via transform() used to be impossible. It now detects the
// negative determinant and reverses the ring(s) before construction, so this succeeds.
TEST(Transformations2DTest, TransformPolygon_Reflection_SucceedsAndPreservesArea) {
  auto poly = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 3), g::Point2D(0, 3)});
  auto moved = gt::transform(poly, gm::Matrix3::Reflection(gm::Vector2(1, 0)));
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);
}

TEST(Transformations2DTest, TransformPolygon_WithHoles_Reflection_SucceedsAndPreservesAreaAndContainment) {
  auto poly = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 4), g::Point2D(0, 4)},
                                 {{g::Point2D(1, 1), g::Point2D(1, 3), g::Point2D(3, 3), g::Point2D(3, 1)}});
  auto moved = gt::transform(poly, gm::Matrix3::Reflection(gm::Vector2(1, 0)));
  EXPECT_TRUE(moved.HasHoles());
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);
  EXPECT_TRUE(moved.IsSimple());
  // Reflecting across the Y axis (normal (1,0)) negates x: the hole (originally x in [1,3]) now sits at
  // x in [-3,-1] -- still correctly a hole (excluded), with the surrounding ring still correctly included.
  EXPECT_FALSE(moved.Contains(g::Point2D(-2, 2)));  // inside the (now-mirrored) hole
  EXPECT_TRUE(moved.Contains(g::Point2D(-0.5, 2)));  // inside the donut ring
}

TEST(Transformations2DTest, TransformPolygon_DegenerateTransform_FallsBackAndThrows) {
  auto poly = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 3), g::Point2D(0, 3)});
  // Scale(0, 1) collapses the square onto a zero-width segment -- singular (det == 0), so transform()
  // must fall back to the fully-validated slow path, which correctly rejects the collapsed input rather
  // than silently producing a degenerate "polygon".
  EXPECT_THROW(gt::transform(poly, gm::Matrix3::Scale(0.0, 1.0)), std::runtime_error);
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

#pragma region 2D transform() -- newly covered types

TEST(Transformations2DTest, TransformRay_TransformsOriginAndDirection) {
  auto ray = g::Ray2D::Make(g::Point2D(1, 1), g::Vector2D(1, 0));
  // m = Rotation * Translation applies Translation first, then Rotation (right operand nearest the point
  // applies first) -- same right-to-left composition order as TransformBuilder3D's call-order chaining.
  auto m = gm::Matrix3::Rotation(std::numbers::pi / 2.0) * gm::Matrix3::Translation(gm::Vector2(3, 0));
  auto moved = gt::transform(ray, m);
  EXPECT_NEAR(moved.Origin().x(), -1.0, 1e-9);
  EXPECT_NEAR(moved.Origin().y(), 4.0, 1e-9);
  EXPECT_NEAR(moved.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(moved.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations2DTest, TransformLine_TransformsOriginAndDirection) {
  auto line = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  auto moved = gt::transform(line, gm::Matrix3::Rotation(std::numbers::pi / 2.0));
  EXPECT_NEAR(moved.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(moved.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations2DTest, TransformGeometryCollection_TransformsEveryMember) {
  g::GeometryCollection2D collection;
  collection.Add(g::Point2D(1, 0));
  collection.Add(g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0)));

  auto moved = gt::transform(collection, gm::Matrix3::Translation(gm::Vector2(10, 0)));
  EXPECT_EQ(moved.Size(), collection.Size());
  auto p = std::get<g::Point2D>(moved.Get(0));
  EXPECT_NEAR(p.x(), 11.0, 1e-9);
  auto seg = std::get<g::LineSegment2D>(moved.Get(1));
  EXPECT_NEAR(seg.First().x(), 10.0, 1e-9);
  EXPECT_NEAR(seg.Last().x(), 11.0, 1e-9);
}

TEST(Transformations2DTest, TransformConnectedMesh_PreservesTotalAreaUnderRigidTransform) {
  auto a = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0, 1));
  auto b = g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1));
  auto mesh = g::ConnectedMesh2D::FromTriangles({a, b});
  auto moved = gt::transform(mesh, gm::Matrix3::Rotation(0.9) * gm::Matrix3::Translation(gm::Vector2(2, -3)));
  EXPECT_EQ(moved.Size(), mesh.Size());
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
}

#pragma endregion

#pragma region 2D translate/rotate/scale/shear/reflect wrappers -- newly covered types

TEST(Transformations2DTest, Vector_RotateScaleReflect) {
  auto r = gt::rotate(g::Vector2D(1, 0), std::numbers::pi / 2.0);
  EXPECT_NEAR(r.x(), 0.0, 1e-9);
  EXPECT_NEAR(r.y(), 1.0, 1e-9);

  auto s = gt::scale(g::Vector2D(2, 3), 2.0, 5.0);
  EXPECT_DOUBLE_EQ(s.x(), 4.0);
  EXPECT_DOUBLE_EQ(s.y(), 15.0);

  auto f = gt::reflect(g::Vector2D(3, 4), gm::Vector2(0, 1));
  EXPECT_NEAR(f.x(), 3.0, 1e-9);
  EXPECT_NEAR(f.y(), -4.0, 1e-9);
}

TEST(Transformations2DTest, Ray_TranslateMovesOrigin_RotateTurnsDirection) {
  auto ray = g::Ray2D::Make(g::Point2D(1, 1), g::Vector2D(1, 0));
  auto moved = gt::translate(ray, gm::Vector2(5, 0));
  EXPECT_NEAR(moved.Origin().x(), 6.0, 1e-9);

  auto turned = gt::rotate(ray, std::numbers::pi / 2.0);
  EXPECT_NEAR(turned.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(turned.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations2DTest, Line_TranslateAndRotate_MatchTransform) {
  auto line = g::Line2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  auto moved = gt::translate(line, gm::Vector2(0, 4));
  EXPECT_TRUE(moved.Contains(g::Point2D(0, 4)));

  auto turned = gt::rotate(line, std::numbers::pi / 2.0);
  EXPECT_NEAR(turned.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(turned.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations2DTest, LineSegment_TranslateOffsets_ScaleMultipliesLength) {
  auto seg = g::LineSegment2D::Make(g::Point2D(0, 0), g::Point2D(1, 0));
  auto moved = gt::translate(seg, gm::Vector2(0, 3));
  EXPECT_DOUBLE_EQ(moved.First().y(), 3.0);

  auto scaled = gt::scale(seg, 3.0);
  EXPECT_NEAR(scaled.Length(), seg.Length() * 3.0, 1e-9);
}

TEST(Transformations2DTest, GeometryCollection_TranslateMovesEveryMember) {
  g::GeometryCollection2D collection;
  collection.Add(g::Point2D(0, 0));
  auto moved = gt::translate(collection, gm::Vector2(2, 3));
  auto p = std::get<g::Point2D>(moved.Get(0));
  EXPECT_NEAR(p.x(), 2.0, 1e-9);
  EXPECT_NEAR(p.y(), 3.0, 1e-9);
}

TEST(Transformations2DTest, Polygon_TranslateOffsets_ScaleMultipliesAreaBySquare) {
  auto poly = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(4, 3), g::Point2D(0, 3)});
  auto moved = gt::translate(poly, gm::Vector2(1, 1));
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);

  auto scaled = gt::scale(poly, 2.0);
  EXPECT_NEAR(scaled.Area(), poly.Area() * 4.0, 1e-9);
}

TEST(Transformations2DTest, Polyline_TranslateOffsets_ScaleMultipliesLength) {
  auto polyline = g::Polyline2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1)});
  auto moved = gt::translate(polyline, gm::Vector2(0, 2));
  EXPECT_NEAR(moved.Length(), polyline.Length(), 1e-9);

  auto scaled = gt::scale(polyline, 2.0);
  EXPECT_NEAR(scaled.Length(), polyline.Length() * 2.0, 1e-9);
}

TEST(Transformations2DTest, Triangle_TranslateRotateScaleShearReflect_MatchDirectTransform) {
  auto tri = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(4, 0), g::Point2D(0, 3));

  auto translated = gt::translate(tri, gm::Vector2(1, 1));
  EXPECT_TRUE(translated.AlmostEquals(gt::transform(tri, gm::Matrix3::Translation(gm::Vector2(1, 1)))));

  auto rotated = gt::rotate(tri, 0.5);
  EXPECT_TRUE(rotated.AlmostEquals(gt::transform(tri, gm::Matrix3::Rotation(0.5))));

  auto sheared = gt::shear(tri, 0.3, 0.0);
  EXPECT_TRUE(sheared.AlmostEquals(gt::transform(tri, gm::Matrix3::Shear(0.3, 0.0))));

  auto reflected = gt::reflect(tri, gm::Vector2(0, 1));
  EXPECT_TRUE(reflected.AlmostEquals(gt::transform(tri, gm::Matrix3::Reflection(gm::Vector2(0, 1)))));
}

TEST(Transformations2DTest, Mesh_TranslateRotate_PreserveTotalArea) {
  auto a = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0, 1));
  auto mesh = g::Mesh2D::FromTriangles({a});
  auto moved = gt::translate(mesh, gm::Vector2(1, 1));
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
  auto turned = gt::rotate(mesh, 0.7);
  EXPECT_NEAR(turned.Area(), mesh.Area(), 1e-9);
}

TEST(Transformations2DTest, PolyMesh_TranslateRotate_PreserveTotalArea) {
  auto p0 = g::Polygon2D::Make({g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1)});
  auto mesh = g::PolyMesh2D::FromPolygons({p0});
  auto moved = gt::translate(mesh, gm::Vector2(1, 1));
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
  auto turned = gt::rotate(mesh, 0.7);
  EXPECT_NEAR(turned.Area(), mesh.Area(), 1e-9);
}

TEST(Transformations2DTest, ConnectedMesh_TranslateRotate_PreserveTotalArea) {
  auto a = g::Triangle2D::Make(g::Point2D(0, 0), g::Point2D(1, 0), g::Point2D(0, 1));
  auto b = g::Triangle2D::Make(g::Point2D(1, 0), g::Point2D(1, 1), g::Point2D(0, 1));
  auto mesh = g::ConnectedMesh2D::FromTriangles({a, b});
  auto moved = gt::translate(mesh, gm::Vector2(1, 1));
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
  auto turned = gt::rotate(mesh, 0.7);
  EXPECT_NEAR(turned.Area(), mesh.Area(), 1e-9);
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

TEST(Transformations3DTest, Shear_Point_OffsetsAxisByMultipleOfOther) {
  // xy = 2: x' = x + 2*y, y and z unchanged.
  auto p = gt::shear(g::Point3D(1, 3, 5), 2.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_DOUBLE_EQ(p.x(), 1.0 + 2.0 * 3.0);
  EXPECT_DOUBLE_EQ(p.y(), 3.0);
  EXPECT_DOUBLE_EQ(p.z(), 5.0);
}

TEST(Transformations3DTest, Reflect_Point_AboutXAxisNormal_FlipsY) {
  auto p = gt::reflect(g::Point3D(3, 4, 5), gm::Vector3(0, 1, 0));
  EXPECT_NEAR(p.x(), 3.0, 1e-9);
  EXPECT_NEAR(p.y(), -4.0, 1e-9);
  EXPECT_NEAR(p.z(), 5.0, 1e-9);
}

TEST(Transformations3DTest, Reflect_Point_ZeroLengthNormal_Throws) {
  EXPECT_THROW(gt::reflect(g::Point3D(1, 1, 1), gm::Vector3(0, 0, 0)), std::invalid_argument);
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

// Regression: a plain rotation used to be able to spuriously throw (see the Polygon3D winding-invariance
// fix) -- and separately, transform() used to hand a reflected (winding-flipped) ring straight to Make(),
// which throws on the "wrong" winding, making reflection of a Polygon3D via transform() impossible.
TEST(Transformations3DTest, TransformPolygon_RotationAboutNonPrincipalAxis_DoesNotThrow) {
  auto poly = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  // This specific axis (X) is exactly the one that used to spuriously throw for a normal-+Z square: the
  // rotated normal lands Y-dominant but negative, which the old world-axis-snap heuristic silently flipped.
  auto moved = gt::transform(poly, gm::Matrix4::Rotation(std::numbers::pi / 2.0, gm::Vector3(1, 0, 0)));
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);
}

TEST(Transformations3DTest, TransformPolygon_Reflection_SucceedsAndPreservesArea) {
  auto poly = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 3, 0), g::Point3D(0, 3, 0)});
  auto moved = gt::transform(poly, gm::Matrix4::Reflection(gm::Vector3(1, 0, 0)));
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);
}

TEST(Transformations3DTest, TransformPolygon_WithHoles_Reflection_SucceedsAndPreservesAreaAndContainment) {
  auto poly =
      g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 4, 0), g::Point3D(0, 4, 0)},
                         {{g::Point3D(1, 1, 0), g::Point3D(1, 3, 0), g::Point3D(3, 3, 0), g::Point3D(3, 1, 0)}});
  auto moved = gt::transform(poly, gm::Matrix4::Reflection(gm::Vector3(1, 0, 0)));
  EXPECT_TRUE(moved.HasHoles());
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);
  EXPECT_TRUE(moved.IsSimple());
  EXPECT_FALSE(moved.Contains(g::Point3D(-2, 2, 0)));    // inside the (now-mirrored) hole
  EXPECT_TRUE(moved.Contains(g::Point3D(-0.5, 2, 0)));   // inside the donut ring
}

TEST(Transformations3DTest, TransformPolygon_DegenerateTransform_FallsBackAndThrows) {
  auto poly = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 3, 0), g::Point3D(0, 3, 0)});
  EXPECT_THROW(gt::transform(poly, gm::Matrix4::Scale(0.0, 1.0, 1.0)), std::runtime_error);
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

#pragma region 3D transform() -- newly covered types

TEST(Transformations3DTest, TransformRay_TransformsOriginAndDirection) {
  auto ray = g::Ray3D::Make(g::Point3D(1, 1, 0), g::Vector3D(1, 0, 0));
  // m = Rotation * Translation applies Translation first, then Rotation -- see the 2D overload's test.
  auto m = gm::Matrix4::Rotation(std::numbers::pi / 2.0, gm::Vector3(0, 0, 1)) *
           gm::Matrix4::Translation(gm::Vector3(3, 0, 0));
  auto moved = gt::transform(ray, m);
  EXPECT_NEAR(moved.Origin().x(), -1.0, 1e-9);
  EXPECT_NEAR(moved.Origin().y(), 4.0, 1e-9);
  EXPECT_NEAR(moved.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(moved.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations3DTest, TransformLine_TransformsOriginAndDirection) {
  auto line = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto moved = gt::transform(line, gm::Matrix4::Rotation(std::numbers::pi / 2.0, gm::Vector3(0, 0, 1)));
  EXPECT_NEAR(moved.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(moved.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations3DTest, TransformGeometryCollection_TransformsEveryMember) {
  g::GeometryCollection3D collection;
  collection.Add(g::Point3D(1, 0, 0));
  collection.Add(g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0)));

  auto moved = gt::transform(collection, gm::Matrix4::Translation(gm::Vector3(10, 0, 0)));
  EXPECT_EQ(moved.Size(), collection.Size());
  auto p = std::get<g::Point3D>(moved.Get(0));
  EXPECT_NEAR(p.x(), 11.0, 1e-9);
  auto seg = std::get<g::LineSegment3D>(moved.Get(1));
  EXPECT_NEAR(seg.First().x(), 10.0, 1e-9);
  EXPECT_NEAR(seg.Last().x(), 11.0, 1e-9);
}

TEST(Transformations3DTest, TransformConnectedMesh_PreservesTotalAreaUnderRigidTransform) {
  auto a = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto b = g::Triangle3D::Make(g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({a, b});
  auto moved =
      gt::transform(mesh, gm::Matrix4::Rotation(0.9, gm::Vector3(0, 1, 0)) * gm::Matrix4::Translation(gm::Vector3(2, -3, 1)));
  EXPECT_EQ(moved.Size(), mesh.Size());
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
}

#pragma endregion

#pragma region 3D translate/rotate/scale/shear/reflect wrappers -- newly covered types

TEST(Transformations3DTest, Vector_RotateScaleReflect) {
  auto r = gt::rotate(g::Vector3D(1, 0, 0), std::numbers::pi / 2.0, gm::Vector3(0, 0, 1));
  EXPECT_NEAR(r.x(), 0.0, 1e-9);
  EXPECT_NEAR(r.y(), 1.0, 1e-9);

  auto s = gt::scale(g::Vector3D(2, 3, 4), 2.0, 5.0, 0.5);
  EXPECT_DOUBLE_EQ(s.x(), 4.0);
  EXPECT_DOUBLE_EQ(s.y(), 15.0);
  EXPECT_DOUBLE_EQ(s.z(), 2.0);

  auto f = gt::reflect(g::Vector3D(3, 4, 5), gm::Vector3(0, 1, 0));
  EXPECT_NEAR(f.x(), 3.0, 1e-9);
  EXPECT_NEAR(f.y(), -4.0, 1e-9);
  EXPECT_NEAR(f.z(), 5.0, 1e-9);
}

TEST(Transformations3DTest, Ray_TranslateMovesOrigin_RotateTurnsDirection) {
  auto ray = g::Ray3D::Make(g::Point3D(1, 1, 0), g::Vector3D(1, 0, 0));
  auto moved = gt::translate(ray, gm::Vector3(5, 0, 0));
  EXPECT_NEAR(moved.Origin().x(), 6.0, 1e-9);

  auto turned = gt::rotate(ray, std::numbers::pi / 2.0, gm::Vector3(0, 0, 1));
  EXPECT_NEAR(turned.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(turned.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations3DTest, Line_TranslateAndRotate_MatchTransform) {
  auto line = g::Line3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto moved = gt::translate(line, gm::Vector3(0, 4, 0));
  EXPECT_TRUE(moved.Contains(g::Point3D(0, 4, 0)));

  auto turned = gt::rotate(line, std::numbers::pi / 2.0, gm::Vector3(0, 0, 1));
  EXPECT_NEAR(turned.Direction().x(), 0.0, 1e-9);
  EXPECT_NEAR(turned.Direction().y(), 1.0, 1e-9);
}

TEST(Transformations3DTest, LineSegment_TranslateOffsets_ScaleMultipliesLength) {
  auto seg = g::LineSegment3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0));
  auto moved = gt::translate(seg, gm::Vector3(0, 3, 0));
  EXPECT_DOUBLE_EQ(moved.First().y(), 3.0);

  auto scaled = gt::scale(seg, 3.0);
  EXPECT_NEAR(scaled.Length(), seg.Length() * 3.0, 1e-9);
}

TEST(Transformations3DTest, GeometryCollection_TranslateMovesEveryMember) {
  g::GeometryCollection3D collection;
  collection.Add(g::Point3D(0, 0, 0));
  auto moved = gt::translate(collection, gm::Vector3(2, 3, 4));
  auto p = std::get<g::Point3D>(moved.Get(0));
  EXPECT_NEAR(p.x(), 2.0, 1e-9);
  EXPECT_NEAR(p.y(), 3.0, 1e-9);
  EXPECT_NEAR(p.z(), 4.0, 1e-9);
}

TEST(Transformations3DTest, Polygon_TranslateOffsets_ScaleMultipliesAreaBySquare) {
  auto poly =
      g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(4, 3, 0), g::Point3D(0, 3, 0)});
  auto moved = gt::translate(poly, gm::Vector3(1, 1, 0));
  EXPECT_NEAR(moved.Area(), poly.Area(), 1e-9);

  auto scaled = gt::scale(poly, 2.0);
  EXPECT_NEAR(scaled.Area(), poly.Area() * 4.0, 1e-9);
}

TEST(Transformations3DTest, Polyline_TranslateOffsets_ScaleMultipliesLength) {
  auto polyline = g::Polyline3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0)});
  auto moved = gt::translate(polyline, gm::Vector3(0, 2, 0));
  EXPECT_NEAR(moved.Length(), polyline.Length(), 1e-9);

  auto scaled = gt::scale(polyline, 2.0);
  EXPECT_NEAR(scaled.Length(), polyline.Length() * 2.0, 1e-9);
}

TEST(Transformations3DTest, Triangle_TranslateRotateScaleShearReflect_MatchDirectTransform) {
  auto tri = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(4, 0, 0), g::Point3D(0, 3, 0));

  auto translated = gt::translate(tri, gm::Vector3(1, 1, 1));
  EXPECT_TRUE(translated.AlmostEquals(gt::transform(tri, gm::Matrix4::Translation(gm::Vector3(1, 1, 1)))));

  auto rotated = gt::rotate(tri, 0.5, gm::Vector3(0, 0, 1));
  EXPECT_TRUE(rotated.AlmostEquals(gt::transform(tri, gm::Matrix4::Rotation(0.5, gm::Vector3(0, 0, 1)))));

  auto sheared = gt::shear(tri, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0);
  EXPECT_TRUE(sheared.AlmostEquals(gt::transform(tri, gm::Matrix4::Shear(0.3, 0.0, 0.0, 0.0, 0.0, 0.0))));

  auto reflected = gt::reflect(tri, gm::Vector3(0, 1, 0));
  EXPECT_TRUE(reflected.AlmostEquals(gt::transform(tri, gm::Matrix4::Reflection(gm::Vector3(0, 1, 0)))));
}

TEST(Transformations3DTest, Mesh_TranslateRotate_PreserveTotalArea) {
  auto a = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto mesh = g::Mesh3D::FromTriangles({a});
  auto moved = gt::translate(mesh, gm::Vector3(1, 1, 1));
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
  auto turned = gt::rotate(mesh, 0.7, gm::Vector3(0, 1, 0));
  EXPECT_NEAR(turned.Area(), mesh.Area(), 1e-9);
}

TEST(Transformations3DTest, PolyMesh_TranslateRotate_PreserveTotalArea) {
  auto p0 = g::Polygon3D::Make({g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0)});
  auto mesh = g::PolyMesh3D::FromPolygons({p0});
  auto moved = gt::translate(mesh, gm::Vector3(1, 1, 1));
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
  auto turned = gt::rotate(mesh, 0.7, gm::Vector3(0, 1, 0));
  EXPECT_NEAR(turned.Area(), mesh.Area(), 1e-9);
}

TEST(Transformations3DTest, ConnectedMesh_TranslateRotate_PreserveTotalArea) {
  auto a = g::Triangle3D::Make(g::Point3D(0, 0, 0), g::Point3D(1, 0, 0), g::Point3D(0, 1, 0));
  auto b = g::Triangle3D::Make(g::Point3D(1, 0, 0), g::Point3D(1, 1, 0), g::Point3D(0, 1, 0));
  auto mesh = g::ConnectedMesh3D::FromTriangles({a, b});
  auto moved = gt::translate(mesh, gm::Vector3(1, 1, 1));
  EXPECT_NEAR(moved.Area(), mesh.Area(), 1e-9);
  auto turned = gt::rotate(mesh, 0.7, gm::Vector3(0, 1, 0));
  EXPECT_NEAR(turned.Area(), mesh.Area(), 1e-9);
}

#pragma endregion

#pragma region TransformBuilder3D

TEST(TransformBuilder3DTest, DefaultConstructed_IsIdentity) {
  gt::TransformBuilder3D builder;
  EXPECT_TRUE(builder.Get() == gm::Matrix4::Identity());
}

TEST(TransformBuilder3DTest, Translate_MovesPoint) {
  gt::TransformBuilder3D builder;
  builder.Translate(gm::Vector3(5, 0, 0));
  auto p = gt::transform(g::Point3D(0, 0, 0), builder.Get());
  EXPECT_DOUBLE_EQ(p.x(), 5.0);
}

TEST(TransformBuilder3DTest, ChainedOps_ApplyInCallOrder) {
  // translate then rotate: (1,0,0) -> translate(+5,0,0) -> (6,0,0) -> rotate 90deg about Z -> (0,6,0)
  gt::TransformBuilder3D builder;
  builder.Translate(gm::Vector3(5, 0, 0)).Rotate(std::numbers::pi / 2.0, gm::Vector3(0, 0, 1));
  auto p = gt::transform(g::Point3D(1, 0, 0), builder.Get());
  EXPECT_NEAR(p.x(), 0.0, 1e-9);
  EXPECT_NEAR(p.y(), 6.0, 1e-9);
}

TEST(TransformBuilder3DTest, ReversedChainOrder_ProducesDifferentResult) {
  // rotate then translate: (1,0,0) -> rotate 90deg about Z -> (0,1,0) -> translate(+5,0,0) -> (5,1,0)
  gt::TransformBuilder3D builder;
  builder.Rotate(std::numbers::pi / 2.0, gm::Vector3(0, 0, 1)).Translate(gm::Vector3(5, 0, 0));
  auto p = gt::transform(g::Point3D(1, 0, 0), builder.Get());
  EXPECT_NEAR(p.x(), 5.0, 1e-9);
  EXPECT_NEAR(p.y(), 1.0, 1e-9);
}

TEST(TransformBuilder3DTest, Scale_UniformAndNonUniform) {
  gt::TransformBuilder3D uniform;
  uniform.Scale(2.0);
  auto up = gt::transform(g::Point3D(1, 2, 3), uniform.Get());
  EXPECT_TRUE(up == g::Point3D(2, 4, 6));

  gt::TransformBuilder3D non_uniform;
  non_uniform.Scale(2.0, 3.0, 4.0);
  auto np = gt::transform(g::Point3D(1, 1, 1), non_uniform.Get());
  EXPECT_TRUE(np == g::Point3D(2, 3, 4));
}

TEST(TransformBuilder3DTest, Combine_AppliesArbitraryMatrix) {
  gt::TransformBuilder3D builder;
  builder.Combine(gm::Matrix4::Translation(gm::Vector3(1, 2, 3)));
  auto p = gt::transform(g::Point3D(0, 0, 0), builder.Get());
  EXPECT_TRUE(p == g::Point3D(1, 2, 3));
}

TEST(TransformBuilder3DTest, Shear_OffsetsAxisByMultipleOfOther) {
  gt::TransformBuilder3D builder;
  builder.Shear(2.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  auto p = gt::transform(g::Point3D(1, 3, 5), builder.Get());
  EXPECT_NEAR(p.x(), 1.0 + 2.0 * 3.0, 1e-9);
  EXPECT_NEAR(p.y(), 3.0, 1e-9);
  EXPECT_NEAR(p.z(), 5.0, 1e-9);
}

TEST(TransformBuilder3DTest, Reflect_AboutXAxisNormal_FlipsY) {
  gt::TransformBuilder3D builder;
  builder.Reflect(gm::Vector3(0, 1, 0));
  auto p = gt::transform(g::Point3D(3, 4, 5), builder.Get());
  EXPECT_NEAR(p.x(), 3.0, 1e-9);
  EXPECT_NEAR(p.y(), -4.0, 1e-9);
  EXPECT_NEAR(p.z(), 5.0, 1e-9);
}

TEST(TransformBuilder3DTest, Reflect_ZeroLengthNormal_Throws) {
  gt::TransformBuilder3D builder;
  EXPECT_THROW(builder.Reflect(gm::Vector3(0, 0, 0)), std::invalid_argument);
}

TEST(TransformBuilder3DTest, Build_ReturnsIndependentCopy) {
  gt::TransformBuilder3D builder;
  builder.Translate(gm::Vector3(1, 0, 0));
  auto snapshot = builder.Build();
  builder.Translate(gm::Vector3(0, 1, 0));  // further chaining must not retroactively change `snapshot`
  EXPECT_TRUE(snapshot == gm::Matrix4::Translation(gm::Vector3(1, 0, 0)));
}

#pragma endregion

#pragma region TransformBuilder2D

TEST(TransformBuilder2DTest, DefaultConstructed_IsIdentity) {
  gt::TransformBuilder2D builder;
  EXPECT_TRUE(builder.Get() == gm::Matrix3::Identity());
}

TEST(TransformBuilder2DTest, Translate_MovesPoint) {
  gt::TransformBuilder2D builder;
  builder.Translate(gm::Vector2(5, 0));
  auto p = gt::transform(g::Point2D(0, 0), builder.Get());
  EXPECT_DOUBLE_EQ(p.x(), 5.0);
}

TEST(TransformBuilder2DTest, ChainedOps_ApplyInCallOrder) {
  // translate then rotate: (1,0) -> translate(+5,0) -> (6,0) -> rotate 90deg -> (0,6)
  gt::TransformBuilder2D builder;
  builder.Translate(gm::Vector2(5, 0)).Rotate(std::numbers::pi / 2.0);
  auto p = gt::transform(g::Point2D(1, 0), builder.Get());
  EXPECT_NEAR(p.x(), 0.0, 1e-9);
  EXPECT_NEAR(p.y(), 6.0, 1e-9);
}

TEST(TransformBuilder2DTest, ReversedChainOrder_ProducesDifferentResult) {
  // rotate then translate: (1,0) -> rotate 90deg -> (0,1) -> translate(+5,0) -> (5,1)
  gt::TransformBuilder2D builder;
  builder.Rotate(std::numbers::pi / 2.0).Translate(gm::Vector2(5, 0));
  auto p = gt::transform(g::Point2D(1, 0), builder.Get());
  EXPECT_NEAR(p.x(), 5.0, 1e-9);
  EXPECT_NEAR(p.y(), 1.0, 1e-9);
}

TEST(TransformBuilder2DTest, Scale_UniformAndNonUniform) {
  gt::TransformBuilder2D uniform;
  uniform.Scale(2.0);
  auto up = gt::transform(g::Point2D(1, 2), uniform.Get());
  EXPECT_TRUE(up == g::Point2D(2, 4));

  gt::TransformBuilder2D non_uniform;
  non_uniform.Scale(2.0, 3.0);
  auto np = gt::transform(g::Point2D(1, 1), non_uniform.Get());
  EXPECT_TRUE(np == g::Point2D(2, 3));
}

TEST(TransformBuilder2DTest, Combine_AppliesArbitraryMatrix) {
  gt::TransformBuilder2D builder;
  builder.Combine(gm::Matrix3::Translation(gm::Vector2(1, 2)));
  auto p = gt::transform(g::Point2D(0, 0), builder.Get());
  EXPECT_TRUE(p == g::Point2D(1, 2));
}

TEST(TransformBuilder2DTest, Shear_OffsetsAxisByMultipleOfOther) {
  gt::TransformBuilder2D builder;
  builder.Shear(2.0, 0.0);
  auto p = gt::transform(g::Point2D(1, 3), builder.Get());
  EXPECT_NEAR(p.x(), 1.0 + 2.0 * 3.0, 1e-9);
  EXPECT_NEAR(p.y(), 3.0, 1e-9);
}

TEST(TransformBuilder2DTest, Reflect_AboutXAxisNormal_FlipsY) {
  gt::TransformBuilder2D builder;
  builder.Reflect(gm::Vector2(0, 1));
  auto p = gt::transform(g::Point2D(3, 4), builder.Get());
  EXPECT_NEAR(p.x(), 3.0, 1e-9);
  EXPECT_NEAR(p.y(), -4.0, 1e-9);
}

TEST(TransformBuilder2DTest, Reflect_ZeroLengthNormal_Throws) {
  gt::TransformBuilder2D builder;
  EXPECT_THROW(builder.Reflect(gm::Vector2(0, 0)), std::invalid_argument);
}

TEST(TransformBuilder2DTest, Build_ReturnsIndependentCopy) {
  gt::TransformBuilder2D builder;
  builder.Translate(gm::Vector2(1, 0));
  auto snapshot = builder.Build();
  builder.Translate(gm::Vector2(0, 1));  // further chaining must not retroactively change `snapshot`
  EXPECT_TRUE(snapshot == gm::Matrix3::Translation(gm::Vector2(1, 0)));
}

#pragma endregion

#pragma region Vector2D/3D direct-arithmetic fast path matches general transform()

TEST(Transformations2DTest, Vector_DirectArithmetic_MatchesMatrixTransform) {
  auto v = g::Vector2D(2, 3);
  EXPECT_TRUE(gt::rotate(v, 0.6) == gt::transform(v, gm::Matrix3::Rotation(0.6)));
  EXPECT_TRUE(gt::scale(v, 2.0, 5.0) == gt::transform(v, gm::Matrix3::Scale(2.0, 5.0)));
  EXPECT_TRUE(gt::shear(v, 0.3, 0.1) == gt::transform(v, gm::Matrix3::Shear(0.3, 0.1)));
  EXPECT_TRUE(gt::reflect(v, gm::Vector2(0, 1)) == gt::transform(v, gm::Matrix3::Reflection(gm::Vector2(0, 1))));
}

TEST(Transformations3DTest, Vector_DirectArithmetic_MatchesMatrixTransform) {
  auto v = g::Vector3D(2, 3, 4);
  EXPECT_TRUE(gt::scale(v, 2.0, 5.0, 0.5) == gt::transform(v, gm::Matrix4::Scale(2.0, 5.0, 0.5)));
  EXPECT_TRUE(gt::shear(v, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0) ==
              gt::transform(v, gm::Matrix4::Shear(0.3, 0.0, 0.0, 0.0, 0.0, 0.0)));
  EXPECT_TRUE(gt::reflect(v, gm::Vector3(0, 1, 0)) == gt::transform(v, gm::Matrix4::Reflection(gm::Vector3(0, 1, 0))));
}

#pragma endregion
