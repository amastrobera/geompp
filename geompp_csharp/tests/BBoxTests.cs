using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class BBoxTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── BBox2D ────────────────────────────────────────────────────────────────────
    Console.WriteLine("\nBBox2D");

    Test("Contains_InsidePoint_True", () => {
      var bb = new BBox2D(new Point2D(0, 0), new Point2D(10, 10));
      IsTrue(bb.Contains(new Point2D(5, 5)));
      IsFalse(bb.Contains(new Point2D(11, 5)));
    });

    // ── BBox3D ────────────────────────────────────────────────────────────────────
    Console.WriteLine("\nBBox3D");

    Test("FromPoints_MinMax", () => {
      var bb = new BBox3D(new Point3D(0, 0, 0), new Point3D(1, 2, 3));
      Eq(0.0, bb.Min().X); Eq(0.0, bb.Min().Y); Eq(0.0, bb.Min().Z);
      Eq(1.0, bb.Max().X); Eq(2.0, bb.Max().Y); Eq(3.0, bb.Max().Z);
    });

    Test("Contains_InsidePoint_True", () => {
      var bb = new BBox3D(new Point3D(0, 0, 0), new Point3D(5, 5, 5));
      IsTrue(bb.Contains(new Point3D(1, 1, 1)));
      IsFalse(bb.Contains(new Point3D(6, 1, 1)));
    });

    Test("FromLineSegment_SpansEndpoints", () => {
      var bb = new BBox3D(LineSegment3D.Make(new Point3D(-1, -2, -3), new Point3D(3, 4, 5)));
      Eq(-1.0, bb.Min().X); Eq(-2.0, bb.Min().Y); Eq(-3.0, bb.Min().Z);
      Eq( 3.0, bb.Max().X); Eq( 4.0, bb.Max().Y); Eq( 5.0, bb.Max().Z);
    });

    Test("FromLineSegment_ReversedEndpoints_SameResult", () => {
      var bb = new BBox3D(LineSegment3D.Make(new Point3D(3, 4, 5), new Point3D(-1, -2, -3)));
      Eq(-1.0, bb.Min().X); Eq(-3.0, bb.Min().Z);
      Eq( 3.0, bb.Max().X); Eq( 5.0, bb.Max().Z);
    });

    Test("FromPolyline_SpansAllKnots", () => {
      var pts = new Point3D[] { new Point3D(0, 5, 1), new Point3D(3, 0, 4), new Point3D(1, 2, 0) };
      var bb = new BBox3D(Polyline3D.Make(pts));
      Eq(0.0, bb.Min().X); Eq(0.0, bb.Min().Y); Eq(0.0, bb.Min().Z);
      Eq(3.0, bb.Max().X); Eq(5.0, bb.Max().Y); Eq(4.0, bb.Max().Z);
    });

    Test("FromPolygon_SpansAllVertices", () => {
      // ZX projection: reversed order so the triangle is CCW
      var pts = new Point3D[] { new Point3D(4, 3, 5), new Point3D(4, 0, 1), new Point3D(0, 0, 1) };
      var bb = new BBox3D(Polygon3D.Make(pts));
      Eq(0.0, bb.Min().X); Eq(1.0, bb.Min().Z);
      Eq(4.0, bb.Max().X); Eq(5.0, bb.Max().Z);
    });

    Test("FromTriangle_SpansAllVertices", () => {
      var bb = new BBox3D(Triangle3D.Make(
        new Point3D(0, 0, 0), new Point3D(2, 0, 0), new Point3D(0, 3, 4)));
      Eq(0.0, bb.Min().X); Eq(0.0, bb.Min().Y); Eq(0.0, bb.Min().Z);
      Eq(2.0, bb.Max().X); Eq(3.0, bb.Max().Y); Eq(4.0, bb.Max().Z);
    });

    // ── BBox2D (additional) ───────────────────────────────────────────────────────
    Console.WriteLine("\nBBox2D (additional)");

    Test("FromPoints_MinMax", () => {
      var bb = new BBox2D(new Point2D(1, 2), new Point2D(5, 6));
      Eq(1.0, bb.Min().X); Eq(2.0, bb.Min().Y);
      Eq(5.0, bb.Max().X); Eq(6.0, bb.Max().Y);
    });

    Test("FromLineSegment_SpansEndpoints", () => {
      var bb = new BBox2D(LineSegment2D.Make(new Point2D(-1,-2), new Point2D(3,4)));
      Eq(-1.0, bb.Min().X); Eq(-2.0, bb.Min().Y);
      Eq( 3.0, bb.Max().X); Eq( 4.0, bb.Max().Y);
    });

    Test("FromPolyline_SpansAllKnots", () => {
      var pl = Polyline2D.Make(new Point2D[] { new(0,5), new(3,0), new(1,2) });
      var bb = new BBox2D(pl);
      Eq(0.0, bb.Min().X); Eq(0.0, bb.Min().Y);
      Eq(3.0, bb.Max().X); Eq(5.0, bb.Max().Y);
    });

    Test("FromPolygon_SpansAllVertices", () => {
      var p  = Polygon2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,3), new(0,3) });
      var bb = new BBox2D(p);
      Eq(0.0, bb.Min().X); Eq(0.0, bb.Min().Y);
      Eq(4.0, bb.Max().X); Eq(3.0, bb.Max().Y);
    });

    Test("FromTriangle_SpansAllVertices", () => {
      var t  = Triangle2D.Make(new Point2D(0,0), new Point2D(4,0), new Point2D(0,3));
      var bb = new BBox2D(t);
      Eq(0.0, bb.Min().X); Eq(0.0, bb.Min().Y);
      Eq(4.0, bb.Max().X); Eq(3.0, bb.Max().Y);
    });

    Test("AlmostEquals_SameBBox_True", () => {
      var a = new BBox2D(new Point2D(0,0), new Point2D(1,1));
      var b = new BBox2D(new Point2D(0,0), new Point2D(1,1));
      IsTrue(a.AlmostEquals(b));
    });

    Test("AlmostEquals_DiffBBox_False", () => {
      var a = new BBox2D(new Point2D(0,0), new Point2D(1,1));
      var b = new BBox2D(new Point2D(0,0), new Point2D(2,2));
      IsFalse(a.AlmostEquals(b));
    });

    Test("ToString_ContainsMinMax", () => {
      var s = new BBox2D(new Point2D(0, 0), new Point2D(1, 1)).ToString();
      IsTrue(s.Contains("BBox2D"), "missing BBox2D label");
    });

    Test("Equality_SameBBox_True", () => {
      var a = new BBox2D(new Point2D(0, 0), new Point2D(1, 1));
      var b = new BBox2D(new Point2D(0, 0), new Point2D(1, 1));
      IsTrue(a == b);
    });

    Test("Equality_DifferentBBox_False", () => {
      var a = new BBox2D(new Point2D(0, 0), new Point2D(1, 1));
      var b = new BBox2D(new Point2D(0, 0), new Point2D(2, 2));
      IsFalse(a == b);
    });

    // ── BBox3D (additional) ───────────────────────────────────────────────────────
    Console.WriteLine("\nBBox3D (additional)");

    Test("AlmostEquals_SameBBox_True", () => {
      var a = new BBox3D(new Point3D(0,0,0), new Point3D(1,2,3));
      var b = new BBox3D(new Point3D(0,0,0), new Point3D(1,2,3));
      IsTrue(a.AlmostEquals(b));
    });

    Test("AlmostEquals_DiffBBox_False", () => {
      var a = new BBox3D(new Point3D(0,0,0), new Point3D(1,1,1));
      var b = new BBox3D(new Point3D(0,0,0), new Point3D(2,2,2));
      IsFalse(a.AlmostEquals(b));
    });

    Test("ToString_ContainsMinMax", () => {
      var s = new BBox3D(new Point3D(0, 0, 0), new Point3D(1, 1, 1)).ToString();
      IsTrue(s.Contains("BBox3D"), "missing BBox3D label");
    });

    Test("Equality_SameBBox_True", () => {
      var a = new BBox3D(new Point3D(0, 0, 0), new Point3D(1, 2, 3));
      var b = new BBox3D(new Point3D(0, 0, 0), new Point3D(1, 2, 3));
      IsTrue(a == b);
    });

    Test("Equality_DifferentBBox_False", () => {
      var a = new BBox3D(new Point3D(0, 0, 0), new Point3D(1, 1, 1));
      var b = new BBox3D(new Point3D(0, 0, 0), new Point3D(2, 2, 2));
      IsFalse(a == b);
    });
  }
}

}  // namespace GeomPPTests
