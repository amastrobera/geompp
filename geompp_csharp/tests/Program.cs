using GeomPP;
using Geompp.Extensions;
using System.IO;

// ── Tiny test harness ─────────────────────────────────────────────────────────

int passed = 0, failed = 0;

void Test(string name, Action body) {
  try {
    body();
    Console.WriteLine($"  PASS  {name}");
    passed++;
  } catch (Exception e) {
    Console.WriteLine($"  FAIL  {name}");
    Console.WriteLine($"        {e.Message}");
    failed++;
  }
}

void Eq(double expected, double actual, int decimals = 3) {
  double tol = 0.5 * Math.Pow(10, -decimals);
  if (Math.Abs(expected - actual) >= tol)
    throw new Exception($"expected {expected}, got {actual} (tolerance {tol})");
}

void IsTrue(bool value, string msg = "expected true") {
  if (!value) throw new Exception(msg);
}

void IsFalse(bool value, string msg = "expected false") {
  if (value) throw new Exception(msg);
}

void NotNull(object? value, string msg = "expected non-null") {
  if (value is null) throw new Exception(msg);
}

void IsNull(object? value, string msg = "expected null") {
  if (value is not null) throw new Exception(msg);
}

// ── Precision ─────────────────────────────────────────────────────────────────
Console.WriteLine("Precision");

Test("DefaultPrecision_IsThree", () => Eq(3, Precision.DecimalPrecision, 0));

Test("SetPrecision_RoundTrips", () => {
  int orig = Precision.DecimalPrecision;
  Precision.DecimalPrecision = Precision.DP_SIX;
  Eq(6, Precision.DecimalPrecision, 0);
  Precision.DecimalPrecision = orig;
});

// ── Point2D ───────────────────────────────────────────────────────────────────
Console.WriteLine("\nPoint2D");

Test("Create_AccessXY", () => {
  var p = new Point2D(3.0, 4.0);
  Eq(3.0, p.X);
  Eq(4.0, p.Y);
});

Test("ToWkt_ContainsCoordinates", () => {
  var wkt = new Point2D(1.0, 2.0).ToWkt();
  IsTrue(wkt.Contains("POINT"), "no POINT keyword");
  IsTrue(wkt.Contains("1"), "no 1");
  IsTrue(wkt.Contains("2"), "no 2");
});

Test("AlmostEquals_SamePoint", () => {
  IsTrue(new Point2D(1.0, 2.0).AlmostEquals(new Point2D(1.0, 2.0)));
});

Test("AlmostEquals_DifferentPoint", () => {
  IsFalse(new Point2D(0.0, 0.0).AlmostEquals(new Point2D(1.0, 0.0)));
});

Test("DistanceTo_KnownValue", () => {
  Eq(5.0, new Point2D(0.0, 0.0).DistanceTo(new Point2D(3.0, 4.0)));
});

Test("Zero_IsOrigin", () => {
  var z = Point2D.Zero();
  Eq(0.0, z.X);
  Eq(0.0, z.Y);
});

Test("CreateFromVector_CopiesComponents", () => {
  var p = new Point2D(new Vector2D(3.0, -4.5));
  Eq(3.0, p.X);
  Eq(-4.5, p.Y);
});

Test("CreateFromVector_RoundtripViaToVector", () => {
  var p0 = new Point2D(1.25, -2.75);
  var p1 = new Point2D(p0.ToVector());
  IsTrue(p0.AlmostEquals(p1));
});

// ── Point3D ───────────────────────────────────────────────────────────────────
Console.WriteLine("\nPoint3D");

Test("Create_AccessXYZ", () => {
  var p = new Point3D(1.0, 2.0, 3.0);
  Eq(1.0, p.X);
  Eq(2.0, p.Y);
  Eq(3.0, p.Z);
});

Test("DistanceTo_KnownValue", () => {
  Eq(1.0, new Point3D(0, 0, 0).DistanceTo(new Point3D(1, 0, 0)));
});

Test("CreateFromVector_CopiesComponents", () => {
  var p = new Point3D(new Vector3D(3.0, -4.5, 6.25));
  Eq(3.0, p.X);
  Eq(-4.5, p.Y);
  Eq(6.25, p.Z);
});

Test("CreateFromVector_RoundtripViaToVector", () => {
  var p0 = new Point3D(1.25, -2.75, 0.5);
  var p1 = new Point3D(p0.ToVector());
  IsTrue(p0.AlmostEquals(p1));
});

// ── Vector3D ──────────────────────────────────────────────────────────────────
Console.WriteLine("\nVector3D");

Test("Create_AccessXYZ", () => {
  var v = new Vector3D(1.0, 2.0, 3.0);
  Eq(1.0, v.X);
  Eq(2.0, v.Y);
  Eq(3.0, v.Z);
});

Test("Length_UnitVector", () => Eq(1.0, new Vector3D(1, 0, 0).Length()));

Test("Dot_PerpendicularVectors_IsZero", () => {
  Eq(0.0, new Vector3D(1, 0, 0).Dot(new Vector3D(0, 1, 0)));
});

Test("Dot_ParallelVectors_IsOne", () => {
  var v = new Vector3D(1, 0, 0);
  Eq(1.0, v.Dot(v));
});

Test("Cross_BasisVectors_IsThird", () => {
  var vz = new Vector3D(1, 0, 0).Cross(new Vector3D(0, 1, 0));
  Eq(0.0, vz.X);
  Eq(0.0, vz.Y);
  Eq(1.0, vz.Z);
});

Test("Normalize_ProducesUnitVector", () => {
  Eq(1.0, new Vector3D(3, 4, 0).Normalize().Length());
});

Test("BasisVectors_AreUnitLength", () => {
  Eq(1.0, Vector3D.BasisX().Length());
  Eq(1.0, Vector3D.BasisY().Length());
  Eq(1.0, Vector3D.BasisZ().Length());
});

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

// ── LineSegment2D ─────────────────────────────────────────────────────────────
Console.WriteLine("\nLineSegment2D");

Test("Make_AndLength", () => {
  Eq(5.0, LineSegment2D.Make(new Point2D(0, 0), new Point2D(3, 4)).Length());
});

Test("Intersects_WithLine", () => {
  var seg = LineSegment2D.Make(new Point2D(1, -1), new Point2D(1, 1));
  var line = Line2D.Make(new Point2D(0, 0), new Point2D(4, 0));
  IsTrue(seg.Intersects(line));
});

Test("Intersection_WithLine_ReturnsCorrectPoint", () => {
  var pt = LineSegment2D.Make(new Point2D(1, -1), new Point2D(1, 1))
                        .Intersection(Line2D.Make(new Point2D(0, 0), new Point2D(4, 0)));
  NotNull(pt);
  Eq(1.0, pt!.X);
  Eq(0.0, pt.Y);
});

Test("Contains_PointOnSegment", () => {
  IsTrue(LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Contains(new Point2D(2, 0)));
});

Test("Contains_PointBeyondEnd_False", () => {
  IsFalse(LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Contains(new Point2D(5, 0)));
});

Test("DistanceTo_PointAboveSegment", () => {
  Eq(3.0, LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).DistanceTo(new Point2D(2, 3)));
});

Test("Location_Midpoint_IsHalf", () => {
  Eq(0.5, LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Location(new Point2D(2, 0)));
});

Test("Interpolate_Midpoint", () => {
  var mid = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0)).Interpolate(0.5);
  NotNull(mid);
  Eq(2.0, mid!.X);
  Eq(0.0, mid.Y);
});

// ── LineSegment3D ─────────────────────────────────────────────────────────────
Console.WriteLine("\nLineSegment3D");

Test("Contains_PointOnSegment", () => {
  IsTrue(LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).Contains(new Point3D(2, 0, 0)));
});

Test("DistanceTo_PointAboveSegment", () => {
  Eq(3.0, LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).DistanceTo(new Point3D(2, 3, 0)));
});

Test("Location_Midpoint_IsHalf", () => {
  Eq(0.5, LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).Location(new Point3D(2, 0, 0)));
});

Test("Interpolate_Midpoint", () => {
  var mid = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0)).Interpolate(0.5);
  NotNull(mid);
  Eq(2.0, mid!.X);
  Eq(0.0, mid.Y);
  Eq(0.0, mid.Z);
});

Test("LineSegment3D_DistanceTo_Line3D_Crossing_IsZero", () => {
  var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var line = Line3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
  Eq(0.0, seg.DistanceTo(line));
});

Test("LineSegment3D_DistanceTo_Line3D_Parallel", () => {
  var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var line = Line3D.Make(new Point3D(0, 3, 0), new Point3D(1, 3, 0));
  Eq(3.0, seg.DistanceTo(line));
});

Test("LineSegment3D_DistanceTo_Line3D_Overlap_IsZero", () => {
  var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var line = Line3D.Make(new Point3D(-2, 0, 0), new Point3D(10, 0, 0));
  Eq(0.0, seg.DistanceTo(line));
  IsNull(seg.Distance(line));
});

Test("LineSegment3D_DistanceTo_Ray3D_Crossing_IsZero", () => {
  var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var ray = Ray3D.Make(new Point3D(2, 3, 0), new Vector3D(0, -1, 0));
  Eq(0.0, seg.DistanceTo(ray));
});

Test("LineSegment3D_DistanceTo_Ray3D_Parallel", () => {
  var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var ray = Ray3D.Make(new Point3D(0, 0, 4), new Vector3D(1, 0, 0));
  Eq(4.0, seg.DistanceTo(ray));
});

Test("LineSegment3D_DistanceTo_Ray3D_Overlap_IsZero", () => {
  var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var ray = Ray3D.Make(new Point3D(1, 0, 0), new Vector3D(1, 0, 0));
  Eq(0.0, seg.DistanceTo(ray));
  IsNull(seg.Distance(ray));
});

// 2D top-down (XY) the ray crosses the segment at (2,0); in 3D ray is at z=5 while
// the segment is at z=0 → non-parallel skew, real distance = 5.
Test("LineSegment3D_DistanceTo_Ray3D_SkewTopCross", () => {
  var seg = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var ray = Ray3D.Make(new Point3D(2, 3, 5), new Vector3D(0, -1, 0));
  Eq(5.0, seg.DistanceTo(ray));
});

Test("LineSegment3D_DistanceTo_LineSegment3D_Crossing_IsZero", () => {
  var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var s2 = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
  Eq(0.0, s1.DistanceTo(s2));
});

Test("LineSegment3D_DistanceTo_LineSegment3D_Parallel", () => {
  var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var s2 = LineSegment3D.Make(new Point3D(0, 3, 0), new Point3D(2, 3, 0));
  Eq(3.0, s1.DistanceTo(s2));
});

Test("LineSegment3D_DistanceTo_LineSegment3D_Overlap_IsZero", () => {
  var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var s2 = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(6, 0, 0));
  Eq(0.0, s1.DistanceTo(s2));
  IsNull(s1.Distance(s2));
});

Test("LineSegment3D_DistanceTo_LineSegment3D_Skew", () => {
  var s1 = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var s2 = LineSegment3D.Make(new Point3D(2, -1, 5), new Point3D(2, 1, 5));
  Eq(5.0, s1.DistanceTo(s2));
});

// ── Ray2D ─────────────────────────────────────────────────────────────────────
Console.WriteLine("\nRay2D");

Test("Contains_PointOnRay", () => {
  IsTrue(Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0)).Contains(new Point2D(3, 0)));
});

Test("Contains_PointBehindOrigin_False", () => {
  IsFalse(Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0)).Contains(new Point2D(-1, 0)));
});

Test("DistanceTo_PointAboveRay", () => {
  Eq(4.0, Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0)).DistanceTo(new Point2D(3, 4)));
});

Test("DistanceTo_PointBehindOrigin", () => {
  var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  Eq(new Point2D(0, 0).DistanceTo(new Point2D(-5, 10)), r.DistanceTo(new Point2D(-5, 10)));
});

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

// ── BBox2D ────────────────────────────────────────────────────────────────────
Console.WriteLine("\nBBox2D");

Test("Contains_InsidePoint_True", () => {
  var bb = new BBox2D(new Point2D(0, 0), new Point2D(10, 10));
  IsTrue(bb.Contains(new Point2D(5, 5)));
  IsFalse(bb.Contains(new Point2D(11, 5)));
});

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

Test("AreCW_CWSquare_True", () => {
  var pts = new List<Point3D> {
    new Point3D(0,0,0), new Point3D(0,1,0),
    new Point3D(1,1,0), new Point3D(1,0,0)
  };
  IsTrue(pts.AreCW(), "CW square must return true");
});

// ── GeomUtil segment-set intersection (Shamos–Hoey / Bentley–Ottmann) ──────────
// NOTE: correctness rides on the (currently provisional) sweep-status comparator; these encode the
// intended behaviour and should be re-verified once the real ordering lands.
List<LineSegment2D> SquareRing() => new List<LineSegment2D> {
  LineSegment2D.Make(new Point2D(0,0), new Point2D(1,0)),
  LineSegment2D.Make(new Point2D(1,0), new Point2D(1,1)),
  LineSegment2D.Make(new Point2D(1,1), new Point2D(0,1)),
  LineSegment2D.Make(new Point2D(0,1), new Point2D(0,0)),
};
List<LineSegment2D> SelfIntersectingRing() => new List<LineSegment2D> {
  // edges (4,0)->(1,3) and (3,3)->(0,0) cross at (2,2)
  LineSegment2D.Make(new Point2D(0,0), new Point2D(4,0)),
  LineSegment2D.Make(new Point2D(4,0), new Point2D(1,3)),
  LineSegment2D.Make(new Point2D(1,3), new Point2D(3,3)),
  LineSegment2D.Make(new Point2D(3,3), new Point2D(0,0)),
};

Test("HasIntersections_SimpleRing_False", () => {
  IsFalse(GeomUtil.HasIntersections(SquareRing()), "simple ring has no self-intersections");
});

Test("HasIntersections_SelfIntersecting_True", () => {
  IsTrue(GeomUtil.HasIntersections(SelfIntersectingRing()), "self-intersecting ring");
});

Test("FindIntersections_SimpleRing_Empty", () => {
  int count = 0;
  foreach (var _ in GeomUtil.FindIntersections(SquareRing())) { count++; }
  Eq(0, count);
});

Test("FindIntersections_ReportsCrossing", () => {
  bool found = false;
  foreach (var p in GeomUtil.FindIntersections(SelfIntersectingRing())) {
    if (System.Math.Abs(p.X - 2.0) < 1e-6 && System.Math.Abs(p.Y - 2.0) < 1e-6) { found = true; }
  }
  IsTrue(found, "expected the (2,2) crossing among reported intersections");
});

// ── ConvexHull ────────────────────────────────────────────────────────────────
Console.WriteLine("\nConvexHull");

List<Point2D> StarPoints() {
  // 5 outer tips at unequal distances + 5 inner concave vertices
  return new List<Point2D> {
    new( 0,  5), new( 2,  1),   // tip0, inner0
    new( 4,  2), new( 2, -1),   // tip1, inner1
    new( 3, -3), new( 0, -1),   // tip2, inner2
    new(-2, -4), new(-1, -1),   // tip3, inner3
    new(-3,  1), new(-1,  2),   // tip4, inner4
  };
}

Test("ConvexHull_AsymmetricStar_IsAPentagon", () => {
  var hull = new List<Point2D>(GeomUtil.ConvexHull(StarPoints()));
  Eq(5, hull.Count);
});

Test("ConvexHull_StarOuterTipsAllOnHull", () => {
  var hull = new List<Point2D>(GeomUtil.ConvexHull(StarPoints()));
  var tips = new List<Point2D> {
    new(0,5), new(4,2), new(3,-3), new(-2,-4), new(-3,1) };
  foreach (var tip in tips) {
    IsTrue(hull.Exists(h => System.Math.Abs(h.X - tip.X) < 1e-6 && System.Math.Abs(h.Y - tip.Y) < 1e-6),
           $"outer tip ({tip.X},{tip.Y}) should be on the hull");
  }
});

Test("ConvexHull_FewPoints_ReturnsAsIs", () => {
  var pts = new List<Point2D> { new(0, 0), new(1, 1) };
  var hull = new List<Point2D>(GeomUtil.ConvexHull(pts));
  Eq(2, hull.Count);
});

// ── ConvexHull 3D ─────────────────────────────────────────────────────────────
Console.WriteLine("\nConvexHull 3D");

List<Point3D> StarPoints3D() {
  return new List<Point3D> {
    new( 0,  5, 0), new( 2,  1, 0),
    new( 4,  2, 0), new( 2, -1, 0),
    new( 3, -3, 0), new( 0, -1, 0),
    new(-2, -4, 0), new(-1, -1, 0),
    new(-3,  1, 0), new(-1,  2, 0),
  };
}

Test("ConvexHull3D_XYPlaneSquare_ReturnsFourCorners", () => {
  var pts = new List<Point3D> {
    new(0, 0, 0), new(4, 0, 0), new(4, 4, 0), new(0, 4, 0) };
  var hull = new List<Point3D>(GeomUtil.ConvexHull(pts));
  Eq(4, hull.Count);
});

Test("ConvexHull3D_AsymmetricStar_IsAPentagon", () => {
  var hull = new List<Point3D>(GeomUtil.ConvexHull(StarPoints3D()));
  Eq(5, hull.Count);
});

Test("ConvexHull3D_StarOuterTipsAllOnHull", () => {
  var hull = new List<Point3D>(GeomUtil.ConvexHull(StarPoints3D()));
  var tips = new List<Point3D> {
    new(0,5,0), new(4,2,0), new(3,-3,0), new(-2,-4,0), new(-3,1,0) };
  foreach (var tip in tips) {
    IsTrue(hull.Exists(h => System.Math.Abs(h.X - tip.X) < 1e-6
                         && System.Math.Abs(h.Y - tip.Y) < 1e-6
                         && System.Math.Abs(h.Z - tip.Z) < 1e-6),
           $"outer tip ({tip.X},{tip.Y},{tip.Z}) should be on the hull");
  }
});

// ── Polygon2D ─────────────────────────────────────────────────────────────────
Console.WriteLine("\nPolygon2D");

Test("Perimeter_Square", () => {
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  Eq(4.0, p.Perimeter());
});

Test("Perimeter_Rectangle", () => {
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(3,0), new(3,4), new(0,4) });
  Eq(14.0, p.Perimeter());
});

Test("Centroid_Square", () => {
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(2,0), new(2,2), new(0,2) });
  var c = p.Centroid();
  NotNull(c);
  Eq(1.0, c!.X); Eq(1.0, c.Y);
});

Test("Centroid_Rectangle", () => {
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,2), new(0,2) });
  var c = p.Centroid();
  NotNull(c);
  Eq(2.0, c!.X); Eq(1.0, c.Y);
});

Test("Contains_Interior_True", () => {
  var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsTrue(sq.Contains(new Point2D(0.5, 0.5)), "center must be inside");
  IsTrue(sq.Contains(new Point2D(0.1, 0.1)), "near corner must be inside");
});

Test("Contains_Exterior_False", () => {
  var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsFalse(sq.Contains(new Point2D(-0.1, 0.5)), "left of square");
  IsFalse(sq.Contains(new Point2D(1.1,  0.5)), "right of square");
  IsFalse(sq.Contains(new Point2D(0.5, -0.1)), "below square");
  IsFalse(sq.Contains(new Point2D(0.5,  1.1)), "above square");
});

Test("Contains_WithHole_InsideOuter_OutsideHole_True", () => {
  var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
  var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
  var poly  = Polygon2D.Make(outer, new[] { hole });
  IsTrue(poly.Contains(new Point2D(0.5, 0.5)), "inside outer, outside hole");
  IsTrue(poly.Contains(new Point2D(3.5, 3.5)), "inside outer, outside hole");
});

Test("Contains_WithHole_InsideHole_False", () => {
  var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
  var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
  var poly  = Polygon2D.Make(outer, new[] { hole });
  IsFalse(poly.Contains(new Point2D(2, 2)), "inside hole must be false");
});

Test("Contains_OnBoundary_True", () => {
  var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsTrue(sq.Contains(new Point2D(0,   0)),   "vertex (0,0)");
  IsTrue(sq.Contains(new Point2D(1,   0)),   "vertex (1,0)");
  IsTrue(sq.Contains(new Point2D(1,   1)),   "vertex (1,1)");
  IsTrue(sq.Contains(new Point2D(0,   1)),   "vertex (0,1)");
  IsTrue(sq.Contains(new Point2D(0.5, 0)),   "bottom edge midpoint");
  IsTrue(sq.Contains(new Point2D(1,   0.5)), "right edge midpoint");
  IsTrue(sq.Contains(new Point2D(0.5, 1)),   "top edge midpoint");
  IsTrue(sq.Contains(new Point2D(0,   0.5)), "left edge midpoint");
  // hole boundary is also included
  var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
  var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
  var poly  = Polygon2D.Make(outer, new[] { hole });
  IsTrue(poly.Contains(new Point2D(2, 0)), "outer bottom edge");
  IsTrue(poly.Contains(new Point2D(4, 2)), "outer right edge");
  IsTrue(poly.Contains(new Point2D(2, 1)), "hole bottom edge");
  IsTrue(poly.Contains(new Point2D(1, 2)), "hole left edge");
});

Test("IsOnBoundary_OnEdge_True", () => {
  var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsTrue(sq.IsOnBoundary(new Point2D(0,   0)),   "vertex (0,0)");
  IsTrue(sq.IsOnBoundary(new Point2D(1,   0)),   "vertex (1,0)");
  IsTrue(sq.IsOnBoundary(new Point2D(1,   1)),   "vertex (1,1)");
  IsTrue(sq.IsOnBoundary(new Point2D(0,   1)),   "vertex (0,1)");
  IsTrue(sq.IsOnBoundary(new Point2D(0.5, 0)),   "bottom edge midpoint");
  IsTrue(sq.IsOnBoundary(new Point2D(1,   0.5)), "right edge midpoint");
  IsTrue(sq.IsOnBoundary(new Point2D(0.5, 1)),   "top edge midpoint");
  IsTrue(sq.IsOnBoundary(new Point2D(0,   0.5)), "left edge midpoint");
  var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
  var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
  var poly  = Polygon2D.Make(outer, new[] { hole });
  IsTrue(poly.IsOnBoundary(new Point2D(2, 0)), "outer bottom edge");
  IsTrue(poly.IsOnBoundary(new Point2D(4, 2)), "outer right edge");
  IsTrue(poly.IsOnBoundary(new Point2D(2, 1)), "hole bottom edge");
  IsTrue(poly.IsOnBoundary(new Point2D(1, 2)), "hole left edge");
});

Test("IsOnBoundary_Interior_False", () => {
  var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsFalse(sq.IsOnBoundary(new Point2D(0.5, 0.5)),  "interior");
  IsFalse(sq.IsOnBoundary(new Point2D(-0.1, 0.5)), "outside left");
  IsFalse(sq.IsOnBoundary(new Point2D(1.1,  0.5)), "outside right");
  var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
  var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
  var poly  = Polygon2D.Make(outer, new[] { hole });
  IsFalse(poly.IsOnBoundary(new Point2D(0.5, 0.5)), "interior strip");
  IsFalse(poly.IsOnBoundary(new Point2D(2,   2)),   "inside hole");
});

// NOTE: IsSimple() correctness rides on the (currently provisional) sweep-line comparator; these encode the
// intended behaviour and should be re-verified once the real sweep-status ordering lands.
Test("IsSimple_Square_True", () => {
  var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsTrue(sq.IsSimple(), "convex square is simple");
});

Test("IsSimple_SelfIntersecting_False", () => {
  // CCW (positive area) but edges (4,0)->(1,3) and (3,3)->(0,0) cross at (2,2)
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(4,0), new(1,3), new(3,3) });
  IsFalse(p.IsSimple(), "self-intersecting polygon is not simple");
});

Test("ConvexHull_StarPolygon_IsAPentagon", () => {
  var star = Polygon2D.Make(new Point2D[] {
    new(-1,  2), new(-3,  1), new(-1, -1), new(-2, -4),
    new( 0, -1), new( 3, -3), new( 2, -1), new( 4,  2),
    new( 2,  1), new( 0,  5),
  });
  var hull = star.ConvexHull();
  Eq(5, hull.Size());
});

Test("ToPoints_RoundTrip", () => {
  var pts = new Point2D[] { new(0,0), new(3,0), new(3,3), new(0,3) };
  var poly = Polygon2D.Make(pts);
  var back = poly.ToPoints();
  Eq(4, back.Length);
  for (int i = 0; i < pts.Length; ++i) {
    IsTrue(pts[i].AlmostEquals(back[i]), $"vertex {i} mismatch after ToPoints round-trip");
  }
});

// ── Polygon3D ─────────────────────────────────────────────────────────────────
Console.WriteLine("\nPolygon3D");

Test("Perimeter_Square", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  Eq(4.0, p.Perimeter());
});

Test("Perimeter_NonXYPlane", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(0,1,0), new(0,1,1), new(0,0,1) });
  Eq(4.0, p.Perimeter());
});

Test("Centroid_Square", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(2,0,0), new(2,2,0), new(0,2,0) });
  var c = p.Centroid();
  NotNull(c);
  Eq(1.0, c!.X); Eq(1.0, c.Y); Eq(0.0, c.Z);
});

Test("Centroid_ElevatedSquare", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,5), new(2,0,5), new(2,2,5), new(0,2,5) });
  var c = p.Centroid();
  NotNull(c);
  Eq(1.0, c!.X); Eq(1.0, c.Y); Eq(5.0, c.Z);
});

Test("GetPlane_XYPlane_NormalPointsInZ", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  var pl = p.GetPlane();
  NotNull(pl);
  Eq(1.0, Math.Abs(pl!.Normal().Z));
});

Test("GetPlane_ContainsAllVertices", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,3), new(1,0,3), new(1,1,3), new(0,1,3) });
  var pl = p.GetPlane();
  NotNull(pl);
  IsTrue(pl!.Contains(new Point3D(0,0,3)), "origin vertex must lie on plane");
  IsTrue(pl.Contains(new Point3D(1,1,3)), "far vertex must lie on plane");
});

Test("Contains_Interior_True", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsTrue(sq.Contains(new Point3D(0.5, 0.5, 0)), "center must be inside");
  IsTrue(sq.Contains(new Point3D(0.1, 0.1, 0)), "near corner must be inside");
});

Test("Contains_Exterior_False", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsFalse(sq.Contains(new Point3D(-0.1, 0.5, 0)), "left of square");
  IsFalse(sq.Contains(new Point3D(1.1,  0.5, 0)), "right of square");
});

Test("Contains_OffPlane_False", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsFalse(sq.Contains(new Point3D(0.5, 0.5,  1)), "above plane");
  IsFalse(sq.Contains(new Point3D(0.5, 0.5, -1)), "below plane");
});

Test("Contains_WithHole_InsideOuter_OutsideHole_True", () => {
  var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
  var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
  var poly  = Polygon3D.Make(outer, new[] { hole });
  IsTrue(poly.Contains(new Point3D(0.5, 0.5, 0)), "inside outer, outside hole");
  IsTrue(poly.Contains(new Point3D(3.5, 3.5, 0)), "inside outer, outside hole");
});

Test("Contains_WithHole_InsideHole_False", () => {
  var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
  var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
  var poly  = Polygon3D.Make(outer, new[] { hole });
  IsFalse(poly.Contains(new Point3D(2, 2, 0)), "inside hole must be false");
  IsFalse(poly.Contains(new Point3D(2, 2, 1)), "above plane must be false");
});

Test("Contains_OnBoundary_True", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsTrue(sq.Contains(new Point3D(0,   0,   0)), "vertex (0,0,0)");
  IsTrue(sq.Contains(new Point3D(1,   0,   0)), "vertex (1,0,0)");
  IsTrue(sq.Contains(new Point3D(1,   1,   0)), "vertex (1,1,0)");
  IsTrue(sq.Contains(new Point3D(0,   1,   0)), "vertex (0,1,0)");
  IsTrue(sq.Contains(new Point3D(0.5, 0,   0)), "bottom edge midpoint");
  IsTrue(sq.Contains(new Point3D(1,   0.5, 0)), "right edge midpoint");
  IsTrue(sq.Contains(new Point3D(0.5, 1,   0)), "top edge midpoint");
  IsTrue(sq.Contains(new Point3D(0,   0.5, 0)), "left edge midpoint");
  var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
  var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
  var poly  = Polygon3D.Make(outer, new[] { hole });
  IsTrue(poly.Contains(new Point3D(2, 0, 0)), "outer bottom edge");
  IsTrue(poly.Contains(new Point3D(2, 1, 0)), "hole bottom edge");
});

Test("IsOnBoundary_OnEdge_True", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsTrue(sq.IsOnBoundary(new Point3D(0,   0,   0)), "vertex (0,0,0)");
  IsTrue(sq.IsOnBoundary(new Point3D(1,   0,   0)), "vertex (1,0,0)");
  IsTrue(sq.IsOnBoundary(new Point3D(1,   1,   0)), "vertex (1,1,0)");
  IsTrue(sq.IsOnBoundary(new Point3D(0,   1,   0)), "vertex (0,1,0)");
  IsTrue(sq.IsOnBoundary(new Point3D(0.5, 0,   0)), "bottom edge midpoint");
  IsTrue(sq.IsOnBoundary(new Point3D(1,   0.5, 0)), "right edge midpoint");
  IsTrue(sq.IsOnBoundary(new Point3D(0.5, 1,   0)), "top edge midpoint");
  IsTrue(sq.IsOnBoundary(new Point3D(0,   0.5, 0)), "left edge midpoint");
  var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
  var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
  var poly  = Polygon3D.Make(outer, new[] { hole });
  IsTrue(poly.IsOnBoundary(new Point3D(2, 0, 0)), "outer bottom edge");
  IsTrue(poly.IsOnBoundary(new Point3D(4, 2, 0)), "outer right edge");
  IsTrue(poly.IsOnBoundary(new Point3D(2, 1, 0)), "hole bottom edge");
  IsTrue(poly.IsOnBoundary(new Point3D(1, 2, 0)), "hole left edge");
});

Test("IsOnBoundary_Interior_False", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsFalse(sq.IsOnBoundary(new Point3D(0.5, 0.5, 0)),    "interior");
  IsFalse(sq.IsOnBoundary(new Point3D(-0.1, 0.5, 0)),   "outside left");
  IsFalse(sq.IsOnBoundary(new Point3D(0.5,  0.5, 0.01)), "off-plane");
  var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
  var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
  var poly  = Polygon3D.Make(outer, new[] { hole });
  IsFalse(poly.IsOnBoundary(new Point3D(0.5, 0.5, 0)), "interior strip");
  IsFalse(poly.IsOnBoundary(new Point3D(2,   2,   0)), "inside hole");
});

// NOTE: IsSimple() correctness rides on the (currently provisional) sweep-line comparator; these encode the
// intended behaviour and should be re-verified once the real sweep-status ordering lands.
Test("IsSimple3D_ConvexSquare_XYPlane_True", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsTrue(sq.IsSimple(), "convex square on XY plane is simple");
});

Test("IsSimple3D_ConvexSquare_YZPlane_True", () => {
  var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(0,1,0), new(0,1,1), new(0,0,1) });
  IsTrue(sq.IsSimple(), "convex square on YZ plane is simple");
});

Test("IsSimple3D_SelfIntersecting_False", () => {
  // CCW (positive area) but edges (4,0,0)->(1,3,0) and (3,3,0)->(0,0,0) cross
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(1,3,0), new(3,3,0) });
  IsFalse(p.IsSimple(), "self-intersecting 3D polygon is not simple");
});

Test("ConvexHull3D_StarPolygon_IsAPentagon", () => {
  var star = Polygon3D.Make(new Point3D[] {
    new(-1,  2, 0), new(-3,  1, 0), new(-1, -1, 0), new(-2, -4, 0),
    new( 0, -1, 0), new( 3, -3, 0), new( 2, -1, 0), new( 4,  2, 0),
    new( 2,  1, 0), new( 0,  5, 0),
  });
  var hull = star.ConvexHull();
  Eq(5, hull.Size());
});

Test("ToPoints3D_RoundTrip", () => {
  var pts = new Point3D[] { new(0,0,0), new(3,0,0), new(3,3,0), new(0,3,0) };
  var poly = Polygon3D.Make(pts);
  var back = poly.ToPoints();
  Eq(4, back.Length);
  for (int i = 0; i < pts.Length; ++i) {
    IsTrue(pts[i].AlmostEquals(back[i]), $"vertex {i} mismatch after ToPoints round-trip");
  }
});

// ── Vector2D ──────────────────────────────────────────────────────────────────
Console.WriteLine("\nVector2D");

Test("Create_AccessXY", () => {
  var v = new Vector2D(3.0, 4.0);
  Eq(3.0, v.X);
  Eq(4.0, v.Y);
});

Test("Length_KnownValue", () => Eq(5.0, new Vector2D(3.0, 4.0).Length()));

Test("AlmostEquals_SameVector", () => IsTrue(new Vector2D(1.0, 2.0).AlmostEquals(new Vector2D(1.0, 2.0))));

Test("AlmostEquals_DifferentVector", () => IsFalse(new Vector2D(1.0, 0.0).AlmostEquals(new Vector2D(0.0, 1.0))));

Test("BasisX_IsUnitX", () => {
  var v = Vector2D.BasisX();
  Eq(1.0, v.X);
  Eq(0.0, v.Y);
  Eq(1.0, v.Length());
});

Test("BasisY_IsUnitY", () => {
  var v = Vector2D.BasisY();
  Eq(0.0, v.X);
  Eq(1.0, v.Y);
  Eq(1.0, v.Length());
});

Test("Dot_PerpendicularVectors_IsZero", () => Eq(0.0, Vector2D.BasisX().Dot(Vector2D.BasisY())));

Test("Dot_SameVector_IsOne", () => Eq(1.0, Vector2D.BasisX().Dot(Vector2D.BasisX())));

Test("Cross_CCW_IsPositive", () => Eq(1.0, Vector2D.BasisX().Cross(Vector2D.BasisY())));

Test("Cross_CW_IsNegative", () => Eq(-1.0, Vector2D.BasisY().Cross(Vector2D.BasisX())));

Test("Perp_OfBasisX_IsBasisY", () => {
  var p = Vector2D.BasisX().Perp();
  Eq(0.0, p.X);
  Eq(1.0, p.Y);
});

Test("Normalize_UnitLength", () => Eq(1.0, new Vector2D(3.0, 4.0).Normalize().Length()));

Test("ToPoint_MatchesComponents", () => {
  var pt = new Vector2D(2.0, 3.0).ToPoint();
  Eq(2.0, pt.X);
  Eq(3.0, pt.Y);
});

Test("Add_TwoVectors", () => {
  var r = Vector2D.BasisX() + Vector2D.BasisY();
  Eq(1.0, r.X);
  Eq(1.0, r.Y);
});

Test("Subtract_TwoVectors", () => {
  var r = Vector2D.BasisX() - Vector2D.BasisY();
  Eq(1.0, r.X);
  Eq(-1.0, r.Y);
});

Test("ScalarMultiply", () => {
  var r = Vector2D.BasisX() * 3.0;
  Eq(3.0, r.X);
  Eq(0.0, r.Y);
});

Test("ScalarDivide", () => {
  var r = new Vector2D(4.0, 0.0) / 2.0;
  Eq(2.0, r.X);
  Eq(0.0, r.Y);
});

Test("UnaryNegate", () => {
  var r = -Vector2D.BasisX();
  Eq(-1.0, r.X);
  Eq(0.0, r.Y);
});

Test("Equality_SameVectors", () => IsTrue(Vector2D.BasisX() == Vector2D.BasisX()));

Test("Equality_DifferentVectors", () => IsFalse(Vector2D.BasisX() == Vector2D.BasisY()));

Test("DotOperator_MatchesMethod", () => {
  var u = new Vector2D(1.0, 2.0);
  var v = new Vector2D(3.0, 4.0);
  Eq(u.Dot(v), u * v);
});

Test("VectorPlusPoint", () => {
  var pt = Vector2D.BasisX() + new Point2D(1.0, 1.0);
  Eq(2.0, pt.X);
  Eq(1.0, pt.Y);
});

Test("WktRoundTrip", () => {
  var v = new Vector2D(1.5, 2.5);
  IsTrue(v.AlmostEquals(Vector2D.FromWkt(v.ToWkt())));
});

// ── WktParser ─────────────────────────────────────────────────────────────────
Console.WriteLine("\nWktParser");

string MakeTempWktFile(params string[] lines) {
  var path = Path.GetTempFileName();
  File.WriteAllLines(path, lines);
  return path;
}

Test("Open_ValidFile_NoThrow", () => {
  var path = MakeTempWktFile("POINT (1 2)");
  using (var parser = WktParser.Open(path)) { NotNull(parser); }
  File.Delete(path);
});

Test("GetFilePath_MatchesInput", () => {
  var path = MakeTempWktFile("POINT (1 2)");
  using (var parser = WktParser.Open(path)) {
    IsTrue(parser.GetFilePath() == path, "file path mismatch");
  }
  File.Delete(path);
});

Test("HasNext_TrueWhenFileHasContent", () => {
  var path = MakeTempWktFile("POINT (1 2)");
  using (var parser = WktParser.Open(path)) { IsTrue(parser.HasNext()); }
  File.Delete(path);
});

Test("Next_ReturnsPoint2D_WithCorrectCoords", () => {
  var path = MakeTempWktFile("POINT (3 4)");
  using (var parser = WktParser.Open(path)) {
    var pt = parser.Next() as Point2D;
    NotNull(pt, "expected Point2D");
    Eq(3.0, pt!.X);
    Eq(4.0, pt.Y);
  }
  File.Delete(path);
});

Test("Next_ReturnsPoint3D", () => {
  var path = MakeTempWktFile("POINT (1 2 3)");
  using (var parser = WktParser.Open(path)) {
    NotNull(parser.Next() as Point3D, "expected Point3D");
  }
  File.Delete(path);
});

Test("Next_ReturnsLineSegment2D", () => {
  var path = MakeTempWktFile("LINESTRING (0 0, 1 0)");
  using (var parser = WktParser.Open(path)) {
    NotNull(parser.Next() as LineSegment2D, "expected LineSegment2D");
  }
  File.Delete(path);
});

Test("Next_MultipleLines_InOrder", () => {
  var path = MakeTempWktFile("POINT (1 0)", "POINT (2 0)");
  using (var parser = WktParser.Open(path)) {
    var a = (parser.Next() as Point2D)!;
    var b = (parser.Next() as Point2D)!;
    Eq(1.0, a.X);
    Eq(2.0, b.X);
  }
  File.Delete(path);
});

Test("Next_ReturnsNullAfterExhausted", () => {
  var path = MakeTempWktFile("POINT (1 2)");
  using (var parser = WktParser.Open(path)) {
    parser.Next();
    IsNull(parser.Next(), "should return null when exhausted");
  }
  File.Delete(path);
});

Test("ToWkt_Point2D_ContainsCoordinates", () => {
  var pt = new Point2D(5.0, 6.0);
  var wkt = WktParser.ToWkt(pt);
  NotNull(wkt);
  IsTrue(wkt!.Contains("POINT"), "missing POINT keyword");
  IsTrue(wkt.Contains("5"), "missing x");
  IsTrue(wkt.Contains("6"), "missing y");
});

// ── GeometryCollection2D ──────────────────────────────────────────────────────
Console.WriteLine("\nGeometryCollection2D");

Test("DefaultConstructor_SizeZero", () => Eq(0, new GeometryCollection2D().Size(), 0));

Test("AddPoint_SizeBecomesOne", () => {
  var gc = new GeometryCollection2D();
  gc.Add(new Point2D(1, 2));
  Eq(1, gc.Size(), 0);
});

Test("AddMultipleTypes_SizeIsCorrect", () => {
  var gc = new GeometryCollection2D();
  gc.Add(new Point2D(0, 0));
  gc.Add(LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 0)));
  Eq(2, gc.Size(), 0);
});

Test("Get_ReturnsCorrectType", () => {
  var gc = new GeometryCollection2D();
  gc.Add(new Point2D(3, 4));
  var pt = gc.Get(0) as Point2D;
  NotNull(pt, "expected Point2D");
  Eq(3.0, pt!.X);
  Eq(4.0, pt.Y);
});

Test("AlmostEquals_SameCollections", () => {
  var a = new GeometryCollection2D();
  a.Add(new Point2D(1, 2));
  var b = new GeometryCollection2D();
  b.Add(new Point2D(1, 2));
  IsTrue(a.AlmostEquals(b));
});

Test("AlmostEquals_DifferentCollections", () => {
  var a = new GeometryCollection2D();
  a.Add(new Point2D(1, 2));
  var b = new GeometryCollection2D();
  b.Add(new Point2D(9, 9));
  IsFalse(a.AlmostEquals(b));
});

Test("AlmostEquals_BothEmpty", () => {
  IsTrue(new GeometryCollection2D().AlmostEquals(new GeometryCollection2D()));
});

Test("ToWkt_Empty_ContainsEMPTY", () => {
  IsTrue(new GeometryCollection2D().ToWkt().Contains("EMPTY"));
});

Test("ToWkt_WithPoint_ContainsKeywords", () => {
  var gc = new GeometryCollection2D();
  gc.Add(new Point2D(1, 2));
  var wkt = gc.ToWkt();
  IsTrue(wkt.Contains("GEOMETRYCOLLECTION"), "missing GEOMETRYCOLLECTION");
  IsTrue(wkt.Contains("POINT"), "missing POINT");
});

Test("FromWkt_RoundTrip", () => {
  var gc = new GeometryCollection2D();
  gc.Add(new Point2D(1, 2));
  IsTrue(gc.AlmostEquals(GeometryCollection2D.FromWkt(gc.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var gc = new GeometryCollection2D();
  gc.Add(new Point2D(7, 8));
  var path = Path.GetTempFileName();
  gc.ToFile(path);
  IsTrue(gc.AlmostEquals(GeometryCollection2D.FromFile(path)));
  File.Delete(path);
});

// ── GeometryCollection3D ──────────────────────────────────────────────────────
Console.WriteLine("\nGeometryCollection3D");

Test("DefaultConstructor_SizeZero", () => Eq(0, new GeometryCollection3D().Size(), 0));

Test("AddPoint_SizeBecomesOne", () => {
  var gc = new GeometryCollection3D();
  gc.Add(new Point3D(1, 2, 3));
  Eq(1, gc.Size(), 0);
});

Test("AddMultipleTypes_SizeIsCorrect", () => {
  var gc = new GeometryCollection3D();
  gc.Add(new Point3D(0, 0, 0));
  gc.Add(LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0)));
  Eq(2, gc.Size(), 0);
});

Test("Get_ReturnsCorrectType", () => {
  var gc = new GeometryCollection3D();
  gc.Add(new Point3D(1, 2, 3));
  var pt = gc.Get(0) as Point3D;
  NotNull(pt, "expected Point3D");
  Eq(1.0, pt!.X);
  Eq(2.0, pt.Y);
  Eq(3.0, pt.Z);
});

Test("AlmostEquals_SameCollections", () => {
  var a = new GeometryCollection3D();
  a.Add(new Point3D(1, 2, 3));
  var b = new GeometryCollection3D();
  b.Add(new Point3D(1, 2, 3));
  IsTrue(a.AlmostEquals(b));
});

Test("AlmostEquals_DifferentCollections", () => {
  var a = new GeometryCollection3D();
  a.Add(new Point3D(1, 2, 3));
  var b = new GeometryCollection3D();
  b.Add(new Point3D(9, 9, 9));
  IsFalse(a.AlmostEquals(b));
});

Test("AlmostEquals_BothEmpty", () => {
  IsTrue(new GeometryCollection3D().AlmostEquals(new GeometryCollection3D()));
});

Test("ToWkt_Empty_ContainsEMPTY", () => {
  IsTrue(new GeometryCollection3D().ToWkt().Contains("EMPTY"));
});

Test("ToWkt_WithPoint_ContainsKeywords", () => {
  var gc = new GeometryCollection3D();
  gc.Add(new Point3D(1, 2, 3));
  var wkt = gc.ToWkt();
  IsTrue(wkt.Contains("GEOMETRYCOLLECTION"), "missing GEOMETRYCOLLECTION");
  IsTrue(wkt.Contains("POINT"), "missing POINT");
});

Test("FromWkt_RoundTrip", () => {
  var gc = new GeometryCollection3D();
  gc.Add(new Point3D(1, 2, 3));
  IsTrue(gc.AlmostEquals(GeometryCollection3D.FromWkt(gc.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var gc = new GeometryCollection3D();
  gc.Add(new Point3D(7, 8, 9));
  var path = Path.GetTempFileName();
  gc.ToFile(path);
  IsTrue(gc.AlmostEquals(GeometryCollection3D.FromFile(path)));
  File.Delete(path);
});

// ── Point2D (additional) ──────────────────────────────────────────────────────
Console.WriteLine("\nPoint2D (additional)");

Test("ToVector_MatchesXY", () => {
  var v = new Point2D(3.0, 4.0).ToVector();
  Eq(3.0, v.X);
  Eq(4.0, v.Y);
});

Test("ToFile_FromFile_RoundTrip", () => {
  var p = new Point2D(1.5, 2.5);
  var path = Path.GetTempFileName();
  p.ToFile(path);
  var q = Point2D.FromFile(path);
  IsTrue(p.AlmostEquals(q));
  File.Delete(path);
});

Test("SubtractPoints_ReturnsVector", () => {
  var v = new Point2D(5.0, 3.0) - new Point2D(2.0, 1.0);
  Eq(3.0, v.X);
  Eq(2.0, v.Y);
});

Test("SubtractVector_ReturnsPoint", () => {
  var p = new Point2D(5.0, 3.0) - new Vector2D(2.0, 1.0);
  Eq(3.0, p.X);
  Eq(2.0, p.Y);
});

Test("ScalarMultiply_Point", () => {
  var p = new Point2D(2.0, 3.0) * 2.0;
  Eq(4.0, p.X);
  Eq(6.0, p.Y);
});

// ── Point3D (additional) ──────────────────────────────────────────────────────
Console.WriteLine("\nPoint3D (additional)");

Test("Zero_IsOrigin", () => {
  var z = Point3D.Zero();
  Eq(0.0, z.X);
  Eq(0.0, z.Y);
  Eq(0.0, z.Z);
});

Test("ToVector_MatchesXYZ", () => {
  var v = new Point3D(1.0, 2.0, 3.0).ToVector();
  Eq(1.0, v.X);
  Eq(2.0, v.Y);
  Eq(3.0, v.Z);
});

Test("AlmostEquals_SamePoint_True", () => {
  IsTrue(new Point3D(1.0, 2.0, 3.0).AlmostEquals(new Point3D(1.0, 2.0, 3.0)));
});

Test("AlmostEquals_DifferentPoint_False", () => {
  IsFalse(new Point3D(0.0, 0.0, 0.0).AlmostEquals(new Point3D(1.0, 0.0, 0.0)));
});

Test("SubtractPoints_ReturnsVector", () => {
  var v = new Point3D(5.0, 3.0, 1.0) - new Point3D(2.0, 1.0, 0.0);
  Eq(3.0, v.X);
  Eq(2.0, v.Y);
  Eq(1.0, v.Z);
});

Test("SubtractVector_ReturnsPoint", () => {
  var p = new Point3D(5.0, 3.0, 1.0) - new Vector3D(2.0, 1.0, 1.0);
  Eq(3.0, p.X);
  Eq(2.0, p.Y);
  Eq(0.0, p.Z);
});

Test("ScalarMultiply_Point", () => {
  var p = new Point3D(1.0, 2.0, 3.0) * 3.0;
  Eq(3.0, p.X);
  Eq(6.0, p.Y);
  Eq(9.0, p.Z);
});

Test("WktRoundTrip", () => {
  var p = new Point3D(1.5, 2.5, 3.5);
  IsTrue(p.AlmostEquals(Point3D.FromWkt(p.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var p = new Point3D(7.0, 8.0, 9.0);
  var path = Path.GetTempFileName();
  p.ToFile(path);
  IsTrue(p.AlmostEquals(Point3D.FromFile(path)));
  File.Delete(path);
});

// ── Vector3D (additional) ─────────────────────────────────────────────────────
Console.WriteLine("\nVector3D (additional)");

Test("ToPoint_MatchesXYZ", () => {
  var pt = new Vector3D(1.0, 2.0, 3.0).ToPoint();
  Eq(1.0, pt.X);
  Eq(2.0, pt.Y);
  Eq(3.0, pt.Z);
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

// ── LineSegment2D (additional) ────────────────────────────────────────────────
Console.WriteLine("\nLineSegment2D (additional)");

Test("ToLine_ReturnsLine2D", () => {
  var seg  = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
  var line = seg.ToLine();
  NotNull(line);
  IsTrue(line is Line2D);
});

Test("AlmostEquals_SameSeg_True", () => {
  var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
  var b = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
  IsTrue(a.AlmostEquals(b));
});

Test("AlmostEquals_DiffSeg_False", () => {
  var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(4, 0));
  var b = LineSegment2D.Make(new Point2D(0, 0), new Point2D(0, 4));
  IsFalse(a.AlmostEquals(b));
});

Test("Intersects_Ray2D_True", () => {
  var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
  var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  IsTrue(seg.Intersects(r));
});

Test("Intersects_Seg2D_True", () => {
  var a = LineSegment2D.Make(new Point2D(0, -1), new Point2D(0, 1));
  var b = LineSegment2D.Make(new Point2D(-1, 0), new Point2D(1, 0));
  IsTrue(a.Intersects(b));
});

Test("Intersects_Seg2D_False", () => {
  var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 0));
  var b = LineSegment2D.Make(new Point2D(2, 0), new Point2D(3, 0));
  IsFalse(a.Intersects(b));
});

Test("Intersection_Ray2D_Hit", () => {
  var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
  var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var pt  = seg.Intersection(r);
  NotNull(pt);
  Eq(2.0, pt!.X);
  Eq(0.0, pt.Y);
});

Test("Intersection_Seg2D_Hit", () => {
  var a  = LineSegment2D.Make(new Point2D(0, -1), new Point2D(0, 1));
  var b  = LineSegment2D.Make(new Point2D(-1, 0), new Point2D(1, 0));
  var pt = a.Intersection(b);
  NotNull(pt);
  Eq(0.0, pt!.X);
  Eq(0.0, pt.Y);
});

Test("Intersection_Seg2D_Miss", () => {
  var a = LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 0));
  var b = LineSegment2D.Make(new Point2D(2, 0), new Point2D(3, 0));
  IsNull(a.Intersection(b));
});

Test("WktRoundTrip", () => {
  var seg = LineSegment2D.Make(new Point2D(1, 2), new Point2D(3, 4));
  IsTrue(seg.AlmostEquals(LineSegment2D.FromWkt(seg.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var seg  = LineSegment2D.Make(new Point2D(1, 2), new Point2D(3, 4));
  var path = Path.GetTempFileName();
  seg.ToFile(path);
  IsTrue(seg.AlmostEquals(LineSegment2D.FromFile(path)));
  File.Delete(path);
});

// ── LineSegment3D (additional) ────────────────────────────────────────────────
Console.WriteLine("\nLineSegment3D (additional)");

Test("Length_KnownValue", () => {
  Eq(5.0, LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(3, 4, 0)).Length());
});

Test("AlmostEquals_SameSeg_True", () => {
  var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var b = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  IsTrue(a.AlmostEquals(b));
});

Test("ToLine_ReturnsLine3D", () => {
  var seg  = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var line = seg.ToLine();
  NotNull(line);
  IsTrue(line is Line3D);
});

Test("Intersects_Line3D_True", () => {
  var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
  var l   = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  IsTrue(seg.Intersects(l));
});

Test("Intersects_Ray3D_True", () => {
  var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
  var r   = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
  IsTrue(seg.Intersects(r));
});

Test("Intersects_Seg3D_True", () => {
  var a = LineSegment3D.Make(new Point3D(0, -1, 0), new Point3D(0, 1, 0));
  var b = LineSegment3D.Make(new Point3D(-1, 0, 0), new Point3D(1, 0, 0));
  IsTrue(a.Intersects(b));
});

Test("Intersects_Seg3D_False", () => {
  var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
  var b = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(3, 0, 0));
  IsFalse(a.Intersects(b));
});

Test("Intersection_Line3D_Hit", () => {
  var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
  var l   = Line3D.Make(new Point3D(0, 0, 0), new Point3D(4, 0, 0));
  var pt  = seg.Intersection(l);
  NotNull(pt);
  Eq(2.0, pt!.X);
  Eq(0.0, pt.Y);
  Eq(0.0, pt.Z);
});

Test("Intersection_Ray3D_Hit", () => {
  var seg = LineSegment3D.Make(new Point3D(2, -1, 0), new Point3D(2, 1, 0));
  var r   = Ray3D.Make(new Point3D(0, 0, 0), new Vector3D(1, 0, 0));
  var pt  = seg.Intersection(r);
  NotNull(pt);
  Eq(2.0, pt!.X);
  Eq(0.0, pt.Y);
  Eq(0.0, pt.Z);
});

Test("Intersection_Seg3D_Hit", () => {
  var a  = LineSegment3D.Make(new Point3D(0, -1, 0), new Point3D(0, 1, 0));
  var b  = LineSegment3D.Make(new Point3D(-1, 0, 0), new Point3D(1, 0, 0));
  var pt = a.Intersection(b);
  NotNull(pt);
  Eq(0.0, pt!.X);
  Eq(0.0, pt.Y);
  Eq(0.0, pt.Z);
});

Test("Intersection_Seg3D_Miss", () => {
  var a = LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0));
  var b = LineSegment3D.Make(new Point3D(2, 0, 0), new Point3D(3, 0, 0));
  IsNull(a.Intersection(b));
});

Test("WktRoundTrip", () => {
  var seg = LineSegment3D.Make(new Point3D(1, 2, 3), new Point3D(4, 5, 6));
  IsTrue(seg.AlmostEquals(LineSegment3D.FromWkt(seg.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var seg  = LineSegment3D.Make(new Point3D(1, 2, 3), new Point3D(4, 5, 6));
  var path = Path.GetTempFileName();
  seg.ToFile(path);
  IsTrue(seg.AlmostEquals(LineSegment3D.FromFile(path)));
  File.Delete(path);
});

// ── Ray2D (additional) ────────────────────────────────────────────────────────
Console.WriteLine("\nRay2D (additional)");

Test("IsAhead_PointInFront_True", () => {
  var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  IsTrue(r.IsAhead(new Point2D(5, 0)));
});

Test("IsAhead_PointBehind_False", () => {
  var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  IsFalse(r.IsAhead(new Point2D(-1, 0)));
});

Test("IsBehind_PointBehind_True", () => {
  var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  IsTrue(r.IsBehind(new Point2D(-1, 0)));
});

Test("IsBehind_PointInFront_False", () => {
  var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  IsFalse(r.IsBehind(new Point2D(5, 0)));
});

Test("Intersects_Line2D_True", () => {
  var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var l = Line2D.Make(new Point2D(2, -2), new Point2D(2, 2));
  IsTrue(r.Intersects(l));
});

Test("Intersects_Ray2D_True", () => {
  var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var r2 = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
  IsTrue(r1.Intersects(r2));
});

Test("Intersects_Ray2D_False", () => {
  // Both rays point away from each other
  var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(-1, 0));
  var r2 = Ray2D.Make(new Point2D(4, 0), new Vector2D(1, 0));
  IsFalse(r1.Intersects(r2));
});

Test("Intersects_Seg2D_True", () => {
  var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
  IsTrue(r.Intersects(seg));
});

Test("Intersection_Line2D_Hit", () => {
  var r  = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var l  = Line2D.Make(new Point2D(2, -2), new Point2D(2, 2));
  var pt = r.Intersection(l);
  NotNull(pt);
  Eq(2.0, pt!.X);
  Eq(0.0, pt.Y);
});

Test("Intersection_Ray2D_Hit", () => {
  var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var r2 = Ray2D.Make(new Point2D(2, -2), new Vector2D(0, 1));
  var pt = r1.Intersection(r2);
  NotNull(pt);
  Eq(2.0, pt!.X);
  Eq(0.0, pt.Y);
});

Test("Intersection_Ray2D_Miss", () => {
  var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(-1, 0));
  var r2 = Ray2D.Make(new Point2D(4, 0), new Vector2D(1, 0));
  IsNull(r1.Intersection(r2));
});

Test("Intersection_Seg2D_Hit", () => {
  var r   = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var seg = LineSegment2D.Make(new Point2D(2, -1), new Point2D(2, 1));
  var pt  = r.Intersection(seg);
  NotNull(pt);
  Eq(2.0, pt!.X);
  Eq(0.0, pt.Y);
});

Test("ToLine_ReturnsLine2D", () => {
  var r = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  NotNull(r.ToLine());
  IsTrue(r.ToLine() is Line2D);
});

Test("AlmostEquals_SameRay_True", () => {
  var r1 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  var r2 = Ray2D.Make(new Point2D(0, 0), new Vector2D(1, 0));
  IsTrue(r1.AlmostEquals(r2));
});

Test("WktRoundTrip", () => {
  var r = Ray2D.Make(new Point2D(1, 2), new Vector2D(1, 0));
  IsTrue(r.AlmostEquals(Ray2D.FromWkt(r.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var r    = Ray2D.Make(new Point2D(1, 2), new Vector2D(1, 0));
  var path = Path.GetTempFileName();
  r.ToFile(path);
  IsTrue(r.AlmostEquals(Ray2D.FromFile(path)));
  File.Delete(path);
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

// ── Polygon2D (additional) ────────────────────────────────────────────────────
Console.WriteLine("\nPolygon2D (additional)");

Test("Area_UnitSquare", () => {
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  Eq(1.0, p.Area());
});

Test("Area_WithHole", () => {
  // 4x4 square (CCW) minus 2x2 inner hole (CW) → area = 16 - 4 = 12
  var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
  var hole  = new Point2D[] { new(1,3), new(3,3), new(3,1), new(1,1) }; // CW winding
  var p = Polygon2D.Make(outer, new[] { hole });
  Eq(12.0, p.Area());
});

Test("AlmostEquals_SamePoly_True", () => {
  var a = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  var b = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsTrue(a.AlmostEquals(b));
});

Test("AlmostEquals_DiffPoly_False", () => {
  var a = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  var b = Polygon2D.Make(new Point2D[] { new(0,0), new(2,0), new(2,2), new(0,2) });
  IsFalse(a.AlmostEquals(b));
});

Test("Indexer_ReturnsCorrectVertex", () => {
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  Eq(1.0, p[1].X);
  Eq(0.0, p[1].Y);
});

Test("WktRoundTrip", () => {
  var p = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
  IsTrue(p.AlmostEquals(Polygon2D.FromWkt(p.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var p    = Polygon2D.Make(new Point2D[] { new(0,0), new(2,0), new(2,2), new(0,2) });
  var path = Path.GetTempFileName();
  p.ToFile(path);
  IsTrue(p.AlmostEquals(Polygon2D.FromFile(path)));
  File.Delete(path);
});

// ── Polygon3D (additional) ────────────────────────────────────────────────────
Console.WriteLine("\nPolygon3D (additional)");

Test("Area_UnitSquare", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  Eq(1.0, p.Area());
});

Test("AlmostEquals_SamePoly_True", () => {
  var a = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  var b = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsTrue(a.AlmostEquals(b));
});

Test("Indexer_ReturnsCorrectVertex", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  Eq(1.0, p[1].X);
  Eq(0.0, p[1].Y);
  Eq(0.0, p[1].Z);
});

Test("WktRoundTrip", () => {
  var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
  IsTrue(p.AlmostEquals(Polygon3D.FromWkt(p.ToWkt())));
});

Test("ToFile_FromFile_RoundTrip", () => {
  var p    = Polygon3D.Make(new Point3D[] { new(0,0,0), new(2,0,0), new(2,2,0), new(0,2,0) });
  var path = Path.GetTempFileName();
  p.ToFile(path);
  IsTrue(p.AlmostEquals(Polygon3D.FromFile(path)));
  File.Delete(path);
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

// ── GeometryCollection2D (additional) ────────────────────────────────────────
Console.WriteLine("\nGeometryCollection2D (additional)");

Test("Add_Line2D_SizeIncreases", () => {
  var gc = new GeometryCollection2D();
  gc.Add(Line2D.Make(new Point2D(0,0), new Point2D(1,0)));
  Eq(1, gc.Size(), 0);
});

Test("Add_Ray2D_SizeIncreases", () => {
  var gc = new GeometryCollection2D();
  gc.Add(Ray2D.Make(new Point2D(0,0), new Vector2D(1,0)));
  Eq(1, gc.Size(), 0);
});

Test("Add_Polyline2D_SizeIncreases", () => {
  var gc = new GeometryCollection2D();
  gc.Add(Polyline2D.Make(new Point2D[] { new(0,0), new(1,0) }));
  Eq(1, gc.Size(), 0);
});

Test("Add_Polygon2D_SizeIncreases", () => {
  var gc = new GeometryCollection2D();
  gc.Add(Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) }));
  Eq(1, gc.Size(), 0);
});

Test("Add_Triangle2D_SizeIncreases", () => {
  var gc = new GeometryCollection2D();
  gc.Add(Triangle2D.Make(new Point2D(0,0), new Point2D(1,0), new Point2D(0,1)));
  Eq(1, gc.Size(), 0);
});

// ── GeometryCollection3D (additional) ────────────────────────────────────────
Console.WriteLine("\nGeometryCollection3D (additional)");

Test("Add_Line3D_SizeIncreases", () => {
  var gc = new GeometryCollection3D();
  gc.Add(Line3D.Make(new Point3D(0,0,0), new Point3D(1,0,0)));
  Eq(1, gc.Size(), 0);
});

Test("Add_Ray3D_SizeIncreases", () => {
  var gc = new GeometryCollection3D();
  gc.Add(Ray3D.Make(new Point3D(0,0,0), new Vector3D(1,0,0)));
  Eq(1, gc.Size(), 0);
});

Test("Add_Polyline3D_SizeIncreases", () => {
  var gc = new GeometryCollection3D();
  gc.Add(Polyline3D.Make(new Point3D[] { new(0,0,0), new(1,0,0) }));
  Eq(1, gc.Size(), 0);
});

Test("Add_Polygon3D_SizeIncreases", () => {
  var gc = new GeometryCollection3D();
  gc.Add(Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) }));
  Eq(1, gc.Size(), 0);
});

Test("Add_Triangle3D_SizeIncreases", () => {
  var gc = new GeometryCollection3D();
  gc.Add(Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0)));
  Eq(1, gc.Size(), 0);
});

// ── Summary ───────────────────────────────────────────────────────────────────
Console.WriteLine($"\n{passed} passed, {failed} failed out of {passed + failed} tests.");
return failed > 0 ? 1 : 0;
