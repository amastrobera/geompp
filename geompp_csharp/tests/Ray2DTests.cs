using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Ray2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Ray2D ─────────────────────────────────────────────────────────────────────
    Console.WriteLine("\nRay2D");

    Test("Contains_PointOnRay", () => {
      IsTrue(Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0)).Contains(new Point2D(3, 0)));
    });

    Test("Contains_PointBehindOrigin_False", () => {
      IsFalse(Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0)).Contains(new Point2D(-1, 0)));
    });

    Test("DistanceTo_PointAboveRay", () => {
      Eq(4.0, Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0)).DistanceTo(new Point2D(3, 4)));
    });

    Test("DistanceTo_PointBehindOrigin", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      Eq(new Point2D(0, 0).DistanceTo(new Point2D(-5, 10)), r.DistanceTo(new Point2D(-5, 10)));
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsTrue(expected.AlmostEquals(Ray2D.FromWkt("RAY (0 0,1 0)")));
      IsTrue(expected.AlmostEquals(Ray2D.FromWkt("RAY (  0 0  ,  1  0  )")));
    });

    // ── Ray2D (additional) ────────────────────────────────────────────────────────
    Console.WriteLine("\nRay2D (additional)");

    Test("IsAhead_PointInFront_True", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsTrue(r.IsAhead(new Point2D(5, 0)));
    });

    Test("IsAhead_PointBehind_False", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsFalse(r.IsAhead(new Point2D(-1, 0)));
    });

    Test("IsBehind_PointBehind_True", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsTrue(r.IsBehind(new Point2D(-1, 0)));
    });

    Test("IsBehind_PointInFront_False", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsFalse(r.IsBehind(new Point2D(5, 0)));
    });

    Test("Intersects_Line2D_True", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var l = Line2D.Make(new Point2D(2, -2), new Point2D(2, 2));
      IsTrue(r.Intersects(l));
    });

    Test("Intersects_Ray2D_True", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
      IsTrue(r1.Intersects(r2));
    });

    Test("Intersects_Ray2D_False", () => {
      // Both rays point away from each other
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(-1, 0));
      var r2 = Ray2D.Make(new Point2D(4, 0), new Vector2D(1, 0));
      IsFalse(r1.Intersects(r2));
    });

    Test("Intersects_Seg2D_True", () => {
      var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      IsTrue(r.Intersects(seg));
    });

    Test("Intersection_Line2D_Hit", () => {
      var r  = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var l  = Line2D.Make(new Point2D(2, -2), new Point2D(2, 2));
      var pt = r.Intersection(l);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Intersection_Ray2D_Hit", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
      var pt = r1.Intersection(r2);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Intersection_Ray2D_Miss", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(-1, 0));
      var r2 = Ray2D.Make(new Point2D(4, 0), new Vector2D(1, 0));
      IsNull(r1.Intersection(r2));
    });

    Test("Intersection_Seg2D_Hit", () => {
      var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      var pt  = r.Intersection(seg);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("ToLine_ReturnsLine2D", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      NotNull(r.ToLine());
      IsTrue(r.ToLine() is Line2D);
    });

    Test("AlmostEquals_SameRay_True", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsTrue(r1.AlmostEquals(r2));
    });

    Test("WktRoundTrip", () => {
      var r = Ray2D.Make(new Point2D(1, 2), new Vector2D(1, 0));
      IsTrue(r.AlmostEquals(Ray2D.FromWkt(r.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var r    = Ray2D.Make(new Point2D(1, 2), new Vector2D(1, 0));
      var path = Path.GetTempFileName();
      r.ToFile(path);
      IsTrue(r.AlmostEquals(Ray2D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0)).ToString();
      IsTrue(s.Contains("RAY"), "missing RAY keyword");
    });

    Test("Equality_SameRay_True", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsTrue(r1 == r2);
    });

    Test("Equality_DifferentRay_False", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(0, 0), new Vector2D(0, 1));
      IsFalse(r1 == r2);
    });

    // ── Overlap — Ray2D ───────────────────────────────────────────────────────────
    Console.WriteLine("\nRay2D Overlap");

    Test("Overlaps_CollinearLine2D_True", () => {
      var r = Ray2D.Make(new Point2D(2, 0), new Vector2D(1, 0));
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      IsTrue(r.Overlaps(x));
      IsTrue(r.Overlap(x) is Ray2D);
    });

    Test("Overlaps_SameDirRay2D_ReturnsRay", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(2, 0), new Vector2D(1, 0));
      IsTrue(r1.Overlaps(r2));
      IsTrue(r1.Overlap(r2) is Ray2D);
    });

    Test("Overlaps_AntiParallelRay2D_ReturnsSegment", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r3 = Ray2D.Make(new Point2D(5, 0), new Vector2D(-1, 0));
      IsTrue(r1.Overlaps(r3));
      IsTrue(r1.Overlap(r3) is LineSegment2D);
    });

    Test("Overlaps_TouchingAntiParallelRay2D_False", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r4 = Ray2D.Make(new Point2D(0, 0), new Vector2D(-1, 0));
      IsFalse(r1.Overlaps(r4));
      IsNull(r1.Overlap(r4));
    });

    Test("Overlaps_SegmentInsideRay2D_True", () => {
      var r = Ray2D.Make(new Point2D(1, 0), new Vector2D(1, 0));
      var s = LineSegment2D.Make(new Point2D(2, 0), new Point2D(4, 0));
      IsTrue(r.Overlaps(s));
      IsTrue(r.Overlap(s)!.AlmostEquals(s));
    });

    Test("Overlaps_SegmentBeforeRay2D_False", () => {
      var r = Ray2D.Make(new Point2D(1, 0), new Vector2D(1, 0));
      var s = LineSegment2D.Make(new Point2D(-3, 0), new Point2D(-1, 0));
      IsFalse(r.Overlaps(s));
      IsNull(r.Overlap(s));
    });

    // ── Touch — Ray2D ─────────────────────────────────────────────────────────────
    Console.WriteLine("\nRay2D Touch");
    Test("Touches_LineAtOrigin_True", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var r = Ray2D.Make(new Point2D(2, 0), new Vector2D(0, 1));
      IsTrue(r.Touches(x));
      NotNull(r.Touch(x));
      IsTrue(r.Touch(x).AlmostEquals(new Point2D(2, 0)));
    });
    Test("Touches_CollinearLine_False", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var r = Ray2D.Make(new Point2D(1, 0), new Vector2D(1, 0));
      IsFalse(r.Touches(x));
      IsNull(r.Touch(x));
    });
    Test("Touches_RayRaySameOriginDifferentDir_True", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(0, 0), new Vector2D(0, 1));
      IsTrue(r1.Touches(r2));
      NotNull(r1.Touch(r2));
      IsTrue(r1.Touch(r2).AlmostEquals(new Point2D(0, 0)));
    });
    Test("Touches_RayRayAntiParallelSameOrigin_True", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(0, 0), new Vector2D(-1, 0));
      IsTrue(r1.Touches(r2));
      IsTrue(r1.Touch(r2).AlmostEquals(new Point2D(0, 0)));
    });
    Test("Touches_RayRayAntiParallelOverlapping_False", () => {
      var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var r2 = Ray2D.Make(new Point2D(3, 0), new Vector2D(-1, 0));
      IsFalse(r1.Touches(r2));
      IsNull(r1.Touch(r2));
    });
    Test("Touches_SegmentEndpoint_True", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var s = LineSegment2D.Make(new Point2D(3, 0), new Point2D(3, 2));
      IsTrue(r.Touches(s));
      IsTrue(r.Touch(s).AlmostEquals(new Point2D(3, 0)));
    });
  }
}

}  // namespace GeomPPTests
