using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class PlaneTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Plane (DistanceTo, ProjectOnto) ───────────────────────────────────────────
    Console.WriteLine("\nPlane (additional)");

    Test("DistanceTo_PointAbovePlane", () => {
      Eq(3.0, Plane.XY().DistanceTo(new Point3D(0, 0, 3)));
    });

    Test("ProjectOnto_PointAbovePlane", () => {
      var proj = Plane.XY().ProjectOnto(new Point3D(1, 2, 5));
      Eq(1.0, proj.X);
      Eq(2.0, proj.Y);
      Eq(0.0, proj.Z);
    });

    // ── Plane (additional) ────────────────────────────────────────────────────────
    Console.WriteLine("\nPlane (additional)");

    Test("XY_NormalIsZ", () => {
      var pl = Plane.XY();
      Eq(0.0, pl.Normal().X);
      Eq(0.0, pl.Normal().Y);
      Eq(1.0, pl.Normal().Z);
    });

    Test("YZ_NormalIsX", () => {
      var pl = Plane.YZ();
      Eq(1.0, Math.Abs(pl.Normal().X));
      Eq(0.0, pl.Normal().Y);
      Eq(0.0, pl.Normal().Z);
    });

    Test("ZX_NormalIsY", () => {
      var pl = Plane.ZX();
      Eq(0.0, pl.Normal().X);
      Eq(1.0, Math.Abs(pl.Normal().Y));
      Eq(0.0, pl.Normal().Z);
    });

    Test("FromOriginAndNormal_NormalMatchesInput", () => {
      var pl = Plane.FromOriginAndNormal(new Point3D(0,0,5), new Vector3D(0,0,1));
      Eq(0.0, pl.Normal().X);
      Eq(0.0, pl.Normal().Y);
      Eq(1.0, pl.Normal().Z);
    });

    Test("FromOriginAndAxes_AxesMatchInput", () => {
      var pl = Plane.FromOriginAndAxes(new Point3D(0,0,0), new Vector3D(1,0,0), new Vector3D(0,1,0));
      Eq(1.0, pl.AxisU().X);
      Eq(0.0, pl.AxisU().Y);
      Eq(0.0, pl.AxisV().X);
      Eq(1.0, pl.AxisV().Y);
    });

    Test("From3Points_NormalPointsInZ", () => {
      var pl = Plane.From3Points(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0));
      Eq(1.0, Math.Abs(pl.Normal().Z));
    });

    Test("Contains_PointOnPlane_True", () => {
      var pl = Plane.XY();
      IsTrue(pl.Contains(new Point3D(3, 4, 0)));
    });

    Test("Contains_PointOffPlane_False", () => {
      var pl = Plane.XY();
      IsFalse(pl.Contains(new Point3D(3, 4, 1)));
    });

    Test("SignedDistanceTo_PositiveSide", () => {
      Eq(2.0, Plane.XY().SignedDistanceTo(new Point3D(0,0,2)));
    });

    Test("SignedDistanceTo_NegativeSide", () => {
      IsTrue(Plane.XY().SignedDistanceTo(new Point3D(0,0,-3)) < 0, "point below XY should be negative");
    });

    Test("Intersects_Line3D_True", () => {
      var pl = Plane.XY();
      var l  = Line3D.Make(new Point3D(0,0,-1), new Point3D(0,0,1));
      IsTrue(pl.Intersects(l));
    });

    Test("Intersection_Line3D_Hit", () => {
      var pl = Plane.XY();
      var l  = Line3D.Make(new Point3D(0,0,-1), new Point3D(0,0,1));
      var pt = pl.Intersection(l);
      NotNull(pt);
      Eq(0.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("AxisU_IsUnitLength", () => {
      Eq(1.0, Plane.XY().AxisU().Length());
    });

    Test("AxisV_IsUnitLength", () => {
      Eq(1.0, Plane.XY().AxisV().Length());
    });

    Test("ProjectInto_ReturnsPoint2D_UVCoords", () => {
      // XY plane: origin (0,0,0), u=X, v=Y
      // A point (3,4,0) on the XY plane should project to UV=(3,4)
      var pl = Plane.XY();
      var uv = pl.ProjectInto(new Point3D(3, 4, 0));
      NotNull(uv);
      Eq(3.0, uv.X);
      Eq(4.0, uv.Y);
    });

    Test("Evaluate_UV_ReturnsPoint3D", () => {
      // XY plane: Evaluate(3,4) in UV should return (3,4,0) in 3D
      var pl = Plane.XY();
      var pt = pl.Evaluate(new Point2D(3, 4));
      NotNull(pt);
      Eq(3.0, pt.X);
      Eq(4.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("AlmostEquals_SamePlane_True", () => {
      IsTrue(Plane.XY().AlmostEquals(Plane.XY()));
    });

    Test("AlmostEquals_DiffPlane_False", () => {
      IsFalse(Plane.XY().AlmostEquals(Plane.YZ()));
    });

    // ── Plane.Intersection / Intersects with Ray3D, LineSegment3D, Plane, Triangle3D ──

    Test("Intersects_Ray3D_True", () => {
      var pl = Plane.XY();
      var r  = Ray3D.Make(new Point3D(5, 3, 4), new Vector3D(0, 0, -1));
      IsTrue(pl.Intersects(r));
    });

    Test("Intersection_Ray3D_HitPoint", () => {
      var pt = Plane.XY().Intersection(Ray3D.Make(new Point3D(5, 3, 4), new Vector3D(0, 0, -1)));
      NotNull(pt);
      Eq(5.0, pt!.X);
      Eq(3.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Ray3D_PointingAway_Null", () => {
      var pt = Plane.XY().Intersection(Ray3D.Make(new Point3D(5, 3, 4), new Vector3D(0, 0, 1)));
      IsNull(pt, "ray pointing away from plane should not intersect");
    });

    Test("Intersects_LineSegment3D_True", () => {
      var pl  = Plane.XY();
      var seg = LineSegment3D.Make(new Point3D(5, 3, -2), new Point3D(5, 3, 4));
      IsTrue(pl.Intersects(seg));
    });

    Test("Intersection_LineSegment3D_HitPoint", () => {
      var pt = Plane.XY().Intersection(
        LineSegment3D.Make(new Point3D(5, 3, -2), new Point3D(5, 3, 4)));
      NotNull(pt);
      Eq(5.0, pt!.X);
      Eq(3.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_LineSegment3D_Above_Null", () => {
      var pt = Plane.XY().Intersection(
        LineSegment3D.Make(new Point3D(0, 0, 1), new Point3D(1, 1, 2)));
      IsNull(pt, "segment entirely above plane should not intersect");
    });

    Test("Intersects_Plane_True", () => {
      IsTrue(Plane.XY().Intersects(Plane.YZ()));
    });

    Test("Intersection_Plane_ReturnsLineInBothPlanes", () => {
      var line = Plane.XY().Intersection(Plane.YZ());
      NotNull(line);
      // origin must lie on both planes (z=0, x=0)
      Eq(0.0, line!.Origin().X);
      Eq(0.0, line.Origin().Z);
      // direction parallel to Y-axis
      Eq(1.0, Math.Abs(line.Direction().Y));
      Eq(0.0, line.Direction().X);
      Eq(0.0, line.Direction().Z);
    });

    Test("Intersection_Plane_Parallel_Null", () => {
      var p1 = Plane.FromOriginAndNormal(new Point3D(0, 0, 0), new Vector3D(0, 0, 1));
      var p2 = Plane.FromOriginAndNormal(new Point3D(0, 0, 5), new Vector3D(0, 0, 1));
      IsNull(p1.Intersection(p2), "parallel distinct planes should not intersect");
    });

    Test("Intersection_Triangle3D_ReturnsSegment", () => {
      // Plane.Intersection(Triangle3D) now delegates to Triangle3D.Intersection(Plane).
      // Use plane y=1 so the cut goes through edge interiors (avoids the all-vertices nullopt rule).
      var y1  = Plane.FromOriginAndNormal(new Point3D(0,1,0), new Vector3D(0,1,0));
      var tri = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      IsTrue(y1.Intersects(tri));
      var seg = y1.Intersection(tri) as LineSegment3D;
      NotNull(seg);
      IsTrue(y1.Contains(seg!.First()));
      IsTrue(y1.Contains(seg.Last()));
      IsTrue(tri.Contains(seg.First()));
      IsTrue(tri.Contains(seg.Last()));
    });

    Test("Intersection_Triangle3D_ParallelPlane_Null", () => {
      var above = Plane.FromOriginAndNormal(new Point3D(0,0,1), new Vector3D(0,0,1));
      var tri   = Triangle3D.Make(new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(0,4,0));
      IsFalse(above.Intersects(tri));
      IsNull(above.Intersection(tri));
    });

    // ── Plane.IsParallel / IsCoplanar ────────────────────────────────────────────

    Test("IsParallel_LineInPlane_True", () => {
      var line = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0));
      IsTrue(Plane.XY().IsParallel(line));
    });

    Test("IsParallel_LinePerpendicular_False", () => {
      var line = Line3D.Make(new Point3D(0, 0, 0), new Point3D(0, 0, 1));
      IsFalse(Plane.XY().IsParallel(line));
    });

    Test("IsParallel_Ray_True", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 3), new Vector3D(1, 0, 0));
      IsTrue(Plane.XY().IsParallel(ray));
    });

    Test("IsParallel_LineSegment_True", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 3), new Point3D(5, 0, 3));
      IsTrue(Plane.XY().IsParallel(seg));
    });

    Test("IsCoplanar_LineInPlane_True", () => {
      var line = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0));
      IsTrue(Plane.XY().IsCoplanar(line));
    });

    Test("IsCoplanar_LineAbovePlane_False", () => {
      var line = Line3D.Make(new Point3D(0, 0, 2), new Point3D(1, 0, 2));
      IsFalse(Plane.XY().IsCoplanar(line));
    });

    Test("IsCoplanar_Ray_InPlane_True", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(Plane.XY().IsCoplanar(ray));
    });

    Test("IsCoplanar_LineSegment_AbovePlane_False", () => {
      var seg = LineSegment3D.Make(new Point3D(0, 0, 2), new Point3D(5, 0, 2));
      IsFalse(Plane.XY().IsCoplanar(seg));
    });

    Test("ToString_ContainsOriginAndNormal", () => {
      var s = Plane.XY().ToString();
      IsTrue(s.Contains("Plane"), "missing Plane label");
      IsTrue(s.Contains("origin"), "missing origin");
      IsTrue(s.Contains("normal"), "missing normal");
    });

    Test("Equality_SamePlane_True", () => {
      IsTrue(Plane.XY() == Plane.XY());
    });

    Test("Equality_DifferentPlane_False", () => {
      IsFalse(Plane.XY() == Plane.YZ());
    });
  }
}

}  // namespace GeomPPTests
