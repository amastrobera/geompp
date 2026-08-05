using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Vector2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Vector2D ──────────────────────────────────────────────────────────────────
    Console.WriteLine("\nVector2D");

    Test("Create_AccessXY", () => {
      var v = new Vector2D(3.0, 4.0);
      Eq(3.0, v.X);
      Eq(4.0, v.Y);
    });

    Test("Length_KnownValue", () => Eq(5.0, new Vector2D(3.0, 4.0).Length()));

    Test("AlmostEquals_SameVector", () => IsTrue(new Vector2D(1.0, 2.0).AlmostEquals(new Vector2D(1.0, 2.0))));

    Test("AlmostEquals_DifferentVector", () => IsFalse(new Vector2D(1.0, 0.0).AlmostEquals(new Vector2D(0.0, 1.0))));

    Test("BasisX_IsUnitX", () => {
      var v = Vector2D.BasisX();
      Eq(1.0, v.X);
      Eq(0.0, v.Y);
      Eq(1.0, v.Length());
    });

    Test("BasisY_IsUnitY", () => {
      var v = Vector2D.BasisY();
      Eq(0.0, v.X);
      Eq(1.0, v.Y);
      Eq(1.0, v.Length());
    });

    Test("Dot_PerpendicularVectors_IsZero", () => Eq(0.0, Vector2D.BasisX().Dot(Vector2D.BasisY())));

    Test("Dot_SameVector_IsOne", () => Eq(1.0, Vector2D.BasisX().Dot(Vector2D.BasisX())));

    Test("Cross_CCW_IsPositive", () => Eq(1.0, Vector2D.BasisX().Cross(Vector2D.BasisY())));

    Test("Cross_CW_IsNegative", () => Eq(-1.0, Vector2D.BasisY().Cross(Vector2D.BasisX())));

    Test("Perp_OfBasisX_IsBasisY", () => {
      var p = Vector2D.BasisX().Perp();
      Eq(0.0, p.X);
      Eq(1.0, p.Y);
    });

    Test("Normalize_UnitLength", () => Eq(1.0, new Vector2D(3.0, 4.0).Normalize().Length()));

    Test("ToPoint_MatchesComponents", () => {
      var pt = new Vector2D(2.0, 3.0).ToPoint();
      Eq(2.0, pt.X);
      Eq(3.0, pt.Y);
    });

    Test("Add_TwoVectors", () => {
      var r = Vector2D.BasisX() + Vector2D.BasisY();
      Eq(1.0, r.X);
      Eq(1.0, r.Y);
    });

    Test("Subtract_TwoVectors", () => {
      var r = Vector2D.BasisX() - Vector2D.BasisY();
      Eq(1.0, r.X);
      Eq(-1.0, r.Y);
    });

    Test("ScalarMultiply", () => {
      var r = Vector2D.BasisX() * 3.0;
      Eq(3.0, r.X);
      Eq(0.0, r.Y);
    });

    Test("ScalarDivide", () => {
      var r = new Vector2D(4.0, 0.0) / 2.0;
      Eq(2.0, r.X);
      Eq(0.0, r.Y);
    });

    Test("UnaryNegate", () => {
      var r = -Vector2D.BasisX();
      Eq(-1.0, r.X);
      Eq(0.0, r.Y);
    });

    Test("Equality_SameVectors", () => IsTrue(Vector2D.BasisX() == Vector2D.BasisX()));

    Test("Equality_DifferentVectors", () => IsFalse(Vector2D.BasisX() == Vector2D.BasisY()));

    Test("DotOperator_MatchesMethod", () => {
      var u = new Vector2D(1.0, 2.0);
      var v = new Vector2D(3.0, 4.0);
      Eq(u.Dot(v), u * v);
    });

    Test("VectorPlusPoint", () => {
      var pt = Vector2D.BasisX() + new Point2D(1.0, 1.0);
      Eq(2.0, pt.X);
      Eq(1.0, pt.Y);
    });

    Test("WktRoundTrip", () => {
      var v = new Vector2D(1.5, 2.5);
      IsTrue(v.AlmostEquals(Vector2D.FromWkt(v.ToWkt())));
    });

    Test("FromWkt_Whitespace", () => {
      IsTrue(new Vector2D(0, 1).AlmostEquals(Vector2D.FromWkt("VECTOR (  0  1  )")));
    });

    Test("IsParallel_SameDirection",  () => IsTrue(new Vector2D(1, 0).IsParallel(new Vector2D(2, 0))));
    Test("IsParallel_AntiParallel",   () => IsTrue(new Vector2D(1, 0).IsParallel(new Vector2D(-3, 0))));
    Test("IsParallel_ScaledSameDir",  () => IsTrue(new Vector2D(1, 1).IsParallel(new Vector2D(2, 2))));
    Test("IsParallel_Perpendicular",  () => IsFalse(new Vector2D(1, 0).IsParallel(new Vector2D(0, 1))));
    Test("IsParallel_NonParallel",    () => IsFalse(new Vector2D(1, 0).IsParallel(new Vector2D(1, 1))));

    Test("ToString_ContainsCoordinates", () => {
      var s = new Vector2D(1.0, 2.0).ToString();
      IsTrue(s.Contains("VECTOR"), "missing VECTOR keyword");
      IsTrue(s.Contains("1") && s.Contains("2"), "missing coordinates");
    });
  }
}

}  // namespace GeomPPTests
