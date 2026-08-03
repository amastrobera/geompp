using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Vector3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Vector3D ──────────────────────────────────────────────────────────────────
    Console.WriteLine("\nVector3D");

    Test("Create_AccessXYZ", () => {
      var v = new Vector3D(1.0, 2.0, 3.0);
      Eq(1.0, v.X);
      Eq(2.0, v.Y);
      Eq(3.0, v.Z);
    });

    Test("Length_UnitVector", () => Eq(1.0, new Vector3D(1, 0, 0).Length()));

    Test("Dot_PerpendicularVectors_IsZero", () => {
      Eq(0.0, new Vector3D(1, 0, 0).Dot(new Vector3D(0, 1, 0)));
    });

    Test("Dot_ParallelVectors_IsOne", () => {
      var v = new Vector3D(1, 0, 0);
      Eq(1.0, v.Dot(v));
    });

    Test("Cross_BasisVectors_IsThird", () => {
      var vz = new Vector3D(1, 0, 0).Cross(new Vector3D(0, 1, 0));
      Eq(0.0, vz.X);
      Eq(0.0, vz.Y);
      Eq(1.0, vz.Z);
    });

    Test("Normalize_ProducesUnitVector", () => {
      Eq(1.0, new Vector3D(3, 4, 0).Normalize().Length());
    });

    Test("BasisVectors_AreUnitLength", () => {
      Eq(1.0, Vector3D.BasisX().Length());
      Eq(1.0, Vector3D.BasisY().Length());
      Eq(1.0, Vector3D.BasisZ().Length());
    });

    Test("FromWkt_Whitespace", () => {
      IsTrue(new Vector3D(0, 1, 2).AlmostEquals(Vector3D.FromWkt("VECTOR (  0  1  2  )")));
    });

    // ── Vector3D (additional) ─────────────────────────────────────────────────────
    Console.WriteLine("\nVector3D (additional)");

    Test("ToPoint_MatchesXYZ", () => {
      var pt = new Vector3D(1.0, 2.0, 3.0).ToPoint();
      Eq(1.0, pt.X);
      Eq(2.0, pt.Y);
      Eq(3.0, pt.Z);
    });

    Test("ToString_ContainsCoordinates", () => {
      var s = new Vector3D(1.0, 2.0, 3.0).ToString();
      IsTrue(s.Contains("VECTOR"), "missing VECTOR keyword");
      IsTrue(s.Contains("1") && s.Contains("2") && s.Contains("3"), "missing coordinates");
    });

    Test("Equality_SameVector_True", () => {
      IsTrue(new Vector3D(1.0, 2.0, 3.0) == new Vector3D(1.0, 2.0, 3.0));
    });

    Test("Equality_DifferentVector_False", () => {
      IsFalse(new Vector3D(1.0, 0.0, 0.0) == new Vector3D(0.0, 1.0, 0.0));
    });
  }
}

}  // namespace GeomPPTests
