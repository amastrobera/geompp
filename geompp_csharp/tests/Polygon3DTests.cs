using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Polygon3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);
    double SumArea3D(System.Collections.Generic.IEnumerable<Triangle3D> triangles) => TestHelpers.SumArea3D(triangles);
    int CountOf<T>(System.Collections.Generic.IEnumerable<T> items) => TestHelpers.CountOf(items);

    // ── Polygon3D ─────────────────────────────────────────────────────────────────
    Console.WriteLine("\nPolygon3D");

    Test("Perimeter_Square", () => {
      var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      Eq(4.0, p.PerimeterSize());
    });

    Test("Perimeter_NonXYPlane", () => {
      var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(0,1,0), new(0,1,1), new(0,0,1) });
      Eq(4.0, p.PerimeterSize());
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

    Test("WithHoles_SelfIntersectingHole_Throws", () => {
      var outer = new Point3D[] { new(0,0,0), new(6,0,0), new(6,6,0), new(0,6,0) };
      var bowtieHole = new Point3D[] { new(1,1,0), new(3,1,0), new(1,3,0), new(3,3,0) };
      bool threw = false;
      try { Polygon3D.Make(outer, new[] { bowtieHole }); } catch { threw = true; }
      IsTrue(threw, "self-intersecting hole should throw");
    });

    Test("WithHoles_TwoHolesOverlap_Throws", () => {
      var outer  = new Point3D[] { new(0,0,0), new(10,0,0), new(10,10,0), new(0,10,0) };
      var holeA  = new Point3D[] { new(1,1,0), new(1,5,0), new(5,5,0), new(5,1,0) };
      var holeB  = new Point3D[] { new(3,3,0), new(3,7,0), new(7,7,0), new(7,3,0) }; // overlaps holeA
      bool threw = false;
      try { Polygon3D.Make(outer, new[] { holeA, holeB }); } catch { threw = true; }
      IsTrue(threw, "overlapping holes should throw");
    });

    Test("WithHoles_TwoHolesTouchAtVertex_DoesNotThrow", () => {
      var outer = new Point3D[] { new(0,0,0), new(10,0,0), new(10,10,0), new(0,10,0) };
      var holeA = new Point3D[] { new(3,3,0), new(3,5,0), new(5,5,0), new(5,3,0) };
      var holeB = new Point3D[] { new(5,5,0), new(5,7,0), new(7,7,0), new(7,5,0) };
      var poly  = Polygon3D.Make(outer, new[] { holeA, holeB });
      Eq(2, poly.Holes().Length);
    });

    Test("WithHoles_HoleStrikesThroughOuter_Throws", () => {
      var outer = new Point3D[] { new(0,0,0), new(10,0,0), new(10,10,0), new(0,10,0) };
      var hole  = new Point3D[] { new(8,4,0), new(8,6,0), new(12,6,0), new(12,4,0) };
      bool threw = false;
      try { Polygon3D.Make(outer, new[] { hole }); } catch { threw = true; }
      IsTrue(threw, "hole striking through outer boundary should throw");
    });

    Test("WithHoles_HoleFlushAgainstOuterEdge_DoesNotThrow", () => {
      var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
      var hole  = new Point3D[] { new(2,2,0), new(2,4,0), new(4,4,0), new(4,2,0) };
      var poly  = Polygon3D.Make(outer, new[] { hole });
      Eq(1, poly.Holes().Length);
    });

    Test("WithHoles_HoleEntirelyOutsideOuter_Throws", () => {
      // Hole never crosses or touches the outer boundary (same plane, disjoint region) — must be rejected
      // by the containment check even though the strikes-through check alone lets it slip by.
      var outer = new Point3D[] { new(0,0,0), new(10,0,0), new(10,10,0), new(0,10,0) };
      var hole  = new Point3D[] { new(20,20,0), new(20,22,0), new(22,22,0), new(22,20,0) };
      bool threw = false;
      try { Polygon3D.Make(outer, new[] { hole }); } catch { threw = true; }
      IsTrue(threw, "hole entirely outside outer boundary should throw");
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

    Test("IsOnPerimeter_OnEdge_True", () => {
      var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      IsTrue(sq.IsOnPerimeter(new Point3D(0,   0,   0)), "vertex (0,0,0)");
      IsTrue(sq.IsOnPerimeter(new Point3D(1,   0,   0)), "vertex (1,0,0)");
      IsTrue(sq.IsOnPerimeter(new Point3D(1,   1,   0)), "vertex (1,1,0)");
      IsTrue(sq.IsOnPerimeter(new Point3D(0,   1,   0)), "vertex (0,1,0)");
      IsTrue(sq.IsOnPerimeter(new Point3D(0.5, 0,   0)), "bottom edge midpoint");
      IsTrue(sq.IsOnPerimeter(new Point3D(1,   0.5, 0)), "right edge midpoint");
      IsTrue(sq.IsOnPerimeter(new Point3D(0.5, 1,   0)), "top edge midpoint");
      IsTrue(sq.IsOnPerimeter(new Point3D(0,   0.5, 0)), "left edge midpoint");
      var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
      var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
      var poly  = Polygon3D.Make(outer, new[] { hole });
      IsTrue(poly.IsOnPerimeter(new Point3D(2, 0, 0)), "outer bottom edge");
      IsTrue(poly.IsOnPerimeter(new Point3D(4, 2, 0)), "outer right edge");
      IsTrue(poly.IsOnPerimeter(new Point3D(2, 1, 0)), "hole bottom edge");
      IsTrue(poly.IsOnPerimeter(new Point3D(1, 2, 0)), "hole left edge");
    });

    Test("IsOnPerimeter_Interior_False", () => {
      var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      IsFalse(sq.IsOnPerimeter(new Point3D(0.5, 0.5, 0)),    "interior");
      IsFalse(sq.IsOnPerimeter(new Point3D(-0.1, 0.5, 0)),   "outside left");
      IsFalse(sq.IsOnPerimeter(new Point3D(0.5,  0.5, 0.01)), "off-plane");
      var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
      var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
      var poly  = Polygon3D.Make(outer, new[] { hole });
      IsFalse(poly.IsOnPerimeter(new Point3D(0.5, 0.5, 0)), "interior strip");
      IsFalse(poly.IsOnPerimeter(new Point3D(2,   2,   0)), "inside hole");
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

    Test("Perimeter3D_RoundTrip", () => {
      var pts = new Point3D[] { new(0,0,0), new(3,0,0), new(3,3,0), new(0,3,0) };
      var poly = Polygon3D.Make(pts);
      var back = poly.Perimeter();
      Eq(4, back.Length);
      for (int i = 0; i < pts.Length; ++i) {
        IsTrue(pts[i].AlmostEquals(back[i]), $"vertex {i} mismatch after Perimeter round-trip");
      }
    });

    Test("DistanceTo_UnitSquare", () => {
      var poly = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      Eq(0.0, poly.DistanceTo(new Point3D(0.5, 0.5, 0)));            // interior
      Eq(0.0, poly.DistanceTo(new Point3D(0.0, 0.5, 0)));            // on boundary (edge)
      Eq(0.0, poly.DistanceTo(new Point3D(1.0, 1.0, 0)));            // on boundary (vertex)
      Eq(1.0, poly.DistanceTo(new Point3D(2.0, 0.5, 0)));            // outside in-plane, nearest edge x=1
      Eq(Math.Sqrt(2.0), poly.DistanceTo(new Point3D(2.0, 2.0, 0))); // outside, nearest corner

      // off-plane: Contains() is false regardless of in-plane position, so this measures true 3D
      // distance to the nearest edge/vertex — even directly above the interior, there is no "inside"
      // shortcut, since distance is always measured to the boundary, never to a projected interior region.
      Eq(Math.Sqrt(1.25), poly.DistanceTo(new Point3D(0.5, 0.5, 1.0)));  // above center: 0.5 in-plane to
                                                                          // nearest edge + 1.0 perpendicular
      Eq(Math.Sqrt(2.0), poly.DistanceTo(new Point3D(2.0, 0.5, 1.0)));   // above + outside in-plane
    });

    Test("DistanceTo_WithHole", () => {
      // A point in the hole must measure to the HOLE's boundary, not the outer ring.
      var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
      var hole  = new Point3D[] { new(1,3,0), new(3,3,0), new(3,1,0), new(1,1,0) };
      var poly  = Polygon3D.Make(outer, new[] { hole });
      Eq(0.0, poly.DistanceTo(new Point3D(0.5, 0.5, 0)));  // in the solid region
      Eq(1.0, poly.DistanceTo(new Point3D(2.0, 2.0, 0)));  // hole center
      Eq(0.0, poly.DistanceTo(new Point3D(1.0, 2.0, 0)));  // on the hole boundary
    });

    Test("HasHoles3D_False_NoHoles", () => {
      var poly = Polygon3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) });
      IsFalse(poly.HasHoles());
      Eq(0, poly.Holes().Length);
    });

    Test("HasHoles3D_True_WithHole", () => {
      var outer = new Point3D[] { new(0,0,0), new(4,0,0), new(4,4,0), new(0,4,0) };
      var hole  = new Point3D[] { new(1,1,0), new(1,3,0), new(3,3,0), new(3,1,0) };
      var poly  = Polygon3D.Make(outer, new Point3D[][] { hole });
      IsTrue(poly.HasHoles());
      var holes = poly.Holes();
      Eq(1, holes.Length);
      Eq(4, holes[0].Length);
      for (int i = 0; i < hole.Length; ++i) {
        IsTrue(hole[i].AlmostEquals(holes[0][i]), $"hole vertex {i} mismatch");
      }
    });

    Test("FromWkt_Whitespace", () => {
      var sq = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      IsTrue(sq.AlmostEquals(Polygon3D.FromWkt("POLYGON ((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))")));
      IsTrue(sq.AlmostEquals(Polygon3D.FromWkt("POLYGON ((  0 0 0  ,  1 0 0  ,  1 1 0  ,  0 1 0  ,  0 0 0  ))")));
    });

    // ── Polygon3D (additional) ────────────────────────────────────────────────────
    Console.WriteLine("\nPolygon3D (additional)");

    Test("Area_UnitSquare", () => {
      var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      Eq(1.0, p.Area());
    });

    Test("Area_SelfIntersectingOuterInXYPlane", () => {
      var p = Polygon3D.Make(new Point3D[] { new(0,0,0), new(4,0,0), new(1,3,0), new(3,3,0) });
      IsFalse(p.IsSimple());
      Eq(5.0, p.Area());
    });

    Test("Area_SelfIntersectingOuterOnTiltedPlane_MatchesSimplifySum", () => {
      // On a non-axis-aligned plane a naive 2D-projected shoelace on the decomposed loops would be wrong
      // (foreshortening) — cross-validated against Simplify() + per-piece Area() instead of hand-deriving
      // the tilted value.
      Point3D Tilt(double x, double y) => new(x, y, 0.3 * x + 0.2 * y);
      var p = Polygon3D.Make(new Point3D[] { Tilt(0,0), Tilt(4,0), Tilt(1,3), Tilt(3,3) });
      IsFalse(p.IsSimple());
      double simplifyTotal = 0.0;
      foreach (var piece in p.Simplify()) {
        simplifyTotal += piece.Area();
      }
      Eq(simplifyTotal, p.Area());
    });

    Test("Centroid_SelfIntersectingOuterOnTiltedPlane_MatchesSimplifyWeightedAverage", () => {
      Point3D Tilt(double x, double y) => new(x, y, 0.3 * x + 0.2 * y);
      var p = Polygon3D.Make(new Point3D[] { Tilt(0,0), Tilt(4,0), Tilt(1,3), Tilt(3,3) });
      IsFalse(p.IsSimple());

      double totalArea = 0.0, wx = 0.0, wy = 0.0, wz = 0.0;
      foreach (var piece in p.Simplify()) {
        double a = piece.Area();
        var c = piece.Centroid();
        totalArea += a;
        wx += a * c.X;
        wy += a * c.Y;
        wz += a * c.Z;
      }

      var actual = p.Centroid();
      Eq(wx / totalArea, actual.X);
      Eq(wy / totalArea, actual.Y);
      Eq(wz / totalArea, actual.Z);
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

    Test("ToString_ContainsWkt", () => {
      var s = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) }).ToString();
      IsTrue(s.Contains("POLYGON"), "missing POLYGON keyword");
    });

    Test("Equality_SamePoly_True", () => {
      var a = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      var b = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      IsTrue(a == b);
    });

    Test("Equality_DifferentPoly_False", () => {
      var a = Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) });
      var b = Polygon3D.Make(new Point3D[] { new(0,0,0), new(2,0,0), new(2,2,0), new(0,2,0) });
      IsFalse(a == b);
    });

    Console.WriteLine("\nPolygon3D::IsConvex");
    {
        var square3d = Polygon3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(1,1,0), new Point3D(0,1,0)
        });
        var concave3d = Polygon3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(4,4,0),
            new Point3D(2,2,0), new Point3D(0,4,0)
        });
        Test("IsConvex_Square_True",   () => IsTrue(square3d.IsConvex()));
        Test("IsConvex_Concave_False", () => IsTrue(!concave3d.IsConvex()));
    }

    Console.WriteLine("\nPolygon3D::Simplify");
    {
        var simple3d = Polygon3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(2,0,0), new Point3D(2,2,0), new Point3D(0,2,0)
        });
        var bowtie3d = Polygon3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(4,0,0), new Point3D(1,3,0), new Point3D(3,3,0)
        });

        Test("Simplify_AlreadySimple_ReturnsOne", () => {
            var r = simple3d.Simplify();
            IsTrue(r.Length == 1, $"expected 1, got {r.Length}");
        });
        Test("Simplify_Bowtie_ReturnsTwoPolygons", () => {
            var r = bowtie3d.Simplify();
            IsTrue(r.Length == 2, $"expected 2, got {r.Length}");
        });
        Test("Simplify_Bowtie_ResultsAreSimple", () => {
            foreach (var p in bowtie3d.Simplify())
                IsTrue(p.IsSimple(), "result polygon not simple");
        });

        // XZ plane bowtie: dominant axis = Y, projection flips chirality
        var bowtieXZ = Polygon3D.Make(new[] {
            new Point3D(3,0,3), new Point3D(1,0,3), new Point3D(4,0,0), new Point3D(0,0,0)
        });
        Test("Simplify_BowtieXZ_ReturnsTwoPolygons", () => {
            var r = bowtieXZ.Simplify();
            IsTrue(r.Length == 2, $"expected 2, got {r.Length}");
        });
        Test("Simplify_BowtieXZ_ResultsAreSimple", () => {
            foreach (var p in bowtieXZ.Simplify())
                IsTrue(p.IsSimple(), "result polygon not simple");
        });
        Test("Simplify_Bowtie_AreasSum", () => {
            var r = bowtie3d.Simplify();
            double total = 0;
            foreach (var p in r) { total += p.Area(); }
            IsTrue(Math.Abs(total - 5.0) < 0.01, $"expected ~5.0, got {total}");
        });
        Test("Simplify_ResultsAreCoplanar", () => {
            foreach (var p in bowtie3d.Simplify()) {
                for (int i = 0; i < p.Size(); i++) {
                    IsTrue(Math.Abs(p[i].Z) < 1e-9, $"vertex z should be 0, got {p[i].Z}");
                }
            }
        });
    }

    Console.WriteLine("\nPolygon3D::Intersection");
    {
        var sq = Polygon3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(1,1,0), new Point3D(0,1,0)
        });
        var lineHit      = Line3D.Make(new Point3D(0.5, 0.5, -1), new Point3D(0.5, 0.5,  1));
        var lineMiss     = Line3D.Make(new Point3D(2,   0.5, -1), new Point3D(2,   0.5,  1));
        var lineParallel = Line3D.Make(new Point3D(0,   0,    1), new Point3D(1,   1,    1));
        var rayHit       = Ray3D.Make(new Point3D(0.5, 0.5,  1), new Vector3D(0, 0, -1));
        var rayAway      = Ray3D.Make(new Point3D(0.5, 0.5,  1), new Vector3D(0, 0,  1));
        var rayMiss      = Ray3D.Make(new Point3D(2,   0.5,  1), new Vector3D(0, 0, -1));
        var segHit       = LineSegment3D.Make(new Point3D(0.5, 0.5, -1), new Point3D(0.5, 0.5,  1));
        var segShort     = LineSegment3D.Make(new Point3D(0.5, 0.5,  0.5), new Point3D(0.5, 0.5, 1));
        var segMiss      = LineSegment3D.Make(new Point3D(2,   0.5, -1), new Point3D(2,   0.5,  1));

        Test("Intersects_Line_Hit",          () => IsTrue(sq.Intersects(lineHit)));
        Test("Intersects_Line_Miss",         () => IsTrue(!sq.Intersects(lineMiss)));
        Test("Intersects_Line_Parallel",     () => IsTrue(!sq.Intersects(lineParallel)));
        Test("Intersection_Line_Hit_Point",  () => {
            var r = sq.Intersection(lineHit);
            NotNull(r);
            Eq(0.5, r!.X, 3); Eq(0.5, r.Y, 3); Eq(0.0, r.Z, 3);
        });
        Test("Intersection_Line_Miss_Null",  () => IsNull(sq.Intersection(lineMiss)));

        Test("Intersects_Ray_Hit",           () => IsTrue(sq.Intersects(rayHit)));
        Test("Intersects_Ray_Away",          () => IsTrue(!sq.Intersects(rayAway)));
        Test("Intersects_Ray_Miss",          () => IsTrue(!sq.Intersects(rayMiss)));
        Test("Intersection_Ray_Hit_Point",   () => {
            var r = sq.Intersection(rayHit);
            NotNull(r);
            Eq(0.5, r!.X, 3); Eq(0.5, r.Y, 3); Eq(0.0, r.Z, 3);
        });
        Test("Intersection_Ray_Away_Null",   () => IsNull(sq.Intersection(rayAway)));

        Test("Intersects_Segment_Hit",       () => IsTrue(sq.Intersects(segHit)));
        Test("Intersects_Segment_Short",     () => IsTrue(!sq.Intersects(segShort)));
        Test("Intersects_Segment_Miss",      () => IsTrue(!sq.Intersects(segMiss)));
        Test("Intersection_Segment_Hit_Point", () => {
            var r = sq.Intersection(segHit);
            NotNull(r);
            Eq(0.5, r!.X, 3); Eq(0.5, r.Y, 3); Eq(0.0, r.Z, 3);
        });
        Test("Intersection_Segment_Short_Null", () => IsNull(sq.Intersection(segShort)));

        // Non-XY plane: YZ square at x=0; normal = +X
        var yz = Polygon3D.Make(new[] {
            new Point3D(0,0,0), new Point3D(0,1,0), new Point3D(0,1,1), new Point3D(0,0,1)
        });
        var lineThruYZ = Line3D.Make(new Point3D(-1, 0.5, 0.5), new Point3D(1, 0.5, 0.5));
        Test("Intersection_Line_YZPlane_Point", () => {
            var r = yz.Intersection(lineThruYZ);
            NotNull(r);
            Eq(0.0, r!.X, 3); Eq(0.5, r.Y, 3); Eq(0.5, r.Z, 3);
        });
    }

    // ── Polygon3D::Triangulate ───────────────────────────────────────────────────
    Test("Polygon3D_Triangulate_FlatConvexQuad_TwoTrianglesFullArea", () => {
      var p = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 2, 0), new(0, 2, 0) });
      var triangles = p.Triangulate();
      Eq(2, CountOf(triangles), 0);
      Eq(p.Area(), SumArea3D(triangles));
    });

    Test("Polygon3D_Triangulate_TiltedPlaneQuad_TwoTrianglesFullArea", () => {
      // Non-XY plane (X-dominant-axis normal) — proves the polygon's own plane normal is used.
      var p = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(0, 4, 0), new(0, 4, 2), new(0, 0, 2) });
      var triangles = p.Triangulate();
      Eq(2, CountOf(triangles), 0);
      Eq(p.Area(), SumArea3D(triangles));
    });
  }
}

}  // namespace GeomPPTests
