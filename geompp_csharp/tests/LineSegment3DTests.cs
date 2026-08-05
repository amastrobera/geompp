using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class LineSegment3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── LineSegment3D ─────────────────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment3D");

    Test("Contains_PointOnSegment", () => {
      IsTrue(LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).Contains(new Point3D(2, 0, 0)));
    });

    Test("DistanceTo_PointAboveSegment", () => {
      Eq(3.0, LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).DistanceTo(new Point3D(2, 3, 0)));
    });

    Test("Location_Midpoint_IsHalf", () => {
      Eq(0.5, LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).Location(new Point3D(2, 0, 0)));
    });

    Test("Interpolate_Midpoint", () => {
      var mid = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).Interpolate(0.5);
      NotNull(mid);
      Eq(2.0, mid!.X);
      Eq(0.0, mid.Y);
      Eq(0.0, mid.Z);
    });

    Test("LineSegment3D_DistanceTo_Line3D_Crossing_IsZero", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var line = Line3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      Eq(0.0, seg.DistanceTo(line));
    });

    Test("LineSegment3D_DistanceTo_Line3D_Parallel", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var line = Line3D.Make(new Point3D(0, 3, 0), new Point3D(1, 3, 0));
      Eq(3.0, seg.DistanceTo(line));
    });

    Test("LineSegment3D_DistanceTo_Line3D_Overlap_IsZero", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var line = Line3D.Make(new Point3D(-2, 0, 0), new Point3D(10, 0, 0));
      Eq(0.0, seg.DistanceTo(line));
      IsNull(seg.Distance(line));
    });

    Test("LineSegment3D_DistanceTo_Ray3D_Crossing_IsZero", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var ray = Ray3D.Make(new Point3D(2, 3, 0), new Vector3D(0, -1, 0));
      Eq(0.0, seg.DistanceTo(ray));
    });

    Test("LineSegment3D_DistanceTo_Ray3D_Parallel", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var ray = Ray3D.Make(new Point3D(0, 0, 4), new Vector3D(1, 0, 0));
      Eq(4.0, seg.DistanceTo(ray));
    });

    Test("LineSegment3D_DistanceTo_Ray3D_Overlap_IsZero", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var ray = Ray3D.Make(new Point3D(1, 0, 0), new Vector3D(1, 0, 0));
      Eq(0.0, seg.DistanceTo(ray));
      IsNull(seg.Distance(ray));
    });

    // 2D top-down (XY) the ray crosses the segment at (2,0); in 3D ray is at z=5 while
    // the segment is at z=0 → non-parallel skew, real distance = 5.
    Test("LineSegment3D_DistanceTo_Ray3D_SkewTopCross", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var ray = Ray3D.Make(new Point3D(2, 3, 5), new Vector3D(0, -1, 0));
      Eq(5.0, seg.DistanceTo(ray));
    });

    Test("LineSegment3D_DistanceTo_LineSegment3D_Crossing_IsZero", () => {
      var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var s2 = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      Eq(0.0, s1.DistanceTo(s2));
    });

    Test("LineSegment3D_DistanceTo_LineSegment3D_Parallel", () => {
      var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var s2 = LineSegment3D.Make(new Point3D(0, 3, 0), new Point3D(2, 3, 0));
      Eq(3.0, s1.DistanceTo(s2));
    });

    Test("LineSegment3D_DistanceTo_LineSegment3D_Overlap_IsZero", () => {
      var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var s2 = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(6, 0, 0));
      Eq(0.0, s1.DistanceTo(s2));
      IsNull(s1.Distance(s2));
    });

    Test("LineSegment3D_DistanceTo_LineSegment3D_Skew", () => {
      var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var s2 = LineSegment3D.Make(new Point3D(2, -1, 5), new Point3D(2, 1, 5));
      Eq(5.0, s1.DistanceTo(s2));
    });

    Test("FromWkt_Whitespace", () => {
      var expected = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0));
      IsTrue(expected.AlmostEquals(LineSegment3D.FromWkt("LINESTRING (0 0 0,1 1 0)")));
      IsTrue(expected.AlmostEquals(LineSegment3D.FromWkt("LINESTRING (  0 0 0  ,  1  1  0  )")));
    });

    // ── LineSegment3D (additional) ────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment3D (additional)");

    Test("Length_KnownValue", () => {
      Eq(5.0, LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(3, 4, 0)).Length());
    });

    Test("AlmostEquals_SameSeg_True", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var b = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      IsTrue(a.AlmostEquals(b));
    });

    Test("ToLine_ReturnsLine3D", () => {
      var seg  = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var line = seg.ToLine();
      NotNull(line);
      IsTrue(line is Line3D);
    });

    Test("Intersects_Line3D_True", () => {
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      var l   = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      IsTrue(seg.Intersects(l));
    });

    Test("Intersects_Ray3D_True", () => {
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      var r   = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(seg.Intersects(r));
    });

    Test("Intersects_Seg3D_True", () => {
      var a = LineSegment3D.Make(new Point3D(0, -1, 0), new Point3D(0, 1, 0));
      var b = LineSegment3D.Make(new Point3D(-1, 0, 0), new Point3D(1, 0, 0));
      IsTrue(a.Intersects(b));
    });

    Test("Intersects_Seg3D_False", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var b = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(3, 0, 0));
      IsFalse(a.Intersects(b));
    });

    Test("Intersection_Line3D_Hit", () => {
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      var l   = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var pt  = seg.Intersection(l);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Ray3D_Hit", () => {
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      var r   = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var pt  = seg.Intersection(r);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Seg3D_Hit", () => {
      var a  = LineSegment3D.Make(new Point3D(0, -1, 0), new Point3D(0, 1, 0));
      var b  = LineSegment3D.Make(new Point3D(-1, 0, 0), new Point3D(1, 0, 0));
      var pt = a.Intersection(b);
      NotNull(pt);
      Eq(0.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Seg3D_Miss", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var b = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(3, 0, 0));
      IsNull(a.Intersection(b));
    });

    Test("WktRoundTrip", () => {
      var seg = LineSegment3D.Make(new Point3D(1, 2, 3), new Point3D(4, 5, 6));
      IsTrue(seg.AlmostEquals(LineSegment3D.FromWkt(seg.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var seg  = LineSegment3D.Make(new Point3D(1, 2, 3), new Point3D(4, 5, 6));
      var path = Path.GetTempFileName();
      seg.ToFile(path);
      IsTrue(seg.AlmostEquals(LineSegment3D.FromFile(path)));
      File.Delete(path);
    });

    Test("Reversed_SwapsEndpoints", () => {
      var s = LineSegment3D.Make(new Point3D(1, 2, 3), new Point3D(4, 5, 6));
      var r = s.Reversed();
      NotNull(r);
      IsTrue(s.Last().AlmostEquals(r!.First()));
      IsTrue(s.First().AlmostEquals(r.Last()));
    });

    Test("Reversed_LengthPreserved", () => {
      var s = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(3, 4, 0));
      Eq(s.Length(), s.Reversed()!.Length());
    });

    Test("Reversed_TwiceIsOriginal", () => {
      var s = LineSegment3D.Make(new Point3D(1, 2, 3), new Point3D(4, 5, 6));
      var rr = s.Reversed()!.Reversed()!;
      IsTrue(s.First().AlmostEquals(rr.First()));
      IsTrue(s.Last().AlmostEquals(rr.Last()));
    });

    Test("ToString_ContainsWkt", () => {
      var s = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0)).ToString();
      IsTrue(s.Contains("LINESTRING"), "missing LINESTRING keyword");
    });

    Test("Equality_SameSeg_True", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var b = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      IsTrue(a == b);
    });

    Test("Equality_DifferentSeg_False", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var b = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(0, 4, 0));
      IsFalse(a == b);
    });

    // ── Overlap — LineSegment3D ───────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment3D Overlap");

    Test("Overlaps_CollinearLine3D_True", () => {
      var s = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(5, 0, 0));
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      IsTrue(s.Overlaps(x));
      IsTrue(s.Overlap(x)!.AlmostEquals(s));
    });

    Test("Overlaps_Segments3D_Partial", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(5, 0, 0));
      var b = LineSegment3D.Make(new Point3D(3, 0, 0), new Point3D(7, 0, 0));
      IsTrue(a.Overlaps(b));
      var ov = a.Overlap(b);
      NotNull(ov);
      IsTrue(ov!.AlmostEquals(LineSegment3D.Make(new Point3D(3, 0, 0), new Point3D(5, 0, 0))));
    });

    Test("Overlaps_Segments3D_TouchEndpoint_False", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(5, 0, 0));
      var b = LineSegment3D.Make(new Point3D(5, 0, 0), new Point3D(8, 0, 0));
      IsFalse(a.Overlaps(b));
      IsNull(a.Overlap(b));
    });

    // ── Touch — LineSegment3D ─────────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment3D Touch");
    Test("Touches_Seg3DLineFirstEndpoint_True", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var s = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(2, 0, 3));
      IsTrue(s.Touches(x));
      IsTrue(s.Touch(x).AlmostEquals(new Point3D(2, 0, 0)));
    });
    Test("Touches_Seg3DRayEndpoint_True", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var s = LineSegment3D.Make(new Point3D(3, 0, 0), new Point3D(3, 0, 2));
      IsTrue(s.Touches(r));
      IsTrue(s.Touch(r).AlmostEquals(new Point3D(3, 0, 0)));
    });
    Test("Touches_Seg3DSegmentTJunction_True", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(5, 0, 0));
      var b = LineSegment3D.Make(new Point3D(3, 0, 0), new Point3D(3, 0, 3));
      IsTrue(a.Touches(b));
      IsTrue(a.Touch(b).AlmostEquals(new Point3D(3, 0, 0)));
    });
    Test("Touches_Seg3DCollinearEndpoint_True", () => {
      var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(5, 0, 0));
      var b = LineSegment3D.Make(new Point3D(5, 0, 0), new Point3D(8, 0, 0));
      IsTrue(a.Touches(b));
      IsTrue(a.Touch(b).AlmostEquals(new Point3D(5, 0, 0)));
    });
  }
}

}  // namespace GeomPPTests
