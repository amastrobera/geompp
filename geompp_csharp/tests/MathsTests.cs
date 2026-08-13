using GeomPP.Maths;

namespace GeomPPTests {

public static class MathsTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 9) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);

    // ── Vector2/3/4 ─────────────────────────────────────────────────────────────
    Console.WriteLine("\nMaths.Vector2/3/4");

    Test("Maths_Vector_Construction_ComponentAccess", () => {
      var v = new Vector3(1, 2, 3);
      Eq(1, v.X, 0); Eq(2, v.Y, 0); Eq(3, v.Z, 0);
      Eq(1, v[0], 0); Eq(3, v[2], 0);
    });

    Test("Maths_Vector_Arithmetic", () => {
      var a = new Vector3(1, 2, 3);
      var b = new Vector3(4, 5, 6);
      IsTrue((a + b) == new Vector3(5, 7, 9));
      IsTrue((b - a) == new Vector3(3, 3, 3));
      IsTrue((a * 2.0) == new Vector3(2, 4, 6));
      IsTrue((2.0 * a) == new Vector3(2, 4, 6));
      IsTrue((-a) == new Vector3(-1, -2, -3));
    });

    Test("Maths_Vector_DotAndCross", () => {
      var x = new Vector3(1, 0, 0);
      var y = new Vector3(0, 1, 0);
      Eq(0, x.Dot(y), 0);
      IsTrue(x.Cross(y) == new Vector3(0, 0, 1));
    });

    Test("Maths_Vector_LengthAndNormalized", () => {
      var v = new Vector2(3, 4);
      Eq(5.0, v.Length());
      var n = v.Normalized();
      Eq(1.0, n.Length());
    });

    Test("Maths_Vector_Normalized_ZeroVector_Throws", () => {
      bool threw = false;
      try { new Vector3(0, 0, 0).Normalized(); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length Normalized() to throw");
    });

    // ── Matrix2/3/4 ─────────────────────────────────────────────────────────────
    Console.WriteLine("\nMaths.Matrix2/3/4");

    Test("Maths_Matrix_Construction_RowMajor", () => {
      var m = new Matrix2(1, 2, 3, 4);
      Eq(1, m.Get(0, 0), 0); Eq(2, m.Get(0, 1), 0);
      Eq(3, m.Get(1, 0), 0); Eq(4, m.Get(1, 1), 0);
    });

    Test("Maths_Matrix_Identity", () => {
      var id = Matrix3.Identity();
      for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
          Eq(r == c ? 1.0 : 0.0, id.Get(r, c), 0);
    });

    Test("Maths_Matrix_MatrixTimesVector", () => {
      var m = new Matrix3(1, 0, 0, 0, 2, 0, 0, 0, 3);
      var v = new Vector3(1, 1, 1);
      IsTrue((m * v) == new Vector3(1, 2, 3));
    });

    Test("Maths_Matrix_MatrixTimesIdentity_ReturnsSame", () => {
      var m = new Matrix3(1, 2, 3, 4, 5, 6, 7, 8, 10);
      IsTrue((m * Matrix3.Identity()) == m);
    });

    Test("Maths_Matrix_Transpose", () => {
      var m = new Matrix2(1, 2, 3, 4);
      var t = m.Transpose();
      Eq(m.Get(1, 0), t.Get(0, 1), 0);
      Eq(m.Get(0, 1), t.Get(1, 0), 0);
    });

    Test("Maths_Matrix_Determinant", () => {
      var m = new Matrix2(1, 2, 3, 4);
      Eq(1 * 4 - 2 * 3, m.Determinant(), 0);
    });

    Test("Maths_Matrix_Inverse_TimesOriginal_IsIdentity", () => {
      var m = new Matrix3(2, 0, 1, 1, 3, 2, 1, 0, 4);
      var product = m * m.Inverse();
      for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
          Eq(r == c ? 1.0 : 0.0, product.Get(r, c), 6);
    });

    Test("Maths_Matrix_Inverse_Singular_Throws", () => {
      var m = new Matrix2(1, 2, 2, 4);
      bool threw = false;
      try { m.Inverse(); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected singular matrix Inverse() to throw");
    });

    Test("Maths_Matrix4_Translation", () => {
      var t = Matrix4.Translation(new Vector3(10, 20, 30));
      var moved = t * new Vector4(1, 2, 3, 1);
      IsTrue(moved == new Vector4(11, 22, 33, 1));
    });

    Test("Maths_Matrix4_Rotation_QuarterTurnAboutZ", () => {
      var r = Matrix4.Rotation(Math.PI / 2.0, new Vector3(0, 0, 1));
      var rotated = r * new Vector4(1, 0, 0, 1);
      Eq(0.0, rotated.X, 6);
      Eq(1.0, rotated.Y, 6);
    });

    Test("Maths_Matrix4_Rotation_ZeroAxis_Throws", () => {
      bool threw = false;
      try { Matrix4.Rotation(1.0, new Vector3(0, 0, 0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length axis Rotation() to throw");
    });

    Test("Maths_Matrix4_UniformAndNonUniformScale", () => {
      var s = Matrix4.Scale(2.0);
      IsTrue((s * new Vector4(1, 2, 3, 1)) == new Vector4(2, 4, 6, 1));
      var s2 = Matrix4.Scale(2.0, 3.0, 4.0);
      IsTrue((s2 * new Vector4(1, 1, 1, 1)) == new Vector4(2, 3, 4, 1));
    });

    Test("Maths_Matrix3_Translation", () => {
      var t = Matrix3.Translation(new Vector2(10, 20));
      IsTrue((t * new Vector3(1, 2, 1)) == new Vector3(11, 22, 1));
    });

    Test("Maths_Matrix3_Rotation_QuarterTurn", () => {
      var r = Matrix3.Rotation(Math.PI / 2.0);
      var rotated = r * new Vector3(1, 0, 1);
      Eq(0.0, rotated.X, 6);
      Eq(1.0, rotated.Y, 6);
    });

    Test("Maths_Matrix3_UniformAndNonUniformScale", () => {
      var s = Matrix3.Scale(2.0);
      IsTrue((s * new Vector3(1, 2, 1)) == new Vector3(2, 4, 1));
      var s2 = Matrix3.Scale(2.0, 3.0);
      IsTrue((s2 * new Vector3(1, 1, 1)) == new Vector3(2, 3, 1));
    });

    Test("Maths_Matrix4_Shear_OffsetsAxisByMultipleOfOther", () => {
      var sh = Matrix4.Shear(2.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      var sheared = sh * new Vector4(1, 3, 5, 1);
      Eq(1.0 + 2.0 * 3.0, sheared.X, 9); Eq(3.0, sheared.Y, 9); Eq(5.0, sheared.Z, 9);
    });

    Test("Maths_Matrix3_Shear_OffsetsAxisByMultipleOfOther", () => {
      var sh = Matrix3.Shear(2.0, 0.0);
      var sheared = sh * new Vector3(1, 3, 1);
      Eq(1.0 + 2.0 * 3.0, sheared.X, 9); Eq(3.0, sheared.Y, 9);
    });

    Test("Maths_Matrix4_Reflection_AboutXAxisNormal_FlipsY", () => {
      var r = Matrix4.Reflection(new Vector3(0, 1, 0));
      var reflected = r * new Vector4(3, 4, 5, 1);
      Eq(3.0, reflected.X, 6); Eq(-4.0, reflected.Y, 6); Eq(5.0, reflected.Z, 6);
    });

    Test("Maths_Matrix4_Reflection_ZeroLengthNormal_Throws", () => {
      bool threw = false;
      try { Matrix4.Reflection(new Vector3(0, 0, 0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length normal Reflection() to throw");
    });

    Test("Maths_Matrix3_Reflection_AboutXAxisNormal_FlipsY", () => {
      var r = Matrix3.Reflection(new Vector2(0, 1));
      var reflected = r * new Vector3(3, 4, 1);
      Eq(3.0, reflected.X, 6); Eq(-4.0, reflected.Y, 6);
    });

    Test("Maths_Matrix3_Reflection_ZeroLengthNormal_Throws", () => {
      bool threw = false;
      try { Matrix3.Reflection(new Vector2(0, 0)); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected zero-length normal Reflection() to throw");
    });

    // ── Solvers ──────────────────────────────────────────────────────────────────
    Console.WriteLine("\nMaths.Solvers");

    Test("Maths_SolveGauss_KnownSystem", () => {
      var a = new Matrix3(1, 1, 1, 0, 2, 5, 2, 5, -1);
      var b = new Vector3(6, -4, 27);
      var x = Solvers.SolveGauss(a, b);
      Eq(5.0, x.X, 6); Eq(3.0, x.Y, 6); Eq(-2.0, x.Z, 6);
    });

    Test("Maths_SolveCramer_MatchesGauss", () => {
      var a = new Matrix3(1, 1, 1, 0, 2, 5, 2, 5, -1);
      var b = new Vector3(6, -4, 27);
      var xg = Solvers.SolveGauss(a, b);
      var xc = Solvers.SolveCramer(a, b);
      Eq(xg.X, xc.X, 6); Eq(xg.Y, xc.Y, 6); Eq(xg.Z, xc.Z, 6);
    });

    Test("Maths_SolveGauss_Singular_Throws", () => {
      var a = new Matrix2(1, 2, 2, 4);
      var b = new Vector2(1, 2);
      bool threw = false;
      try { Solvers.SolveGauss(a, b); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected singular system SolveGauss() to throw");
    });

    Test("Maths_SolveCramer_Singular_Throws", () => {
      var a = new Matrix2(1, 2, 2, 4);
      var b = new Vector2(1, 2);
      bool threw = false;
      try { Solvers.SolveCramer(a, b); } catch (Exception) { threw = true; }
      IsTrue(threw, "expected singular system SolveCramer() to throw");
    });
  }
}

}  // namespace GeomPPTests
