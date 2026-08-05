using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Polyline2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Polyline2D ────────────────────────────────────────────────────────────────
    Console.WriteLine("\nPolyline2D");

    Test("Contains_PointOnPolyline", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      IsTrue(pl.Contains(new Point2D(2, 0)));
    });

    Test("Contains_PointOffPolyline_False", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      IsFalse(pl.Contains(new Point2D(2, 1)));
    });

    Test("DistanceTo_PointAboveSegment", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      Eq(2.0, pl.DistanceTo(new Point2D(2, 3)));
    });

    Test("Location_StartPoint_IsZero", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      Eq(0.0, pl.Location(new Point2D(0, 0)));
    });

    Test("Location_EndPoint_IsOne", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      Eq(1.0, pl.Location(new Point2D(4, 4)));
    });

    Test("Interpolate_StartPoint", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var p = pl.Interpolate(0.0);
      NotNull(p);
      Eq(0.0, p!.X);
      Eq(0.0, p.Y);
    });

    Test("Interpolate_EndPoint", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var p = pl.Interpolate(1.0);
      NotNull(p);
      Eq(4.0, p!.X);
      Eq(4.0, p.Y);
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Polyline2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1) });
      IsTrue(expected.AlmostEquals(Polyline2D.FromWkt("LINESTRING (0 0, 1 0, 1 1)")));
      IsTrue(expected.AlmostEquals(Polyline2D.FromWkt("LINESTRING (  0 0  ,  1 0  ,  1 1  )")));
    });

    // ── Polyline2D (additional) ───────────────────────────────────────────────────
    Console.WriteLine("\nPolyline2D (additional)");

    Test("Length_LShapedPolyline", () => {
      // Two segments each of length 4 → total 8
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      Eq(8.0, pl.Length());
    });

    Test("ToSegments_CountAndType", () => {
      var pl   = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var segs = pl.ToSegments();
      Eq(2, segs.Length, 0);
      IsTrue(segs[0] is LineSegment2D);
    });

    Test("AlmostEquals_SamePolyline_True", () => {
      var a = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var b = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      IsTrue(a.AlmostEquals(b));
    });

    Test("ConvexHull_TooFewPoints_Throws", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(1,0) });
      bool threw = false;
      try { pl.ConvexHull(); } catch (Exception) { threw = true; }
      IsTrue(threw, "ConvexHull on 2-point polyline should throw");
    });

    Test("ConvexHull_ThreePoints_ReturnsTriangle", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(2,3) });
      var hull = pl.ConvexHull();
      Eq(3, hull.Size());
    });

    Test("ConvexHull_ConcavePath_InnerPointExcluded", () => {
      // simple path: outer corners with inner dip at (2,1) — hull is the 4 outer corners
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4), new(2,1), new(0,4) });
      var hull = pl.ConvexHull();
      Eq(4, hull.Size());
      var expected = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
      foreach (var e in expected) {
        bool found = false;
        for (int i = 0; i < hull.Size(); i++) if (hull[i].AlmostEquals(e)) { found = true; break; }
        IsTrue(found, $"{e} should be on hull");
      }
    });

    Test("AlmostEquals_DiffPolyline_False", () => {
      var a = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0) });
      var b = Polyline2D.Make(new Point2D[] { new(0,0), new(0,4) });
      IsFalse(a.AlmostEquals(b));
    });

    Test("Intersects_Line2D_True", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var l  = Line2D.Make(new Point2D(2, -2), new Point2D(2, 2));
      IsTrue(pl.Intersects(l));
    });

    Test("Intersects_Ray2D_True", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var r  = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
      IsTrue(pl.Intersects(r));
    });

    Test("Intersects_Seg2D_True", () => {
      var pl  = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      IsTrue(pl.Intersects(seg));
    });

    Test("Intersects_Polyline2D_True", () => {
      var a = Polyline2D.Make(new Point2D[] { new(0,-1), new(0,1) });
      var b = Polyline2D.Make(new Point2D[] { new(-1,0), new(1,0) });
      IsTrue(a.Intersects(b));
    });

    Test("Intersection_Line2D_ReturnsNonNull", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var l  = Line2D.Make(new Point2D(2, -2), new Point2D(2, 2));
      NotNull(pl.Intersection(l));
    });

    Test("Intersection_Ray2D_ReturnsNonNull", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var r  = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
      NotNull(pl.Intersection(r));
    });

    Test("Intersection_Seg2D_ReturnsNonNull", () => {
      var pl  = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
      NotNull(pl.Intersection(seg));
    });

    Test("Intersection_Polyline2D_ReturnsNonNull", () => {
      var a = Polyline2D.Make(new Point2D[] { new(0,-1), new(0,1) });
      var b = Polyline2D.Make(new Point2D[] { new(-1,0), new(1,0) });
      NotNull(a.Intersection(b));
    });

    Test("Intersection_NoHit_ReturnsNull", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(1,0) });
      var l  = Line2D.Make(new Point2D(0, 5), new Point2D(1, 5));
      IsNull(pl.Intersection(l));
    });

    Test("WktRoundTrip", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      IsTrue(pl.AlmostEquals(Polyline2D.FromWkt(pl.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var pl   = Polyline2D.Make(new Point2D[] { new(1,2), new(3,4), new(5,6) });
      var path = Path.GetTempFileName();
      pl.ToFile(path);
      IsTrue(pl.AlmostEquals(Polyline2D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) }).ToString();
      IsTrue(s.Contains("LINESTRING"), "missing LINESTRING keyword");
    });

    Test("Equality_SamePolyline_True", () => {
      var a = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      var b = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4) });
      IsTrue(a == b);
    });

    Test("Equality_DifferentPolyline_False", () => {
      var a = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0) });
      var b = Polyline2D.Make(new Point2D[] { new(0,0), new(0,4) });
      IsFalse(a == b);
    });

    // ── Polyline2D Overlap / Touch ────────────────────────────────────────────────

    Console.WriteLine("\nPolyline2D::Overlap / Touch");

    Test("Polyline2D_Overlaps_Line_CollinearSegment", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,3) });
        var line = Line2D.Make(new Point2D(0,0), new Vector2D(1,0));
        IsTrue(pl.Overlaps(line));
        var ov = pl.Overlap(line);
        NotNull(ov);
        IsTrue(ov.Length == 1);
        IsTrue(ov[0].AlmostEquals(LineSegment2D.Make(new Point2D(0,0), new Point2D(4,0))));
    });

    Test("Polyline2D_Overlaps_Line_Perpendicular_NoOverlap", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0) });
        var line = Line2D.Make(new Point2D(0,0), new Vector2D(0,1));
        IsFalse(pl.Overlaps(line));
        IsNull(pl.Overlap(line));
    });

    Test("Polyline2D_Overlaps_Ray_CollinearPartial", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(6,0) });
        var ray = Ray2D.Make(new Point2D(2,0), new Vector2D(1,0));
        IsTrue(pl.Overlaps(ray));
        var ov = pl.Overlap(ray);
        NotNull(ov);
        IsTrue(ov.Length == 1);
        IsTrue(ov[0].AlmostEquals(LineSegment2D.Make(new Point2D(2,0), new Point2D(6,0))));
    });

    Test("Polyline2D_Overlaps_Segment_Partial", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(5,0) });
        var seg = LineSegment2D.Make(new Point2D(3,0), new Point2D(7,0));
        IsTrue(pl.Overlaps(seg));
        var ov = pl.Overlap(seg);
        NotNull(ov);
        IsTrue(ov.Length == 1);
        IsTrue(ov[0].AlmostEquals(LineSegment2D.Make(new Point2D(3,0), new Point2D(5,0))));
    });

    Test("Polyline2D_Overlaps_Polyline_Shared", () => {
        var pl1 = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,3) });
        var pl2 = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0) });
        IsTrue(pl1.Overlaps(pl2));
        NotNull(pl1.Overlap(pl2));
    });

    Test("Polyline2D_Overlaps_Polyline_NoOverlap", () => {
        var pl1 = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0) });
        var pl2 = Polyline2D.Make(new Point2D[] { new(0,1), new(4,1) });
        IsFalse(pl1.Overlaps(pl2));
        IsNull(pl1.Overlap(pl2));
    });

    Test("Polyline2D_Touches_Line_EndpointOnLine", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(2,0), new(2,3) });
        var line = Line2D.Make(new Point2D(0,0), new Vector2D(1,0));
        IsTrue(pl.Touches(line));
        var tp = pl.Touch(line);
        NotNull(tp);
        IsTrue(tp.Length == 1);
        IsTrue(tp[0].AlmostEquals(new Point2D(2,0)));
    });

    Test("Polyline2D_Touches_Line_Collinear_NotTouch", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(0,0), new(4,0) });
        var line = Line2D.Make(new Point2D(0,0), new Vector2D(1,0));
        IsFalse(pl.Touches(line));
        IsNull(pl.Touch(line));
    });

    Test("Polyline2D_Touches_Ray_EndpointOnRay", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(3,0), new(3,2) });
        var ray = Ray2D.Make(new Point2D(0,0), new Vector2D(1,0));
        IsTrue(pl.Touches(ray));
        var tp = pl.Touch(ray);
        NotNull(tp);
        IsTrue(tp.Length == 1);
        IsTrue(tp[0].AlmostEquals(new Point2D(3,0)));
    });

    Test("Polyline2D_Touches_Segment_TJunction", () => {
        var pl = Polyline2D.Make(new Point2D[] { new(3,0), new(3,3) });
        var seg = LineSegment2D.Make(new Point2D(0,0), new Point2D(5,0));
        IsTrue(pl.Touches(seg));
        var tp = pl.Touch(seg);
        NotNull(tp);
        IsTrue(tp.Length == 1);
        IsTrue(tp[0].AlmostEquals(new Point2D(3,0)));
    });

    Test("Polyline2D_Touches_Polyline_SharedEndpoint", () => {
        var pl1 = Polyline2D.Make(new Point2D[] { new(0,0), new(3,0) });
        var pl2 = Polyline2D.Make(new Point2D[] { new(3,0), new(3,3) });
        IsTrue(pl1.Touches(pl2));
        var tp = pl1.Touch(pl2);
        NotNull(tp);
        IsTrue(tp.Length == 1);
        IsTrue(tp[0].AlmostEquals(new Point2D(3,0)));
    });

    Test("Polyline2D_Touches_Polyline_Disjoint", () => {
        var pl1 = Polyline2D.Make(new Point2D[] { new(0,0), new(2,0) });
        var pl2 = Polyline2D.Make(new Point2D[] { new(5,0), new(5,3) });
        IsFalse(pl1.Touches(pl2));
        IsNull(pl1.Touch(pl2));
    });

    // ── Polyline2D.Reduce ─────────────────────────────────────────────────────────
    Console.WriteLine("\nPolyline2D.Reduce");

    Test("Polyline2D_Reduce_TwoPoints_ReturnsUnchanged", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0, 0), new(1, 1) });
      IsTrue(pl.AlmostEquals(pl.Reduce()));
    });

    Test("Polyline2D_Reduce_RadialDistance", () => {
      // Note: a "peak" shape (not a straight line) is deliberate — Polyline2D.Make() prunes exactly
      // collinear knots at construction time, so a flat clustered dataset would collapse to its 2
      // endpoints regardless of what Reduce() does, defeating the test.
      var pl = Polyline2D.Make(new Point2D[] {
        new(0, 0), new(0.1, 0.05), new(0.2, -0.05), new(5, 5), new(5.1, 5.05), new(10, 0) });
      var reduced = pl.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.RadialDistance, 1.0));
      Eq(3, reduced.Size(), 0);
      IsTrue(reduced[0].AlmostEquals(new Point2D(0, 0)));
      IsTrue(reduced[1].AlmostEquals(new Point2D(5, 5)));
      IsTrue(reduced[2].AlmostEquals(new Point2D(10, 0)));
    });

    Test("Polyline2D_Reduce_RamerDouglasPeucker", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(4, 5), new(6, 0), new(8, 0) });
      var reduced = pl.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.RamerDouglasPeucker, 2.0));
      var expected = Polyline2D.Make(new Point2D[] { new(0, 0), new(4, 5), new(8, 0) });
      IsTrue(reduced.AlmostEquals(expected));
    });

    Test("Polyline2D_Reduce_VisvalingamWhyatt", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(4, 5), new(6, 0), new(8, 0) });
      var reduced = pl.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.VisvalingamWhyatt, 6.0));
      var expected = Polyline2D.Make(new Point2D[] { new(0, 0), new(4, 5), new(8, 0) });
      IsTrue(reduced.AlmostEquals(expected));
    });

    Test("Polyline2D_Reduce_DefaultParams_MatchesExplicitRdpHalfThreshold", () => {
      var pl = Polyline2D.Make(new Point2D[] {
        new(0, 0), new(1, 0.01), new(2, -0.01), new(3, 0), new(4, 0) });
      var reducedDefault = pl.Reduce();
      var reducedExplicit = pl.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.RamerDouglasPeucker, 0.5));
      IsTrue(reducedDefault.AlmostEquals(reducedExplicit));
      var expected = Polyline2D.Make(new Point2D[] { new(0, 0), new(4, 0) });
      IsTrue(reducedDefault.AlmostEquals(expected));
    });
  }
}

}  // namespace GeomPPTests
