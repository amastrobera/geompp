using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Ray3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Ray3D ─────────────────────────────────────────────────────────────────────
    Console.WriteLine("\nRay3D");

    Test("Contains_PointOnRay", () => {
      IsTrue(Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0)).Contains(new Point3D(3, 0, 0)));
    });

    Test("Contains_PointOffRay_False", () => {
      IsFalse(Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0)).Contains(new Point3D(3, 1, 0)));
    });

    Test("Contains_PointBehindOrigin_False", () => {
      IsFalse(Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0)).Contains(new Point3D(-1, 0, 0)));
    });

    Test("DistanceTo_PointAboveRay", () => {
      Eq(4.0, Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0)).DistanceTo(new Point3D(3, 4, 0)));
    });

    Test("DistanceTo_PointBehindOrigin", () => {
      Eq(2.0, Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0)).DistanceTo(new Point3D(-2, 0, 0)));
    });

    Test("Ray3D_DistanceTo_Line3D_Crossing_IsZero", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var line = Line3D.Make(new Point3D(0, -1, 0), new Point3D(0, 1, 0));
      Eq(0.0, ray.DistanceTo(line));
    });

    Test("Ray3D_DistanceTo_Line3D_Parallel", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var line = Line3D.Make(new Point3D(0, 5, 0), new Point3D(1, 5, 0));
      Eq(5.0, ray.DistanceTo(line));
    });

    Test("Ray3D_DistanceTo_Line3D_Overlap_IsZero", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var line = Line3D.Make(new Point3D(-2, 0, 0), new Point3D(7, 0, 0));
      Eq(0.0, ray.DistanceTo(line));
      IsNull(ray.Distance(line));
    });

    Test("Ray3D_DistanceTo_Ray3D_Crossing_IsZero", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(3, 1, 0), new Vector3D(0, -1, 0));
      Eq(0.0, r1.DistanceTo(r2));
    });

    Test("Ray3D_DistanceTo_Ray3D_Skew", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(0, 0, 4), new Vector3D(0, 1, 0));
      Eq(4.0, r1.DistanceTo(r2));
    });

    Test("Ray3D_DistanceTo_Ray3D_Overlap_IsZero", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(2, 0, 0), new Vector3D(1, 0, 0));
      Eq(0.0, r1.DistanceTo(r2));
      IsNull(r1.Distance(r2));
    });

    Test("Ray3D_DistanceTo_Ray3D_Parallel", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(0, 3, 0), new Vector3D(1, 0, 0));
      Eq(3.0, r1.DistanceTo(r2));
    });

    Test("Ray3D_DistanceTo_LineSegment3D_Crossing_IsZero", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      Eq(0.0, ray.DistanceTo(seg));
    });

    Test("Ray3D_DistanceTo_LineSegment3D_Parallel", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(0, 0, 5), new Point3D(4, 0, 5));
      Eq(5.0, ray.DistanceTo(seg));
    });

    Test("Ray3D_DistanceTo_LineSegment3D_Overlap_IsZero", () => {
      var ray = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(1, 0, 0), new Point3D(3, 0, 0));
      Eq(0.0, ray.DistanceTo(seg));
      IsNull(ray.Distance(seg));
    });

    Test("FromWkt_Whitespace", () => {
      var expected = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(expected.AlmostEquals(Ray3D.FromWkt("RAY (0 0 0,1 0 0)")));
      IsTrue(expected.AlmostEquals(Ray3D.FromWkt("RAY (  0 0 0  ,  1  0  0  )")));
    });

    // ── Ray3D (additional) ────────────────────────────────────────────────────────
    Console.WriteLine("\nRay3D (additional)");

    Test("IsAhead_PointInFront_True", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(r.IsAhead(new Point3D(5, 0, 0)));
    });

    Test("IsAhead_PointBehind_False", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsFalse(r.IsAhead(new Point3D(-1, 0, 0)));
    });

    Test("IsBehind_PointBehind_True", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(r.IsBehind(new Point3D(-1, 0, 0)));
    });

    Test("IsBehind_PointInFront_False", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsFalse(r.IsBehind(new Point3D(5, 0, 0)));
    });

    Test("Intersects_Line3D_True", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var l = Line3D.Make(new Point3D(2, -2, 0), new Point3D(2, 2, 0));
      IsTrue(r.Intersects(l));
    });

    Test("Intersects_Ray3D_True", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(2, -2, 0), new Vector3D(0, 1, 0));
      IsTrue(r1.Intersects(r2));
    });

    Test("Intersects_Ray3D_False", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(-1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(4, 0, 0), new Vector3D(1, 0, 0));
      IsFalse(r1.Intersects(r2));
    });

    Test("Intersects_Seg3D_True", () => {
      var r   = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      IsTrue(r.Intersects(seg));
    });

    Test("Intersection_Line3D_Hit", () => {
      var r  = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var l  = Line3D.Make(new Point3D(2, -2, 0), new Point3D(2, 2, 0));
      var pt = r.Intersection(l);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Ray3D_Hit", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(2, -2, 0), new Vector3D(0, 1, 0));
      var pt = r1.Intersection(r2);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("Intersection_Ray3D_Miss", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(-1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(4, 0, 0), new Vector3D(1, 0, 0));
      IsNull(r1.Intersection(r2));
    });

    Test("Intersection_Seg3D_Hit", () => {
      var r   = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
      var pt  = r.Intersection(seg);
      NotNull(pt);
      Eq(2.0, pt!.X);
      Eq(0.0, pt.Y);
      Eq(0.0, pt.Z);
    });

    Test("ToLine_ReturnsLine3D", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      NotNull(r.ToLine());
      IsTrue(r.ToLine() is Line3D);
    });

    Test("AlmostEquals_SameRay_True", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(r1.AlmostEquals(r2));
    });

    Test("WktRoundTrip", () => {
      var r = Ray3D.Make(new Point3D(1, 2, 3), new Vector3D(1, 0, 0));
      IsTrue(r.AlmostEquals(Ray3D.FromWkt(r.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var r    = Ray3D.Make(new Point3D(1, 2, 3), new Vector3D(1, 0, 0));
      var path = Path.GetTempFileName();
      r.ToFile(path);
      IsTrue(r.AlmostEquals(Ray3D.FromFile(path)));
      File.Delete(path);
    });

    Test("ToString_ContainsWkt", () => {
      var s = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0)).ToString();
      IsTrue(s.Contains("RAY"), "missing RAY keyword");
    });

    Test("Equality_SameRay_True", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(r1 == r2);
    });

    Test("Equality_DifferentRay_False", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(0, 1, 0));
      IsFalse(r1 == r2);
    });

    // ── Overlap — Ray3D ───────────────────────────────────────────────────────────
    Console.WriteLine("\nRay3D Overlap");

    Test("Overlaps_SameDirRay3D_ReturnsRay", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(2, 0, 0), new Vector3D(1, 0, 0));
      IsTrue(r1.Overlaps(r2));
      IsTrue(r1.Overlap(r2) is Ray3D);
    });

    Test("Overlaps_AntiParallelRay3D_ReturnsSegment", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r3 = Ray3D.Make(new Point3D(5, 0, 0), new Vector3D(-1, 0, 0));
      IsTrue(r1.Overlaps(r3));
      IsTrue(r1.Overlap(r3) is LineSegment3D);
    });

    Test("Overlaps_TouchingAntiParallelRay3D_False", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r4 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(-1, 0, 0));
      IsFalse(r1.Overlaps(r4));
      IsNull(r1.Overlap(r4));
    });

    // ── Touch — Ray3D ─────────────────────────────────────────────────────────────
    Console.WriteLine("\nRay3D Touch");
    Test("Touches_Line3DAtOrigin_True", () => {
      var x = Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
      var r = Ray3D.Make(new Point3D(2, 0, 0), new Vector3D(0, 0, 1));
      IsTrue(r.Touches(x));
      IsTrue(r.Touch(x).AlmostEquals(new Point3D(2, 0, 0)));
    });
    Test("Touches_Ray3DRaySameOrigin_True", () => {
      var r1 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var r2 = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(0, 0, 1));
      IsTrue(r1.Touches(r2));
      IsTrue(r1.Touch(r2).AlmostEquals(new Point3D(0, 0, 0)));
    });
    Test("Touches_Ray3DSegmentEndpoint_True", () => {
      var r = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
      var s = LineSegment3D.Make(new Point3D(3, 0, 0), new Point3D(3, 0, 2));
      IsTrue(r.Touches(s));
      IsTrue(r.Touch(s).AlmostEquals(new Point3D(3, 0, 0)));
    });
  }
}

}  // namespace GeomPPTests
