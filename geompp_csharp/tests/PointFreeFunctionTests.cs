using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class PointFreeFunctionTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── GeomUtil / List<Point3D> extensions ───────────────────────────────────────
    Test("AreCoplanar_XYPoints_True", () => {
      var pts = new List<Point3D> {
        new Point3D(0,0,0), new Point3D(1,0,0),
        new Point3D(1,1,0), new Point3D(0,1,0)
      };
      IsTrue(pts.AreCoplanar(), "all XY-plane points must be coplanar");
    });

    Test("AreCoplanar_NonCoplanar_False", () => {
      var pts = new List<Point3D> {
        new Point3D(0,0,0), new Point3D(1,0,0),
        new Point3D(1,1,0), new Point3D(0,1,1)
      };
      IsFalse(pts.AreCoplanar(), "off-plane point must make them non-coplanar");
    });

    Test("ClosestWorldPlaneTo_XYPoints_ReturnsXY", () => {
      var pts = new List<Point3D> {
        new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0)
      };
      var plane = pts.ClosestWorldPlaneTo();
      IsTrue(plane.Normal().AlmostEquals(new Vector3D(0,0,1)), "XY-plane points → normal must be (0,0,1)");
    });

    Test("AreCCW_CCWSquare_True", () => {
      var pts = new List<Point3D> {
        new Point3D(0,0,0), new Point3D(1,0,0),
        new Point3D(1,1,0), new Point3D(0,1,0)
      };
      IsTrue(pts.AreCCW(), "CCW square must return true");
    });

    Test("AreCCW_CWSquare_False", () => {
      var pts = new List<Point3D> {
        new Point3D(0,0,0), new Point3D(0,1,0),
        new Point3D(1,1,0), new Point3D(1,0,0)
      };
      IsFalse(pts.AreCCW(), "CW square must return false");
    });

    Test("Lerp_Point2D_Midpoint", () => {
      var p0 = new Point2D(0, 0);
      var p1 = new Point2D(10, 20);
      IsTrue(GeomUtil.Lerp(p0, p1, 0.0).AlmostEquals(p0));
      IsTrue(GeomUtil.Lerp(p0, p1, 1.0).AlmostEquals(p1));
      IsTrue(GeomUtil.Lerp(p0, p1, 0.5).AlmostEquals(new Point2D(5, 10)));
    });

    Test("Lerp_Point3D_ExtrapolatesPastEndpoints", () => {
      var p0 = new Point3D(0, 0, 0);
      var p1 = new Point3D(10, 20, 30);
      IsTrue(GeomUtil.Lerp(p0, p1, -1.0).AlmostEquals(new Point3D(-10, -20, -30)));
      IsTrue(GeomUtil.Lerp(p0, p1, 2.0).AlmostEquals(new Point3D(20, 40, 60)));
    });

    Test("Clip_Point2D_OverlappingSquares", () => {
      var clipper = new List<Point2D> { new(0.5,0.5), new(1.5,0.5), new(1.5,1.5), new(0.5,1.5) };
      var subject = new List<Point2D> { new(0,0), new(1,0), new(1,1), new(0,1) };
      var rings = new List<List<Point2D>>(GeomUtil.Clip(clipper, subject));
      IsTrue(rings.Count == 1, "expected 1 ring");
      var poly = Polygon2D.Make(rings[0].ToArray());
      IsTrue(Math.Abs(poly.Area() - 0.25) < 1e-6, "expected area 0.25");
    });

    Test("Clip_Point3D_CoplanarOverlappingSquares", () => {
      var clipper = new List<Point3D> { new(0.5,0.5,0), new(1.5,0.5,0), new(1.5,1.5,0), new(0.5,1.5,0) };
      var subject = new List<Point3D> { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) };
      var rings = new List<List<Point3D>>(GeomUtil.Clip(clipper, subject));
      IsTrue(rings.Count == 1, "expected 1 ring");
      var poly = Polygon3D.Make(rings[0].ToArray());
      IsTrue(Math.Abs(poly.Area() - 0.25) < 1e-6, "expected area 0.25");
    });

    Test("Clip_Point3D_NonCoplanar_Throws", () => {
      var clipper = new List<Point3D> { new(0,0,0), new(0,1,0), new(0,1,1), new(0,0,1) };
      var subject = new List<Point3D> { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) };
      bool threw = false;
      try { var _ = new List<List<Point3D>>(GeomUtil.Clip(clipper, subject)); }
      catch { threw = true; }
      IsTrue(threw, "expected non-coplanar clip to throw");
    });

    Test("Polygon2D_Union_Intersection_Difference_Xor", () => {
      var a = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var b = Polygon2D.Make(new Point2D[] { new(0.5,0.5), new(1.5,0.5), new(1.5,1.5), new(0.5,1.5) });

      IsTrue(a.Intersects(b), "overlapping squares must intersect");

      var union = a.Union(b);
      IsTrue(union.Length == 1, "expected 1 union piece");
      IsTrue(Math.Abs(union[0].Area() - 1.75) < 1e-6, "expected union area 1.75");

      var inter = a.Intersection(b);
      IsTrue(inter.Length == 1, "expected 1 intersection piece");
      IsTrue(Math.Abs(inter[0].Area() - 0.25) < 1e-6, "expected intersection area 0.25");

      var diff = a.Difference(b);
      IsTrue(diff.Length == 1, "expected 1 difference piece");
      IsTrue(Math.Abs(diff[0].Area() - 0.75) < 1e-6, "expected difference area 0.75");

      var xorResult = a.Xor(b);
      IsTrue(xorResult.Length == 1, "expected 1 xor piece (connected octagon with a hole)");
      IsTrue(xorResult[0].HasHoles(), "xor result should have a hole");
    });

    Test("Polygon3D_Union_Difference_Xor_Coplanar", () => {
      var a = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      var b = Polygon3D.Make(new Point3D[] { new(0.5,0.5,0), new(1.5,0.5,0), new(1.5,1.5,0), new(0.5,1.5,0) });

      IsTrue(a.Intersects(b), "overlapping coplanar squares must intersect");

      var union = a.Union(b);
      IsTrue(union.Length == 1 && Math.Abs(union[0].Area() - 1.75) < 1e-6, "expected union area 1.75");

      var diff = a.Difference(b);
      IsTrue(diff.Length == 1 && Math.Abs(diff[0].Area() - 0.75) < 1e-6, "expected difference area 0.75");
    });

    Test("Polygon3D_Union_NotCoplanar_Throws", () => {
      var a = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      var b = Polygon3D.Make(new Point3D[] { new(0,0,0), new(0,1,0), new(0,1,1), new(0,0,1) });
      bool threw = false;
      try { a.Union(b); } catch { threw = true; }
      IsTrue(threw, "expected non-coplanar Union to throw");
    });

    Test("Polygon3D_Intersection_CoplanarReturnsPolygons", () => {
      var a = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      var b = Polygon3D.Make(new Point3D[] { new(0.5,0.5,0), new(1.5,0.5,0), new(1.5,1.5,0), new(0.5,1.5,0) });
      var result = a.Intersection(b);
      NotNull(result, "expected a result");
      var polys = result as Polygon3D[];
      NotNull(polys, "expected an array of Polygon3D");
      IsTrue(polys!.Length == 1 && Math.Abs(polys[0].Area() - 0.25) < 1e-6, "expected 1 polygon, area 0.25");
    });

    Test("Polygon3D_Intersection_PlanesCrossing_ReturnsSegments", () => {
      var a = Polygon3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) });
      var b = Polygon3D.Make(new Point3D[] { new(1,2,3), new(3,2,3), new(3,2,-1), new(1,2,-1) });
      var result = a.Intersection(b);
      NotNull(result, "expected a result");
      var segs = result as LineSegment3D[];
      NotNull(segs, "expected an array of LineSegment3D");
      IsTrue(segs!.Length == 1, "expected 1 chord segment");
    });

    Test("AreCW_CWSquare_True", () => {
      var pts = new List<Point3D> {
        new Point3D(0,0,0), new Point3D(0,1,0),
        new Point3D(1,1,0), new Point3D(1,0,0)
      };
      IsTrue(pts.AreCW(), "CW square must return true");
    });
  }
}

}  // namespace GeomPPTests
