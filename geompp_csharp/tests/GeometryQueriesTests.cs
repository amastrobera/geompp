using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class GeometryQueriesTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── GeomUtil segment-set intersection (Shamos–Hoey / Bentley–Ottmann) ──────────
    // NOTE: correctness rides on the (currently provisional) sweep-status comparator; these encode the
    // intended behaviour and should be re-verified once the real ordering lands.
    List<LineSegment2D> SquareRing() => new List<LineSegment2D> {
      LineSegment2D.Make(new Point2D(0,0), new Point2D(1,0)),
      LineSegment2D.Make(new Point2D(1,0), new Point2D(1,1)),
      LineSegment2D.Make(new Point2D(1,1), new Point2D(0,1)),
      LineSegment2D.Make(new Point2D(0,1), new Point2D(0,0)),
    };
    List<LineSegment2D> SelfIntersectingRing() => new List<LineSegment2D> {
      // edges (4,0)->(1,3) and (3,3)->(0,0) cross at (2,2)
      LineSegment2D.Make(new Point2D(0,0), new Point2D(4,0)),
      LineSegment2D.Make(new Point2D(4,0), new Point2D(1,3)),
      LineSegment2D.Make(new Point2D(1,3), new Point2D(3,3)),
      LineSegment2D.Make(new Point2D(3,3), new Point2D(0,0)),
    };

    Test("HasIntersections_SimpleRing_False", () => {
      IsFalse(GeomUtil.HasIntersections(SquareRing()), "simple ring has no self-intersections");
    });

    Test("HasIntersections_SelfIntersecting_True", () => {
      IsTrue(GeomUtil.HasIntersections(SelfIntersectingRing()), "self-intersecting ring");
    });

    Test("FindIntersections_SimpleRing_Empty", () => {
      int count = 0;
      foreach (var _ in GeomUtil.FindIntersections(SquareRing())) { count++; }
      Eq(0, count);
    });

    Test("FindIntersections_ReportsCrossing", () => {
      bool found = false;
      foreach (var p in GeomUtil.FindIntersections(SelfIntersectingRing())) {
        if (System.Math.Abs(p.X - 2.0) < 1e-6 && System.Math.Abs(p.Y - 2.0) < 1e-6) { found = true; }
      }
      IsTrue(found, "expected the (2,2) crossing among reported intersections");
    });

    Console.WriteLine("\nGeomUtil::PrincipalAxes/Normal/Direction");
    {
        var cloud = new System.Collections.Generic.List<Point3D> {
            new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(2,0,0), new Point3D(3,0,0),
            new Point3D(0,0.1,0), new Point3D(1,0.1,0), new Point3D(2,0.1,0), new Point3D(3,0.1,0)
        };

        Test("PrincipalAxes_NotNull",         () => NotNull(GeomUtil.PrincipalAxes(cloud)));
        Test("PrincipalAxes_X_NotNull",       () => NotNull(GeomUtil.PrincipalAxes(cloud).X));
        Test("PrincipalAxes_Z_IsNormal",      () => {
            var frame = GeomUtil.PrincipalAxes(cloud);
            // Z should be ~(0,0,1) or (0,0,-1)
            Eq(1.0, Math.Abs(frame.Z.Z), 2);
        });
        Test("PrincipalNormal_NotNull",       () => NotNull(GeomUtil.PrincipalNormal(cloud)));
        Test("PrincipalDirection_NotNull",    () => NotNull(GeomUtil.PrincipalDirection(cloud)));
        Test("PrincipalDirection_AlongX",     () => {
            var dir = GeomUtil.PrincipalDirection(cloud);
            Eq(1.0, Math.Abs(dir.X), 2);
        });
    }

    // ── GeomUtil::FindExtremePoints ───────────────────────────────────────────────
    Console.WriteLine("\nGeomUtil::FindExtremePoints");
    {
        // 2D convex diamond → O(log n) Sunday binary search
        var diamond = Polygon2D.Make(new Point2D[] { new(2, 0), new(4, 2), new(2, 4), new(0, 2) });
        Test("FindExtremePoints2D_Convex_AlongX", () => {
            var ex = GeomUtil.FindExtremePoints(diamond, Line2D.Make(new Point2D(0, 0), new Point2D(1, 0)));
            IsTrue(ex.MinPoint.AlmostEquals(new Point2D(0, 2)), "min");
            IsTrue(ex.MaxPoint.AlmostEquals(new Point2D(4, 2)), "max");
        });

        // 2D concave dart → O(n) brute force
        var dart = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(2, 1), new(0, 4) });
        Test("FindExtremePoints2D_Concave", () => {
            var ex = GeomUtil.FindExtremePoints(dart, Line2D.Make(new Point2D(0, 0), new Point2D(1, 2)));
            IsTrue(ex.MinPoint.AlmostEquals(new Point2D(0, 0)), "min");
            IsTrue(ex.MaxPoint.AlmostEquals(new Point2D(4, 4)), "max");
        });

        // 3D diamond in the XY plane
        var diamond3 = Polygon3D.Make(new Point3D[] { new(2, 0, 0), new(4, 2, 0), new(2, 4, 0), new(0, 2, 0) });
        Test("FindExtremePoints3D_Convex_AlongX", () => {
            var ex = GeomUtil.FindExtremePoints(diamond3, Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0)));
            IsTrue(ex.MinPoint.AlmostEquals(new Point3D(0, 2, 0)), "min");
            IsTrue(ex.MaxPoint.AlmostEquals(new Point3D(4, 2, 0)), "max");
        });

        // 3D convex parallelogram in the tilted plane x = z
        var para = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(2, 0, 2), new(2, 2, 2), new(0, 2, 0) });
        Test("FindExtremePoints3D_TiltedPlane", () => {
            var ex = GeomUtil.FindExtremePoints(para, Line3D.Make(new Point3D(0, 0, 0), new Point3D(1, 1, 0)));
            IsTrue(ex.MinPoint.AlmostEquals(new Point3D(0, 0, 0)), "min");
            IsTrue(ex.MaxPoint.AlmostEquals(new Point3D(2, 2, 2)), "max");
        });
    }

    // ── GeomUtil::DistanceTo ────────────────────────────────────────────────────────
    Console.WriteLine("\nGeomUtil::DistanceTo");
    {
        var square = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(0, 4) });
        Test("DistanceTo2D_LineCrossing_IsZero", () => {
            var line = Line2D.Make(new Point2D(2, -1), new Point2D(2, 5));
            Eq(0.0, GeomUtil.DistanceTo(square, line), 9);
        });
        Test("DistanceTo2D_LineOutside", () => {
            var line = Line2D.Make(new Point2D(6, -1), new Point2D(6, 5));
            Eq(2.0, GeomUtil.DistanceTo(square, line), 9);
        });

        var dart = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(2, 1), new(0, 4) });
        Test("DistanceTo2D_NonConvex_LineOutside", () => {
            var line = Line2D.Make(new Point2D(10, -1), new Point2D(10, 5));
            Eq(6.0, GeomUtil.DistanceTo(dart, line), 9);
        });

        var square3 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 4, 0), new(0, 4, 0) });
        Test("DistanceTo3D_Coplanar_LineOutside", () => {
            var line = Line3D.Make(new Point3D(6, -1, 0), new Point3D(6, 5, 0));
            Eq(2.0, GeomUtil.DistanceTo(square3, line), 9);
        });
        Test("DistanceTo3D_ParallelOffset_PythagoreanCombination", () => {
            // h=3, d2d=2 (same in-plane line as the coplanar case) -> sqrt(9+4) = sqrt(13)
            var line = Line3D.Make(new Point3D(6, 0, 3), new Point3D(6, 1, 3));
            Eq(Math.Sqrt(13.0), GeomUtil.DistanceTo(square3, line), 9);
        });
        Test("DistanceTo3D_SkewPerpendicular_CrossingInside_IsZero", () => {
            var line = Line3D.Make(new Point3D(2, 2, -1), new Point3D(2, 2, 1));
            Eq(0.0, GeomUtil.DistanceTo(square3, line), 9);
        });
        Test("DistanceTo3D_SkewOblique_CrossingOutside_AnisotropicMetric", () => {
            // Crosses the plane at 45 degrees off the normal at (6,2,0), outside the square.
            // Correct answer sqrt(2) ~= 1.41421356, not the naive in-plane 2 (6-4).
            var line = Line3D.Make(new Point3D(6, 2, 0), new Point3D(7, 2, 1));
            Eq(Math.Sqrt(2.0), GeomUtil.DistanceTo(square3, line), 9);
        });
    }

    // ── GeomUtil::TangentsTo ────────────────────────────────────────────────────────
    Console.WriteLine("\nGeomUtil::TangentsTo");
    {
        var square = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(0, 4) });
        Test("TangentsTo2D_ConvexSquare_Point", () => {
            var t = GeomUtil.TangentsTo(square, new Point2D(10, -2));
            IsTrue(t.Left.Last().AlmostEquals(new Point2D(0, 0)), "left");
            IsTrue(t.Right.Last().AlmostEquals(new Point2D(4, 4)), "right");
        });

        var dart = Polygon2D.Make(new Point2D[] { new(0, 0), new(4, 0), new(4, 4), new(2, 1), new(0, 4) });
        Test("TangentsTo2D_NonConvexDart_Point_ReducesToConvexHull", () => {
            var t = GeomUtil.TangentsTo(dart, new Point2D(-6, 2));
            IsTrue(t.Left.Last().AlmostEquals(new Point2D(0, 4)), "left");
            IsTrue(t.Right.Last().AlmostEquals(new Point2D(0, 0)), "right");
        });

        var squareB = Polygon2D.Make(new Point2D[] { new(10, 1), new(14, 1), new(14, 5), new(10, 5) });
        Test("TangentsTo2D_ConvexSquares_Polygon", () => {
            var t = GeomUtil.TangentsTo(square, squareB);
            IsTrue(t.Left.First().AlmostEquals(new Point2D(0, 4)), "left.first");
            IsTrue(t.Left.Last().AlmostEquals(new Point2D(10, 5)), "left.last");
            IsTrue(t.Right.First().AlmostEquals(new Point2D(4, 0)), "right.first");
            IsTrue(t.Right.Last().AlmostEquals(new Point2D(14, 1)), "right.last");
        });

        var dartB = Polygon2D.Make(new Point2D[] { new(10, 1), new(14, 1), new(14, 5), new(12, 2), new(10, 5) });
        Test("TangentsTo2D_NonConvexDarts_Polygon_ReducesBothToConvexHull", () => {
            var t = GeomUtil.TangentsTo(dart, dartB);
            IsTrue(t.Left.First().AlmostEquals(new Point2D(0, 4)), "left.first");
            IsTrue(t.Left.Last().AlmostEquals(new Point2D(10, 5)), "left.last");
            IsTrue(t.Right.First().AlmostEquals(new Point2D(4, 0)), "right.first");
            IsTrue(t.Right.Last().AlmostEquals(new Point2D(14, 1)), "right.last");
        });

        var square3 = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 0), new(4, 4, 0), new(0, 4, 0) });
        Test("TangentsTo3D_ConvexSquare_OnXYPlane_Point", () => {
            var t = GeomUtil.TangentsTo(square3, new Point3D(10, -2, 0));
            IsTrue(t.Left.Last().AlmostEquals(new Point3D(0, 0, 0)), "left");
            IsTrue(t.Right.Last().AlmostEquals(new Point3D(4, 4, 0)), "right");
        });

        var tilted = Polygon3D.Make(new Point3D[] { new(0, 0, 0), new(4, 0, 4), new(4, 4, 4), new(0, 4, 0) });
        Test("TangentsTo3D_ConvexSquare_OnTiltedPlane_Point", () => {
            var t = GeomUtil.TangentsTo(tilted, new Point3D(10, -2, 10));
            IsTrue(t.Left.Last().AlmostEquals(new Point3D(0, 0, 0)), "left");
            IsTrue(t.Right.Last().AlmostEquals(new Point3D(4, 4, 4)), "right");
        });

        var tiltedB = Polygon3D.Make(new Point3D[] { new(10, 1, 10), new(14, 1, 14), new(14, 5, 14), new(10, 5, 10) });
        Test("TangentsTo3D_ConvexSquares_OnTiltedPlane_Polygon", () => {
            var t = GeomUtil.TangentsTo(tilted, tiltedB);
            IsTrue(t.Left.First().AlmostEquals(new Point3D(0, 4, 0)), "left.first");
            IsTrue(t.Left.Last().AlmostEquals(new Point3D(10, 5, 10)), "left.last");
            IsTrue(t.Right.First().AlmostEquals(new Point3D(4, 0, 4)), "right.first");
            IsTrue(t.Right.Last().AlmostEquals(new Point3D(14, 1, 14)), "right.last");
        });

        Test("TangentsTo3D_Point_ThrowsWhenNotCoplanar", () => {
            bool threw = false;
            try { GeomUtil.TangentsTo(square3, new Point3D(10, -2, 1)); }
            catch (Exception) { threw = true; }
            IsTrue(threw, "expected throw for non-coplanar point");
        });

        var square3B = Polygon3D.Make(new Point3D[] { new(10, 1, 1), new(14, 1, 1), new(14, 5, 1), new(10, 5, 1) });
        Test("TangentsTo3D_Polygon_ThrowsWhenNotCoplanar", () => {
            bool threw = false;
            try { GeomUtil.TangentsTo(square3, square3B); }
            catch (Exception) { threw = true; }
            IsTrue(threw, "expected throw for non-coplanar polygons");
        });
    }
  }
}

}  // namespace GeomPPTests
