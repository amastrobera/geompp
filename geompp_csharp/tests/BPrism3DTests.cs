using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class BPrism3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── BPrism3D ──────────────────────────────────────────────────────────────────
    Console.WriteLine("\nBPrism3D");

    Test("Constructor_AxisAlignedBox", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var p = new BPrism3D(pts);
      IsTrue(Math.Abs(p.Volume() - 24.0) < 0.5, "volume should be ~24");
      foreach (var pt in pts)
        IsTrue(p.Contains(pt), $"prism must contain {pt}");
    });

    Test("Constructor_Empty_Throws", () => {
      bool threw = false;
      try { new BPrism3D(new Point3D[] {}); } catch { threw = true; }
      IsTrue(threw, "BPrism3D from empty list should throw");
    });

    Test("Constructor_SinglePoint_Throws", () => {
      bool threw = false;
      try { new BPrism3D(new Point3D[] { new Point3D(3, 4, 5) }); } catch { threw = true; }
      IsTrue(threw, "BPrism3D from 1 point should throw");
    });

    Test("Constructor_TwoPoints_Throws", () => {
      bool threw = false;
      try { new BPrism3D(new Point3D[] { new Point3D(0, 0, 0), new Point3D(4, 0, 0) }); } catch { threw = true; }
      IsTrue(threw, "BPrism3D from 2 points should throw");
    });

    Test("Constructor_FlatCloud_WIsEpsilon", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
      };
      var p = new BPrism3D(pts);
      IsTrue(p.HalfLenW() > 0, "flat cloud w half-length must be > 0");
    });

    Test("Constructor_NonConvex_AllPointsContained", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(3, 0, 0),
        new Point3D(3, 2, 0), new Point3D(0, 2, 0),
        new Point3D(1, 0.5, 1), new Point3D(2, 1.5, 0.5),
      };
      var p = new BPrism3D(pts);
      foreach (var pt in pts)
        IsTrue(p.Contains(pt), $"prism must contain {pt}");
    });

    Test("Axes_AreUnitVectors", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var p = new BPrism3D(pts);
      Eq(1.0, p.AxisU().Length());
      Eq(1.0, p.AxisV().Length());
      Eq(1.0, p.AxisW().Length());
    });

    Test("Axes_AreOrthogonal", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var p = new BPrism3D(pts);
      Eq(0.0, p.AxisU().Dot(p.AxisV()));
      Eq(0.0, p.AxisU().Dot(p.AxisW()));
      Eq(0.0, p.AxisV().Dot(p.AxisW()));
    });

    Test("Corners_EightPointsAllContained", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var p = new BPrism3D(pts);
      var corners = p.Corners();
      IsTrue(corners.Length == 8, "must return 8 corners");
      foreach (var c in corners)
        IsTrue(p.Contains(c), "each corner must be inside the prism");
    });

    Test("Contains_Center_True", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var p = new BPrism3D(pts);
      IsTrue(p.Contains(p.Center()));
    });

    Test("Contains_Outside_False", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var p = new BPrism3D(pts);
      IsFalse(p.Contains(new Point3D(10, 10, 10)));
    });

    Test("AlmostEquals_SamePrism", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var p1 = new BPrism3D(pts);
      var p2 = new BPrism3D(pts);
      IsTrue(p1.AlmostEquals(p2));
      IsTrue(p1 == p2);
    });

    Test("AlmostEquals_DifferentPrism_False", () => {
      var pts1 = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
      };
      var pts2 = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(6, 0, 0),
        new Point3D(6, 3, 0), new Point3D(0, 3, 0),
      };
      var p1 = new BPrism3D(pts1);
      var p2 = new BPrism3D(pts2);
      IsFalse(p1.AlmostEquals(p2));
    });

    Test("ToString_ContainsCenterAndHalfLengths", () => {
      var pts = new Point3D[] {
        new Point3D(0, 0, 0), new Point3D(4, 0, 0),
        new Point3D(4, 3, 0), new Point3D(0, 3, 0),
        new Point3D(0, 0, 2), new Point3D(4, 0, 2),
        new Point3D(4, 3, 2), new Point3D(0, 3, 2),
      };
      var s = new BPrism3D(pts).ToString();
      IsTrue(s.Contains("BPrism3D"), "missing BPrism3D label");
    });
  }
}

}  // namespace GeomPPTests
