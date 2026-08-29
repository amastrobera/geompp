using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class MeshTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── Mesh2D / Mesh3D ───────────────────────────────────────────────────────────
    Console.WriteLine("\nMesh2D / Mesh3D");

    Test("Mesh2D_FromTriangles_Empty_Throws", () => {
      bool threw = false;
      try { Mesh2D.FromTriangles(new Triangle2D[] { }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected empty triangle list to throw");
    });

    Test("Mesh2D_FromTriangles_NonManifoldEdge_Throws", () => {
      var a = Triangle2D.Make(new(0, 0), new(1, 0), new(0.5, 1));
      var b = Triangle2D.Make(new(1, 0), new(0, 0), new(0.5, -1));
      var c = Triangle2D.Make(new(0, 0), new(1, 0), new(0.5, -2));
      bool threw = false;
      try { Mesh2D.FromTriangles(new[] { a, b, c }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a non-manifold edge (3 facets sharing it) to throw");
    });

    Test("Mesh2D_FromTriangles_TJunction_DefaultAssert_Throws", () => {
      // Big triangle A sitting on two small triangles B, C -- B and C's shared vertex (2,0) lies in
      // the interior of A's base edge (0,0)-(4,0), a T-junction. Default conformity is Assert.
      var A = Triangle2D.Make(new(0, 0), new(4, 0), new(2, 3));
      var B = Triangle2D.Make(new(0, 0), new(1, -1.5), new(2, 0));
      var C = Triangle2D.Make(new(2, 0), new(3, -1.5), new(4, 0));
      bool threw = false;
      try { Mesh2D.FromTriangles(new[] { A, B, C }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a T-junction to throw under the default (Assert) conformity");
    });

    Test("Mesh2D_FromTriangles_TJunction_Enforce_ReTriangulatesAndWelds", () => {
      var A = Triangle2D.Make(new(0, 0), new(4, 0), new(2, 3));
      var B = Triangle2D.Make(new(0, 0), new(1, -1.5), new(2, 0));
      var C = Triangle2D.Make(new(2, 0), new(3, -1.5), new(4, 0));
      double areaBefore = A.Area() + B.Area() + C.Area();

      var mesh = Mesh2D.FromTriangles(new[] { A, B, C }, AdjacencyConformity.Enforce);

      Eq(4, mesh.Size(), 0);  // A re-triangulates into 2, B and C pass through unchanged
      Eq(areaBefore, mesh.Area());
    });

    Test("Mesh2D_FromTriangles_TJunction_Guaranteed_SkipsCheckAndSucceeds", () => {
      var A = Triangle2D.Make(new(0, 0), new(4, 0), new(2, 3));
      var B = Triangle2D.Make(new(0, 0), new(1, -1.5), new(2, 0));
      var C = Triangle2D.Make(new(2, 0), new(3, -1.5), new(4, 0));
      Mesh2D.FromTriangles(new[] { A, B, C }, AdjacencyConformity.Guaranteed);  // must not throw
    });

    Test("Mesh2D_FromTriangles_Single", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      var mesh = Mesh2D.FromTriangles(new[] { t });
      Eq(1, mesh.Size(), 0);
      Eq(0.5, mesh.Area());
    });

    Test("Mesh2D_Indexer_OutOfRange_Throws", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      var mesh = Mesh2D.FromTriangles(new[] { t });
      bool threw = false;
      try { var _ = mesh[1]; }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected out-of-range index to throw");
    });

    // Regression test for the vertex-index off-by-one bug: two triangles sharing an edge
    // must weld to 4 unique vertices and each face must round-trip correctly.
    Test("Mesh2D_FromTriangles_SharedEdge_WeldsAndPreservesFaces", () => {
      var t0 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1));
      var t1 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1));
      var mesh = Mesh2D.FromTriangles(new[] { t0, t1 });

      Eq(2, mesh.Size(), 0);
      Eq(1.0, mesh.Area());
      IsTrue(t0.AlmostEquals(mesh[0]), "first face's vertices were not preserved");
      IsTrue(t1.AlmostEquals(mesh[1]), "second face's vertices were not preserved");
    });

    Test("Mesh2D_ToString_ContainsSizeAndArea", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      var mesh = Mesh2D.FromTriangles(new[] { t });
      var s = mesh.ToString();
      IsTrue(s.Contains("Mesh2D"), "missing Mesh2D label");
    });

    Test("Mesh2D_Connect_PreservesSizeAndArea", () => {
      var t0 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1));
      var t1 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1));
      var mesh = Mesh2D.FromTriangles(new[] { t0, t1 });
      var connected = mesh.Connect();
      Eq(mesh.Size(), connected.Size(), 0);
      Eq(mesh.Area(), connected.Area());
    });

    Test("Mesh2D_Polygonize_UnitSquareFromTwoTriangles_ReturnsSingleQuad", () => {
      var t0 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1));
      var t1 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1));
      var mesh = Mesh2D.FromTriangles(new[] { t0, t1 });
      var settings = new PolygonizationParams(PolygonizationStrategy.PlanarBoundaryExtraction);
      var polyMesh = mesh.Polygonize(settings);
      Eq(1, polyMesh.Size(), 0);
      Eq(1.0, polyMesh.Area());
      Eq(4, polyMesh[0].Size(), 0);
    });

    Test("Mesh2D_Polygonize_MatchesConnectThenPolygonize", () => {
      var t0 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1));
      var t1 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1));
      var mesh = Mesh2D.FromTriangles(new[] { t0, t1 });
      var settings = new PolygonizationParams(PolygonizationStrategy.HertelMehlhorn);
      var direct = mesh.Polygonize(settings);
      var viaConnect = mesh.Connect().Polygonize(settings);
      Eq(direct.Size(), viaConnect.Size(), 0);
      Eq(direct.Area(), viaConnect.Area());
    });

    Test("Mesh2D_Polygonize_LShape_HertelMehlhorn_DoesNotThrowTJunction", () => {
      // 2x2 grid, top-left cell skipped: an L-shape. HertelMehlhorn returns 2 convex pieces -- a 2x1
      // rectangle and a 1x1 square -- whose shared corner sits exactly at the midpoint of the
      // rectangle's top edge. Regression test: this used to throw here (though not from the free
      // GeomUtil.Polygonize(), which has no mesh-conformity requirement to violate) because Polygonize()
      // packaged each piece via Polygon2D.Make(), which silently drops that midpoint as collinear on
      // the rectangle's own ring alone -- leaving the square's corner touching the middle of a
      // neighbor's edge once PolyMesh2D.FromPolygons() re-welds and validates adjacency.
      var mesh = Mesh2D.FromTriangles(new[] {
        Triangle2D.Make(new(0, 0), new(1, 0), new(1, 1)),
        Triangle2D.Make(new(0, 0), new(1, 1), new(0, 1)),
        Triangle2D.Make(new(1, 0), new(2, 0), new(2, 1)),
        Triangle2D.Make(new(1, 0), new(2, 1), new(1, 1)),
        Triangle2D.Make(new(1, 1), new(2, 1), new(2, 2)),
        Triangle2D.Make(new(1, 1), new(2, 2), new(1, 2)),
      });
      var settings = new PolygonizationParams(PolygonizationStrategy.HertelMehlhorn);
      PolyMesh2D? polyMesh = null;
      bool threw = false;
      try { polyMesh = mesh.Polygonize(settings); }
      catch (Exception) { threw = true; }
      IsFalse(threw, "expected the L-shape's HertelMehlhorn result not to throw a T-junction error");
      NotNull(polyMesh);
      Eq(2, polyMesh!.Size(), 0);
      Eq(3.0, polyMesh.Area());
    });

    Test("Mesh3D_FromTriangles_Empty_Throws", () => {
      bool threw = false;
      try { Mesh3D.FromTriangles(new Triangle3D[] { }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected empty triangle list to throw");
    });

    Test("Mesh3D_FromTriangles_NonManifoldEdge_Throws", () => {
      var a = Triangle3D.Make(new(0, 0, 0), new(1, 0, 0), new(0.5, 1, 0));
      var b = Triangle3D.Make(new(1, 0, 0), new(0, 0, 0), new(0.5, 0, 1));
      var c = Triangle3D.Make(new(0, 0, 0), new(1, 0, 0), new(0.5, -1, 0));
      bool threw = false;
      try { Mesh3D.FromTriangles(new[] { a, b, c }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a non-manifold edge (3 facets sharing it) to throw");
    });

    Test("Mesh3D_FromTriangles_TJunction_DefaultAssert_Throws", () => {
      // 3D counterpart of Mesh2D's own version, folded into the y=0 plane (x -> x, y(2D) -> z).
      var A = Triangle3D.Make(new(0, 0, 0), new(4, 0, 0), new(2, 0, 3));
      var B = Triangle3D.Make(new(0, 0, 0), new(1, 0, -1.5), new(2, 0, 0));
      var C = Triangle3D.Make(new(2, 0, 0), new(3, 0, -1.5), new(4, 0, 0));
      bool threw = false;
      try { Mesh3D.FromTriangles(new[] { A, B, C }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a T-junction to throw under the default (Assert) conformity");
    });

    Test("Mesh3D_FromTriangles_TJunction_Enforce_ReTriangulatesAndWelds", () => {
      var A = Triangle3D.Make(new(0, 0, 0), new(4, 0, 0), new(2, 0, 3));
      var B = Triangle3D.Make(new(0, 0, 0), new(1, 0, -1.5), new(2, 0, 0));
      var C = Triangle3D.Make(new(2, 0, 0), new(3, 0, -1.5), new(4, 0, 0));
      double areaBefore = A.Area() + B.Area() + C.Area();

      var mesh = Mesh3D.FromTriangles(new[] { A, B, C }, AdjacencyConformity.Enforce);

      Eq(4, mesh.Size(), 0);
      Eq(areaBefore, mesh.Area());
    });

    Test("Mesh3D_FromTriangles_TJunction_Guaranteed_SkipsCheckAndSucceeds", () => {
      var A = Triangle3D.Make(new(0, 0, 0), new(4, 0, 0), new(2, 0, 3));
      var B = Triangle3D.Make(new(0, 0, 0), new(1, 0, -1.5), new(2, 0, 0));
      var C = Triangle3D.Make(new(2, 0, 0), new(3, 0, -1.5), new(4, 0, 0));
      Mesh3D.FromTriangles(new[] { A, B, C }, AdjacencyConformity.Guaranteed);  // must not throw
    });

    Test("Mesh3D_FromTriangles_SharedEdge_WeldsAndPreservesFaces", () => {
      var t0 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(1, 1, 0));
      var t1 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0));
      var mesh = Mesh3D.FromTriangles(new[] { t0, t1 });

      Eq(2, mesh.Size(), 0);
      Eq(1.0, mesh.Area());
      IsTrue(t0.AlmostEquals(mesh[0]), "first face's vertices were not preserved");
      IsTrue(t1.AlmostEquals(mesh[1]), "second face's vertices were not preserved");
    });

    Test("Mesh3D_Indexer_OutOfRange_Throws", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var mesh = Mesh3D.FromTriangles(new[] { t });
      bool threw = false;
      try { var _ = mesh[1]; }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected out-of-range index to throw");
    });

    Test("Mesh3D_ToString_ContainsSizeAndArea", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var mesh = Mesh3D.FromTriangles(new[] { t });
      var s = mesh.ToString();
      IsTrue(s.Contains("Mesh3D"), "missing Mesh3D label");
    });

    Test("Mesh3D_Connect_PreservesSizeAndArea", () => {
      var t0 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(1, 1, 0));
      var t1 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0));
      var mesh = Mesh3D.FromTriangles(new[] { t0, t1 });
      var connected = mesh.Connect();
      Eq(mesh.Size(), connected.Size(), 0);
      Eq(mesh.Area(), connected.Area());
    });

    Test("Mesh3D_Polygonize_MatchesConnectThenPolygonize", () => {
      var t0 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(1, 1, 0));
      var t1 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0));
      var mesh = Mesh3D.FromTriangles(new[] { t0, t1 });
      var settings = new PolygonizationParams(PolygonizationStrategy.PlanarBoundaryExtraction);
      var direct = mesh.Polygonize(settings);
      var viaConnect = mesh.Connect().Polygonize(settings);
      Eq(direct.Size(), viaConnect.Size(), 0);
      Eq(direct.Area(), viaConnect.Area());
      Eq(1.0, direct.Area());
    });

    Test("Mesh3D_Polygonize_LShape_HertelMehlhorn_DoesNotThrowTJunction", () => {
      // 3D counterpart of Mesh2D's own version, flat on z=0 -- see its comment for the explanation.
      var mesh = Mesh3D.FromTriangles(new[] {
        Triangle3D.Make(new(0, 0, 0), new(1, 0, 0), new(1, 1, 0)),
        Triangle3D.Make(new(0, 0, 0), new(1, 1, 0), new(0, 1, 0)),
        Triangle3D.Make(new(1, 0, 0), new(2, 0, 0), new(2, 1, 0)),
        Triangle3D.Make(new(1, 0, 0), new(2, 1, 0), new(1, 1, 0)),
        Triangle3D.Make(new(1, 1, 0), new(2, 1, 0), new(2, 2, 0)),
        Triangle3D.Make(new(1, 1, 0), new(2, 2, 0), new(1, 2, 0)),
      });
      var settings = new PolygonizationParams(PolygonizationStrategy.HertelMehlhorn);
      PolyMesh3D? polyMesh = null;
      bool threw = false;
      try { polyMesh = mesh.Polygonize(settings); }
      catch (Exception) { threw = true; }
      IsFalse(threw, "expected the L-shape's HertelMehlhorn result not to throw a T-junction error");
      NotNull(polyMesh);
      Eq(2, polyMesh!.Size(), 0);
      Eq(3.0, polyMesh.Area());
    });
  }
}

}  // namespace GeomPPTests
