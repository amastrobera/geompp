using GeomPP;
using Geompp.Extensions;

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
  Eq(3.0, pl.DistanceTo(new Point2D(2, 3)));
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
  Eq(3.0, pl.DistanceTo(new Point3D(2, 3, 0)));
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

// ── Summary ───────────────────────────────────────────────────────────────────
Console.WriteLine($"\n{passed} passed, {failed} failed out of {passed + failed} tests.");
return failed > 0 ? 1 : 0;
