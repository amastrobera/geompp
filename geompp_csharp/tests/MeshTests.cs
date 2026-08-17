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
  }
}

}  // namespace GeomPPTests
