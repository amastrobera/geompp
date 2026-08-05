using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Point2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Point2D ───────────────────────────────────────────────────────────────────
    Console.WriteLine("\nPoint2D");

    Test("Create_AccessXY", () => {
      var p = new Point2D(3.0, 4.0);
      Eq(3.0, p.X);
      Eq(4.0, p.Y);
    });

    Test("ToWkt_ContainsCoordinates", () => {
      var wkt = new Point2D(1.0, 2.0).ToWkt();
      IsTrue(wkt.Contains("POINT"), "no POINT keyword");
      IsTrue(wkt.Contains("1"), "no 1");
      IsTrue(wkt.Contains("2"), "no 2");
    });

    Test("AlmostEquals_SamePoint", () => {
      IsTrue(new Point2D(1.0, 2.0).AlmostEquals(new Point2D(1.0, 2.0)));
    });

    Test("AlmostEquals_DifferentPoint", () => {
      IsFalse(new Point2D(0.0, 0.0).AlmostEquals(new Point2D(1.0, 0.0)));
    });

    Test("DistanceTo_KnownValue", () => {
      Eq(5.0, new Point2D(0.0, 0.0).DistanceTo(new Point2D(3.0, 4.0)));
    });

    Test("Zero_IsOrigin", () => {
      var z = Point2D.Zero();
      Eq(0.0, z.X);
      Eq(0.0, z.Y);
    });

    Test("CreateFromVector_CopiesComponents", () => {
      var p = new Point2D(new Vector2D(3.0, -4.5));
      Eq(3.0, p.X);
      Eq(-4.5, p.Y);
    });

    Test("CreateFromVector_RoundtripViaToVector", () => {
      var p0 = new Point2D(1.25, -2.75);
      var p1 = new Point2D(p0.ToVector());
      IsTrue(p0.AlmostEquals(p1));
    });

    Test("FromWkt_Whitespace", () => {
      IsTrue(new Point2D(0, 1).AlmostEquals(Point2D.FromWkt("POINT (  0  1  )")));
    });

    // ── Point2D (additional) ──────────────────────────────────────────────────────
    Console.WriteLine("\nPoint2D (additional)");

    Test("ToVector_MatchesXY", () => {
      var v = new Point2D(3.0, 4.0).ToVector();
      Eq(3.0, v.X);
      Eq(4.0, v.Y);
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var p = new Point2D(1.5, 2.5);
      var path = Path.GetTempFileName();
      p.ToFile(path);
      var q = Point2D.FromFile(path);
      IsTrue(p.AlmostEquals(q));
      File.Delete(path);
    });

    Test("SubtractPoints_ReturnsVector", () => {
      var v = new Point2D(5.0, 3.0) - new Point2D(2.0, 1.0);
      Eq(3.0, v.X);
      Eq(2.0, v.Y);
    });

    Test("SubtractVector_ReturnsPoint", () => {
      var p = new Point2D(5.0, 3.0) - new Vector2D(2.0, 1.0);
      Eq(3.0, p.X);
      Eq(2.0, p.Y);
    });

    Test("ScalarMultiply_Point", () => {
      var p = new Point2D(2.0, 3.0) * 2.0;
      Eq(4.0, p.X);
      Eq(6.0, p.Y);
    });

    Test("ToString_ContainsCoordinates", () => {
      var s = new Point2D(1.0, 2.0).ToString();
      IsTrue(s.Contains("POINT"), "missing POINT keyword");
      IsTrue(s.Contains("1") && s.Contains("2"), "missing coordinates");
    });

    Test("Equality_SamePoint_True", () => {
      IsTrue(new Point2D(1.0, 2.0) == new Point2D(1.0, 2.0));
    });

    Test("Equality_DifferentPoint_False", () => {
      IsFalse(new Point2D(0.0, 0.0) == new Point2D(1.0, 0.0));
    });
  }
}

}  // namespace GeomPPTests
