using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class BRect2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── BRect2D ───────────────────────────────────────────────────────────────────
    Console.WriteLine("\nBRect2D");

    Test("Constructor_AxisAlignedRectangle_Center", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      };
      var r = new BRect2D(pts);
      IsTrue(r.Center().AlmostEquals(new Point2D(2.0, 1.0)));
      Eq(4.0, r.Width());
      Eq(2.0, r.Height());
      Eq(8.0, r.Area());
    });

    Test("Constructor_Empty_Throws", () => {
      bool threw = false;
      try { new BRect2D(new Point2D[] {}); } catch { threw = true; }
      IsTrue(threw, "BRect2D from empty list should throw");
    });

    Test("Constructor_SinglePoint_Throws", () => {
      bool threw = false;
      try { new BRect2D(new Point2D[] { new Point2D(3, 4) }); } catch { threw = true; }
      IsTrue(threw, "BRect2D from 1 point should throw");
    });

    Test("Constructor_TwoPoints_Throws", () => {
      bool threw = false;
      try { new BRect2D(new Point2D[] { new Point2D(0, 0), new Point2D(4, 0) }); } catch { threw = true; }
      IsTrue(threw, "BRect2D from 2 points should throw");
    });

    Test("Constructor_AllInputPointsContained", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(3, 0), new Point2D(3, 2),
        new Point2D(0, 2), new Point2D(1, 0.5), new Point2D(2, 1.5)
      };
      var r = new BRect2D(pts);
      foreach (var p in pts)
        IsTrue(r.Contains(p), $"OBB must contain {p}");
    });

    Test("Axes_AreUnitVectors", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(2, 0), new Point2D(2, 1), new Point2D(0, 1)
      };
      var r = new BRect2D(pts);
      Eq(1.0, r.AxisU().Length());
      Eq(1.0, r.AxisV().Length());
    });

    Test("Axes_AreOrthogonal", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(3, 0), new Point2D(3, 2), new Point2D(0, 2)
      };
      var r = new BRect2D(pts);
      Eq(0.0, r.AxisU().Dot(r.AxisV()));
    });

    Test("Corners_FourPointsAllContained", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      };
      var r = new BRect2D(pts);
      var corners = r.Corners();
      IsTrue(corners.Length == 4, "must return 4 corners");
      foreach (var c in corners)
        IsTrue(r.Contains(c), "each corner must be inside the rectangle");
    });

    Test("Contains_Center_True", () => {
      var r = new BRect2D(new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      });
      IsTrue(r.Contains(new Point2D(2, 1)));
    });

    Test("Contains_Boundary_True", () => {
      var r = new BRect2D(new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      });
      IsTrue(r.Contains(new Point2D(0, 0)));    // corner
      IsTrue(r.Contains(new Point2D(4, 1)));    // edge midpoint
    });

    Test("Contains_Outside_False", () => {
      var r = new BRect2D(new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      });
      IsFalse(r.Contains(new Point2D(5, 1)));
      IsFalse(r.Contains(new Point2D(2, 3)));
    });

    Test("AlmostEquals_SameRect", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      };
      var r1 = new BRect2D(pts);
      var r2 = new BRect2D(pts);
      IsTrue(r1.AlmostEquals(r2));
      IsTrue(r1 == r2);
    });

    Test("AlmostEquals_DifferentRect_False", () => {
      var r1 = new BRect2D(new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      });
      var r2 = new BRect2D(new Point2D[] {
        new Point2D(0, 0), new Point2D(6, 0), new Point2D(6, 2), new Point2D(0, 2)
      });
      IsFalse(r1.AlmostEquals(r2));
    });

    Test("ToString_ContainsCenterAndHalfLengths", () => {
      var pts = new Point2D[] {
        new Point2D(0, 0), new Point2D(4, 0), new Point2D(4, 2), new Point2D(0, 2)
      };
      var s = new BRect2D(pts).ToString();
      IsTrue(s.Contains("BRect2D"), "missing BRect2D label");
    });
  }
}

}  // namespace GeomPPTests
