using GeomPP;
using System.Collections.Generic;

namespace GeomPPTests {

public static class PolygonizeTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    int CountOf<T>(IEnumerable<T> items) => TestHelpers.CountOf(items);

    List<Triangle2D> GridTriangles(int rows, int cols, HashSet<(int, int)>? skip = null) {
      skip ??= new HashSet<(int, int)>();
      var triangles = new List<Triangle2D>();
      for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
          if (skip.Contains((r, c))) continue;
          var p00 = new Point2D(c, r);
          var p10 = new Point2D(c + 1, r);
          var p11 = new Point2D(c + 1, r + 1);
          var p01 = new Point2D(c, r + 1);
          triangles.Add(Triangle2D.Make(p00, p10, p11));
          triangles.Add(Triangle2D.Make(p00, p11, p01));
        }
      }
      return triangles;
    }

    // ── GeomUtil.Polygonize (free function) ───────────────────────────────────────
    Console.WriteLine("\nPolygonize (free function)");

    Test("GeomUtil_Polygonize_DefaultStrategyIsHertelMehlhorn", () => {
      IsTrue(new PolygonizationParams().Strategy == PolygonizationStrategy.HertelMehlhorn);
    });

    Test("GeomUtil_Polygonize_UnitSquareFromTwoTriangles_ReturnsSingleQuad", () => {
      var polys = GeomUtil.Polygonize(GridTriangles(1, 1).ToArray(), new PolygonizationParams());
      Eq(1, CountOf(polys), 0);
      var poly = System.Linq.Enumerable.First(polys);
      Eq(4, poly.Size(), 0);
      Eq(1.0, poly.Area());
    });

    Test("GeomUtil_Polygonize_PlanarBoundaryExtraction_GridWithCenterHole", () => {
      var settings = new PolygonizationParams(PolygonizationStrategy.PlanarBoundaryExtraction);
      var triangles = GridTriangles(3, 3, new HashSet<(int, int)> { (1, 1) });
      var polys = GeomUtil.Polygonize(triangles.ToArray(), settings);
      Eq(1, CountOf(polys), 0);
      var poly = System.Linq.Enumerable.First(polys);
      Eq(8.0, poly.Area());  // 9 (outer) - 1 (hole)
    });

    Test("GeomUtil_Polygonize_PlanarQuads_TwoTriangles_ReturnsSingleQuad", () => {
      var settings = new PolygonizationParams(PolygonizationStrategy.PlanarQuads);
      var polys = GeomUtil.Polygonize(GridTriangles(1, 1).ToArray(), settings);
      Eq(1, CountOf(polys), 0);
      var poly = System.Linq.Enumerable.First(polys);
      Eq(4, poly.Size(), 0);
    });

    Test("GeomUtil_Polygonize_HertelMehlhorn_2x2Grid_MergesIntoOneConvexPiece", () => {
      var settings = new PolygonizationParams(PolygonizationStrategy.HertelMehlhorn);
      var polys = GeomUtil.Polygonize(GridTriangles(2, 2).ToArray(), settings);
      Eq(1, CountOf(polys), 0);
      var poly = System.Linq.Enumerable.First(polys);
      Eq(4.0, poly.Area());
    });

    Test("GeomUtil_Polygonize_LShape_HertelMehlhorn_PreservesSharedTJunctionVertex", () => {
      // 2x2 grid, top-left cell skipped -- an L-shape with a reflex vertex at (1, 1). HertelMehlhorn
      // returns 2 convex pieces (a 2x1 rectangle and a 1x1 square) whose shared corner sits exactly at
      // the midpoint of the rectangle's top edge. Regression test: Polygonize() must not silently drop
      // that vertex as collinear-on-its-own-ring, or Mesh2D.Polygonize()/PolyMesh2D would reject the
      // result as a T-junction once welded together (see MeshTests.cs's mirror of this same case).
      var triangles = GridTriangles(2, 2, new HashSet<(int, int)> { (1, 0) });
      var polys = GeomUtil.Polygonize(triangles.ToArray(), new PolygonizationParams());
      Eq(2, CountOf(polys), 0);

      Polygon2D? rectangle = null;
      foreach (var p in polys) {
        if (System.Math.Abs(p.Area() - 2.0) < 1e-9) rectangle = p;
      }
      IsTrue(rectangle != null, "expected a piece with area 2.0");
      Eq(6, rectangle!.Size(), 0);  // 4 real corners + (1,0) and (1,1), deliberately not simplified
      bool hasMidpoint = false;
      foreach (var v in rectangle.Perimeter()) {
        if (v.AlmostEquals(new Point2D(1, 1))) hasMidpoint = true;
      }
      IsTrue(hasMidpoint, "expected the rectangle to keep (1,1) as an explicit vertex");
    });

    Test("GeomUtil_Polygonize_EmptyInput_Throws", () => {
      bool threw = false;
      try { GeomUtil.Polygonize(new Triangle2D[] { }, new PolygonizationParams()); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected empty triangle list to throw");
    });

    Test("GeomUtil_Polygonize_NonManifoldInput_Throws", () => {
      var a = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0.5, 1));
      var b = Triangle2D.Make(new Point2D(1, 0), new Point2D(0, 0), new Point2D(0.5, -1));
      var c = Triangle2D.Make(new Point2D(1, 0), new Point2D(0, 0), new Point2D(0.5, -2));
      bool threw = false;
      try { GeomUtil.Polygonize(new[] { a, b, c }, new PolygonizationParams()); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected non-manifold input to throw");
    });

    Test("GeomUtil_Polygonize_3D_TiltedSquareFromTwoTriangles", () => {
      var p00 = new Point3D(0, 0, 0);
      var p10 = new Point3D(1, 0, 1);
      var p11 = new Point3D(1, 1, 1);
      var p01 = new Point3D(0, 1, 0);
      var triangles = new[] { Triangle3D.Make(p00, p10, p11), Triangle3D.Make(p00, p11, p01) };
      var polys = GeomUtil.Polygonize(triangles, new PolygonizationParams());
      Eq(1, CountOf(polys), 0);
      var poly = System.Linq.Enumerable.First(polys);
      Eq(4, poly.Size(), 0);
      Eq(System.Math.Sqrt(2.0), poly.Area());
    });

    // ── GeomUtil.Merge (free function) ────────────────────────────────────────────
    Console.WriteLine("\nMerge (free function)");

    Test("GeomUtil_Merge_EmptyInput_ReturnsEmpty", () => {
      var result = GeomUtil.Merge(new Polygon2D[] { });
      Eq(0, CountOf(result), 0);
    });

    Test("GeomUtil_Merge_TwoTouchingSquares_ReturnsSingleMergedOuter", () => {
      var a = Polygon2D.Make(new Point2D[] { new(0, 0), new(2, 0), new(2, 2), new(0, 2) });
      var b = Polygon2D.Make(new Point2D[] { new(2, 0), new(4, 0), new(4, 2), new(2, 2) });
      var result = GeomUtil.Merge(new[] { a, b });
      Eq(1, CountOf(result), 0);
      var poly = System.Linq.Enumerable.First(result);
      Eq(8.0, poly.Area());
    });

    Test("GeomUtil_Merge_ThreeSquares_PreservesSharedTJunctionVertex", () => {
      // a and b share a full edge (x=1, y:0-1) and merge into a 2x1 rectangle; c only touches the
      // merged piece at the single point (1, 1) (no full shared edge with a or b). Regression test for
      // the same class of bug as the Polygonize() version above, but via Merge()'s own packaging.
      var a = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var b = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var c = Polygon2D.Make(new Point2D[] { new(1, 1), new(1.5, 1), new(1.5, 1.5), new(1, 1.5) });
      var result = GeomUtil.Merge(new[] { a, b, c });

      Polygon2D? rectangle = null;
      double totalArea = 0.0;
      foreach (var p in result) {
        totalArea += p.Area();
        if (System.Math.Abs(p.Area() - 2.0) < 1e-9) rectangle = p;
      }
      IsTrue(rectangle != null, "expected a piece with area 2.0");
      bool hasMidpoint = false;
      foreach (var v in rectangle!.Perimeter()) {
        if (v.AlmostEquals(new Point2D(1, 1))) hasMidpoint = true;
      }
      IsTrue(hasMidpoint, "expected the rectangle to keep (1,1) as an explicit vertex");
      Eq(2.25, totalArea);  // 2.0 (rectangle) + 0.25 (c)
    });

    Test("GeomUtil_Merge_TwoDisjointSquares_ReturnsBothUnchanged", () => {
      var a = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var b = Polygon2D.Make(new Point2D[] { new(10, 0), new(11, 0), new(11, 1), new(10, 1) });
      var result = GeomUtil.Merge(new[] { a, b });
      Eq(2, CountOf(result), 0);
    });

    Test("GeomUtil_Merge_TwoPolygonsWithTouchingHoles_MergesIntoOneBiggerHole", () => {
      // Holes must be given in CW order -- Polygon2D.Make rejects a CCW hole outright.
      var a = Polygon2D.Make(
          new Point2D[] { new(0, 0), new(2, 0), new(2, 2), new(0, 2) },
          new Point2D[][] { new Point2D[] { new(1, 0.5), new(1, 1.5), new(2, 1.5), new(2, 0.5) } });
      var b = Polygon2D.Make(
          new Point2D[] { new(2, 0), new(4, 0), new(4, 2), new(2, 2) },
          new Point2D[][] { new Point2D[] { new(2, 0.5), new(2, 1.5), new(3, 1.5), new(3, 0.5) } });
      var result = GeomUtil.Merge(new[] { a, b });
      Eq(1, CountOf(result), 0);
      var poly = System.Linq.Enumerable.First(result);
      IsTrue(poly.HasHoles(), "expected the merged polygon to have a hole");
      Eq(6.0, poly.Area());  // 8 (outer) - 2 (merged 1x2 hole)
    });

    Test("GeomUtil_Merge_3D_TwoParallelSameNormalDifferentOffsetSquares_StayUnmerged", () => {
      var a = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      var b = Polygon3D.Make(new Point3D[] { new(0, 0, 5), new(1, 0, 5), new(1, 1, 5), new(0, 1, 5) });
      var result = GeomUtil.Merge(new[] { a, b });
      Eq(2, CountOf(result), 0);
    });
  }
}

}  // namespace GeomPPTests
