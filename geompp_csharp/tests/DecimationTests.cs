using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class DecimationTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    Console.WriteLine("\nPolyline decimation (free functions)");

    Test("DistDecimation_RemovesClusteredPoints", () => {
      var pts = new List<Point2D> {
        new(0, 0), new(0.1, 0), new(0.2, 0), new(5, 0), new(5.1, 0), new(10, 0) };
      var result = new List<Point2D>(GeomUtil.DistDecimation(pts, 1.0));
      Eq(3, result.Count, 0);
      IsTrue(result[0].AlmostEquals(new Point2D(0, 0)));
      IsTrue(result[1].AlmostEquals(new Point2D(5, 0)));
      IsTrue(result[2].AlmostEquals(new Point2D(10, 0)));
    });

    Test("DistDecimation_TooFewPoints_ReturnsUnchanged", () => {
      var pts = new List<Point3D> { new(0, 0, 0), new(1, 1, 1) };
      var result = new List<Point3D>(GeomUtil.DistDecimation(pts, 5.0));
      Eq(2, result.Count, 0);
    });

    Test("RdpDecimation_CollinearPoints_CollapseToEndpoints", () => {
      var pts = new List<Point2D> { new(0, 0), new(1, 0), new(2, 0), new(3, 0), new(4, 0) };
      var result = new List<Point2D>(GeomUtil.RdpDecimation(pts, 0.5));
      Eq(2, result.Count, 0);
      IsTrue(result[0].AlmostEquals(new Point2D(0, 0)));
      IsTrue(result[1].AlmostEquals(new Point2D(4, 0)));
    });

    Test("RdpDecimation_KeepsPeakDiscardsShoulders", () => {
      var pts = new List<Point3D> { new(0, 0, 0), new(2, 0, 0), new(4, 0, 5), new(6, 0, 0), new(8, 0, 0) };
      var result = new List<Point3D>(GeomUtil.RdpDecimation(pts, 2.0));
      Eq(3, result.Count, 0);
      IsTrue(result[0].AlmostEquals(new Point3D(0, 0, 0)));
      IsTrue(result[1].AlmostEquals(new Point3D(4, 0, 5)));
      IsTrue(result[2].AlmostEquals(new Point3D(8, 0, 0)));
    });

    Test("VwDecimation_KeepsHighAreaVertex", () => {
      var pts = new List<Point2D> { new(0, 0), new(2, 0), new(4, 5), new(6, 0), new(8, 0) };
      var result = new List<Point2D>(GeomUtil.VwDecimation(pts, 6.0));
      Eq(3, result.Count, 0);
      IsTrue(result[0].AlmostEquals(new Point2D(0, 0)));
      IsTrue(result[1].AlmostEquals(new Point2D(4, 5)));
      IsTrue(result[2].AlmostEquals(new Point2D(8, 0)));
    });

    Test("VwDecimation_ThresholdBelowAllAreas_KeepsAllPoints", () => {
      var pts = new List<Point3D> { new(0, 0, 0), new(2, 0, 0), new(4, 0, 5), new(6, 0, 0), new(8, 0, 0) };
      var result = new List<Point3D>(GeomUtil.VwDecimation(pts, 1.0));
      Eq(pts.Count, result.Count, 0);
    });

    // ── BezierSmoothing2 (GeomUtil.BezierSmoothing2: MinDistance / NumSegments overloads) ─

    // ── PolylineDecimationParams ─────────────────────────────────────────────────
    Console.WriteLine("\nPolylineDecimationParams");

    Test("PolylineDecimationParams_Defaults", () => {
      var p = new PolylineDecimationParams();
      IsTrue(p.Strategy == PolylineDecimationStrategy.RamerDouglasPeucker);
      Eq(0.5, p.Threshold);
    });

    Test("PolylineDecimationParams_ExplicitConstructionAndReadWrite", () => {
      var p = new PolylineDecimationParams(PolylineDecimationStrategy.VisvalingamWhyatt, 2.0);
      IsTrue(p.Strategy == PolylineDecimationStrategy.VisvalingamWhyatt);
      Eq(2.0, p.Threshold);
      p.Threshold = 3.5;
      Eq(3.5, p.Threshold);
    });
  }
}

}  // namespace GeomPPTests
