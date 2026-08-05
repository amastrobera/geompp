using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class PolyMeshTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── PolyMesh2D / PolyMesh3D ───────────────────────────────────────────────────
    Console.WriteLine("\nPolyMesh2D / PolyMesh3D");

    Test("PolyMesh2D_FromPolygons_Empty_Throws", () => {
      bool threw = false;
      try { PolyMesh2D.FromPolygons(new Polygon2D[] { }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected empty polygon list to throw");
    });

    Test("PolyMesh2D_FromPolygons_WithHoles_Throws", () => {
      var outer = Polygon2D.Make(
        new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(0, 4) },
        new Point2D[][] { new Point2D[] { new(1, 1), new(1, 2), new(2, 2), new(2, 1) } });
      bool threw = false;
      try { PolyMesh2D.FromPolygons(new[] { outer }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected polygon with holes to throw");
    });

    Test("PolyMesh2D_FromPolygons_TJunction_Throws", () => {
      // Two unit squares side by side, plus a roof triangle spanning both squares' top -- its base
      // edge passes straight through the squares' shared vertex. Classic T-junction.
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var roof = Polygon2D.Make(new Point2D[] { new(0, 1), new(2, 1), new(1, 2) });
      bool threw = false;
      try { PolyMesh2D.FromPolygons(new[] { p0, p1, roof }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a T-junction to throw");
    });

    Test("PolyMesh2D_FromPolygons_SingleQuad", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var mesh = PolyMesh2D.FromPolygons(new[] { p });
      Eq(1, mesh.Size(), 0);
      Eq(1.0, mesh.Area());
    });

    // Regression test for the vertex-index off-by-one bug: two quads sharing an edge must
    // weld shared vertices and each face must round-trip correctly.
    Test("PolyMesh2D_FromPolygons_SharedEdge_WeldsAndPreservesFaces", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var mesh = PolyMesh2D.FromPolygons(new[] { p0, p1 });

      Eq(2, mesh.Size(), 0);
      Eq(2.0, mesh.Area());
      IsTrue(p0.AlmostEquals(mesh[0]), "first face's vertices were not preserved");
      IsTrue(p1.AlmostEquals(mesh[1]), "second face's vertices were not preserved");
    });

    Test("PolyMesh2D_ToString_ContainsSizeAndArea", () => {
      var p = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var mesh = PolyMesh2D.FromPolygons(new[] { p });
      var s = mesh.ToString();
      IsTrue(s.Contains("PolyMesh2D"), "missing PolyMesh2D label");
    });

    Test("PolyMesh3D_FromPolygons_Empty_Throws", () => {
      bool threw = false;
      try { PolyMesh3D.FromPolygons(new Polygon3D[] { }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected empty polygon list to throw");
    });

    Test("PolyMesh3D_FromPolygons_WithHoles_Throws", () => {
      var outer = Polygon3D.Make(
        new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 4, 0), new(0, 4, 0) },
        new Point3D[][] { new Point3D[] { new(1, 1, 0), new(1, 2, 0), new(2, 2, 0), new(2, 1, 0) } });
      bool threw = false;
      try { PolyMesh3D.FromPolygons(new[] { outer }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected polygon with holes to throw");
    });

    Test("PolyMesh3D_FromPolygons_TJunction_Throws", () => {
      var p0 = Polygon3D.Make(new Point3D[] { new(0, 0, 1), new(1, 0, 1), new(1, 0, 0), new(0, 0, 0) });
      var p1 = Polygon3D.Make(new Point3D[] { new(1, 0, 1), new(2, 0, 1), new(2, 0, 0), new(1, 0, 0) });
      var roof = Polygon3D.Make(new Point3D[] { new(1, 0, 2), new(2, 0, 1), new(0, 0, 1) });
      bool threw = false;
      try { PolyMesh3D.FromPolygons(new[] { p0, p1, roof }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected a T-junction to throw");
    });

    Test("PolyMesh3D_FromPolygons_SharedEdge_WeldsAndPreservesFaces", () => {
      var p0 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      var p1 = Polygon3D.Make(new Point3D[] { new(1, 0, 0), new(2, 0, 0), new(2, 1, 0), new(1, 1, 0) });
      var mesh = PolyMesh3D.FromPolygons(new[] { p0, p1 });

      Eq(2, mesh.Size(), 0);
      Eq(2.0, mesh.Area());
      IsTrue(p0.AlmostEquals(mesh[0]), "first face's vertices were not preserved");
      IsTrue(p1.AlmostEquals(mesh[1]), "second face's vertices were not preserved");
    });

    Test("PolyMesh3D_Indexer_OutOfRange_Throws", () => {
      var p = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      var mesh = PolyMesh3D.FromPolygons(new[] { p });
      bool threw = false;
      try { var _ = mesh[1]; }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected out-of-range index to throw");
    });

    Test("PolyMesh3D_ToString_ContainsSizeAndArea", () => {
      var p = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      var mesh = PolyMesh3D.FromPolygons(new[] { p });
      var s = mesh.ToString();
      IsTrue(s.Contains("PolyMesh3D"), "missing PolyMesh3D label");
    });

    // ── PolyMesh2D/3D::Triangulate ────────────────────────────────────────────────
    Test("PolyMesh2D_Triangulate_SharedEdge_PreservesTotalArea", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(1, 0), new(2, 0), new(2, 1), new(1, 1) });
      var mesh = PolyMesh2D.FromPolygons(new[] { p0, p1 });
      var triMesh = mesh.Triangulate();
      Eq(4, triMesh.Size(), 0);
      Eq(mesh.Area(), triMesh.Area());
    });

    // Regression test: PolyMesh2D.Triangulate() used to triangulate the whole (shared, deduplicated)
    // vertex buffer as if it were a single ring, which only "worked" by coincidence for facets that
    // happened to share welded edges. Two disjoint facets — no welding to paper over the bug — exposes
    // it directly: the old code produced a wrong triangle count and a wrong total area.
    Test("PolyMesh2D_Triangulate_DisjointFacets_PreservesTotalArea", () => {
      var p0 = Polygon2D.Make(new Point2D[] { new(0, 0), new(1, 0), new(1, 1), new(0, 1) });
      var p1 = Polygon2D.Make(new Point2D[] { new(5, 5), new(6, 5), new(6, 6), new(5, 6) });
      var mesh = PolyMesh2D.FromPolygons(new[] { p0, p1 });
      var triMesh = mesh.Triangulate();
      Eq(4, triMesh.Size(), 0);
      Eq(mesh.Area(), triMesh.Area());
    });

    Test("PolyMesh3D_Triangulate_SharedEdge_PreservesTotalArea", () => {
      var p0 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      var p1 = Polygon3D.Make(new Point3D[] { new(1, 0, 0), new(2, 0, 0), new(2, 1, 0), new(1, 1, 0) });
      var mesh = PolyMesh3D.FromPolygons(new[] { p0, p1 });
      var triMesh = mesh.Triangulate();
      Eq(4, triMesh.Size(), 0);
      Eq(mesh.Area(), triMesh.Area());
    });

    Test("PolyMesh3D_Triangulate_DisjointFacets_PreservesTotalArea", () => {
      var p0 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(1, 0, 0), new(1, 1, 0), new(0, 1, 0) });
      var p1 = Polygon3D.Make(new Point3D[] { new(5, 5, 0), new(6, 5, 0), new(6, 6, 0), new(5, 6, 0) });
      var mesh = PolyMesh3D.FromPolygons(new[] { p0, p1 });
      var triMesh = mesh.Triangulate();
      Eq(4, triMesh.Size(), 0);
      Eq(mesh.Area(), triMesh.Area());
    });
  }
}

}  // namespace GeomPPTests
