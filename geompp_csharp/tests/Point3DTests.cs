using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Point3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Point3D ───────────────────────────────────────────────────────────────────
    Console.WriteLine("\nPoint3D");

    Test("Create_AccessXYZ", () => {
      var p = new Point3D(1.0, 2.0, 3.0);
      Eq(1.0, p.X);
      Eq(2.0, p.Y);
      Eq(3.0, p.Z);
    });

    Test("DistanceTo_KnownValue", () => {
      Eq(1.0, new Point3D(0, 0, 0).DistanceTo(new Point3D(1, 0, 0)));
    });

    Test("CreateFromVector_CopiesComponents", () => {
      var p = new Point3D(new Vector3D(3.0, -4.5, 6.25));
      Eq(3.0, p.X);
      Eq(-4.5, p.Y);
      Eq(6.25, p.Z);
    });

    Test("CreateFromVector_RoundtripViaToVector", () => {
      var p0 = new Point3D(1.25, -2.75, 0.5);
      var p1 = new Point3D(p0.ToVector());
      IsTrue(p0.AlmostEquals(p1));
    });

    Test("FromWkt_Whitespace", () => {
      IsTrue(new Point3D(0, 1, 2).AlmostEquals(Point3D.FromWkt("POINT (  0  1  2  )")));
    });

    // ── Point3D (additional) ──────────────────────────────────────────────────────
    Console.WriteLine("\nPoint3D (additional)");

    Test("Zero_IsOrigin", () => {
      var z = Point3D.Zero();
      Eq(0.0, z.X);
      Eq(0.0, z.Y);
      Eq(0.0, z.Z);
    });

    Test("ToVector_MatchesXYZ", () => {
      var v = new Point3D(1.0, 2.0, 3.0).ToVector();
      Eq(1.0, v.X);
      Eq(2.0, v.Y);
      Eq(3.0, v.Z);
    });

    Test("AlmostEquals_SamePoint_True", () => {
      IsTrue(new Point3D(1.0, 2.0, 3.0).AlmostEquals(new Point3D(1.0, 2.0, 3.0)));
    });

    Test("AlmostEquals_DifferentPoint_False", () => {
      IsFalse(new Point3D(0.0, 0.0, 0.0).AlmostEquals(new Point3D(1.0, 0.0, 0.0)));
    });

    Test("SubtractPoints_ReturnsVector", () => {
      var v = new Point3D(5.0, 3.0, 1.0) - new Point3D(2.0, 1.0, 0.0);
      Eq(3.0, v.X);
      Eq(2.0, v.Y);
      Eq(1.0, v.Z);
    });

    Test("SubtractVector_ReturnsPoint", () => {
      var p = new Point3D(5.0, 3.0, 1.0) - new Vector3D(2.0, 1.0, 1.0);
      Eq(3.0, p.X);
      Eq(2.0, p.Y);
      Eq(0.0, p.Z);
    });

    Test("ScalarMultiply_Point", () => {
      var p = new Point3D(1.0, 2.0, 3.0) * 3.0;
      Eq(3.0, p.X);
      Eq(6.0, p.Y);
      Eq(9.0, p.Z);
    });

    Test("WktRoundTrip", () => {
      var p = new Point3D(1.5, 2.5, 3.5);
      IsTrue(p.AlmostEquals(Point3D.FromWkt(p.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var p = new Point3D(7.0, 8.0, 9.0);
      var path = Path.GetTempFileName();
      p.ToFile(path);
      IsTrue(p.AlmostEquals(Point3D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsCoordinates", () => {
      var s = new Point3D(1.0, 2.0, 3.0).ToString();
      IsTrue(s.Contains("POINT"), "missing POINT keyword");
      IsTrue(s.Contains("1") && s.Contains("2") && s.Contains("3"), "missing coordinates");
    });

    Test("Equality_SamePoint_True", () => {
      IsTrue(new Point3D(1.0, 2.0, 3.0) == new Point3D(1.0, 2.0, 3.0));
    });

    Test("Equality_DifferentPoint_False", () => {
      IsFalse(new Point3D(0.0, 0.0, 0.0) == new Point3D(1.0, 0.0, 0.0));
    });
  }
}

}  // namespace GeomPPTests
