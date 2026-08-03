using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class ConnectedMeshTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── ConnectedMesh2D ────────────────────────────────────────────────────────────
    Console.WriteLine("\nConnectedMesh2D");

    Test("ConnectedMesh2D_FromTriangles_Empty_Throws", () => {
      bool threw = false;
      try { ConnectedMesh2D.FromTriangles(new Triangle2D[] { }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected empty triangle list to throw");
    });

    Test("ConnectedMesh2D_FromTriangles_Single", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      var mesh = ConnectedMesh2D.FromTriangles(new[] { t });
      Eq(1, mesh.Size(), 0);
      Eq(0.5, mesh.Area());
    });

    Test("ConnectedMesh2D_Indexer_OutOfRange_Throws", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      var mesh = ConnectedMesh2D.FromTriangles(new[] { t });
      bool threw = false;
      try { var _ = mesh[1]; }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected out-of-range index to throw");
    });

    Test("ConnectedMesh2D_FromTriangles_SharedEdge_WeldsAndPreservesFaces", () => {
      var t0 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1));
      var t1 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1));
      var mesh = ConnectedMesh2D.FromTriangles(new[] { t0, t1 });

      Eq(2, mesh.Size(), 0);
      Eq(1.0, mesh.Area());
      Eq(0, mesh[0].Id(), 0);
      IsTrue(t0.AlmostEquals(mesh[0].Geometry()), "first face's vertices were not preserved");
      IsTrue(t1.AlmostEquals(mesh[1].Geometry()), "second face's vertices were not preserved");
    });

    // Regression coverage for a fix in the native GridCellMapForConnectedMesh2D::Make() where
    // per-triangle adjacency indexing went out of bounds for any triangle beyond the first — a
    // fan of 4 triangles sharing an edge each exercises that path end-to-end through the binding.
    Test("ConnectedMesh2D_FromTriangles_Fan_AllFacesPreserved", () => {
      var triangles = new[] {
        Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1)),
        Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1)),
        Triangle2D.Make(new Point2D(0, 0), new Point2D(0, 1), new Point2D(-1, 1)),
        Triangle2D.Make(new Point2D(0, 0), new Point2D(-1, 1), new Point2D(-1, 0)),
      };
      var mesh = ConnectedMesh2D.FromTriangles(triangles);
      Eq(4, mesh.Size(), 0);
      for (int i = 0; i < triangles.Length; i++) {
        IsTrue(triangles[i].AlmostEquals(mesh[i].Geometry()), $"face {i} mismatch");
      }
    });

    // Fan of 4 triangles sharing the origin, laid out left-to-right (triangle i's Third edge is welded
    // to triangle i+1's First edge). Starting at face 0 and always crossing Third, the walk visits
    // 0 -> 1 -> 2 -> 3, always entering the next face through its First edge, and face 3's Third edge
    // is a boundary (end of the fan) — mirrors the C++/Python FaceView neighbor-walk tests.
    Test("ConnectedMesh2D_FaceView_Neighbor_WalksKnownAdjacencyPattern", () => {
      var triangles = new[] {
        Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1)),
        Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1)),
        Triangle2D.Make(new Point2D(0, 0), new Point2D(0, 1), new Point2D(-1, 1)),
        Triangle2D.Make(new Point2D(0, 0), new Point2D(-1, 1), new Point2D(-1, 0)),
      };
      var mesh = ConnectedMesh2D.FromTriangles(triangles);

      var face = mesh[0];
      Eq(0, face.Id(), 0);

      for (int expectedTarget = 1; expectedTarget < triangles.Length; expectedTarget++) {
        IsTrue(face.NeighborEntryEdge(TriangleEdge.Third) == TriangleEdge.First,
            $"face {face.Id()} should be entered through its First edge");

        var next = face.Neighbor(TriangleEdge.Third);
        IsTrue(next != null, $"face {face.Id()}'s Third edge should not be a boundary");

        face = next;
        Eq(expectedTarget, face.Id(), 0);
        IsTrue(triangles[expectedTarget].AlmostEquals(face.Geometry()), $"face {expectedTarget} mismatch");
      }

      // face 3 is the end of the fan: its Third edge has no twin.
      IsTrue(face.Neighbor(TriangleEdge.Third) == null, "face 3's Third edge should be a boundary");
      IsTrue(face.NeighborEntryEdge(TriangleEdge.Third) == TriangleEdge.Invalid, "boundary entry edge should be Invalid");
    });

    Test("ConnectedMesh2D_FaceView_Neighbor_SingleTriangle_AllEdgesAreBoundary", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      var mesh = ConnectedMesh2D.FromTriangles(new[] { t });
      var face = mesh[0];
      foreach (var edge in new[] { TriangleEdge.First, TriangleEdge.Second, TriangleEdge.Third }) {
        IsTrue(face.Neighbor(edge) == null, $"edge {edge} should be a boundary");
        IsTrue(face.NeighborEntryEdge(edge) == TriangleEdge.Invalid, $"edge {edge} entry should be Invalid");
      }
    });

    Test("ConnectedMesh2D_FaceView_Neighbor_CrossingBackViaEntryEdge_ReturnsToOrigin", () => {
      var t0 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(1, 1));
      var t1 = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 1), new Point2D(0, 1));
      var mesh = ConnectedMesh2D.FromTriangles(new[] { t0, t1 });

      var face0 = mesh[0];
      var entryEdge = face0.NeighborEntryEdge(TriangleEdge.Third);
      var face1 = face0.Neighbor(TriangleEdge.Third);
      IsTrue(face1 != null, "face 0's Third edge should not be a boundary");
      Eq(1, face1.Id(), 0);

      var back = face1.Neighbor(entryEdge);
      IsTrue(back != null, "crossing back should not be a boundary");
      Eq(face0.Id(), back.Id(), 0);
      IsTrue(face1.NeighborEntryEdge(entryEdge) == TriangleEdge.Third, "crossing back should re-enter via Third");
    });

    Test("ConnectedMesh2D_ToString_ContainsSizeAndArea", () => {
      var t = Triangle2D.Make(new Point2D(0, 0), new Point2D(1, 0), new Point2D(0, 1));
      var mesh = ConnectedMesh2D.FromTriangles(new[] { t });
      var s = mesh.ToString();
      IsTrue(s.Contains("ConnectedMesh2D"), "missing ConnectedMesh2D label");
    });

    // ── ConnectedMesh3D ────────────────────────────────────────────────────────────
    Console.WriteLine("\nConnectedMesh3D");

    Test("ConnectedMesh3D_FromTriangles_Empty_Throws", () => {
      bool threw = false;
      try { ConnectedMesh3D.FromTriangles(new Triangle3D[] { }); }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected empty triangle list to throw");
    });

    Test("ConnectedMesh3D_FromTriangles_Single", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var mesh = ConnectedMesh3D.FromTriangles(new[] { t });
      Eq(1, mesh.Size(), 0);
      Eq(0.5, mesh.Area());
    });

    Test("ConnectedMesh3D_Indexer_OutOfRange_Throws", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var mesh = ConnectedMesh3D.FromTriangles(new[] { t });
      bool threw = false;
      try { var _ = mesh[1]; }
      catch (Exception) { threw = true; }
      IsTrue(threw, "expected out-of-range index to throw");
    });

    Test("ConnectedMesh3D_FromTriangles_SharedEdge_WeldsAndPreservesFaces", () => {
      var t0 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(1, 1, 0));
      var t1 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0));
      var mesh = ConnectedMesh3D.FromTriangles(new[] { t0, t1 });

      Eq(2, mesh.Size(), 0);
      Eq(1.0, mesh.Area());
      Eq(0, mesh[0].Id(), 0);
      IsTrue(t0.AlmostEquals(mesh[0].Geometry()), "first face's vertices were not preserved");
      IsTrue(t1.AlmostEquals(mesh[1].Geometry()), "second face's vertices were not preserved");
    });

    // Regression coverage for a fix in the native GridCellMapForConnectedMesh3D::Make() where
    // per-triangle adjacency indexing went out of bounds for any triangle beyond the first — a
    // fan of 4 triangles sharing an edge each exercises that path end-to-end through the binding.
    Test("ConnectedMesh3D_FromTriangles_Fan_AllFacesPreserved", () => {
      var triangles = new[] {
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(1, 1, 0)),
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0)),
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(0, 1, 0), new Point3D(-1, 1, 0)),
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(-1, 1, 0), new Point3D(-1, 0, 0)),
      };
      var mesh = ConnectedMesh3D.FromTriangles(triangles);
      Eq(4, mesh.Size(), 0);
      for (int i = 0; i < triangles.Length; i++) {
        IsTrue(triangles[i].AlmostEquals(mesh[i].Geometry()), $"face {i} mismatch");
      }
    });

    // Fan of 4 triangles sharing the origin, laid out left-to-right (triangle i's Third edge is welded
    // to triangle i+1's First edge). Starting at face 0 and always crossing Third, the walk visits
    // 0 -> 1 -> 2 -> 3, always entering the next face through its First edge, and face 3's Third edge
    // is a boundary (end of the fan) — mirrors the C++/Python FaceView neighbor-walk tests.
    Test("ConnectedMesh3D_FaceView_Neighbor_WalksKnownAdjacencyPattern", () => {
      var triangles = new[] {
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(1, 1, 0)),
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0)),
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(0, 1, 0), new Point3D(-1, 1, 0)),
        Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(-1, 1, 0), new Point3D(-1, 0, 0)),
      };
      var mesh = ConnectedMesh3D.FromTriangles(triangles);

      var face = mesh[0];
      Eq(0, face.Id(), 0);

      for (int expectedTarget = 1; expectedTarget < triangles.Length; expectedTarget++) {
        IsTrue(face.NeighborEntryEdge(TriangleEdge.Third) == TriangleEdge.First,
            $"face {face.Id()} should be entered through its First edge");

        var next = face.Neighbor(TriangleEdge.Third);
        IsTrue(next != null, $"face {face.Id()}'s Third edge should not be a boundary");

        face = next;
        Eq(expectedTarget, face.Id(), 0);
        IsTrue(triangles[expectedTarget].AlmostEquals(face.Geometry()), $"face {expectedTarget} mismatch");
      }

      // face 3 is the end of the fan: its Third edge has no twin.
      IsTrue(face.Neighbor(TriangleEdge.Third) == null, "face 3's Third edge should be a boundary");
      IsTrue(face.NeighborEntryEdge(TriangleEdge.Third) == TriangleEdge.Invalid, "boundary entry edge should be Invalid");
    });

    Test("ConnectedMesh3D_FaceView_Neighbor_SingleTriangle_AllEdgesAreBoundary", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var mesh = ConnectedMesh3D.FromTriangles(new[] { t });
      var face = mesh[0];
      foreach (var edge in new[] { TriangleEdge.First, TriangleEdge.Second, TriangleEdge.Third }) {
        IsTrue(face.Neighbor(edge) == null, $"edge {edge} should be a boundary");
        IsTrue(face.NeighborEntryEdge(edge) == TriangleEdge.Invalid, $"edge {edge} entry should be Invalid");
      }
    });

    Test("ConnectedMesh3D_FaceView_Neighbor_CrossingBackViaEntryEdge_ReturnsToOrigin", () => {
      var t0 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(1, 1, 0));
      var t1 = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0), new Point3D(0, 1, 0));
      var mesh = ConnectedMesh3D.FromTriangles(new[] { t0, t1 });

      var face0 = mesh[0];
      var entryEdge = face0.NeighborEntryEdge(TriangleEdge.Third);
      var face1 = face0.Neighbor(TriangleEdge.Third);
      IsTrue(face1 != null, "face 0's Third edge should not be a boundary");
      Eq(1, face1.Id(), 0);

      var back = face1.Neighbor(entryEdge);
      IsTrue(back != null, "crossing back should not be a boundary");
      Eq(face0.Id(), back.Id(), 0);
      IsTrue(face1.NeighborEntryEdge(entryEdge) == TriangleEdge.Third, "crossing back should re-enter via Third");
    });

    Test("ConnectedMesh3D_ToString_ContainsSizeAndArea", () => {
      var t = Triangle3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0), new Point3D(0, 1, 0));
      var mesh = ConnectedMesh3D.FromTriangles(new[] { t });
      var s = mesh.ToString();
      IsTrue(s.Contains("ConnectedMesh3D"), "missing ConnectedMesh3D label");
    });
  }
}

}  // namespace GeomPPTests
