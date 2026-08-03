using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Triangle2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Triangle2D (Contains, Interpolate) ────────────────────────────────────────
    Console.WriteLine("\nTriangle2D (additional)");

    Test("Contains_PointInside_True", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      IsTrue(t.Contains(new Point2D(1, 1)));
    });

    Test("Contains_PointOutside_False", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      IsFalse(t.Contains(new Point2D(5, 5)));
    });

    Test("Contains_OnBoundary_True", () => {
      // right triangle P0=(0,0), P1=(4,0), P2=(0,3)
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      IsTrue(t.Contains(new Point2D(0,   0)),   "vertex P0");
      IsTrue(t.Contains(new Point2D(4,   0)),   "vertex P1");
      IsTrue(t.Contains(new Point2D(0,   3)),   "vertex P2");
      IsTrue(t.Contains(new Point2D(2,   0)),   "base edge midpoint");
      IsTrue(t.Contains(new Point2D(0,   1.5)), "left edge midpoint");
      IsTrue(t.Contains(new Point2D(2,   1.5)), "hypotenuse midpoint");
      IsFalse(t.Contains(new Point2D(2,  -0.01)), "just below base");
      IsFalse(t.Contains(new Point2D(-0.01, 1.5)), "just left of edge");
    });

    Test("Interpolate_AtP0_ReturnsP0", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      var p = t.Interpolate(0.0, 0.0);
      NotNull(p);
      Eq(0.0, p!.X);
      Eq(0.0, p.Y);
    });

    Test("Interpolate_AtP1_ReturnsP1", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      var p = t.Interpolate(1.0, 0.0);
      NotNull(p);
      Eq(4.0, p!.X);
      Eq(0.0, p.Y);
    });

    Test("Interpolate_OutsideTriangle_ReturnsNull", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(4, 0), new Point2D(0, 3));
      IsNull(t.Interpolate(0.8, 0.8));
    });

    // ── Triangle2D ────────────────────────────────────────────────────────────────
    Console.WriteLine("\nTriangle2D");

    Test("IsCCW_CCW_ReturnsTrue_2D", () => {
      var t = Triangle2D.Make(new Point2D(-1, 1), new Point2D(0, -1), new Point2D(1, 1));
      IsTrue(t.IsCCW(), "CCW triangle should return true");
      IsTrue(t.SignedArea() > 0, "CCW triangle should have positive signed area");
      IsTrue(t.IsCCW() == (t.SignedArea() > 0), "IsCCW must match sign of SignedArea");
    });

    Test("IsCCW_CW_ReturnsFalse_2D", () => {
      var t = Triangle2D.Make(new Point2D(-1, 1), new Point2D(1, 1), new Point2D(0, -1));
      IsFalse(t.IsCCW(), "CW triangle should return false");
      IsTrue(t.SignedArea() < 0, "CW triangle should have negative signed area");
      IsTrue(t.IsCCW() == (t.SignedArea() > 0), "IsCCW must match sign of SignedArea");
    });

    Test("Location_Vertices_ReturnExpectedCoords_2D", () => {
      var t = Triangle2D.Make(new Point2D(0, -1), new Point2D(1, 0), new Point2D(-1, 0));
      var p0 = new Point2D(0, -1);
      var st0 = t.Location(p0);  NotNull(st0);  Eq(0.0, st0!.Item1);  Eq(0.0, st0.Item2);  IsTrue(t.Contains(p0));
      var p1 = new Point2D(1, 0);
      var st1 = t.Location(p1);  NotNull(st1);  Eq(1.0, st1!.Item1);  Eq(0.0, st1.Item2);  IsTrue(t.Contains(p1));
      var p2 = new Point2D(-1, 0);
      var st2 = t.Location(p2);  NotNull(st2);  Eq(0.0, st2!.Item1);  Eq(1.0, st2.Item2);  IsTrue(t.Contains(p2));
    });

    Test("Location_Centroid_OneThirdEach_2D", () => {
      var t  = Triangle2D.Make(new Point2D(0, -1), new Point2D(1, 0), new Point2D(-1, 0));
      var c  = t.Centroid()!;
      var st = t.Location(c);
      NotNull(st);
      Eq(1.0 / 3.0, st!.Item1);
      Eq(1.0 / 3.0, st.Item2);
      IsTrue(t.Contains(c));
    });

    Test("Location_NullImpliesNotContained_2D", () => {
      var t = Triangle2D.Make(new Point2D(0, -1), new Point2D(1, 0), new Point2D(-1, 0));
      var outside1 = new Point2D(0,  1);
      var outside2 = new Point2D(2,  0);
      var outside3 = new Point2D(0, -2);
      IsNull(t.Location(outside1)); IsFalse(t.Contains(outside1), "above base: location null → contains false");
      IsNull(t.Location(outside2)); IsFalse(t.Contains(outside2), "right of P1: location null → contains false");
      IsNull(t.Location(outside3)); IsFalse(t.Contains(outside3), "below P0: location null → contains false");
    });

    Test("Location_RoundTrip_A_And_B_2D", () => {
      var t = Triangle2D.Make(new Point2D(0, -1), new Point2D(1, 0), new Point2D(-1, 0));
      // Round-trip A: Location → Interpolate recovers original point
      var p     = new Point2D(0, -0.5);
      var st    = t.Location(p);
      NotNull(st);
      IsTrue(t.Contains(p));
      var p_back = t.Interpolate(st!.Item1, st.Item2);
      NotNull(p_back);
      IsTrue(p.AlmostEquals(p_back!), "round-trip A must recover original point");
      // Round-trip B: Interpolate → Location recovers original (s,t)
      double s_in = 0.25, t_in = 0.25;
      var q = t.Interpolate(s_in, t_in);
      NotNull(q);
      IsTrue(t.Contains(q!));
      var st_q = t.Location(q!);
      NotNull(st_q);
      Eq(s_in, st_q!.Item1);
      Eq(t_in, st_q.Item2);
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      IsTrue(expected.AlmostEquals(Triangle2D.FromWkt("TRIANGLE (0 0, 1 0, 0 1)")));
      IsTrue(expected.AlmostEquals(Triangle2D.FromWkt("TRIANGLE (  0 0  ,  1 0  ,  0 1  )")));
    });

    // ── Triangle2D (additional methods) ──────────────────────────────────────────
    Console.WriteLine("\nTriangle2D (new methods)");

    Test("Area_RightTriangle", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      Eq(6.0, t.Area());
    });

    Test("Perimeter_RightTriangle", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      Eq(12.0, t.Perimeter());
    });

    Test("Centroid_KnownValue", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(3,0), new Point2D(0,3));
      var c = t.Centroid();
      Eq(1.0, c.X);
      Eq(1.0, c.Y);
    });

    Test("Vertices_ReturnsThreePoints", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(1,0), new Point2D(0,1));
      var v = t.Vertices();
      NotNull(v);
      Eq(0.0, v.Item1.X); Eq(0.0, v.Item1.Y);
      Eq(1.0, v.Item2.X); Eq(0.0, v.Item2.Y);
      Eq(0.0, v.Item3.X); Eq(1.0, v.Item3.Y);
    });

    Test("ToPolygon_ReturnsPolygon2D", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(1,0), new Point2D(0,1));
      var poly = t.ToPolygon();
      NotNull(poly);
      IsTrue(poly is Polygon2D);
      Eq(3, poly.Size(), 0);
    });

    Test("ToAxis_ReturnsTwoVectors", () => {
      var t    = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var axis = t.ToAxis();
      NotNull(axis);
      NotNull(axis.Item1);
      NotNull(axis.Item2);
      IsTrue(axis.Item1.Length() > 0, "axis u must have non-zero length");
      IsTrue(axis.Item2.Length() > 0, "axis v must have non-zero length");
    });

    Test("WktRoundTrip", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      IsTrue(t.AlmostEquals(Triangle2D.FromWkt(t.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var t    = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var path = Path.GetTempFileName();
      t.ToFile(path);
      IsTrue(t.AlmostEquals(Triangle2D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3)).ToString();
      IsTrue(s.Contains("TRIANGLE"), "missing TRIANGLE keyword");
    });

    Test("Equality_SameTriangle_True", () => {
      var a = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var b = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      IsTrue(a == b);
    });

    Test("Equality_DifferentTriangle_False", () => {
      var a = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var b = Triangle2D.Make(new Point2D(0,0), new Point2D(1,0), new Point2D(0,1));
      IsFalse(a == b);
    });

    // Intersects/Intersection(Line2D/Ray2D/LineSegment2D/Triangle2D) -- previously bound with only the
    // Line2D overload; Ray2D/LineSegment2D/Triangle2D were missing entirely (Triangle3D had all 5).
    Test("Intersects_Line2D_CutsInterior_True", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var l = Line2D.Make(new Point2D(-1, 1), new Point2D(5, 1));
      IsTrue(t.Intersects(l));
      NotNull(t.Intersection(l));
    });

    Test("Intersects_Ray2D_FromInside_ReturnsPoint", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var r = Ray2D.Make(new Point2D(0.5, 0.5), Vector2D.BasisX());
      IsTrue(t.Intersects(r));
      var hit = t.Intersection(r);
      IsTrue(hit is Point2D, "expected a Point2D result");
    });

    Test("Intersects_Ray2D_PointingAway_False", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var r = Ray2D.Make(new Point2D(-3, 0), -Vector2D.BasisX());
      IsFalse(t.Intersects(r));
      IsNull(t.Intersection(r));
    });

    Test("Intersects_LineSegment2D_CutsBothSides_ReturnsSegment", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var s = LineSegment2D.Make(new Point2D(-3, 1), new Point2D(3, 1));
      IsTrue(t.Intersects(s));
      var hit = t.Intersection(s);
      IsTrue(hit is LineSegment2D, "expected a LineSegment2D result");
    });

    Test("Intersects_LineSegment2D_Outside_False", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var s = LineSegment2D.Make(new Point2D(-3, 1), new Point2D(-1, 1));
      IsFalse(t.Intersects(s));
      IsNull(t.Intersection(s));
    });

    Test("Intersects_Triangle2D_Overlapping_ReturnsTriangleWithPositiveArea", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var other = Triangle2D.Make(new Point2D(1,1), new Point2D(5,1), new Point2D(1,4));
      IsTrue(t.Intersects(other));
      var hit = t.Intersection(other) as Triangle2D;
      NotNull(hit);
      IsTrue(hit!.Area() > 0);
    });

    Test("Intersects_Triangle2D_Disjoint_False", () => {
      var t = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var far = Triangle2D.Make(new Point2D(100,100), new Point2D(104,100), new Point2D(100,104));
      IsFalse(t.Intersects(far));
      IsNull(t.Intersection(far));
    });
  }
}

}  // namespace GeomPPTests
