using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Line3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Line3D ────────────────────────────────────────────────────────────────────
    Console.WriteLine("\nLine3D");

    Test("Make_TwoPoints", () => NotNull(Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0))));

    Test("Intersects_CrossingLines_True", () => {
      var l1 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var l2 = Line3D.Make(new Point3D(2, -2, 0), new Point3D(2, 2, 0));
      IsTrue(l1.Intersects(l2));
    });

    Test("Intersects_ParallelLines_False", () => {
      var l1 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var l2 = Line3D.Make(new Point3D(0, 1, 0), new Point3D(4, 1, 0));
      IsFalse(l1.Intersects(l2));
    });

    Test("Intersects_SkewLines_False", () => {
      var l1 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var l2 = Line3D.Make(new Point3D(0, 0, 1), new Point3D(0, 4, 1));
      IsFalse(l1.Intersects(l2));
    });

    Test("Intersection_CrossingLines_ReturnsCorrectPoint", () => {
      var pt = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0))
                     .Intersection(Line3D.Make(new Point3D(2, -2, 0), new Point3D(2, 2, 0)));
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("ProjectOnto_PointAboveLine", () => {
      var proj = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0))
                       .ProjectOnto(new Point3D(2, 3, 0));
      Eq(2.0, proj.X);
      Eq(0.0, proj.Y);
      Eq(0.0, proj.Z);
    });

    Test("Contains_PointOnLine", () => {
      IsTrue(Line3D.Make(new Point3D(0, 0, 0), new Point3D(3, 0, 0)).Contains(new Point3D(1, 0, 0)));
    });

    Test("Contains_PointOffLine", () => {
      IsFalse(Line3D.Make(new Point3D(0, 0, 0), new Point3D(3, 0, 0)).Contains(new Point3D(1, 1, 0)));
    });

    Test("DistanceTo_PointAboveLine", () => {
      Eq(3.0, Line3D.Make(new Point3D(0, 0, 0), new Point3D(3, 0, 0)).DistanceTo(new Point3D(0, 3, 0)));
    });

    Test("Line3D_DistanceTo_Line3D_Crossing_IsZero", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var y = Line3D.Make(new Point3D(0, 0, 0), new Point3D(0, 1, 0));
      Eq(0.0, x.DistanceTo(y));
      IsNull(x.Distance(y));
    });

    Test("Line3D_DistanceTo_Line3D_Skew", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var skew = Line3D.Make(new Point3D(0, -1, 5), new Point3D(0, 1, 5));
      Eq(5.0, x.DistanceTo(skew));
      var dseg = x.Distance(skew);
      NotNull(dseg);
      Eq(5.0, dseg!.Length());
    });

    Test("Line3D_DistanceTo_Line3D_ParallelDistinct", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var p = Line3D.Make(new Point3D(0, 3, 0), new Point3D(1, 3, 0));
      Eq(3.0, x.DistanceTo(p));
    });

    Test("Line3D_DistanceTo_Line3D_Overlap_IsZero", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var overlap = Line3D.Make(new Point3D(5, 0, 0), new Point3D(7, 0, 0));
      Eq(0.0, x.DistanceTo(overlap));
      IsNull(x.Distance(overlap));
    });

    Test("Line3D_DistanceTo_Ray3D_Crossing_IsZero", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var ray = Ray3D.Make(new Point3D(0, 2, 0), new Vector3D(0, -1, 0));
      Eq(0.0, x.DistanceTo(ray));
    });

    Test("Line3D_DistanceTo_Ray3D_AwayFromLine", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var ray = Ray3D.Make(new Point3D(0, 2, 0), new Vector3D(0, 1, 0));
      Eq(2.0, x.DistanceTo(ray));
    });

    Test("Line3D_DistanceTo_Ray3D_Overlap_IsZero", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var ray = Ray3D.Make(new Point3D(3, 0, 0), new Vector3D(1, 0, 0));
      Eq(0.0, x.DistanceTo(ray));
      IsNull(x.Distance(ray));
    });

    Test("Line3D_DistanceTo_LineSegment3D_Crossing_IsZero", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(0, -1, 0), new Point3D(0, 1, 0));
      Eq(0.0, x.DistanceTo(seg));
    });

    Test("Line3D_DistanceTo_LineSegment3D_Parallel", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(0, 4, 0), new Point3D(3, 4, 0));
      Eq(4.0, x.DistanceTo(seg));
    });

    Test("Line3D_DistanceTo_LineSegment3D_Overlap_IsZero", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(5, 0, 0));
      Eq(0.0, x.DistanceTo(seg));
      IsNull(x.Distance(seg));
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0));
      IsTrue(expected.AlmostEquals(Line3D.FromWkt("LINE (0 0 0,1 1 0)")));
      IsTrue(expected.AlmostEquals(Line3D.FromWkt("LINE (  0 0 0  ,  1  1  0  )")));
    });

    // ── Line3D (additional) ───────────────────────────────────────────────────────
    Console.WriteLine("\nLine3D (additional)");

    Test("AlmostEquals_SameLine_True", () => {
      var l1 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var l2 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      IsTrue(l1.AlmostEquals(l2));
    });

    Test("Intersects_Ray3D_True", () => {
      var l = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var r = Ray3D.Make(new Point3D(2, -2, 0), new Vector3D(0, 1, 0));
      IsTrue(l.Intersects(r));
    });

    Test("Intersects_Seg3D_True", () => {
      var l   = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      IsTrue(l.Intersects(seg));
    });

    Test("Intersection_Ray3D_Hit", () => {
      var l  = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var r  = Ray3D.Make(new Point3D(2, -2, 0), new Vector3D(0, 1, 0));
      var pt = l.Intersection(r);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var l    = Line3D.Make(new Point3D(1, 2, 3), new Point3D(4, 5, 6));
      var path = Path.GetTempFileName();
      l.ToFile(path);
      IsTrue(l.AlmostEquals(Line3D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0)).ToString();
      IsTrue(s.Contains("LINE"), "missing LINE keyword");
    });

    Test("Equality_SameLine_True", () => {
      var l1 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var l2 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      IsTrue(l1 == l2);
    });

    Test("Equality_DifferentLine_False", () => {
      var l1 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var l2 = Line3D.Make(new Point3D(0, 0, 0), new Point3D(0, 1, 0));
      IsFalse(l1 == l2);
    });

    // ── Overlap — Line3D ──────────────────────────────────────────────────────────
    Console.WriteLine("\nLine3D Overlap");

    Test("Overlaps_SameLine3D_True", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var x2 = Line3D.Make(new Point3D(5, 0, 0), new Point3D(9, 0, 0));
      IsTrue(x.Overlaps(x2));
      IsTrue(x.Overlap(x2) is Line3D);
    });

    Test("Overlaps_CollinearRay3D_True", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var r = Ray3D.Make(new Point3D(2, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(x.Overlaps(r));
      IsTrue(x.Overlap(r) is Ray3D);
    });

    Test("Overlaps_CollinearSegment3D_True", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
      var s = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(5, 0, 0));
      IsTrue(x.Overlaps(s));
      IsTrue(x.Overlap(s) is LineSegment3D);
    });

    // ── Touch — Line3D ────────────────────────────────────────────────────────────
    Console.WriteLine("\nLine3D Touch");
    Test("Touches_RayOriginOnLine3D_True", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var r = Ray3D.Make(new Point3D(3, 0, 0), new Vector3D(0, 0, 1));
      IsTrue(x.Touches(r));
      IsTrue(x.Touch(r).AlmostEquals(new Point3D(3, 0, 0)));
    });
    Test("Touches_CollinearRay3D_False", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var r = Ray3D.Make(new Point3D(1, 0, 0), new Vector3D(1, 0, 0));
      IsFalse(x.Touches(r));
      IsNull(x.Touch(r));
    });
  }
}

}  // namespace GeomPPTests
