using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class ConvexHullTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── ConvexHull ────────────────────────────────────────────────────────────────
    Console.WriteLine("\nConvexHull");

    List<Point2D> StarPoints() {
      // 5 outer tips at unequal distances + 5 inner concave vertices
      return new List<Point2D> {
        new( 0,  5), new( 2,  1),   // tip0, inner0
        new( 4,  2), new( 2, -1),   // tip1, inner1
        new( 3, -3), new( 0, -1),   // tip2, inner2
        new(-2, -4), new(-1, -1),   // tip3, inner3
        new(-3,  1), new(-1,  2),   // tip4, inner4
      };
    }

    Test("ConvexHull_AsymmetricStar_IsAPentagon", () => {
      var hull = new List<Point2D>(GeomUtil.ConvexHull(StarPoints()));
      Eq(5, hull.Count);
    });

    Test("ConvexHull_StarOuterTipsAllOnHull", () => {
      var hull = new List<Point2D>(GeomUtil.ConvexHull(StarPoints()));
      var tips = new List<Point2D> {
        new(0,5), new(4,2), new(3,-3), new(-2,-4), new(-3,1) };
      foreach (var tip in tips) {
        IsTrue(hull.Exists(h => System.Math.Abs(h.X - tip.X) < 1e-6 && System.Math.Abs(h.Y - tip.Y) < 1e-6),
               $"outer tip ({tip.X},{tip.Y}) should be on the hull");
      }
    });

    Test("ConvexHull_FewPoints_ReturnsAsIs", () => {
      var pts = new List<Point2D> { new(0, 0), new(1, 1) };
      var hull = new List<Point2D>(GeomUtil.ConvexHull(pts));
      Eq(2, hull.Count);
    });

    // ── ConvexHull 3D ─────────────────────────────────────────────────────────────
    Console.WriteLine("\nConvexHull 3D");

    List<Point3D> StarPoints3D() {
      return new List<Point3D> {
        new( 0,  5, 0), new( 2,  1, 0),
        new( 4,  2, 0), new( 2, -1, 0),
        new( 3, -3, 0), new( 0, -1, 0),
        new(-2, -4, 0), new(-1, -1, 0),
        new(-3,  1, 0), new(-1,  2, 0),
      };
    }

    Test("ConvexHull3D_XYPlaneSquare_ReturnsFourCorners", () => {
      var pts = new List<Point3D> {
        new(0, 0, 0), new(4, 0, 0), new(4, 4, 0), new(0, 4, 0) };
      var hull = new List<Point3D>(GeomUtil.ConvexHull(pts));
      Eq(4, hull.Count);
    });

    Test("ConvexHull3D_AsymmetricStar_IsAPentagon", () => {
      var hull = new List<Point3D>(GeomUtil.ConvexHull(StarPoints3D()));
      Eq(5, hull.Count);
    });

    Test("ConvexHull3D_StarOuterTipsAllOnHull", () => {
      var hull = new List<Point3D>(GeomUtil.ConvexHull(StarPoints3D()));
      var tips = new List<Point3D> {
        new(0,5,0), new(4,2,0), new(3,-3,0), new(-2,-4,0), new(-3,1,0) };
      foreach (var tip in tips) {
        IsTrue(hull.Exists(h => System.Math.Abs(h.X - tip.X) < 1e-6
                             && System.Math.Abs(h.Y - tip.Y) < 1e-6
                             && System.Math.Abs(h.Z - tip.Z) < 1e-6),
               $"outer tip ({tip.X},{tip.Y},{tip.Z}) should be on the hull");
      }
    });

    Test("ConvexHull3D_NonCoplanar_SmallZJitter_StillFindsHull", () => {
      var pts = new List<Point3D> {
        new(0, 0, 0.1), new(4, 0, -0.1), new(4, 4, 0.05), new(0, 4, -0.05),
        new(2, 2, 0.02) };
      var hull = new List<Point3D>(GeomUtil.ConvexHull(pts));
      Eq(4, hull.Count);
    });

    Test("ConvexHull3D_NonCoplanar_TiltedPlane_ReturnsTriangle", () => {
      var pts = new List<Point3D> {
        new(3, 0, 0), new(0, 3, 0), new(0, 0, 3), new(1, 1, 1) };
      var hull = new List<Point3D>(GeomUtil.ConvexHull(pts));
      Eq(3, hull.Count);
    });

    Test("ConvexHull3D_NonCoplanar_AllHullPointsPresent", () => {
      var tips = new List<Point3D> {
        new(0, 5, 1), new(4, 2, 0.5), new(3, -3, 0), new(-2, -4, 0.5), new(-3, 1, 1) };
      var pts = new List<Point3D>(tips) { new(0, 0, 0.6) };
      var hull = new List<Point3D>(GeomUtil.ConvexHull(pts));
      Eq(5, hull.Count);
      foreach (var tip in tips) {
        IsTrue(hull.Exists(h => System.Math.Abs(h.X - tip.X) < 1e-6
                             && System.Math.Abs(h.Y - tip.Y) < 1e-6
                             && System.Math.Abs(h.Z - tip.Z) < 1e-6),
               $"tip ({tip.X},{tip.Y},{tip.Z}) should be on the hull");
      }
    });
  }
}

}  // namespace GeomPPTests
