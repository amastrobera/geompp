using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class LineSegment2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── LineSegment2D ─────────────────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment2D");

    Test("Make_AndLength", () => {
      Eq(5.0, LineSegment2D.Make(new Point2D(0, 0), new Point2D(3, 4)).Length());
    });

    Test("Intersects_WithLine", () => {
      var seg = LineSegment2D.Make(new Point2D(1, -1), new Point2D(1, 1));
      var line = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      IsTrue(seg.Intersects(line));
    });

    Test("Intersection_WithLine_ReturnsCorrectPoint", () => {
      var pt = LineSegment2D.Make(new Point2D(1, -1), new Point2D(1, 1))
                            .Intersection(Line2D.Make(new Point2D(0, 0), new Point2D(4, 0)));
      NotNull(pt);
      Eq(1.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Contains_PointOnSegment", () => {
      IsTrue(LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Contains(new Point2D(2, 0)));
    });

    Test("Contains_PointBeyondEnd_False", () => {
      IsFalse(LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Contains(new Point2D(5, 0)));
    });

    Test("DistanceTo_PointAboveSegment", () => {
      Eq(3.0, LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).DistanceTo(new Point2D(2, 3)));
    });

    Test("Location_Midpoint_IsHalf", () => {
      Eq(0.5, LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Location(new Point2D(2, 0)));
    });

    Test("Interpolate_Midpoint", () => {
      var mid = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Interpolate(0.5);
      NotNull(mid);
      Eq(2.0, mid!.X);
      Eq(0.0, mid.Y);
    });

    Test("FromWkt_Whitespace", () => {
      var expected = LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 1));
      IsTrue(expected.AlmostEquals(LineSegment2D.FromWkt("LINESTRING (0 0,1 1)")));
      IsTrue(expected.AlmostEquals(LineSegment2D.FromWkt("LINESTRING (  0 0  ,  1  1  )")));
    });

    // ── LineSegment2D (additional) ────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment2D (additional)");

    Test("ToLine_ReturnsLine2D", () => {
      var seg  = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var line = seg.ToLine();
      NotNull(line);
      IsTrue(line is Line2D);
    });

    Test("AlmostEquals_SameSeg_True", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var b = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      IsTrue(a.AlmostEquals(b));
    });

    Test("AlmostEquals_DiffSeg_False", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var b = LineSegment2D.Make(new Point2D(0, 0), new Point2D(0, 4));
      IsFalse(a.AlmostEquals(b));
    });

    Test("Intersects_Ray2D_True", () => {
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      IsTrue(seg.Intersects(r));
    });

    Test("Intersects_Seg2D_True", () => {
      var a = LineSegment2D.Make(new Point2D(0, -1), new Point2D(0, 1));
      var b = LineSegment2D.Make(new Point2D(-1, 0), new Point2D(1, 0));
      IsTrue(a.Intersects(b));
    });

    Test("Intersects_Seg2D_False", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var b = LineSegment2D.Make(new Point2D(2, 0), new Point2D(3, 0));
      IsFalse(a.Intersects(b));
    });

    Test("Intersection_Ray2D_Hit", () => {
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var pt  = seg.Intersection(r);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Intersection_Seg2D_Hit", () => {
      var a  = LineSegment2D.Make(new Point2D(0, -1), new Point2D(0, 1));
      var b  = LineSegment2D.Make(new Point2D(-1, 0), new Point2D(1, 0));
      var pt = a.Intersection(b);
      NotNull(pt);
      Eq(0.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Intersection_Seg2D_Miss", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var b = LineSegment2D.Make(new Point2D(2, 0), new Point2D(3, 0));
      IsNull(a.Intersection(b));
    });

    Test("WktRoundTrip", () => {
      var seg = LineSegment2D.Make(new Point2D(1, 2), new Point2D(3, 4));
      IsTrue(seg.AlmostEquals(LineSegment2D.FromWkt(seg.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var seg  = LineSegment2D.Make(new Point2D(1, 2), new Point2D(3, 4));
      var path = Path.GetTempFileName();
      seg.ToFile(path);
      IsTrue(seg.AlmostEquals(LineSegment2D.FromFile(path)));
      File.Delete(path);
    });

    Test("Reversed_SwapsEndpoints", () => {
      var s = LineSegment2D.Make(new Point2D(1, 2), new Point2D(3, 4));
      var r = s.Reversed();
      NotNull(r);
      IsTrue(s.Last().AlmostEquals(r!.First()));
      IsTrue(s.First().AlmostEquals(r.Last()));
    });

    Test("Reversed_LengthPreserved", () => {
      var s = LineSegment2D.Make(new Point2D(0, 0), new Point2D(3, 4));
      Eq(s.Length(), s.Reversed()!.Length());
    });

    Test("Reversed_TwiceIsOriginal", () => {
      var s = LineSegment2D.Make(new Point2D(1, 2), new Point2D(3, 4));
      var rr = s.Reversed()!.Reversed()!;
      IsTrue(s.First().AlmostEquals(rr.First()));
      IsTrue(s.Last().AlmostEquals(rr.Last()));
    });

    Test("ToString_ContainsWkt", () => {
      var s = LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 1)).ToString();
      IsTrue(s.Contains("LINESTRING"), "missing LINESTRING keyword");
    });

    Test("Equality_SameSeg_True", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var b = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      IsTrue(a == b);
    });

    Test("Equality_DifferentSeg_False", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var b = LineSegment2D.Make(new Point2D(0, 0), new Point2D(0, 4));
      IsFalse(a == b);
    });

    // ── Overlap — LineSegment2D ───────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment2D Overlap");

    Test("Overlaps_CollinearLine2D_True", () => {
      var s = LineSegment2D.Make(new Point2D(2, 0), new Point2D(5, 0));
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      IsTrue(s.Overlaps(x));
      IsTrue(s.Overlap(x)!.AlmostEquals(s));
    });

    Test("Overlaps_Segments_Partial", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(5, 0));
      var b = LineSegment2D.Make(new Point2D(3, 0), new Point2D(7, 0));
      IsTrue(a.Overlaps(b));
      var ov = a.Overlap(b);
      NotNull(ov);
      IsTrue(ov!.AlmostEquals(LineSegment2D.Make(new Point2D(3, 0), new Point2D(5, 0))));
    });

    Test("Overlaps_Segments_TouchEndpoint_False", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(5, 0));
      var b = LineSegment2D.Make(new Point2D(5, 0), new Point2D(8, 0));
      IsFalse(a.Overlaps(b));
      IsNull(a.Overlap(b));
    });

    Test("Overlaps_Segments_Disjoint_False", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(5, 0));
      var b = LineSegment2D.Make(new Point2D(6, 0), new Point2D(9, 0));
      IsFalse(a.Overlaps(b));
      IsNull(a.Overlap(b));
    });

    // ── Touch — LineSegment2D ─────────────────────────────────────────────────────
    Console.WriteLine("\nLineSegment2D Touch");
    Test("Touches_LineFirstEndpoint_True", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var s = LineSegment2D.Make(new Point2D(2, 0), new Point2D(2, 3));
      IsTrue(s.Touches(x));
      IsTrue(s.Touch(x).AlmostEquals(new Point2D(2, 0)));
    });
    Test("Touches_RayEndpoint_True", () => {
      var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
      var s = LineSegment2D.Make(new Point2D(3, 0), new Point2D(3, 2));
      IsTrue(s.Touches(r));
      IsTrue(s.Touch(r).AlmostEquals(new Point2D(3, 0)));
    });
    Test("Touches_SegmentTJunction_True", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(5, 0));
      var b = LineSegment2D.Make(new Point2D(3, 0), new Point2D(3, 3));
      IsTrue(a.Touches(b));
      IsTrue(a.Touch(b).AlmostEquals(new Point2D(3, 0)));
    });
    Test("Touches_SegmentCollinearEndpoint_True", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(5, 0));
      var b = LineSegment2D.Make(new Point2D(5, 0), new Point2D(8, 0));
      IsTrue(a.Touches(b));
      IsTrue(a.Touch(b).AlmostEquals(new Point2D(5, 0)));
    });
    Test("Touches_SegmentOverlap_False", () => {
      var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(5, 0));
      var b = LineSegment2D.Make(new Point2D(3, 0), new Point2D(7, 0));
      IsFalse(a.Touches(b));
      IsNull(a.Touch(b));
    });
  }
}

}  // namespace GeomPPTests
