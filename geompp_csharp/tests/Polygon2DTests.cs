using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class Polygon2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);
    double SumArea2D(System.Collections.Generic.IEnumerable<Triangle2D> triangles) => TestHelpers.SumArea2D(triangles);
    int CountOf<T>(System.Collections.Generic.IEnumerable<T> items) => TestHelpers.CountOf(items);

    // ── Polygon2D ─────────────────────────────────────────────────────────────────
    Console.WriteLine("\nPolygon2D");

    Test("Perimeter_Square", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      Eq(4.0, p.PerimeterSize());
    });

    Test("Perimeter_Rectangle", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0,0), new(3,0), new(3,4), new(0,4) });
      Eq(14.0, p.PerimeterSize());
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

    Test("WithHoles_SelfIntersectingHole_Throws", () => {
      var outer = new Point2D[] { new(0,0), new(6,0), new(6,6), new(0,6) };
      // Bowtie hole (self-crossing): (1,1)->(3,1)->(1,3)->(3,3).
      var bowtieHole = new Point2D[] { new(1,1), new(3,1), new(1,3), new(3,3) };
      bool threw = false;
      try { Polygon2D.Make(outer, new[] { bowtieHole }); } catch { threw = true; }
      IsTrue(threw, "self-intersecting hole should throw");
    });

    Test("WithHoles_TwoHolesOverlap_Throws", () => {
      var outer  = new Point2D[] { new(0,0), new(10,0), new(10,10), new(0,10) };
      var holeA  = new Point2D[] { new(1,1), new(1,5), new(5,5), new(5,1) };
      var holeB  = new Point2D[] { new(3,3), new(3,7), new(7,7), new(7,3) }; // overlaps holeA
      bool threw = false;
      try { Polygon2D.Make(outer, new[] { holeA, holeB }); } catch { threw = true; }
      IsTrue(threw, "overlapping holes should throw");
    });

    Test("WithHoles_TwoHolesTouchAtVertex_DoesNotThrow", () => {
      var outer = new Point2D[] { new(0,0), new(10,0), new(10,10), new(0,10) };
      // Two squares touching diagonally at the single shared corner (5,5).
      var holeA = new Point2D[] { new(3,3), new(3,5), new(5,5), new(5,3) };
      var holeB = new Point2D[] { new(5,5), new(5,7), new(7,7), new(7,5) };
      var poly  = Polygon2D.Make(outer, new[] { holeA, holeB });
      Eq(2, poly.Holes().Length);
    });

    Test("WithHoles_TwoHolesDisjoint_DoesNotThrow", () => {
      var outer = new Point2D[] { new(0,0), new(10,0), new(10,10), new(0,10) };
      var holeA = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
      var holeB = new Point2D[] { new(6,6), new(6,8), new(8,8), new(8,6) };
      var poly  = Polygon2D.Make(outer, new[] { holeA, holeB });
      Eq(2, poly.Holes().Length);
    });

    Test("WithHoles_HoleStrikesThroughOuter_Throws", () => {
      var outer = new Point2D[] { new(0,0), new(10,0), new(10,10), new(0,10) };
      // Hole straddles the outer boundary at x=10: half inside, half poking out.
      var hole  = new Point2D[] { new(8,4), new(8,6), new(12,6), new(12,4) };
      bool threw = false;
      try { Polygon2D.Make(outer, new[] { hole }); } catch { threw = true; }
      IsTrue(threw, "hole striking through outer boundary should throw");
    });

    Test("WithHoles_HoleFlushAgainstOuterEdge_DoesNotThrow", () => {
      // Hole touches the outer boundary along a full edge, not just a vertex — a normal "notched corner"
      // shape, must not be rejected as "striking through".
      var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
      var hole  = new Point2D[] { new(2,2), new(2,4), new(4,4), new(4,2) };
      var poly  = Polygon2D.Make(outer, new[] { hole });
      Eq(1, poly.Holes().Length);
    });

    Test("WithHoles_HoleEntirelyOutsideOuter_Throws", () => {
      // Hole never crosses or touches the outer boundary, so the strikes-through check alone lets it slip
      // by, but it sits wholly outside the outer square.
      var outer = new Point2D[] { new(0,0), new(10,0), new(10,10), new(0,10) };
      var hole  = new Point2D[] { new(20,20), new(20,22), new(22,22), new(22,20) };
      bool threw = false;
      try { Polygon2D.Make(outer, new[] { hole }); } catch { threw = true; }
      IsTrue(threw, "hole entirely outside outer boundary should throw");
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

    Test("IsOnPerimeter_OnEdge_True", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsTrue(sq.IsOnPerimeter(new Point2D(0,   0)),   "vertex (0,0)");
      IsTrue(sq.IsOnPerimeter(new Point2D(1,   0)),   "vertex (1,0)");
      IsTrue(sq.IsOnPerimeter(new Point2D(1,   1)),   "vertex (1,1)");
      IsTrue(sq.IsOnPerimeter(new Point2D(0,   1)),   "vertex (0,1)");
      IsTrue(sq.IsOnPerimeter(new Point2D(0.5, 0)),   "bottom edge midpoint");
      IsTrue(sq.IsOnPerimeter(new Point2D(1,   0.5)), "right edge midpoint");
      IsTrue(sq.IsOnPerimeter(new Point2D(0.5, 1)),   "top edge midpoint");
      IsTrue(sq.IsOnPerimeter(new Point2D(0,   0.5)), "left edge midpoint");
      var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
      var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
      var poly  = Polygon2D.Make(outer, new[] { hole });
      IsTrue(poly.IsOnPerimeter(new Point2D(2, 0)), "outer bottom edge");
      IsTrue(poly.IsOnPerimeter(new Point2D(4, 2)), "outer right edge");
      IsTrue(poly.IsOnPerimeter(new Point2D(2, 1)), "hole bottom edge");
      IsTrue(poly.IsOnPerimeter(new Point2D(1, 2)), "hole left edge");
    });

    Test("IsOnPerimeter_Interior_False", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsFalse(sq.IsOnPerimeter(new Point2D(0.5, 0.5)),  "interior");
      IsFalse(sq.IsOnPerimeter(new Point2D(-0.1, 0.5)), "outside left");
      IsFalse(sq.IsOnPerimeter(new Point2D(1.1,  0.5)), "outside right");
      var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
      var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
      var poly  = Polygon2D.Make(outer, new[] { hole });
      IsFalse(poly.IsOnPerimeter(new Point2D(0.5, 0.5)), "interior strip");
      IsFalse(poly.IsOnPerimeter(new Point2D(2,   2)),   "inside hole");
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

    Test("DistanceTo_UnitSquare", () => {
      var poly = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      Eq(0.0, poly.DistanceTo(new Point2D(0.5, 0.5)));       // interior
      Eq(0.0, poly.DistanceTo(new Point2D(0.0, 0.5)));       // on boundary (edge)
      Eq(0.0, poly.DistanceTo(new Point2D(1.0, 1.0)));       // on boundary (vertex)
      Eq(1.0, poly.DistanceTo(new Point2D(2.0, 0.5)));       // outside, nearest edge x=1
      Eq(3.0, poly.DistanceTo(new Point2D(0.5, -3.0)));      // outside, nearest edge y=0
      Eq(Math.Sqrt(2.0), poly.DistanceTo(new Point2D(2.0, 2.0)));  // outside, nearest corner
    });

    Test("DistanceTo_WithHole", () => {
      // A point in the hole must measure to the HOLE's boundary, not the outer ring.
      var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
      var hole  = new Point2D[] { new(1,3), new(3,3), new(3,1), new(1,1) };
      var poly  = Polygon2D.Make(outer, new[] { hole });
      Eq(0.0, poly.DistanceTo(new Point2D(0.5, 0.5)));  // in the solid region
      Eq(1.0, poly.DistanceTo(new Point2D(2.0, 2.0)));  // hole center — 1 unit from any hole edge
      Eq(0.0, poly.DistanceTo(new Point2D(1.0, 2.0)));  // on the hole boundary
    });

    Test("Perimeter_RoundTrip", () => {
      var pts = new Point2D[] { new(0,0), new(3,0), new(3,3), new(0,3) };
      var poly = Polygon2D.Make(pts);
      var back = poly.Perimeter();
      Eq(4, back.Length);
      for (int i = 0; i < pts.Length; ++i) {
        IsTrue(pts[i].AlmostEquals(back[i]), $"vertex {i} mismatch after Perimeter round-trip");
      }
    });

    Test("HasHoles_False_NoHoles", () => {
      var poly = Polygon2D.Make(new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) });
      IsFalse(poly.HasHoles());
      Eq(0, poly.Holes().Length);
    });

    Test("HasHoles_True_WithHole", () => {
      var outer = new Point2D[] { new(0,0), new(4,0), new(4,4), new(0,4) };
      var hole  = new Point2D[] { new(1,1), new(1,3), new(3,3), new(3,1) };
      var poly  = Polygon2D.Make(outer, new Point2D[][] { hole });
      IsTrue(poly.HasHoles());
      var holes = poly.Holes();
      Eq(1, holes.Length);
      Eq(4, holes[0].Length);
      for (int i = 0; i < hole.Length; ++i) {
        IsTrue(hole[i].AlmostEquals(holes[0][i]), $"hole vertex {i} mismatch");
      }
    });

    Test("FromWkt_Whitespace", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsTrue(sq.AlmostEquals(Polygon2D.FromWkt("POLYGON ((0 0, 1 0, 1 1, 0 1, 0 0))")));
      IsTrue(sq.AlmostEquals(Polygon2D.FromWkt("POLYGON ((  0 0  ,  1 0  ,  1 1  ,  0 1  ,  0 0  ))")));
    });

    Console.WriteLine("\nPolygon2D::Intersection");

    // Polygon2D intersects Line2D → chord segment
    Test("Intersection_Line_PassesThrough_ReturnsChord", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var line = Line2D.Make(new Point2D(0, 0.5), new Point2D(1, 0.5));
      var r = sq.Intersection(line);
      NotNull(r);
      IsTrue(r is LineSegment2D[], "expected array<LineSegment2D>");
      var segs = (LineSegment2D[])r!;
      Eq(1, segs.Length);
    });

    Test("Intersection_Line_Misses_ReturnsNull", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var line = Line2D.Make(new Point2D(5, 0), new Point2D(5, 1));
      IsNull(sq.Intersection(line));
    });

    Test("Intersects_Line_True_And_False", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsTrue(sq.Intersects(Line2D.Make(new Point2D(0.5, -1), new Point2D(0.5, 2))));
      IsFalse(sq.Intersects(Line2D.Make(new Point2D(5, 0), new Point2D(5, 1))));
    });

    // Polygon2D intersects Ray2D
    Test("Intersection_Ray_Hits_ReturnsChord", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var ray = Ray2D.Make(new Point2D(-1, 0.5), new Vector2D(1, 0));
      var r = sq.Intersection(ray);
      NotNull(r);
      IsTrue(r is LineSegment2D[], "expected array<LineSegment2D>");
      var segs = (LineSegment2D[])r!;
      Eq(1, segs.Length);
    });

    Test("Intersection_Ray_PointingAway_ReturnsNull", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsNull(sq.Intersection(Ray2D.Make(new Point2D(5, 0.5), new Vector2D(1, 0))));
    });

    Test("Intersection_Ray_OriginInside_ReturnsClipped", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var ray = Ray2D.Make(new Point2D(0.5, 0.5), new Vector2D(1, 0));
      var r = sq.Intersection(ray);
      NotNull(r);
      IsTrue(r is LineSegment2D[]);
    });

    Test("Intersects_Ray_True_And_False", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsTrue(sq.Intersects(Ray2D.Make(new Point2D(-1, 0.5), new Vector2D(1, 0))));
      IsFalse(sq.Intersects(Ray2D.Make(new Point2D(5, 0.5), new Vector2D(1, 0))));
    });

    // Polygon2D intersects LineSegment2D
    Test("Intersection_Segment_Pierces_ReturnsChord", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var seg = LineSegment2D.Make(new Point2D(-0.5, 0.5), new Point2D(1.5, 0.5));
      var r = sq.Intersection(seg);
      NotNull(r);
      IsTrue(r is LineSegment2D[]);
      var segs = (LineSegment2D[])r!;
      Eq(1, segs.Length);
    });

    Test("Intersection_Segment_TooShort_ReturnsNull", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsNull(sq.Intersection(LineSegment2D.Make(new Point2D(-2, 0.5), new Point2D(-0.5, 0.5))));
    });

    Test("Intersection_Segment_EntirelyInside", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var seg = LineSegment2D.Make(new Point2D(0.2, 0.5), new Point2D(0.8, 0.5));
      var r = sq.Intersection(seg);
      NotNull(r);
      IsTrue(r is LineSegment2D[]);
    });

    Test("Intersects_Segment_True_And_False", () => {
      var sq = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsTrue(sq.Intersects(LineSegment2D.Make(new Point2D(-0.5, 0.5), new Point2D(1.5, 0.5))));
      IsFalse(sq.Intersects(LineSegment2D.Make(new Point2D(-2, 0.5), new Point2D(-0.5, 0.5))));
    });

    // ── Polygon2D (additional) ────────────────────────────────────────────────────
    Console.WriteLine("\nPolygon2D (additional)");

    Test("Area_UnitSquare", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      Eq(1.0, p.Area());
    });

    Test("Area_SelfIntersectingOuterWithHole", () => {
      // Bowtie outer (lobes 4.0 + 1.0 = 5.0 total covered area) with a small 1x0.3 hole safely inside the
      // larger lobe, away from the self-crossing — exercises Area()'s slow path together with its direct
      // per-hole subtraction.
      var outer = new Point2D[] { new(0,0), new(4,0), new(1,3), new(3,3) };
      var hole  = new Point2D[] { new(1.5,0.2), new(1.5,0.5), new(2.5,0.5), new(2.5,0.2) };
      var p = Polygon2D.Make(outer, new[] { hole });
      IsFalse(p.IsSimple());
      Eq(4.7, p.Area());
    });

    Test("Centroid_SelfIntersectingOuter_MatchesSimplifyWeightedAverage", () => {
      // Cross-validated against an independently-computed area-weighted average over Simplify()'s pieces,
      // rather than hand-deriving the expected centroid.
      var p = Polygon2D.Make(new Point2D[] { new(0,0), new(4,0), new(1,3), new(3,3) });
      IsFalse(p.IsSimple());

      double totalArea = 0.0, wx = 0.0, wy = 0.0;
      foreach (var piece in p.Simplify()) {
        double a = piece.Area();
        var c = piece.Centroid();
        totalArea += a;
        wx += a * c.X;
        wy += a * c.Y;
      }

      var actual = p.Centroid();
      Eq(wx / totalArea, actual.X);
      Eq(wy / totalArea, actual.Y);
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

    Test("ToString_ContainsWkt", () => {
      var s = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) }).ToString();
      IsTrue(s.Contains("POLYGON"), "missing POLYGON keyword");
    });

    Test("Equality_SamePoly_True", () => {
      var a = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var b = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      IsTrue(a == b);
    });

    Test("Equality_DifferentPoly_False", () => {
      var a = Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) });
      var b = Polygon2D.Make(new Point2D[] { new(0,0), new(2,0), new(2,2), new(0,2) });
      IsFalse(a == b);
    });

    Console.WriteLine("\nPolygon2D::IsConvex");
    {
        var square = Polygon2D.Make(new[] {
            new Point2D(0,0), new Point2D(1,0), new Point2D(1,1), new Point2D(0,1)
        });
        var concave = Polygon2D.Make(new[] {
            new Point2D(0,0), new Point2D(4,0), new Point2D(4,4),
            new Point2D(2,2), new Point2D(0,4)
        });
        var outer = new[] { new Point2D(0,0), new Point2D(4,0), new Point2D(4,4), new Point2D(0,4) };
        var hole  = new[] { new Point2D(1,1), new Point2D(1,2), new Point2D(2,2), new Point2D(2,1) };
        var holed = Polygon2D.Make(outer, new[] { hole });

        Test("IsConvex_Square_True",    () => IsTrue(square.IsConvex()));
        Test("IsConvex_Concave_False",  () => IsTrue(!concave.IsConvex()));
        Test("IsConvex_WithHole_False", () => IsTrue(!holed.IsConvex()));
    }

    Console.WriteLine("\nPolygon2D::Simplify");
    {
        var simple = Polygon2D.Make(new[] {
            new Point2D(0,0), new Point2D(2,0), new Point2D(2,2), new Point2D(0,2)
        });
        var bowtie = Polygon2D.Make(new[] {
            new Point2D(0,0), new Point2D(4,0), new Point2D(1,3), new Point2D(3,3)
        });

        Test("Simplify_AlreadySimple_ReturnsOne", () => {
            var r = simple.Simplify();
            IsTrue(r.Length == 1, $"expected 1, got {r.Length}");
        });
        Test("Simplify_Bowtie_ReturnsTwoPolygons", () => {
            var r = bowtie.Simplify();
            IsTrue(r.Length == 2, $"expected 2, got {r.Length}");
        });
        Test("Simplify_Bowtie_ResultsAreSimple", () => {
            foreach (var p in bowtie.Simplify())
                IsTrue(p.IsSimple(), "result polygon not simple");
        });
        Test("Simplify_Bowtie_AreasSum", () => {
            var r = bowtie.Simplify();
            double total = 0;
            foreach (var p in r) { total += p.Area(); }
            IsTrue(Math.Abs(total - 5.0) < 0.01, $"expected ~5.0, got {total}");
        });

        var wideBowtie = Polygon2D.Make(new[] {
            new Point2D(0,0), new Point2D(10,0), new Point2D(2,6), new Point2D(8,6)
        });
        Test("Simplify_WideBowtie_ReturnsTwoPolygons", () => {
            var r = wideBowtie.Simplify();
            IsTrue(r.Length == 2, $"expected 2, got {r.Length}");
        });
        Test("Simplify_WideBowtie_ResultsAreSimple", () => {
            foreach (var p in wideBowtie.Simplify())
                IsTrue(p.IsSimple(), "result polygon not simple");
        });
    }

    // ── Polygon2D::Triangulate ───────────────────────────────────────────────────
    // A valid triangulation must never let an original polygon vertex land in the MIDDLE of a triangle
    // edge (a "hanging"/T-junction vertex) -- every polygon vertex the triangulation touches must be an
    // actual corner (endpoint) of every triangle edge it lies on.
    void ExpectNoPolygonVertexHangsOnTriangleEdge(Polygon2D polygon, System.Collections.Generic.IEnumerable<Triangle2D> triangles) {
      var polygonPoints = polygon.Perimeter();
      int ti = 0;
      foreach (var t in triangles) {
        var v = t.Vertices();
        var edges = new (Point2D, Point2D)[] { (v.Item1, v.Item2), (v.Item2, v.Item3), (v.Item3, v.Item1) };
        for (int ei = 0; ei < edges.Length; ++ei) {
          var (a, b) = edges[ei];
          var seg = LineSegment2D.Make(a, b);
          foreach (var p in polygonPoints) {
            if (p.AlmostEquals(a) || p.AlmostEquals(b)) continue;  // p IS this edge's endpoint
            IsTrue(!seg.Contains(p), $"polygon vertex {p.ToWkt()} hangs on triangle {ti}'s edge {ei} "
                                    + $"({a.ToWkt()} -> {b.ToWkt()}) without being one of its endpoints");
          }
        }
        ++ti;
      }
    }

    // A valid triangulation must be edge-manifold: every oriented triangle edge (a "half-edge") either
    // has no twin at all (a polygon boundary edge, used by exactly one triangle) or has EXACTLY one twin
    // in another triangle traversing the same undirected edge in the OPPOSITE direction (twin.start ==
    // edge.end && twin.end == edge.start) -- the standard consequence of every triangle sharing the same
    // CCW winding.
    void ExpectHalfEdgesAreManifold(System.Collections.Generic.IEnumerable<Triangle2D> triangles) {
      var halfEdges = new System.Collections.Generic.List<(Point2D start, Point2D end, int ti, int ei)>();
      int idx = 0;
      foreach (var t in triangles) {
        var v = t.Vertices();
        halfEdges.Add((v.Item1, v.Item2, idx, 0));
        halfEdges.Add((v.Item2, v.Item3, idx, 1));
        halfEdges.Add((v.Item3, v.Item1, idx, 2));
        ++idx;
      }
      for (int i = 0; i < halfEdges.Count; ++i) {
        var e = halfEdges[i];
        var twins = new System.Collections.Generic.List<(Point2D start, Point2D end, int ti, int ei)>();
        for (int j = 0; j < halfEdges.Count; ++j) {
          if (j == i) continue;
          var o = halfEdges[j];
          bool sameUndirected = (e.start.AlmostEquals(o.start) && e.end.AlmostEquals(o.end))
                              || (e.start.AlmostEquals(o.end) && e.end.AlmostEquals(o.start));
          if (sameUndirected) twins.Add(o);
        }
        IsTrue(twins.Count <= 1, $"triangle {e.ti}'s edge {e.ei} ({e.start.ToWkt()} -> {e.end.ToWkt()}) "
                                + $"has {twins.Count} twins -- should have at most 1");
        if (twins.Count == 1) {
          var twin = twins[0];
          IsTrue(twin.start.AlmostEquals(e.end) && twin.end.AlmostEquals(e.start),
                 $"triangle {e.ti}'s edge {e.ei} ({e.start.ToWkt()} -> {e.end.ToWkt()}) and triangle "
               + $"{twin.ti}'s edge {twin.ei} ({twin.start.ToWkt()} -> {twin.end.ToWkt()}) share an "
               + "undirected edge but traverse it in the SAME direction");
        }
      }
    }

    Test("Polygon2D_Triangulate_ConvexQuad_TwoTrianglesFullArea", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 2), new(0, 2) });
      var triangles = p.Triangulate();
      Eq(2, CountOf(triangles), 0);
      Eq(p.Area(), SumArea2D(triangles));
      ExpectNoPolygonVertexHangsOnTriangleEdge(p, triangles);
      ExpectHalfEdgesAreManifold(triangles);
    });

    Test("Polygon2D_Triangulate_ConcavePolygon_CorrectAreaAndCount", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(2, 1), new(0, 4) });
      var triangles = p.Triangulate();
      Eq(3, CountOf(triangles), 0);
      Eq(p.Area(), SumArea2D(triangles));
      ExpectNoPolygonVertexHangsOnTriangleEdge(p, triangles);
      ExpectHalfEdgesAreManifold(triangles);
    });

    Test("Polygon2D_Triangulate_MonotonePolygonStrategy_Throws", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 2), new(0, 2) });
      bool threw = false;
      try { p.Triangulate(TriangulationStrategy.MonotonePolygon); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected MonotonePolygon strategy to throw (not yet implemented)");
    });

    Test("Polygon2D_Triangulate_DelaunayStrategy_Throws", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 2), new(0, 2) });
      bool threw = false;
      try { p.Triangulate(TriangulationStrategy.Delaunay); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected Delaunay strategy to throw (not yet implemented)");
    });

    // Regression test: this L-shape's reflex vertex (2, 2) sits exactly on the diagonal between the
    // non-adjacent vertices (0, 4) and (4, 0) (all three satisfy x + y == 4). A strict point-in-triangle
    // ear-validity check missed this collinear case and accepted a diagonal that actually exits the
    // polygon through the notch.
    Test("Polygon2D_Triangulate_ReflexVertexOnNonAdjacentDiagonal_StaysInsidePolygon", () => {
      var p = Polygon2D.Make(new Point2D[] {
          new(0, 0), new(4, 0), new(4, 2), new(2, 2), new(2, 4), new(0, 4) });
      var triangles = p.Triangulate();
      Eq(4, CountOf(triangles), 0);
      double total = 0.0;
      foreach (var t in triangles) {
        IsTrue(p.Contains(t.Centroid()), $"triangle {t.ToWkt()} strays outside the polygon");
        total += t.Area();
      }
      Eq(p.Area(), total);
      ExpectNoPolygonVertexHangsOnTriangleEdge(p, triangles);
      ExpectHalfEdgesAreManifold(triangles);
    });

    // 5-pointed star (same shape used in visual_doc_and_sample_code.md's Triangulation example) --
    // concave, with a reflex vertex at each of its 5 inner corners.
    Point2D[] StarPolygonPoints() => new Point2D[] {
        new(3.0, 6.0), new(2.29, 3.97), new(0.15, 3.93), new(1.86, 2.63), new(1.24, 0.57),
        new(3.0, 1.8), new(4.76, 0.57), new(4.14, 2.63), new(5.85, 3.93), new(3.71, 3.97) };

    Test("Polygon2D_Triangulate_FivePointedStar_TriangleAreasSumToPolygonArea", () => {
      var p = Polygon2D.Make(StarPolygonPoints());
      var triangles = new List<Triangle2D>(p.Triangulate());
      Eq(8, triangles.Count, 0);
      double total = 0.0;
      foreach (var t in triangles) {
        IsTrue(p.Contains(t.Centroid()), $"triangle {t.ToWkt()} strays outside the polygon");
        total += t.Area();
      }
      Eq(p.Area(), total);
      ExpectNoPolygonVertexHangsOnTriangleEdge(p, triangles);
      ExpectHalfEdgesAreManifold(triangles);
    });

    Test("Polygon2D_Triangulate_FivePointedStar_TrianglesExactlyTileWithNoOverlap", () => {
      // Stronger companion to the area-sum test above: area-sum-equals-polygon-area alone can't rule out
      // two triangles overlapping while a third has a compensating gap (areas would still cancel out).
      // Proves an exact tiling two ways: (1) no pair of triangles shares more than an edge/vertex, and
      // (2) a dense sampling grid over the bounding box confirms "inside the polygon" and "inside some
      // triangle" are the same set of points (skipping a thin margin around every edge, since
      // Polygon2D.Contains()/Triangle2D.Contains() use different algorithms and can disagree right at a
      // shared boundary -- that's boundary-inclusion noise, not a real gap/overlap).
      var p = Polygon2D.Make(StarPolygonPoints());
      var triangles = new List<Triangle2D>(p.Triangulate());
      Eq(8, triangles.Count, 0);

      // Triangle2D.Intersection(Triangle2D) isn't bound in C# (only the Line2D overload is), so go
      // through ToPolygon().Intersection() instead -- Polygon2D.Intersection(Polygon2D) is bound.
      for (int i = 0; i < triangles.Count; ++i) {
        for (int j = i + 1; j < triangles.Count; ++j) {
          var pieces = triangles[i].ToPolygon().Intersection(triangles[j].ToPolygon());
          double overlapArea = 0.0;
          foreach (var piece in pieces) overlapArea += piece.Area();
          IsTrue(overlapArea < 1e-9, $"triangles {i} and {j} overlap with real area ({overlapArea})");
        }
      }

      var edges = new List<LineSegment2D>();
      var perimeter = p.Perimeter();
      for (int k = 0; k < perimeter.Length; ++k) {
        edges.Add(LineSegment2D.Make(perimeter[k], perimeter[(k + 1) % perimeter.Length]));
      }
      foreach (var t in triangles) {
        var v = t.Vertices();
        edges.Add(LineSegment2D.Make(v.Item1, v.Item2));
        edges.Add(LineSegment2D.Make(v.Item2, v.Item3));
        edges.Add(LineSegment2D.Make(v.Item3, v.Item1));
      }

      const double margin = 0.02;
      const int steps = 97;  // prime step count so the grid never aligns with the star's straight edges
      int mismatches = 0, skipped = 0;
      for (int ix = 0; ix < steps; ++ix) {
        for (int iy = 0; iy < steps; ++iy) {
          double x = 6.0 * (ix + 0.37) / steps;
          double y = 6.0 * (iy + 0.61) / steps;
          var pt = new Point2D(x, y);

          bool nearEdge = false;
          foreach (var e in edges) {
            if (e.DistanceTo(pt) < margin) { nearEdge = true; break; }
          }
          if (nearEdge) { skipped++; continue; }

          bool inPoly = p.Contains(pt);
          bool inAnyTri = false;
          foreach (var t in triangles) {
            if (t.Contains(pt)) { inAnyTri = true; break; }
          }
          if (inPoly != inAnyTri) mismatches++;
        }
      }
      IsTrue(skipped < steps * steps, "every grid point was too close to an edge -- margin too large?");
      Eq(0, mismatches, 0);
    });

    Test("Polygon2D_Triangulate_CombPolygon_ProducesCorrectAreaAndCount", () => {
      // A 3-tooth "comb" -- the classic adversarial shape for naive ear-clipping (deep, narrow notches
      // between tall teeth). Unlike the other shapes tested here, this one genuinely needs multiple laps
      // around the ring before it fully triangulates (see the visual doc's Triangulation section for why).
      var p = Polygon2D.Make(new Point2D[] {
          new(5, 0), new(5, 10), new(4, 10), new(4, 9), new(3, 9), new(3, 10),
          new(2, 10), new(2, 9), new(1, 9), new(1, 10), new(0, 10), new(0, 0) });
      var triangles = p.Triangulate();
      Eq(10, CountOf(triangles), 0);
      Eq(p.Area(), SumArea2D(triangles));
      ExpectNoPolygonVertexHangsOnTriangleEdge(p, triangles);
      ExpectHalfEdgesAreManifold(triangles);
    });
  }
}

}  // namespace GeomPPTests
