using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class CurveSmoothingTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    Console.WriteLine("\nBezierSmoothing2 (free function)");

    Test("BezierSmoothing2_MinDistanceNotPositive_Throws", () => {
      var p0 = new Point2D(0, 0); var p1 = new Point2D(2, 0); var p2 = new Point2D(2, 2);
      bool threw = false;
      try { GeomUtil.BezierSmoothing2(p0, p1, p2, 0.5, 0.0); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected throw for min_distance <= 0");
    });

    Test("BezierSmoothing2_NumSegmentsLessThanOne_Throws", () => {
      var p0 = new Point2D(0, 0); var p1 = new Point2D(2, 0); var p2 = new Point2D(2, 2);
      bool threw = false;
      try { GeomUtil.BezierSmoothing2(p0, p1, p2, 0.5, 0); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected throw for num_segments < 1");
    });

    Test("BezierSmoothing2_EndpointsAreTrimmedTangents", () => {
      // len1 == len2 == 2, smoothness=1.0 -> max_trim=1: T0=(1,0), T1=(2,1)
      var p0 = new Point2D(0, 0); var p1 = new Point2D(2, 0); var p2 = new Point2D(2, 2);
      var result = new List<Point2D>(GeomUtil.BezierSmoothing2(p0, p1, p2, 1.0, 1.0));
      IsTrue(result[0].AlmostEquals(new Point2D(1, 0)));
      IsTrue(result[result.Count - 1].AlmostEquals(new Point2D(2, 1)));
    });

    Test("BezierSmoothing2_NumSegments_ProducesExactPointCount", () => {
      var p0 = new Point2D(0, 0); var p1 = new Point2D(2, 0); var p2 = new Point2D(2, 2);
      var result = new List<Point2D>(GeomUtil.BezierSmoothing2(p0, p1, p2, 1.0, 3));
      Eq(4, result.Count, 0);
    });

    Test("BezierSmoothing2_SmoothnessZero_CollapsesToCorner", () => {
      var p0 = new Point2D(0, 0); var p1 = new Point2D(2, 0); var p2 = new Point2D(2, 2);
      var result = new List<Point2D>(GeomUtil.BezierSmoothing2(p0, p1, p2, 0.0, 0.5));
      foreach (var p in result) IsTrue(p.AlmostEquals(p1));
    });

    Test("BezierSmoothing2_CoincidentP0P1_NoNaNCollapsesToCorner", () => {
      var p1 = new Point2D(1, 1); var p2 = new Point2D(3, 1);
      var result = new List<Point2D>(GeomUtil.BezierSmoothing2(p1, p1, p2, 0.5, 0.1));
      foreach (var p in result) {
        IsFalse(double.IsNaN(p.X));
        IsFalse(double.IsNaN(p.Y));
        IsTrue(p.AlmostEquals(p1));
      }
    });

    Test("BezierSmoothing2_Point3D_EndpointsAreTrimmedTangents", () => {
      var p0 = new Point3D(0, 0, 0); var p1 = new Point3D(2, 0, 0); var p2 = new Point3D(2, 2, 0);
      var result = new List<Point3D>(GeomUtil.BezierSmoothing2(p0, p1, p2, 1.0, 1.0));
      IsTrue(result[0].AlmostEquals(new Point3D(1, 0, 0)));
      IsTrue(result[result.Count - 1].AlmostEquals(new Point3D(2, 1, 0)));
    });

    Test("BezierSmoothing2_MinSegmentLength_SkipsWholeCornerWhenBothSidesShort", () => {
      var p0 = new Point2D(0, 0); var p1 = new Point2D(0.5, 0); var p2 = new Point2D(0.5, 0.5);
      var result = new List<Point2D>(GeomUtil.BezierSmoothing2(p0, p1, p2, 1.0, 4, 1.0));
      foreach (var p in result) IsTrue(p.AlmostEquals(p1));
    });
  }
}

}  // namespace GeomPPTests
