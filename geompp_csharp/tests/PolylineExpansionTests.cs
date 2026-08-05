using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class PolylineExpansionTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── PolylineExpansionParams ────────────────────────────────────────────────────
    Console.WriteLine("\nPolylineExpansionParams");

    Test("PolylineExpansionParams_Defaults", () => {
      var p = new PolylineExpansionParams();
      Eq(0.5, p.Smoothness);
      IsTrue(p.Mode == PolylineExpansionMode.FixedSegments);
      Eq(4, p.SegmentsPerCorner, 0);
      Eq(0.1, p.MinDistance);
    });

    Test("PolylineExpansionParams_ExplicitConstructionAndReadWrite", () => {
      var p = new PolylineExpansionParams(1.0, PolylineExpansionMode.MinDistance, 4, 0.2, 0.05);
      Eq(1.0, p.Smoothness);
      IsTrue(p.Mode == PolylineExpansionMode.MinDistance);
      Eq(0.2, p.MinDistance);
      Eq(0.05, p.MinSegmentLength);
      p.SegmentsPerCorner = 10;
      Eq(10, p.SegmentsPerCorner, 0);
    });

    // ── PolylineExpansion (free function) ──────────────────────────────────────────
    Console.WriteLine("\nPolylineExpansion (free function)");

    Test("PolylineExpansion_TwoPointInput_ReturnsUnchanged", () => {
      var pts = new List<Point2D> { new(0, 0), new(1, 1) };
      var result = new List<Point2D>(GeomUtil.PolylineExpansion(pts, new PolylineExpansionParams()));
      Eq(2, result.Count, 0);
      IsTrue(result[0].AlmostEquals(pts[0]));
      IsTrue(result[1].AlmostEquals(pts[1]));
    });

    Test("PolylineExpansion_MultipleCorners_EachCornerUsesItsOwnOriginalKnots", () => {
      // Regression guard: p0/p1 for each corner must come from the original input, not the already-built
      // output — see the C++/Python test of the same name for the full rationale.
      var pts = new List<Point2D> { new(0, 0), new(2, 0), new(2, 2), new(0, 2) };
      var settings = new PolylineExpansionParams(1.0, PolylineExpansionMode.FixedSegments, 3, 0.1, 1e-6);
      var result = new List<Point2D>(GeomUtil.PolylineExpansion(pts, settings));
      IsTrue(result[0].AlmostEquals(new Point2D(0, 0)));
      IsTrue(result[result.Count - 1].AlmostEquals(new Point2D(0, 2)));
      int midpointCount = 0;
      bool hasCorner2T1 = false;
      foreach (var p in result) {
        if (p.AlmostEquals(new Point2D(2, 1))) midpointCount++;
        if (p.AlmostEquals(new Point2D(1, 2))) hasCorner2T1 = true;
      }
      Eq(1, midpointCount, 0);
      IsTrue(hasCorner2T1);
    });

    // ── Polyline2D.Expand / Polyline3D.Expand ──────────────────────────────────────
    Console.WriteLine("\nPolyline2D.Expand / Polyline3D.Expand");

    Test("Polyline2D_Expand_DefaultParams_Work", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(2, 2) });
      var expanded = pl.Expand();
      IsTrue(expanded.Size() >= pl.Size());
    });

    Test("Polyline2D_Expand_InvalidSegmentsPerCorner_Throws", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(2, 2) });
      bool threw = false;
      try { pl.Expand(new PolylineExpansionParams(0.5, PolylineExpansionMode.FixedSegments, 0, 0.1, 1e-6)); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected throw for segments_per_corner < 1");
    });

    Test("Polyline2D_Expand_MinDistanceMode_SmallerMinDistanceYieldsMorePoints", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(2, 2) });
      var coarse = pl.Expand(new PolylineExpansionParams(1.0, PolylineExpansionMode.MinDistance, 4, 1.0, 1e-6));
      var fine = pl.Expand(new PolylineExpansionParams(1.0, PolylineExpansionMode.MinDistance, 4, 0.5, 1e-6));
      IsTrue(coarse.Size() < fine.Size());
    });

    Test("Polyline3D_Expand_DefaultParams_Work", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0, 0, 0), new(2, 0, 0), new(2, 2, 0) });
      var expanded = pl.Expand();
      IsTrue(expanded.Size() >= pl.Size());
    });

    Test("Polyline3D_Expand_InvalidSegmentsPerCorner_Throws", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0, 0, 0), new(2, 0, 0), new(2, 2, 0) });
      bool threw = false;
      try { pl.Expand(new PolylineExpansionParams(0.5, PolylineExpansionMode.FixedSegments, 0, 0.1, 1e-6)); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected throw for segments_per_corner < 1");
    });
  }
}

}  // namespace GeomPPTests
