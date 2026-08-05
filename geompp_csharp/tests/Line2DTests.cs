using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Line2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Line2D ────────────────────────────────────────────────────────────────────
    Console.WriteLine("\nLine2D");

    Test("Make_TwoPoints", () => NotNull(Line2D.Make(new Point2D(0, 0), new Point2D(1, 0))));

    Test("Intersects_CrossingLines_True", () => {
      var l1 = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var l2 = Line2D.Make(new Point2D(2, -2), new Point2D(2, 2));
      IsTrue(l1.Intersects(l2));
    });

    Test("Intersects_ParallelLines_False", () => {
      var l1 = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var l2 = Line2D.Make(new Point2D(0, 1), new Point2D(4, 1));
      IsFalse(l1.Intersects(l2));
    });

    Test("Intersection_CrossingLines_ReturnsCorrectPoint", () => {
      var pt = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0))
                     .Intersection(Line2D.Make(new Point2D(2, -2), new Point2D(2, 2)));
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Intersection_ParallelLines_ReturnsNull", () => {
      var result = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0))
                         .Intersection(Line2D.Make(new Point2D(0, 1), new Point2D(4, 1)));
      IsNull(result);
    });

    Test("Contains_PointOnLine", () => {
      IsTrue(Line2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Contains(new Point2D(2, 0)));
    });

    Test("Contains_PointOffLine", () => {
      IsFalse(Line2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Contains(new Point2D(2, 1)));
    });

    Test("DistanceTo_PointAboveLine", () => {
      var l = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      Eq(3.0, l.DistanceTo(new Point2D(2, 3)));
    });

    Test("ProjectOnto_PointAboveLine", () => {
      var proj = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0))
                       .ProjectOnto(new Point2D(3, 5));
      Eq(3.0, proj.X);
      Eq(0.0, proj.Y);
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Line2D.Make(new Point2D(0, 0), new Point2D(1, 1));
      IsTrue(expected.AlmostEquals(Line2D.FromWkt("LINE (0 0,1 1)")));
      IsTrue(expected.AlmostEquals(Line2D.FromWkt("LINE (  0 0  ,  1  1  )")));
    });

    // ── Line2D (additional) ───────────────────────────────────────────────────────
    Console.WriteLine("\nLine2D (additional)");

    Test("AlmostEquals_SameLine_True", () => {
      var l1 = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var l2 = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      IsTrue(l1.AlmostEquals(l2));
    });

    Test("Intersects_Ray2D_True", () => {
      var l = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var r = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
      IsTrue(l.Intersects(r));
    });

    Test("Intersects_Ray2D_False", () => {
      // Ray pointing away — origin behind line, ray going further behind
      var l = Line2D.Make(new Point2D(0, 1), new Point2D(4, 1));
      var r = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, -1));
      IsFalse(l.Intersects(r));
    });

    Test("Intersects_Seg2D_True", () => {
      var l   = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      IsTrue(l.Intersects(seg));
    });

    Test("Intersects_Seg2D_False", () => {
      // Segment parallel to and above the line — no crossing
      var l   = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var seg = LineSegment2D.Make(new Point2D(0, 1), new Point2D(4, 1));
      IsFalse(l.Intersects(seg));
    });

    Test("Intersection_Ray2D_Hit", () => {
      var l = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var r = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
      var pt = l.Intersection(r);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Intersection_Ray2D_Miss", () => {
      var l = Line2D.Make(new Point2D(0, 1), new Point2D(4, 1));
      var r = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, -1));
      IsNull(l.Intersection(r));
    });

    Test("Intersection_Seg2D_Hit", () => {
      var l   = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      var pt  = l.Intersection(seg);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
    });

    Test("Intersection_Seg2D_Miss", () => {
      var l   = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var seg = LineSegment2D.Make(new Point2D(0, 1), new Point2D(4, 1));
      IsNull(l.Intersection(seg));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var l    = Line2D.Make(new Point2D(1, 2), new Point2D(3, 4));
      var path = Path.GetTempFileName();
      l.ToFile(path);
      IsTrue(l.AlmostEquals(Line2D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Line2D.Make(new Point2D(0, 0), new Point2D(1, 1)).ToString();
      IsTrue(s.Contains("LINE"), "missing LINE keyword");
    });

    Test("Equality_SameLine_True", () => {
      var l1 = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var l2 = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      IsTrue(l1 == l2);
    });

    Test("Equality_DifferentLine_False", () => {
      var l1 = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var l2 = Line2D.Make(new Point2D(0, 0), new Point2D(0, 1));
      IsFalse(l1 == l2);
    });

    // ── Overlap — Line2D ──────────────────────────────────────────────────────────
    Console.WriteLine("\nLine2D Overlap");

    Test("Overlaps_SameLine_True", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var x2 = Line2D.Make(new Point2D(5, 0), new Point2D(9, 0));
      IsTrue(x.Overlaps(x2));
      NotNull(x.Overlap(x2));
      IsTrue(x.Overlap(x2) is Line2D);
    });

    Test("Overlaps_CrossingLines_False", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var y = Line2D.Make(new Point2D(0, 0), new Point2D(0, 4));
      IsFalse(x.Overlaps(y));
      IsNull(x.Overlap(y));
    });

    Test("Overlaps_CollinearRay_True", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var r = Ray2D.Make(new Point2D(2, 0), new Vector2D(1, 0));
      IsTrue(x.Overlaps(r));
      IsTrue(x.Overlap(r) is Ray2D);
    });

    Test("Overlaps_CollinearSegment_True", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
      var s = LineSegment2D.Make(new Point2D(2, 0), new Point2D(5, 0));
      IsTrue(x.Overlaps(s));
      IsTrue(x.Overlap(s) is LineSegment2D);
    });

    // ── Touch — Line2D ────────────────────────────────────────────────────────────
    Console.WriteLine("\nLine2D Touch");
    Test("Touches_RayOriginOnLine_True", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var r = Ray2D.Make(new Point2D(3, 0), new Vector2D(0, 1));
      IsTrue(x.Touches(r));
      NotNull(x.Touch(r));
      IsTrue(x.Touch(r).AlmostEquals(new Point2D(3, 0)));
    });
    Test("Touches_CollinearRay_False", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var r = Ray2D.Make(new Point2D(1, 0), new Vector2D(1, 0));
      IsFalse(x.Touches(r));
      IsNull(x.Touch(r));
    });
    Test("Touches_SegmentEndpointOnLine_True", () => {
      var x = Line2D.Make(new Point2D(0, 0), new Point2D(1, 0));
      var s = LineSegment2D.Make(new Point2D(2, 0), new Point2D(2, 3));
      IsTrue(x.Touches(s));
      NotNull(x.Touch(s));
      IsTrue(x.Touch(s).AlmostEquals(new Point2D(2, 0)));
    });
  }
}

}  // namespace GeomPPTests
