using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class BBallTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── BBall2D ───────────────────────────────────────────────────────────────────
    Console.WriteLine("\nBBall2D");

    Test("Constructor_CenterRadius", () => {
      var b = new BBall2D(new Point2D(1.0, 2.0), 5.0);
      IsTrue(b.Center().AlmostEquals(new Point2D(1.0, 2.0)));
      Eq(5.0, b.Radius());
    });

    Test("Constructor_FromSinglePoint_RadiusZero", () => {
      var b = new BBall2D(new Point2D[] { new Point2D(3.0, 4.0) });
      IsTrue(b.Center().AlmostEquals(new Point2D(3.0, 4.0)));
      Eq(0.0, b.Radius());
    });

    Test("Constructor_FromTwoPoints_MidpointCenter", () => {
      var b = new BBall2D(new Point2D[] { new Point2D(0, 0), new Point2D(4, 0) });
      IsTrue(b.Center().AlmostEquals(new Point2D(2.0, 0.0)));
      Eq(2.0, b.Radius());
    });

    Test("Constructor_FromPoints_AllContained", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(2, 3), new Point2D(-1, 1.5)
      };
      var b = new BBall2D(pts);
      foreach (var p in pts)
        IsTrue(b.Contains(p), $"ball should contain {p}");
    });

    Test("Contains_Center_True", () => {
      var b = new BBall2D(new Point2D(0, 0), 5.0);
      IsTrue(b.Contains(new Point2D(0, 0)));
    });

    Test("Contains_Boundary_True", () => {
      var b = new BBall2D(new Point2D(0, 0), 5.0);
      IsTrue(b.Contains(new Point2D(3, 4)));   // 3-4-5
      IsTrue(b.Contains(new Point2D(5, 0)));
    });

    Test("Contains_Outside_False", () => {
      var b = new BBall2D(new Point2D(0, 0), 5.0);
      IsFalse(b.Contains(new Point2D(4, 4)));  // dist ≈ 5.657
      IsFalse(b.Contains(new Point2D(6, 0)));
    });

    Test("AlmostEquals_SameBall", () => {
      var b1 = new BBall2D(new Point2D(1, 2), 3.0);
      var b2 = new BBall2D(new Point2D(1, 2), 3.0);
      IsTrue(b1.AlmostEquals(b2));
      IsTrue(b1 == b2);
    });

    Test("AlmostEquals_DifferentBall", () => {
      var b1 = new BBall2D(new Point2D(1, 2), 3.0);
      var b2 = new BBall2D(new Point2D(0, 0), 1.0);
      IsFalse(b1.AlmostEquals(b2));
    });

    Test("ToString_ContainsCenterAndRadius", () => {
      var s = new BBall2D(new Point2D(1, 2), 3.0).ToString();
      IsTrue(s.Contains("BBall2D"), "missing BBall2D label");
      IsTrue(s.Contains("radius"), "missing radius");
    });

    // ── BBall3D ───────────────────────────────────────────────────────────────────
    Console.WriteLine("\nBBall3D");

    Test("Constructor_CenterRadius", () => {
      var b = new BBall3D(new Point3D(1, 2, 3), 5.0);
      IsTrue(b.Center().AlmostEquals(new Point3D(1, 2, 3)));
      Eq(5.0, b.Radius());
    });

    Test("Constructor_FromSinglePoint_RadiusZero", () => {
      var b = new BBall3D(new Point3D[] { new Point3D(1, 2, 3) });
      IsTrue(b.Center().AlmostEquals(new Point3D(1, 2, 3)));
      Eq(0.0, b.Radius());
    });

    Test("Constructor_FromTwoPoints_MidpointCenter", () => {
      var b = new BBall3D(new Point3D[] { new Point3D(0, 0, 0), new Point3D(4, 0, 0) });
      IsTrue(b.Center().AlmostEquals(new Point3D(2, 0, 0)));
      Eq(2.0, b.Radius());
    });

    Test("Constructor_FromPoints_AllContained", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(0, 3, 0), new Point3D(0, 0, 2)
      };
      var b = new BBall3D(pts);
      foreach (var p in pts)
        IsTrue(b.Contains(p), $"ball should contain {p}");
    });

    Test("Contains_Center_True", () => {
      var b = new BBall3D(new Point3D(0, 0, 0), 5.0);
      IsTrue(b.Contains(new Point3D(0, 0, 0)));
    });

    Test("Contains_Boundary_True", () => {
      var b = new BBall3D(new Point3D(0, 0, 0), 5.0);
      IsTrue(b.Contains(new Point3D(3, 4, 0)));   // 3-4-5 in XY
      IsTrue(b.Contains(new Point3D(0, 0, 5)));
    });

    Test("Contains_Outside_False", () => {
      var b = new BBall3D(new Point3D(0, 0, 0), 5.0);
      IsFalse(b.Contains(new Point3D(4, 4, 0)));  // dist ≈ 5.657
      IsFalse(b.Contains(new Point3D(0, 0, 6)));
    });

    Test("AlmostEquals_SameBall", () => {
      var b1 = new BBall3D(new Point3D(1, 2, 3), 4.0);
      var b2 = new BBall3D(new Point3D(1, 2, 3), 4.0);
      IsTrue(b1.AlmostEquals(b2));
      IsTrue(b1 == b2);
    });

    Test("AlmostEquals_DifferentBall", () => {
      var b1 = new BBall3D(new Point3D(1, 2, 3), 4.0);
      var b2 = new BBall3D(new Point3D(0, 0, 0), 1.0);
      IsFalse(b1.AlmostEquals(b2));
    });

    Test("ToString_ContainsCenterAndRadius", () => {
      var s = new BBall3D(new Point3D(1, 2, 3), 4.0).ToString();
      IsTrue(s.Contains("BBall3D"), "missing BBall3D label");
      IsTrue(s.Contains("radius"), "missing radius");
    });
  }
}

}  // namespace GeomPPTests
