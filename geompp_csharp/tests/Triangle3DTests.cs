using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Triangle3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Triangle3D (Interpolate) ───────────────────────────────────────────────────
    Console.WriteLine("\nTriangle3D (additional)");

    Test("Interpolate_AtP0_ReturnsP0", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var p = t.Interpolate(0.0, 0.0);
      NotNull(p);
      Eq(0.0, p!.X);
      Eq(0.0, p.Y);
      Eq(0.0, p.Z);
    });

    Test("Interpolate_AtP1_ReturnsP1", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var p = t.Interpolate(1.0, 0.0);
      NotNull(p);
      Eq(1.0, p!.X);
      Eq(0.0, p.Y);
      Eq(0.0, p.Z);
    });

    Test("Interpolate_OutsideTriangle_ReturnsNull", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      IsNull(t.Interpolate(0.8, 0.8));
    });

    // ── Triangle3D ────────────────────────────────────────────────────────────────
    Console.WriteLine("\nTriangle3D");

    Test("Area_RightTriangleLegs1x1", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      Eq(0.5, t.Area());
    });

    Test("SignedArea_CCW_Positive", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var refN = new Vector3D(0, 0, 1);
      IsTrue(t.SignedArea(refN) > 0, "CCW triangle should have positive signed area");
    });

    Test("SignedArea_CW_Negative", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(0, 1, 0), new Point3D(1, 0, 0));
      var refN = new Vector3D(0, 0, 1);
      IsTrue(t.SignedArea(refN) < 0, "CW triangle should have negative signed area");
    });

    Test("SignedArea_FlippedRef_FlipsSign", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var refN = new Vector3D(0, 0, 1);
      IsTrue(t.SignedArea(refN) > 0, "positive with +Z ref");
      IsTrue(t.SignedArea(new Vector3D(0, 0, -1)) < 0, "negative with -Z ref");
    });

    Test("AreaVector_PointsInZ_LengthEqualsArea", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var av = t.AreaVector();
      NotNull(av);
      Eq(0.5, av!.Length());
      Eq(t.Area(), av.Length());
    });

    Test("ToPlane_NormalPointsInZ", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var pl = t.ToPlane();
      NotNull(pl);
      Eq(1.0, Math.Abs(pl!.Normal().Z));
    });

    Test("IsCCW_CCW_ReturnsTrue", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var refN = new Vector3D(0, 0, 1);
      IsTrue(t.IsCCW(refN), "CCW triangle should return true");
      IsTrue(t.SignedArea(refN) > 0, "CCW triangle should have positive signed area");
      IsTrue(t.IsCCW(refN) == (t.SignedArea(refN) > 0), "IsCCW must match sign of SignedArea");
    });

    Test("IsCCW_CW_ReturnsFalse", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(0, 1, 0), new Point3D(1, 0, 0));
      var refN = new Vector3D(0, 0, 1);
      IsFalse(t.IsCCW(refN), "CW triangle should return false");
      IsTrue(t.SignedArea(refN) < 0, "CW triangle should have negative signed area");
      IsTrue(t.IsCCW(refN) == (t.SignedArea(refN) > 0), "IsCCW must match sign of SignedArea");
    });

    Test("IsCCW_FlippedRef_FlipsResult", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      IsTrue(t.IsCCW(new Vector3D(0, 0, 1)),   "CCW with +Z ref");
      IsFalse(t.IsCCW(new Vector3D(0, 0, -1)), "CW when ref is flipped");
    });

    Test("Location_Vertices_ReturnExpectedCoords", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
      // location non-null ↔ contains true
      var st0 = t.Location(new Point3D(0, 0, 0));  NotNull(st0);  Eq(0.0, st0!.Item1);  Eq(0.0, st0.Item2);  IsTrue(t.Contains(new Point3D(0, 0, 0)));
      var st1 = t.Location(new Point3D(2, 0, 0));  NotNull(st1);  Eq(1.0, st1!.Item1);  Eq(0.0, st1.Item2);  IsTrue(t.Contains(new Point3D(2, 0, 0)));
      var st2 = t.Location(new Point3D(0, 2, 0));  NotNull(st2);  Eq(0.0, st2!.Item1);  Eq(1.0, st2.Item2);  IsTrue(t.Contains(new Point3D(0, 2, 0)));
    });

    Test("Location_Centroid_OneThirdEach", () => {
      var t  = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
      var c  = t.Centroid()!;
      var st = t.Location(c);
      NotNull(st);
      Eq(1.0 / 3.0, st!.Item1);
      Eq(1.0 / 3.0, st.Item2);
      IsTrue(t.Contains(c));  // location non-null ↔ contains true
    });

    Test("Location_NullImpliesNotContained", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
      // location null ↔ contains false
      IsNull(t.Location(new Point3D(0.3, 0.3,  1)));  IsFalse(t.Contains(new Point3D(0.3, 0.3,  1)));
      IsNull(t.Location(new Point3D(0.3, 0.3, -1)));  IsFalse(t.Contains(new Point3D(0.3, 0.3, -1)));
      IsNull(t.Location(new Point3D(-1,  0,    0)));  IsFalse(t.Contains(new Point3D(-1,  0,    0)));
      IsNull(t.Location(new Point3D(2,   2,    0)));  IsFalse(t.Contains(new Point3D(2,   2,    0)));
    });

    Test("Location_RoundTrip_A_And_B", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));

      // round-trip A: Interpolate(Location(p)) == p
      var p  = new Point3D(0.5, 0.5, 0);
      var st = t.Location(p);
      NotNull(st);
      var p_back = t.Interpolate(st!.Item1, st.Item2);
      NotNull(p_back);
      IsTrue(p.AlmostEquals(p_back!), "A: Interpolate(Location(p)) must recover p");

      // round-trip B: Location(Interpolate(s,t)) == (s,t)
      double s_in = 0.25, t_in = 0.25;
      var q = t.Interpolate(s_in, t_in);
      NotNull(q);
      IsTrue(t.Contains(q!), "Interpolate result must be inside the triangle");
      var st_q = t.Location(q!);
      NotNull(st_q);
      Eq(s_in, st_q!.Item1);
      Eq(t_in, st_q.Item2);
    });

    Test("Contains_InteriorPoint_True", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
      IsTrue(t.Contains(t.Centroid()!), "centroid must be inside");
      IsTrue(t.Contains(new Point3D(0.1, 0.1, 0)), "near-origin interior");
    });

    Test("Contains_ExteriorPoint_False", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
      IsFalse(t.Contains(new Point3D(-0.5, 0.5, 0)),  "left of triangle");
      IsFalse(t.Contains(new Point3D(0.5, -0.5, 0)),  "below triangle");
      IsFalse(t.Contains(new Point3D(1.5, 1.5, 0)),   "past hypotenuse");
    });

    Test("Contains_OffPlane_False", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
      IsFalse(t.Contains(new Point3D(0.3, 0.3, 1)),  "above plane");
      IsFalse(t.Contains(new Point3D(0.3, 0.3, -1)), "below plane");
    });

    Test("Contains_OnBoundary_True", () => {
      // right triangle P0=(0,0,0), P1=(2,0,0), P2=(0,2,0)
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 2, 0));
      IsTrue(t.Contains(new Point3D(0, 0, 0)),  "vertex P0");
      IsTrue(t.Contains(new Point3D(2, 0, 0)),  "vertex P1");
      IsTrue(t.Contains(new Point3D(0, 2, 0)),  "vertex P2");
      IsTrue(t.Contains(new Point3D(1, 0, 0)),  "base edge midpoint");
      IsTrue(t.Contains(new Point3D(0, 1, 0)),  "left edge midpoint");
      IsTrue(t.Contains(new Point3D(1, 1, 0)),  "hypotenuse midpoint");
      IsFalse(t.Contains(new Point3D(1, 0, 0.01)), "just off-plane");
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      IsTrue(expected.AlmostEquals(Triangle3D.FromWkt("TRIANGLE (0 0 0, 1 0 0, 0 1 0)")));
      IsTrue(expected.AlmostEquals(Triangle3D.FromWkt("TRIANGLE (  0 0 0  ,  1 0 0  ,  0 1 0  )")));
    });

    // ── Triangle3D (additional methods) ──────────────────────────────────────────
    Console.WriteLine("\nTriangle3D (new methods)");

    Test("Perimeter_RightTriangleLegs1x1", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      Eq(1.0 + 1.0 + Math.Sqrt(2), t.Perimeter());
    });

    Test("Centroid_KnownValue", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(3,0,0), new Point3D(0,3,0));
      var c = t.Centroid();
      Eq(1.0, c.X);
      Eq(1.0, c.Y);
      Eq(0.0, c.Z);
    });

    Test("Vertices_ReturnsThreePoints", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      var v = t.Vertices();
      NotNull(v);
      Eq(0.0, v.Item1.X); Eq(0.0, v.Item1.Y); Eq(0.0, v.Item1.Z);
      Eq(1.0, v.Item2.X); Eq(0.0, v.Item2.Y); Eq(0.0, v.Item2.Z);
      Eq(0.0, v.Item3.X); Eq(1.0, v.Item3.Y); Eq(0.0, v.Item3.Z);
    });

    Test("ToPolygon_ReturnsPolygon3D", () => {
      var t    = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      var poly = t.ToPolygon();
      NotNull(poly);
      IsTrue(poly is Polygon3D);
      Eq(3, poly.Size(), 0);
    });

    Test("ToAxis_ReturnsTwoVectors", () => {
      var t    = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,3,0));
      var axis = t.ToAxis();
      NotNull(axis);
      NotNull(axis.Item1);
      NotNull(axis.Item2);
      IsTrue(axis.Item1.Length() > 0, "axis u must have non-zero length");
      IsTrue(axis.Item2.Length() > 0, "axis v must have non-zero length");
    });

    Test("WktRoundTrip", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      IsTrue(t.AlmostEquals(Triangle3D.FromWkt(t.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var t    = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,3,0));
      var path = Path.GetTempFileName();
      t.ToFile(path);
      IsTrue(t.AlmostEquals(Triangle3D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0)).ToString();
      IsTrue(s.Contains("TRIANGLE"), "missing TRIANGLE keyword");
    });

    Test("Equality_SameTriangle_True", () => {
      var a = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      var b = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      IsTrue(a == b);
    });

    Test("Equality_DifferentTriangle_False", () => {
      var a = Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      var b = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      IsFalse(a == b);
    });

    // ── Triangle3D.Intersection (Line / Ray / Segment) ────────────────────────────
    Console.WriteLine("\nTriangle3D (Intersection Line / Ray / Segment)");

    Test("Intersects_Line3D_ThroughInterior_True", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var l = Line3D.Make(new Point3D(0.5, 0.5, -1), new Point3D(0.5, 0.5, 1));
      IsTrue(t.Intersects(l));
    });

    Test("Intersection_Line3D_ReturnsPointInTriangle", () => {
      var t  = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var l  = Line3D.Make(new Point3D(0.5, 0.5, -1), new Point3D(0.5, 0.5, 1));
      var pt = t.Intersection(l) as Point3D;
      NotNull(pt);
      Eq(0.5, pt!.X);
      Eq(0.5, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Line3D_MissPlaneOutsideTriangle_Null", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var l = Line3D.Make(new Point3D(3, 3, -1), new Point3D(3, 3, 1));
      IsFalse(t.Intersects(l));
      IsNull(t.Intersection(l), "line hits plane outside the triangle — no intersection");
    });

    Test("Intersection_Line3D_Coplanar_Null", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var l = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0));  // lies in the triangle's plane
      IsFalse(t.Intersects(l));
      IsNull(t.Intersection(l), "coplanar line — API limitation: returns null");
    });

    Test("Intersects_Ray3D_Down_True", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var r = Ray3D.Make(new Point3D(0.5, 0.5, 4), new Vector3D(0, 0, -1));
      IsTrue(t.Intersects(r));
    });

    Test("Intersection_Ray3D_HitPoint", () => {
      var t  = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var r  = Ray3D.Make(new Point3D(0.5, 0.5, 4), new Vector3D(0, 0, -1));
      var pt = t.Intersection(r) as Point3D;
      NotNull(pt);
      Eq(0.5, pt!.X);
      Eq(0.5, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Ray3D_PointingAway_Null", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var r = Ray3D.Make(new Point3D(0.5, 0.5, 4), new Vector3D(0, 0, 1));
      IsNull(t.Intersection(r), "ray pointing away from plane — no intersection");
    });

    Test("Intersects_LineSegment3D_Crossing_True", () => {
      var t   = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var seg = LineSegment3D.Make(new Point3D(0.5, 0.5, -2), new Point3D(0.5, 0.5, 3));
      IsTrue(t.Intersects(seg));
    });

    Test("Intersection_LineSegment3D_HitPoint", () => {
      var t   = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var seg = LineSegment3D.Make(new Point3D(0.5, 0.5, -2), new Point3D(0.5, 0.5, 3));
      var pt  = t.Intersection(seg) as Point3D;
      NotNull(pt);
      Eq(0.5, pt!.X);
      Eq(0.5, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_LineSegment3D_Above_Null", () => {
      var t   = Triangle3D.Make(new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(0,2,0));
      var seg = LineSegment3D.Make(new Point3D(0.5, 0.5, 1), new Point3D(0.5, 0.5, 3));
      IsNull(t.Intersection(seg), "segment entirely above the plane — no intersection");
    });

    Test("Intersects_Plane_True", () => {
      // Use a large enough triangle that plane y=1 cuts through edge interiors (not just vertices).
      var t  = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      var y1 = Plane.FromOriginAndNormal(new Point3D(0,1,0), new Vector3D(0,1,0));
      IsTrue(t.Intersects(y1), "plane y=1 cuts the triangle's interior");
    });

    Test("Intersection_Plane_ReturnsSegmentInBoth", () => {
      var t   = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      var y1  = Plane.FromOriginAndNormal(new Point3D(0,1,0), new Vector3D(0,1,0));
      var seg = t.Intersection(y1) as LineSegment3D;
      NotNull(seg);
      // both endpoints must lie on the triangle and on the plane
      IsTrue(t.Contains(seg!.First()));
      IsTrue(t.Contains(seg.Last()));
      IsTrue(y1.Contains(seg.First()));
      IsTrue(y1.Contains(seg.Last()));
    });

    Test("Intersection_Plane_ParallelAbove_Null", () => {
      var t     = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      var above = Plane.FromOriginAndNormal(new Point3D(0,0,1), new Vector3D(0,0,1));
      IsFalse(t.Intersects(above));
      IsNull(t.Intersection(above), "plane parallel above the triangle — no intersection");
    });

    Test("Intersection_Plane_Coplanar_Null", () => {
      var t = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      // same plane as the triangle — Plane∩Plane returns null (API limitation), so we get null here too
      IsNull(t.Intersection(Plane.XY()), "coplanar plane — API limitation: returns null");
    });

    Test("Intersects_Triangle3D_ParallelAbove_False", () => {
      var t1 = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      var t2 = Triangle3D.Make(new Point3D(0,0,1), new Point3D(1,0,1), new Point3D(0,1,1));
      IsFalse(t1.Intersects(t2));
      IsNull(t1.Intersection(t2));
    });

    Test("Intersection_Triangle3D_InteriorCut_ReturnsSegment", () => {
      // t1 on XY, t2 on plane y=1 — their plane-cut segments overlap on (1,1,0)→(3,1,0).
      var t1  = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      var t2  = Triangle3D.Make(new Point3D(1,1,-1), new Point3D(1,1,1), new Point3D(3,1,0));
      IsTrue(t1.Intersects(t2));
      var seg = t1.Intersection(t2) as LineSegment3D;
      NotNull(seg);
      IsTrue(t1.Contains(seg!.First()));
      IsTrue(t1.Contains(seg.Last()));
      IsTrue(t2.Contains(seg.First()));
      IsTrue(t2.Contains(seg.Last()));
    });
  }
}

}  // namespace GeomPPTests
