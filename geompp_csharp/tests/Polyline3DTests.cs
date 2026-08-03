using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Polyline3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Polyline3D ────────────────────────────────────────────────────────────────
    Console.WriteLine("\nPolyline3D");

    Test("Contains_PointOnPolyline", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      IsTrue(pl.Contains(new Point3D(2, 0, 0)));
    });

    Test("Contains_PointOffPolyline_False", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      IsFalse(pl.Contains(new Point3D(2, 1, 0)));
    });

    Test("DistanceTo_PointAboveSegment", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      Eq(2.0, pl.DistanceTo(new Point3D(2, 3, 0)));
    });

    Test("Location_StartPoint_IsZero", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      Eq(0.0, pl.Location(new Point3D(0, 0, 0)));
    });

    Test("Location_EndPoint_IsOne", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      Eq(1.0, pl.Location(new Point3D(4, 4, 0)));
    });

    Test("Interpolate_StartPoint", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var p = pl.Interpolate(0.0);
      NotNull(p);
      Eq(0.0, p!.X);
      Eq(0.0, p.Y);
      Eq(0.0, p.Z);
    });

    Test("Interpolate_EndPoint", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var p = pl.Interpolate(1.0);
      NotNull(p);
      Eq(4.0, p!.X);
      Eq(4.0, p.Y);
      Eq(0.0, p.Z);
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Polyline3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0) });
      IsTrue(expected.AlmostEquals(Polyline3D.FromWkt("LINESTRING (0 0 0, 1 0 0, 1 1 0)")));
      IsTrue(expected.AlmostEquals(Polyline3D.FromWkt("LINESTRING (  0 0 0  ,  1 0 0  ,  1 1 0  )")));
    });

    // ── Polyline3D (additional) ───────────────────────────────────────────────────
    Console.WriteLine("\nPolyline3D (additional)");

    Test("Length_LShapedPolyline", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      Eq(8.0, pl.Length());
    });

    Test("ToSegments_CountAndType", () => {
      var pl   = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var segs = pl.ToSegments();
      Eq(2, segs.Length, 0);
      IsTrue(segs[0] is LineSegment3D);
    });

    Test("AlmostEquals_SamePolyline_True", () => {
      var a = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0) });
      var b = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0) });
      IsTrue(a.AlmostEquals(b));
    });

    Test("Indexer_ReturnsCorrectPoint", () => {
      // (1,0,3) → (4,5,6) → (7,8,9): cross product (3,5,3)×(6,8,6) = (6,0,-6) ≠ 0 → non-collinear, middle kept
      var pl = Polyline3D.Make(new Point3D[] { new(1,0,3), new(4,5,6), new(7,8,9) });
      Eq(4.0, pl[1].X);
      Eq(5.0, pl[1].Y);
      Eq(6.0, pl[1].Z);
    });

    Test("Intersects_Line3D_True", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var l  = Line3D.Make(new Point3D(2,-2,0), new Point3D(2,2,0));
      IsTrue(pl.Intersects(l));
    });

    Test("Intersects_Ray3D_True", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var r  = Ray3D.Make(new Point3D(2,-2,0), new Vector3D(0,1,0));
      IsTrue(pl.Intersects(r));
    });

    Test("Intersects_Seg3D_True", () => {
      var pl  = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var seg = LineSegment3D.Make(new Point3D(2,-1,0), new Point3D(2,1,0));
      IsTrue(pl.Intersects(seg));
    });

    Test("Intersects_Polyline3D_True", () => {
      var a = Polyline3D.Make(new Point3D[] { new(0,-1,0), new(0,1,0) });
      var b = Polyline3D.Make(new Point3D[] { new(-1,0,0), new(1,0,0) });
      IsTrue(a.Intersects(b));
    });

    Test("Intersection_Line3D_ReturnsNonNull", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var l  = Line3D.Make(new Point3D(2,-2,0), new Point3D(2,2,0));
      NotNull(pl.Intersection(l));
    });

    Test("Intersection_Ray3D_ReturnsNonNull", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var r  = Ray3D.Make(new Point3D(2,-2,0), new Vector3D(0,1,0));
      NotNull(pl.Intersection(r));
    });

    Test("Intersection_Seg3D_ReturnsNonNull", () => {
      var pl  = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      var seg = LineSegment3D.Make(new Point3D(2,-1,0), new Point3D(2,1,0));
      NotNull(pl.Intersection(seg));
    });

    Test("Intersection_Polyline3D_ReturnsNonNull", () => {
      var a = Polyline3D.Make(new Point3D[] { new(0,-1,0), new(0,1,0) });
      var b = Polyline3D.Make(new Point3D[] { new(-1,0,0), new(1,0,0) });
      NotNull(a.Intersection(b));
    });

    Test("WktRoundTrip", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) });
      IsTrue(pl.AlmostEquals(Polyline3D.FromWkt(pl.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var pl   = Polyline3D.Make(new Point3D[] { new(1,2,3), new(4,5,6) });
      var path = Path.GetTempFileName();
      pl.ToFile(path);
      IsTrue(pl.AlmostEquals(Polyline3D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0) }).ToString();
      IsTrue(s.Contains("LINESTRING"), "missing LINESTRING keyword");
    });

    Test("Equality_SamePolyline_True", () => {
      var a = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0) });
      var b = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0) });
      IsTrue(a == b);
    });

    Test("Equality_DifferentPolyline_False", () => {
      var a = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0) });
      var b = Polyline3D.Make(new Point3D[] { new(0,0,0), new(0,4,0) });
      IsFalse(a == b);
    });

    Console.WriteLine("\nPolyline3D::IsPlanar/IsSimple/IsConvex/ConvexHull/ToPolygon");
    {
        var planar = Polyline3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(2,2,0), new Point3D(0,2,0)
        });
        var nonPlanar = Polyline3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(1,1,1), new Point3D(0,1,2)
        });
        var star = Polyline3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(1,1,0),
            new Point3D(2,2,0), new Point3D(0,2,0)
        });

        Test("IsPlanar_XY_True",         () => IsTrue(planar.IsPlanar()));
        Test("IsPlanar_NonPlanar_False",  () => IsTrue(!nonPlanar.IsPlanar()));
        Test("IsSimple_True",             () => IsTrue(planar.IsSimple()));
        Test("IsConvex_Planar_True",      () => IsTrue(planar.IsConvex()));
        Test("IsConvex_NotPlanar_Throws", () => {
            try { nonPlanar.IsConvex(); IsTrue(false, "expected throw"); }
            catch (Exception) { }
        });
        Test("ConvexHull_ReturnsPolyline", () => NotNull(star.ConvexHull()));
        Test("ConvexHull_ThenToPolygon",   () => NotNull(star.ConvexHull().ToPolygon()));
        Test("ToPolygon_Valid",            () => NotNull(planar.ToPolygon()));
        Test("ToPolygon_NotPlanar_Throws", () => {
            try { nonPlanar.ToPolygon(); IsTrue(false, "expected throw"); }
            catch (Exception) { }
        });
    }

    // ── Polyline3D Overlap / Touch ────────────────────────────────────────────────

    Console.WriteLine("\nPolyline3D::Overlap / Touch");

    Test("Polyline3D_Overlaps_Line_CollinearSegment", () => {
        var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,0,3) });
        var line = Line3D.Make(new Point3D(0,0,0), new Point3D(1,0,0));
        IsTrue(pl.Overlaps(line));
        var ov = pl.Overlap(line);
        NotNull(ov);
        IsTrue(ov.Length == 1);
        IsTrue(ov[0].AlmostEquals(LineSegment3D.Make(new Point3D(0,0,0), new Point3D(4,0,0))));
    });

    Test("Polyline3D_Overlaps_Ray_Partial", () => {
        var pl = Polyline3D.Make(new Point3D[] { new(0,0,0), new(6,0,0) });
        var ray = Ray3D.Make(new Point3D(2,0,0), new Vector3D(1,0,0));
        IsTrue(pl.Overlaps(ray));
        var ov = pl.Overlap(ray);
        NotNull(ov);
        IsTrue(ov.Length == 1);
        IsTrue(ov[0].AlmostEquals(LineSegment3D.Make(new Point3D(2,0,0), new Point3D(6,0,0))));
    });

    Test("Polyline3D_Overlaps_Polyline_NoOverlap", () => {
        var pl1 = Polyline3D.Make(new Point3D[] { new(0,0,0), new(4,0,0) });
        var pl2 = Polyline3D.Make(new Point3D[] { new(0,1,0), new(4,1,0) });
        IsFalse(pl1.Overlaps(pl2));
        IsNull(pl1.Overlap(pl2));
    });

    Test("Polyline3D_Touches_Line_EndpointOnLine", () => {
        var pl = Polyline3D.Make(new Point3D[] { new(2,0,0), new(2,0,3) });
        var line = Line3D.Make(new Point3D(0,0,0), new Point3D(1,0,0));
        IsTrue(pl.Touches(line));
        var tp = pl.Touch(line);
        NotNull(tp);
        IsTrue(tp.Length == 1);
        IsTrue(tp[0].AlmostEquals(new Point3D(2,0,0)));
    });

    Test("Polyline3D_Touches_Segment_TJunction", () => {
        var pl = Polyline3D.Make(new Point3D[] { new(3,0,0), new(3,0,3) });
        var seg = LineSegment3D.Make(new Point3D(0,0,0), new Point3D(5,0,0));
        IsTrue(pl.Touches(seg));
        var tp = pl.Touch(seg);
        NotNull(tp);
        IsTrue(tp.Length == 1);
        IsTrue(tp[0].AlmostEquals(new Point3D(3,0,0)));
    });

    Test("Polyline3D_Touches_Polyline_SharedEndpoint", () => {
        var pl1 = Polyline3D.Make(new Point3D[] { new(0,0,0), new(3,0,0) });
        var pl2 = Polyline3D.Make(new Point3D[] { new(3,0,0), new(3,0,3) });
        IsTrue(pl1.Touches(pl2));
        var tp = pl1.Touch(pl2);
        NotNull(tp);
        IsTrue(tp.Length == 1);
        IsTrue(tp[0].AlmostEquals(new Point3D(3,0,0)));
    });

    Test("Polyline3D_Touches_Polyline_Disjoint", () => {
        var pl1 = Polyline3D.Make(new Point3D[] { new(0,0,0), new(2,0,0) });
        var pl2 = Polyline3D.Make(new Point3D[] { new(5,0,0), new(5,0,3) });
        IsFalse(pl1.Touches(pl2));
        IsNull(pl1.Touch(pl2));
    });

    // ── Polyline decimation (GeomUtil.DistDecimation / RdpDecimation / VwDecimation) ─

    // ── Polyline3D.Reduce ─────────────────────────────────────────────────────────
    Console.WriteLine("\nPolyline3D.Reduce");

    Test("Polyline3D_Reduce_TwoPoints_ReturnsUnchanged", () => {
      var pl = Polyline3D.Make(new Point3D[] { new(0, 0, 0), new(1, 1, 1) });
      IsTrue(pl.AlmostEquals(pl.Reduce()));
    });

    Test("Polyline3D_Reduce_RadialDistance", () => {
      // Note: a "peak" shape (not a straight line) is deliberate — Polyline3D.Make() prunes exactly
      // collinear knots at construction time, so a flat clustered dataset would collapse to its 2
      // endpoints regardless of what Reduce() does, defeating the test.
      var pl = Polyline3D.Make(new Point3D[] {
        new(0, 0, 0), new(0.1, 0, 0.05), new(0.2, 0, -0.05), new(5, 0, 5), new(5.1, 0, 5.05), new(10, 0, 0) });
      var reduced = pl.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.RadialDistance, 1.0));
      Eq(3, reduced.Size(), 0);
      IsTrue(reduced[0].AlmostEquals(new Point3D(0, 0, 0)));
      IsTrue(reduced[1].AlmostEquals(new Point3D(5, 0, 5)));
      IsTrue(reduced[2].AlmostEquals(new Point3D(10, 0, 0)));
    });

    Test("Polyline3D_Reduce_RamerDouglasPeucker", () => {
      var pl = Polyline3D.Make(new Point3D[] {
        new(0, 0, 0), new(2, 0, 0), new(4, 0, 5), new(6, 0, 0), new(8, 0, 0) });
      var reduced = pl.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.RamerDouglasPeucker, 2.0));
      var expected = Polyline3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 5), new(8, 0, 0) });
      IsTrue(reduced.AlmostEquals(expected));
    });

    Test("Polyline3D_Reduce_VisvalingamWhyatt", () => {
      var pl = Polyline3D.Make(new Point3D[] {
        new(0, 0, 0), new(2, 0, 0), new(4, 0, 5), new(6, 0, 0), new(8, 0, 0) });
      var reduced = pl.Reduce(new PolylineDecimationParams(PolylineDecimationStrategy.VisvalingamWhyatt, 6.0));
      var expected = Polyline3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 5), new(8, 0, 0) });
      IsTrue(reduced.AlmostEquals(expected));
    });
  }
}

}  // namespace GeomPPTests
