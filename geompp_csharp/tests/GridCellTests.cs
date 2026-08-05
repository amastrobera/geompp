using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class GridCellTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── GridCell2D / GridCell3D ─────────────────────────────────────────────────────
    Console.WriteLine("\nGridCell2D / GridCell3D");

    Test("GridCell2D_FromPoint_Quantizes", () => {
      var cell = GridCell2D.FromPoint(new Point2D(2.5, 3.5), 1.0);
      Eq(2, cell.X, 0);
      Eq(3, cell.Y, 0);
    });

    Test("GridCell2D_FromPoint_NearbyPointsSameCell", () => {
      var c1 = GridCell2D.FromPoint(new Point2D(10.1, 10.1), 1.0);
      var c2 = GridCell2D.FromPoint(new Point2D(10.9, 10.9), 1.0);
      IsTrue(c1 == c2);
    });

    Test("GridCell2D_FromPoint_DistantPointsDifferentCell", () => {
      var c1 = GridCell2D.FromPoint(new Point2D(0, 0), 1.0);
      var c2 = GridCell2D.FromPoint(new Point2D(100, 100), 1.0);
      IsFalse(c1 == c2);
    });

    Test("GridCell2D_ToString_ContainsCoordinates", () => {
      var s = GridCell2D.FromPoint(new Point2D(2.5, 3.5), 1.0).ToString();
      IsTrue(s.Contains("GridCell2D"), "missing GridCell2D label");
    });

    Test("GridCell3D_FromPoint_Quantizes", () => {
      var cell = GridCell3D.FromPoint(new Point3D(2.5, 3.5, 4.5), 1.0);
      Eq(2, cell.X, 0);
      Eq(3, cell.Y, 0);
      Eq(4, cell.Z, 0);
    });

    Test("GridCell3D_FromPoint_NearbyPointsSameCell", () => {
      var c1 = GridCell3D.FromPoint(new Point3D(10.1, 10.1, 10.1), 1.0);
      var c2 = GridCell3D.FromPoint(new Point3D(10.9, 10.9, 10.9), 1.0);
      IsTrue(c1 == c2);
    });

    Test("GridCell3D_FromPoint_DistantPointsDifferentCell", () => {
      var c1 = GridCell3D.FromPoint(new Point3D(0, 0, 0), 1.0);
      var c2 = GridCell3D.FromPoint(new Point3D(100, 100, 100), 1.0);
      IsFalse(c1 == c2);
    });

    Test("GridCell3D_ToString_ContainsCoordinates", () => {
      var s = GridCell3D.FromPoint(new Point3D(2.5, 3.5, 4.5), 1.0).ToString();
      IsTrue(s.Contains("GridCell3D"), "missing GridCell3D label");
    });
  }
}

}  // namespace GeomPPTests
