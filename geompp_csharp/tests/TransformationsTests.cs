using GeomPP;
using GeomPP.Maths;
using GeomPP.Transformations;

namespace GeomPPTests {

public static class TransformationsTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 9) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);

    // ── 2D fast path ────────────────────────────────────────────────────────────
    Console.WriteLine("\nTransformations 2D (fast path)");

    Test("Transform_Translate_Point2D_AddsOffset", () => {
      var p = Transform.Translate(new Point2D(1, 2), new Vector2(10, 20));
      Eq(11, p.X, 0); Eq(22, p.Y, 0);
    });

    Test("Transform_Rotate_Point2D_QuarterTurn_RotatesXOntoY", () => {
      var p = Transform.Rotate(new Point2D(1, 0), Math.PI / 2.0);
      Eq(0, p.X, 6); Eq(1, p.Y, 6);
    });

    Test("Transform_Scale_Point2D_UniformAndNonUniform", () => {
      var uniform = Transform.Scale(new Point2D(2, 3), 2.0);
      Eq(4, uniform.X, 0); Eq(6, uniform.Y, 0);
      var nonUniform = Transform.Scale(new Point2D(2, 3), 2.0, 5.0);
      Eq(4, nonUniform.X, 0); Eq(15, nonUniform.Y, 0);
    });

    Test("Transform_Shear_Point2D_OffsetsAxisByMultipleOfOther", () => {
      var p = Transform.Shear(new Point2D(1, 3), 2.0, 0.0);
      Eq(1.0 + 2.0 * 3.0, p.X, 9); Eq(3.0, p.Y, 9);
    });

    Test("Transform_Reflect_Point2D_AboutXAxisNormal_FlipsY", () => {
      var p = Transform.Reflect(new Point2D(3, 4), new Vector2(0, 1));
      Eq(3.0, p.X, 6); Eq(-4.0, p.Y, 6);
    });

    Test("Transform_Reflect_Point2D_ZeroLengthNormal_Throws", () => {
      bool threw = false;
      try { Transform.Reflect(new Point2D(1, 1), new Vector2(0, 0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length normal Reflect() to throw");
    });

    // ── 2D general path ─────────────────────────────────────────────────────────
    Console.WriteLine("\nTransformations 2D (Matrix3)");

    Test("Transform_Apply_Vector2D_IgnoresTranslation", () => {
      var m = Matrix3.Translation(new Vector2(100, 100));
      var v = Transform.Apply(new Vector2D(1, 0), m);
      Eq(1, v.X, 0); Eq(0, v.Y, 0);
    });

    Test("Transform_Apply_LineSegment2D_PreservesLength", () => {
      var seg = LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var m = Matrix3.Rotation(0.6) * Matrix3.Translation(new Vector2(3, -2));
      var moved = Transform.Apply(seg, m);
      Eq(seg.Length(), moved.Length(), 9);
    });

    Test("Transform_Apply_Triangle2D_PreservesAreaUnderRigidTransform", () => {
      var tri = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      var m = Matrix3.Rotation(0.7) * Matrix3.Translation(new Vector2(2, 2));
      var moved = Transform.Apply(tri, m);
      Eq(tri.Area(), moved.Area(), 9);
    });

    Test("Transform_Apply_Triangle2D_ScaleMultipliesAreaBySquare", () => {
      var tri = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      var moved = Transform.Apply(tri, Matrix3.Scale(2.0));
      Eq(tri.Area() * 4.0, moved.Area(), 9);
    });

    Test("Transform_Apply_Polygon2D_WithHoles_PreservesArea", () => {
      var outer = new Point2D[] { new(0, 0), new(10, 0), new(10, 10), new(0, 10) };
      var hole = new Point2D[] { new(4, 4), new(4, 6), new(6, 6), new(6, 4) };
      var poly = Polygon2D.Make(outer, new Point2D[][] { hole });
      var m = Matrix3.Rotation(0.4) * Matrix3.Translation(new Vector2(-5, 8));
      var moved = Transform.Apply(poly, m);
      IsTrue(moved.HasHoles());
      Eq(poly.Area(), moved.Area(), 9);
    });

    // Regression: transform() used to hand the raw (now CW) reflected ring straight to Make(), which
    // throws on non-CCW input -- reflecting a Polygon2D via transform() used to be impossible. It now
    // detects the negative determinant and reverses the ring(s) before construction, so this succeeds.
    Test("Transform_Apply_Polygon2D_Reflection_SucceedsAndPreservesArea", () => {
      var poly = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 3), new(0, 3) });
      var moved = Transform.Apply(poly, Matrix3.Reflection(new Vector2(1, 0)));
      Eq(poly.Area(), moved.Area(), 9);
    });

    Test("Transform_Apply_Polygon2D_WithHoles_Reflection_SucceedsAndPreservesContainment", () => {
      var outer = new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(0, 4) };
      var hole = new Point2D[] { new(1, 1), new(1, 3), new(3, 3), new(3, 1) };
      var poly = Polygon2D.Make(outer, new Point2D[][] { hole });
      var moved = Transform.Apply(poly, Matrix3.Reflection(new Vector2(1, 0)));
      IsTrue(moved.HasHoles());
      Eq(poly.Area(), moved.Area(), 9);
      IsTrue(moved.IsSimple());
      // Reflecting across the Y axis (normal (1,0)) negates x: the hole (originally x in [1,3]) now sits
      // at x in [-3,-1] -- still correctly a hole, with the surrounding ring still correctly included.
      IsTrue(!moved.Contains(new Point2D(-2, 2)), "expected the mirrored hole location to be excluded");
      IsTrue(moved.Contains(new Point2D(-0.5, 2)), "expected the donut ring to still be included");
    });

    Test("Transform_Apply_Polygon2D_DegenerateTransform_FallsBackAndThrows", () => {
      var poly = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 3), new(0, 3) });
      bool threw = false;
      try { Transform.Apply(poly, Matrix3.Scale(0.0, 1.0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected a singular (det==0) transform to fall back to Make() and throw");
    });

    Test("Transform_Apply_Mesh2D_PreservesTotalArea", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var t0 = Triangle2D.Make(p0[0], p0[1], p0[2]);
      var t1 = Triangle2D.Make(p0[0], p0[2], p0[3]);
      var mesh = Mesh2D.FromTriangles(new[] { t0, t1 });
      var m = Matrix3.Rotation(1.2) * Matrix3.Translation(new Vector2(1, 1));
      var moved = Transform.Apply(mesh, m);
      Eq(mesh.Size(), moved.Size(), 0);
      Eq(mesh.Area(), moved.Area(), 9);
    });

    Test("Transform_Apply_PolyMesh2D_PreservesTotalArea", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var mesh = PolyMesh2D.FromPolygons(new[] { p0, p1 });
      var m = Matrix3.Rotation(0.3) * Matrix3.Translation(new Vector2(-1, 2));
      var moved = Transform.Apply(mesh, m);
      Eq(mesh.Size(), moved.Size(), 0);
      Eq(mesh.Area(), moved.Area(), 9);
    });

    // ── 3D fast path ────────────────────────────────────────────────────────────
    Console.WriteLine("\nTransformations 3D (fast path)");

    Test("Transform_Translate_Point3D_AddsOffset", () => {
      var p = Transform.Translate(new Point3D(1, 2, 3), new Vector3(10, 20, 30));
      Eq(11, p.X, 0); Eq(22, p.Y, 0); Eq(33, p.Z, 0);
    });

    Test("Transform_Rotate_Point3D_QuarterTurnAboutZ_RotatesXOntoY", () => {
      var p = Transform.Rotate(new Point3D(1, 0, 0), Math.PI / 2.0, new Vector3(0, 0, 1));
      Eq(0, p.X, 6); Eq(1, p.Y, 6);
    });

    Test("Transform_Rotate_Point3D_ZeroAxis_Throws", () => {
      bool threw = false;
      try { Transform.Rotate(new Point3D(1, 0, 0), 1.0, new Vector3(0, 0, 0)); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length axis Rotate() to throw");
    });

    Test("Transform_Scale_Point3D_UniformAndNonUniform", () => {
      var uniform = Transform.Scale(new Point3D(2, 3, 4), 2.0);
      Eq(4, uniform.X, 0); Eq(8, uniform.Z, 0);
      var nonUniform = Transform.Scale(new Point3D(2, 3, 4), 2.0, 5.0, 0.5);
      Eq(4, nonUniform.X, 0); Eq(15, nonUniform.Y, 0); Eq(2, nonUniform.Z, 0);
    });

    Test("Transform_Shear_Point3D_OffsetsAxisByMultipleOfOther", () => {
      var p = Transform.Shear(new Point3D(1, 3, 5), 2.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      Eq(1.0 + 2.0 * 3.0, p.X, 9); Eq(3.0, p.Y, 9); Eq(5.0, p.Z, 9);
    });

    Test("Transform_Reflect_Point3D_AboutXAxisNormal_FlipsY", () => {
      var p = Transform.Reflect(new Point3D(3, 4, 5), new Vector3(0, 1, 0));
      Eq(3.0, p.X, 6); Eq(-4.0, p.Y, 6); Eq(5.0, p.Z, 6);
    });

    Test("Transform_Reflect_Point3D_ZeroLengthNormal_Throws", () => {
      bool threw = false;
      try { Transform.Reflect(new Point3D(1, 1, 1), new Vector3(0, 0, 0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length normal Reflect() to throw");
    });

    // ── 3D general path ─────────────────────────────────────────────────────────
    Console.WriteLine("\nTransformations 3D (Matrix4)");

    Test("Transform_Apply_Vector3D_IgnoresTranslation", () => {
      var m = Matrix4.Translation(new Vector3(100, 100, 100));
      var v = Transform.Apply(new Vector3D(1, 0, 0), m);
      Eq(1, v.X, 0); Eq(0, v.Y, 0); Eq(0, v.Z, 0);
    });

    Test("Transform_Apply_Triangle3D_PreservesAreaUnderRigidTransform", () => {
      var tri = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0), new Point3D(0, 3, 0));
      var m = Matrix4.Rotation(0.5, new Vector3(1, 1, 1)) * Matrix4.Translation(new Vector3(1, 2, 3));
      var moved = Transform.Apply(tri, m);
      Eq(tri.Area(), moved.Area(), 9);
    });

    Test("Transform_Apply_Polygon3D_WithHoles_PreservesArea", () => {
      var outer = new Point3D[] { new(0, 0, 0), new(10, 0, 0), new(10, 10, 0), new(0, 10, 0) };
      var hole = new Point3D[] { new(4, 4, 0), new(4, 6, 0), new(6, 6, 0), new(6, 4, 0) };
      var poly = Polygon3D.Make(outer, new Point3D[][] { hole });
      var m = Matrix4.Rotation(0.4, new Vector3(0, 0, 1)) * Matrix4.Translation(new Vector3(-5, 8, 1));
      var moved = Transform.Apply(poly, m);
      IsTrue(moved.HasHoles());
      Eq(poly.Area(), moved.Area(), 9);
    });

    // Regression: a plain rotation used to be able to spuriously throw (see the Polygon3D winding-
    // invariance fix) -- and separately, transform() used to hand a reflected (winding-flipped) ring
    // straight to Make(), which throws on the "wrong" winding, making reflection impossible.
    Test("Transform_Apply_Polygon3D_RotationAboutNonPrincipalAxis_DoesNotThrow", () => {
      var poly = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      // This specific axis (X) is exactly the one that used to spuriously throw for a normal-+Z square.
      var moved = Transform.Apply(poly, Matrix4.Rotation(Math.PI / 2.0, new Vector3(1, 0, 0)));
      Eq(poly.Area(), moved.Area(), 9);
    });

    Test("Transform_Apply_Polygon3D_Reflection_SucceedsAndPreservesArea", () => {
      var poly = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 3, 0), new(0, 3, 0) });
      var moved = Transform.Apply(poly, Matrix4.Reflection(new Vector3(1, 0, 0)));
      Eq(poly.Area(), moved.Area(), 9);
    });

    Test("Transform_Apply_Polygon3D_WithHoles_Reflection_SucceedsAndPreservesContainment", () => {
      var outer = new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 4, 0), new(0, 4, 0) };
      var hole = new Point3D[] { new(1, 1, 0), new(1, 3, 0), new(3, 3, 0), new(3, 1, 0) };
      var poly = Polygon3D.Make(outer, new Point3D[][] { hole });
      var moved = Transform.Apply(poly, Matrix4.Reflection(new Vector3(1, 0, 0)));
      IsTrue(moved.HasHoles());
      Eq(poly.Area(), moved.Area(), 9);
      IsTrue(moved.IsSimple());
      IsTrue(!moved.Contains(new Point3D(-2, 2, 0)), "expected the mirrored hole location to be excluded");
      IsTrue(moved.Contains(new Point3D(-0.5, 2, 0)), "expected the donut ring to still be included");
    });

    Test("Transform_Apply_Polygon3D_DegenerateTransform_FallsBackAndThrows", () => {
      var poly = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 3, 0), new(0, 3, 0) });
      bool threw = false;
      try { Transform.Apply(poly, Matrix4.Scale(0.0, 1.0, 1.0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected a singular (det==0) transform to fall back to Make() and throw");
    });

    Test("Transform_Apply_Mesh3D_PreservesTotalArea", () => {
      var a = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var b = Triangle3D.Make(new Point3D(1, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0));
      var mesh = Mesh3D.FromTriangles(new[] { a, b });
      var m = Matrix4.Rotation(1.1, new Vector3(0, 1, 0)) * Matrix4.Translation(new Vector3(1, 1, 1));
      var moved = Transform.Apply(mesh, m);
      Eq(mesh.Size(), moved.Size(), 0);
      Eq(mesh.Area(), moved.Area(), 9);
    });

    Test("Transform_Apply_PolyMesh3D_PreservesTotalArea", () => {
      var p0 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      var p1 = Polygon3D.Make(new Point3D[] { new(1, 0, 0), new(2, 0, 0), new(2, 1, 0), new(1, 1, 0) });
      var mesh = PolyMesh3D.FromPolygons(new[] { p0, p1 });
      var m = Matrix4.Rotation(0.2, new Vector3(1, 0, 0)) * Matrix4.Translation(new Vector3(-1, 2, 0));
      var moved = Transform.Apply(mesh, m);
      Eq(mesh.Size(), moved.Size(), 0);
      Eq(mesh.Area(), moved.Area(), 9);
    });

    // ── TransformBuilder3D ─────────────────────────────────────────────────────────
    Console.WriteLine("\nTransformBuilder3D");

    Test("TransformBuilder3D_DefaultConstructed_IsIdentity", () => {
      var builder = new TransformBuilder3D();
      IsTrue(builder.Get() == Matrix4.Identity());
    });

    Test("TransformBuilder3D_ChainedOps_ApplyInCallOrder", () => {
      // translate then rotate: (1,0,0) -> translate(+5,0,0) -> (6,0,0) -> rotate 90deg about Z -> (0,6,0)
      var builder = new TransformBuilder3D();
      builder.Translate(new Vector3(5, 0, 0)).Rotate(Math.PI / 2.0, new Vector3(0, 0, 1));
      var p = Transform.Apply(new Point3D(1, 0, 0), builder.Get());
      Eq(0, p.X, 6); Eq(6, p.Y, 6);
    });

    Test("TransformBuilder3D_ReversedChainOrder_ProducesDifferentResult", () => {
      var builder = new TransformBuilder3D();
      builder.Rotate(Math.PI / 2.0, new Vector3(0, 0, 1)).Translate(new Vector3(5, 0, 0));
      var p = Transform.Apply(new Point3D(1, 0, 0), builder.Get());
      Eq(5, p.X, 6); Eq(1, p.Y, 6);
    });

    Test("TransformBuilder3D_Combine_AppliesArbitraryMatrix", () => {
      var builder = new TransformBuilder3D();
      builder.Combine(Matrix4.Translation(new Vector3(1, 2, 3)));
      var p = Transform.Apply(new Point3D(0, 0, 0), builder.Get());
      IsTrue(p == new Point3D(1, 2, 3));
    });

    Test("TransformBuilder3D_Shear_OffsetsAxisByMultipleOfOther", () => {
      var builder = new TransformBuilder3D();
      builder.Shear(2.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      var p = Transform.Apply(new Point3D(1, 3, 5), builder.Get());
      Eq(1.0 + 2.0 * 3.0, p.X, 6); Eq(3.0, p.Y, 6); Eq(5.0, p.Z, 6);
    });

    Test("TransformBuilder3D_Reflect_AboutXAxisNormal_FlipsY", () => {
      var builder = new TransformBuilder3D();
      builder.Reflect(new Vector3(0, 1, 0));
      var p = Transform.Apply(new Point3D(3, 4, 5), builder.Get());
      Eq(3.0, p.X, 6); Eq(-4.0, p.Y, 6); Eq(5.0, p.Z, 6);
    });

    Test("TransformBuilder3D_Reflect_ZeroLengthNormal_Throws", () => {
      var builder = new TransformBuilder3D();
      bool threw = false;
      try { builder.Reflect(new Vector3(0, 0, 0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length normal Reflect() to throw");
    });

    Test("TransformBuilder3D_Build_ReturnsIndependentSnapshot", () => {
      var builder = new TransformBuilder3D();
      builder.Translate(new Vector3(1, 0, 0));
      var snapshot = builder.Build();
      builder.Translate(new Vector3(0, 1, 0));
      IsTrue(snapshot == Matrix4.Translation(new Vector3(1, 0, 0)));
    });

    Test("TransformBuilder3D_Apply_MatchesTransformApplyWithGet", () => {
      var builder = new TransformBuilder3D();
      builder.Translate(new Vector3(5, 0, 0)).Rotate(Math.PI / 2.0, new Vector3(0, 0, 1));
      var p = new Point3D(1, 0, 0);
      IsTrue(builder.Apply(p) == Transform.Apply(p, builder.Get()));
    });

    Test("TransformBuilder3D_Apply_WorksOnCompositePrimitive", () => {
      var builder = new TransformBuilder3D();
      builder.Translate(new Vector3(0, 0, 5));
      var poly = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      IsTrue(builder.Apply(poly) == Transform.Apply(poly, builder.Get()));
    });

    Test("TransformBuilder3D_Apply_DoesNotConsumeShapeOrBuilder", () => {
      var builder = new TransformBuilder3D();
      builder.Translate(new Vector3(1, 0, 0));
      var p = new Point3D(0, 0, 0);

      var first = builder.Apply(p);
      IsTrue(p == new Point3D(0, 0, 0));
      IsTrue(first == new Point3D(1, 0, 0));

      builder.Translate(new Vector3(0, 1, 0));
      var second = builder.Apply(p);
      IsTrue(second == new Point3D(1, 1, 0));
    });

    // ── TransformBuilder2D ─────────────────────────────────────────────────────────
    Console.WriteLine("\nTransformBuilder2D");

    Test("TransformBuilder2D_DefaultConstructed_IsIdentity", () => {
      var builder = new TransformBuilder2D();
      IsTrue(builder.Get() == Matrix3.Identity());
    });

    Test("TransformBuilder2D_ChainedOps_ApplyInCallOrder", () => {
      // translate then rotate: (1,0) -> translate(+5,0) -> (6,0) -> rotate 90deg -> (0,6)
      var builder = new TransformBuilder2D();
      builder.Translate(new Vector2(5, 0)).Rotate(Math.PI / 2.0);
      var p = Transform.Apply(new Point2D(1, 0), builder.Get());
      Eq(0, p.X, 6); Eq(6, p.Y, 6);
    });

    Test("TransformBuilder2D_ReversedChainOrder_ProducesDifferentResult", () => {
      var builder = new TransformBuilder2D();
      builder.Rotate(Math.PI / 2.0).Translate(new Vector2(5, 0));
      var p = Transform.Apply(new Point2D(1, 0), builder.Get());
      Eq(5, p.X, 6); Eq(1, p.Y, 6);
    });

    Test("TransformBuilder2D_Combine_AppliesArbitraryMatrix", () => {
      var builder = new TransformBuilder2D();
      builder.Combine(Matrix3.Translation(new Vector2(1, 2)));
      var p = Transform.Apply(new Point2D(0, 0), builder.Get());
      IsTrue(p == new Point2D(1, 2));
    });

    Test("TransformBuilder2D_Shear_OffsetsAxisByMultipleOfOther", () => {
      var builder = new TransformBuilder2D();
      builder.Shear(2.0, 0.0);
      var p = Transform.Apply(new Point2D(1, 3), builder.Get());
      Eq(1.0 + 2.0 * 3.0, p.X, 6); Eq(3.0, p.Y, 6);
    });

    Test("TransformBuilder2D_Reflect_AboutXAxisNormal_FlipsY", () => {
      var builder = new TransformBuilder2D();
      builder.Reflect(new Vector2(0, 1));
      var p = Transform.Apply(new Point2D(3, 4), builder.Get());
      Eq(3.0, p.X, 6); Eq(-4.0, p.Y, 6);
    });

    Test("TransformBuilder2D_Reflect_ZeroLengthNormal_Throws", () => {
      var builder = new TransformBuilder2D();
      bool threw = false;
      try { builder.Reflect(new Vector2(0, 0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length normal Reflect() to throw");
    });

    Test("TransformBuilder2D_Build_ReturnsIndependentSnapshot", () => {
      var builder = new TransformBuilder2D();
      builder.Translate(new Vector2(1, 0));
      var snapshot = builder.Build();
      builder.Translate(new Vector2(0, 1));
      IsTrue(snapshot == Matrix3.Translation(new Vector2(1, 0)));
    });

    Test("TransformBuilder2D_Apply_MatchesTransformApplyWithGet", () => {
      var builder = new TransformBuilder2D();
      builder.Translate(new Vector2(5, 0)).Rotate(Math.PI / 2.0);
      var p = new Point2D(1, 0);
      IsTrue(builder.Apply(p) == Transform.Apply(p, builder.Get()));
    });

    Test("TransformBuilder2D_Apply_WorksOnCompositePrimitive", () => {
      var builder = new TransformBuilder2D();
      builder.Scale(2.0);
      var poly = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      IsTrue(builder.Apply(poly) == Transform.Apply(poly, builder.Get()));
    });

    Test("TransformBuilder2D_Apply_DoesNotConsumeShapeOrBuilder", () => {
      var builder = new TransformBuilder2D();
      builder.Translate(new Vector2(1, 0));
      var p = new Point2D(0, 0);

      var first = builder.Apply(p);
      IsTrue(p == new Point2D(0, 0));
      IsTrue(first == new Point2D(1, 0));

      builder.Translate(new Vector2(0, 1));
      var second = builder.Apply(p);
      IsTrue(second == new Point2D(1, 1));
    });
  }
}

}  // namespace GeomPPTests
