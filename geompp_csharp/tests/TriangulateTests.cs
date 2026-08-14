using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class TriangulateTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);
    double SumArea2D(System.Collections.Generic.IEnumerable<Triangle2D> triangles) => TestHelpers.SumArea2D(triangles);
    double SumArea3D(System.Collections.Generic.IEnumerable<Triangle3D> triangles) => TestHelpers.SumArea3D(triangles);
    int CountOf<T>(System.Collections.Generic.IEnumerable<T> items) => TestHelpers.CountOf(items);

    // ── GeomUtil.Triangulate (free function) ──────────────────────────────────────
    Console.WriteLine("\nTriangulate (free function)");

    Test("GeomUtil_Triangulate_2D_DefaultSettings", () => {
      var pts = new System.Collections.Generic.List<Point2D> { new(0, 0), new(4, 0), new(4, 2), new(0, 2) };
      var triangles = GeomUtil.Triangulate(pts, new TriangulationParams());
      Eq(2, CountOf(triangles), 0);
      Eq(8.0, SumArea2D(triangles));
    });

    Test("GeomUtil_Triangulate_3D_WithExplicitNormal", () => {
      var pts = new System.Collections.Generic.List<Point3D> { new(0, 0, 0), new(4, 0, 0), new(4, 2, 0), new(0, 2, 0) };
      var triangles = GeomUtil.Triangulate(pts, new Vector3D(0, 0, 1), new TriangulationParams());
      Eq(2, CountOf(triangles), 0);
      Eq(8.0, SumArea3D(triangles));
    });

    Test("GeomUtil_Triangulate_3D_WithoutNormal_FitsViaPCA", () => {
      var pts = new System.Collections.Generic.List<Point3D> { new(0, 0, 0), new(4, 0, 0), new(4, 2, 0), new(0, 2, 0) };
      var triangles = GeomUtil.Triangulate(pts, new TriangulationParams());
      Eq(2, CountOf(triangles), 0);
      Eq(8.0, SumArea3D(triangles));
    });

    Test("GeomUtil_Triangulate_FewerThanThreePoints_Throws", () => {
      var pts = new System.Collections.Generic.List<Point2D> { new(0, 0), new(1, 0) };
      bool threw = false;
      try { var _ = GeomUtil.Triangulate(pts, new TriangulationParams()); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected fewer than 3 points to throw");
    });

    Test("GeomUtil_Triangulate_WindingAssert_ThrowsOnClockwiseInput", () => {
      var cwSquare = new System.Collections.Generic.List<Point2D> { new(0, 0), new(0, 1), new(1, 1), new(1, 0) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Guaranteed,
                                             TriangulationWinding.Assert, TriangulationCollinearity.Guaranteed);
      bool threw = false;
      try { var _ = GeomUtil.Triangulate(cwSquare, settings); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected clockwise input to throw under Winding.Assert");
    });

    Test("GeomUtil_Triangulate_WindingEnforce_FixesClockwiseInput", () => {
      var cwSquare = new System.Collections.Generic.List<Point2D> { new(0, 0), new(0, 1), new(1, 1), new(1, 0) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Guaranteed,
                                             TriangulationWinding.Enforce, TriangulationCollinearity.Guaranteed);
      var triangles = GeomUtil.Triangulate(cwSquare, settings);
      Eq(2, CountOf(triangles), 0);
      Eq(1.0, SumArea2D(triangles));
    });

    Test("GeomUtil_Triangulate_CollinearityAssert_ThrowsOnCollinearPoint", () => {
      var withCollinear = new System.Collections.Generic.List<Point2D> { new(0, 0), new(2, 0), new(4, 0), new(4, 4), new(0, 4) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Guaranteed,
                                             TriangulationWinding.Guaranteed, TriangulationCollinearity.Assert);
      bool threw = false;
      try { var _ = GeomUtil.Triangulate(withCollinear, settings); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected collinear point to throw under Collinearity.Assert");
    });

    Test("GeomUtil_Triangulate_CollinearityEnforce_RemovesCollinearPoint", () => {
      var withCollinear = new System.Collections.Generic.List<Point2D> { new(0, 0), new(2, 0), new(4, 0), new(4, 4), new(0, 4) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Guaranteed,
                                             TriangulationWinding.Guaranteed, TriangulationCollinearity.Enforce);
      var triangles = GeomUtil.Triangulate(withCollinear, settings);
      Eq(2, CountOf(triangles), 0);
      Eq(16.0, SumArea2D(triangles));
    });

    Test("GeomUtil_Triangulate_SimplicityAssert_ThrowsOnSelfIntersectingInput", () => {
      var bowtie = new System.Collections.Generic.List<Point2D> { new(0, 0), new(1, 0), new(0, 1), new(1, 1) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Assert,
                                             TriangulationWinding.Guaranteed, TriangulationCollinearity.Guaranteed);
      bool threw = false;
      try { var _ = GeomUtil.Triangulate(bowtie, settings); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected self-intersecting input to throw under Simplicity.Assert");
    });

    // Regression tests: a 4x4 square with a redundant vertex (2, 0) sitting exactly on the middle of
    // the bottom edge -- neither reflex nor a normal convex ear. With collinearity Guaranteed (skip the
    // check, so the redundant point survives into ear_clipping_triangulation), a stale index bug in the
    // collinear-vertex skip branch could previously hang the algorithm forever.
    Test("GeomUtil_Triangulate_CollinearityGuaranteed_WithMidpointVertex_DoesNotHang", () => {
      var squareWithMidpoint = new System.Collections.Generic.List<Point2D> {
          new(0, 0), new(2, 0), new(4, 0), new(4, 4), new(0, 4) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Guaranteed,
                                             TriangulationWinding.Guaranteed, TriangulationCollinearity.Guaranteed);
      var triangles = GeomUtil.Triangulate(squareWithMidpoint, settings);
      Eq(3, CountOf(triangles), 0);
      Eq(16.0, SumArea2D(triangles));
    });

    Test("GeomUtil_Triangulate_CollinearityGuaranteed_WithMidpointAtRingStart_DoesNotHang", () => {
      // Same shape, rotated so the collinear vertex is at index 0 -- the exact case that hung.
      var squareWithMidpoint = new System.Collections.Generic.List<Point2D> {
          new(2, 0), new(4, 0), new(4, 4), new(0, 4), new(0, 0) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Guaranteed,
                                             TriangulationWinding.Guaranteed, TriangulationCollinearity.Guaranteed);
      var triangles = GeomUtil.Triangulate(squareWithMidpoint, settings);
      Eq(3, CountOf(triangles), 0);
      Eq(16.0, SumArea2D(triangles));
    });

    Test("TriangulationParams_DefaultStrategy_IsEarClippingBestFit", () => {
      var settings = new TriangulationParams();
      IsTrue(settings.Strategy == TriangulationStrategy.EarClippingBestFit,
             $"expected default strategy EarClippingBestFit, got {settings.Strategy}");
    });

    Test("GeomUtil_Triangulate_EarClippingBestFitStrategy_Succeeds", () => {
      var pts = new System.Collections.Generic.List<Point2D> { new(0, 0), new(4, 0), new(4, 2), new(0, 2) };
      var settings = new TriangulationParams(TriangulationStrategy.EarClippingBestFit, TriangulationSimplicity.Enforce,
                                             TriangulationWinding.Enforce, TriangulationCollinearity.Enforce);
      var triangles = GeomUtil.Triangulate(pts, settings);
      Eq(2, CountOf(triangles), 0);
      Eq(8.0, SumArea2D(triangles));
    });

    Test("GeomUtil_Triangulate_EarClippingBestFit_PicksDifferentDiagonalsThanPlainEarClipping", () => {
      // A 5-pointed star: EarClipping (first valid ear in scan order) fans every triangle out from one
      // vertex; EarClippingBestFit (best-scoring valid ear each step) clips all 5 outer points first,
      // then fans only the remaining inner pentagon. Both are valid triangulations of the same polygon
      // -- same triangle count and total area -- but via genuinely different diagonals.
      var star = new System.Collections.Generic.List<Point2D> {
          new(3.0, 6.0), new(2.29, 3.97), new(0.15, 3.93), new(1.86, 2.63), new(1.24, 0.57),
          new(3.0, 1.8), new(4.76, 0.57), new(4.14, 2.63), new(5.85, 3.93), new(3.71, 3.97) };

      var plainSettings = new TriangulationParams(TriangulationStrategy.EarClipping, TriangulationSimplicity.Enforce,
                                                   TriangulationWinding.Enforce, TriangulationCollinearity.Enforce);
      var bestFitSettings = new TriangulationParams(TriangulationStrategy.EarClippingBestFit, TriangulationSimplicity.Enforce,
                                                     TriangulationWinding.Enforce, TriangulationCollinearity.Enforce);

      var plain = GeomUtil.Triangulate(star, plainSettings);
      var bestFit = GeomUtil.Triangulate(star, bestFitSettings);

      Eq(8, CountOf(plain), 0);
      Eq(8, CountOf(bestFit), 0);
      Eq(SumArea2D(plain), SumArea2D(bestFit));

      var plainWkt = new System.Collections.Generic.HashSet<string>();
      foreach (var t in plain) plainWkt.Add(t.ToWkt());
      var bestFitWkt = new System.Collections.Generic.HashSet<string>();
      foreach (var t in bestFit) bestFitWkt.Add(t.ToWkt());
      IsFalse(plainWkt.SetEquals(bestFitWkt), "expected EarClipping and EarClippingBestFit to pick different diagonals");
    });

    // ── mesh-conformity checking ("every edge has at most 1 neighbor") ────────────────────────────
    Console.WriteLine("\nGeomUtil.ValidateAdjacency / FixAdjacency (batch Triangulate)");

    Test("GeomUtil_ValidateAdjacency_ConformingSharedEdge_ReturnsNoViolations", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var violations = GeomUtil.ValidateAdjacency(new[] { p0, p1 });
      Eq(0, CountOf(violations), 0);
    });

    Test("GeomUtil_ValidateAdjacency_TJunction_DetectsViolation", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var roof = Polygon2D.Make(new Point2D[] { new(0, 1), new(2, 1), new(1, 2) });
      var violations = GeomUtil.ValidateAdjacency(new[] { p0, p1, roof });
      bool anyFound = false;
      bool allTJunctions = true;
      foreach (var v in violations) { anyFound = true; if (v.IsNonManifold) allTJunctions = false; }
      IsTrue(anyFound, "expected at least one T-junction violation");
      IsTrue(allTJunctions, "expected every violation to be a T-junction, not non-manifold");
    });

    Test("GeomUtil_ValidateAdjacency_NonManifoldEdge_DetectsViolation", () => {
      var a = Triangle2D.Make(new(0, 0), new(1, 0), new(0.5, 1));
      var b = Triangle2D.Make(new(1, 0), new(0, 0), new(0.5, -1));
      var c = Triangle2D.Make(new(0, 0), new(1, 0), new(0.5, -2));
      var violations = GeomUtil.ValidateAdjacency(new[] { a, b, c });
      AdjacencyViolation2D first = null;
      foreach (var v in violations) { first = v; break; }
      NotNull(first, "expected at least one violation");
      IsTrue(first.IsNonManifold, "expected a non-manifold-edge violation");
      Eq(3, CountOf(first.FacetIndices), 0);
    });

    Test("GeomUtil_FixAdjacency_TJunction_SplitsFacetAndPreservesTotalArea", () => {
      // roof has exactly 1 T-junction (the shared p0/p1 corner (1,1) lands on its base edge).
      // FixAdjacency() cuts a diagonal from that spliced vertex to its nearest valid ring vertex --
      // here that's roof's apex, so roof splits clean into 2 triangles instead of growing into a
      // single 4-vertex flat-vertex-laden ring.
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var roof = Polygon2D.Make(new Point2D[] { new(0, 1), new(2, 1), new(1, 2) });
      var facets = new[] { p0, p1, roof };
      double areaBefore = p0.Area() + p1.Area() + roof.Area();

      var fixedRings = GeomUtil.FixAdjacency(facets);

      Eq(4, fixedRings.Length, 0);  // p0, p1 pass through unchanged, roof splits into 2
      foreach (var ring in fixedRings)
        IsTrue(ring.Length <= 4, "expected no ring to grow into a flat-vertex-laden polygon");

      var guaranteedCollinearity = new TriangulationParams(TriangulationStrategy.EarClippingBestFit,
          TriangulationSimplicity.Guaranteed, TriangulationWinding.Guaranteed, TriangulationCollinearity.Guaranteed);
      double areaAfter = 0.0;
      foreach (var ring in fixedRings) {
        var points = new System.Collections.Generic.List<Point2D>(ring);
        foreach (var t in GeomUtil.Triangulate(points, guaranteedCollinearity))
          areaAfter += t.Area();
      }
      Eq(areaBefore, areaAfter);
    });

    Test("GeomUtil_FixAdjacency_NonManifoldEdge_Throws", () => {
      var a = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(0.5, 1) });
      var b = Polygon2D.Make(new Point2D[] { new(1, 0), new(0, 0), new(0.5, -1) });
      var c = Polygon2D.Make(new Point2D[] { new(1, 0), new(0, 0), new(0.5, -2) });
      bool threw = false;
      try { GeomUtil.FixAdjacency(new[] { a, b, c }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a non-manifold edge to throw");
    });

    Test("GeomUtil_FixAdjacency_MultipleTJunctionsOnOneEdge_SplitsIntoSeveralPieces", () => {
      // The "T-junction house": a wide base spanning both squares plus their side overhangs, with 3
      // foreign vertices ((0,0), (1,0), (2,0)) landing on its single top edge -- each cuts its own
      // diagonal, carving the base into 4 rectangular strips instead of one 7-vertex polygon.
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var baseFacet = Polygon2D.Make(new Point2D[] { new(-0.5, -1.2), new(2.5, -1.2), new(2.5, 0), new(-0.5, 0) });
      var facets = new[] { baseFacet, p0, p1 };
      double areaBefore = baseFacet.Area() + p0.Area() + p1.Area();

      var fixedRings = GeomUtil.FixAdjacency(facets);

      Eq(6, fixedRings.Length, 0);  // base -> 4 strips, p0/p1 pass through unchanged (2)
      foreach (var ring in fixedRings)
        IsTrue(ring.Length <= 4, "expected no ring to grow into a flat-vertex-laden polygon");

      var guaranteedCollinearity = new TriangulationParams(TriangulationStrategy.EarClippingBestFit,
          TriangulationSimplicity.Guaranteed, TriangulationWinding.Guaranteed, TriangulationCollinearity.Guaranteed);
      double areaAfter = 0.0;
      foreach (var ring in fixedRings) {
        var points = new System.Collections.Generic.List<Point2D>(ring);
        foreach (var t in GeomUtil.Triangulate(points, guaranteedCollinearity))
          areaAfter += t.Area();
      }
      Eq(areaBefore, areaAfter);
    });

    Test("GeomUtil_FixAdjacency_TriangleTJunction_ReTriangulatesAndPreservesTotalArea", () => {
      // Big triangle A sitting on two small triangles B, C -- B and C's shared vertex (2,0) lies in
      // the interior of A's base edge (0,0)-(4,0), a T-junction. A can't just absorb (2,0) and stay a
      // triangle, so FixAdjacency() must re-triangulate it into 2 triangles covering the same area.
      var a = Triangle2D.Make(new(0, 0), new(4, 0), new(2, 3));
      var b = Triangle2D.Make(new(0, 0), new(1, -1.5), new(2, 0));
      var c = Triangle2D.Make(new(2, 0), new(3, -1.5), new(4, 0));
      var facets = new[] { a, b, c };
      double areaBefore = a.Area() + b.Area() + c.Area();
      Eq(9.0, areaBefore);

      var fixedTriangles = GeomUtil.FixAdjacency(facets);
      Eq(4, fixedTriangles.Length, 0);  // a -> 2 triangles, b and c pass through unchanged
      Eq(0, CountOf(GeomUtil.ValidateAdjacency(fixedTriangles)), 0);

      double areaAfter = SumArea2D(fixedTriangles);
      Eq(areaBefore, areaAfter);
    });

    Test("GeomUtil_FixAdjacency_TriangleNonManifoldEdge_Throws", () => {
      var a = Triangle2D.Make(new(0, 0), new(1, 0), new(0.5, 1));
      var b = Triangle2D.Make(new(1, 0), new(0, 0), new(0.5, -1));
      var c = Triangle2D.Make(new(1, 0), new(0, 0), new(0.5, -2));
      bool threw = false;
      try { GeomUtil.FixAdjacency(new[] { a, b, c }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a non-manifold edge to throw");
    });

    Test("GeomUtil_Triangulate_PolygonBatch_DefaultEnforce_FixesTJunctionAndTriangulates", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var roof = Polygon2D.Make(new Point2D[] { new(0, 1), new(2, 1), new(1, 2) });
      var triangles = GeomUtil.Triangulate(new[] { p0, p1, roof },
          new TriangulationParams(TriangulationStrategy.EarClippingBestFit, TriangulationSimplicity.Enforce,
              TriangulationWinding.Enforce, TriangulationCollinearity.Enforce, AdjacencyConformity.Enforce));
      Eq(3.0, SumArea2D(triangles));
    });

    Test("GeomUtil_Triangulate_PolygonBatch_Assert_ThrowsOnTJunction", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var roof = Polygon2D.Make(new Point2D[] { new(0, 1), new(2, 1), new(1, 2) });
      bool threw = false;
      try {
        GeomUtil.Triangulate(new[] { p0, p1, roof },
            new TriangulationParams(TriangulationStrategy.EarClippingBestFit, TriangulationSimplicity.Enforce,
                TriangulationWinding.Enforce, TriangulationCollinearity.Enforce, AdjacencyConformity.Assert));
      }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected Assert to throw on a T-junction");
    });
  }
}

}  // namespace GeomPPTests
