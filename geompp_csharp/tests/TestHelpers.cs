using GeomPP;

namespace GeomPPTests {

// Shared helpers used across multiple topic files (Polygon2DTests, Polygon3DTests, TriangulateTests).
// Single-use helpers (e.g. StarPolygonPoints, ExpectNoPolygonVertexHangsOnTriangleEdge) stay local to
// the one file that needs them instead of living here.
public static class TestHelpers {
  public static double SumArea2D(System.Collections.Generic.IEnumerable<Triangle2D> triangles) {
    double total = 0;
    foreach (var t in triangles) total += t.Area();
    return total;
  }

  public static double SumArea3D(System.Collections.Generic.IEnumerable<Triangle3D> triangles) {
    double total = 0;
    foreach (var t in triangles) total += t.Area();
    return total;
  }

  public static int CountOf<T>(System.Collections.Generic.IEnumerable<T> items) {
    int n = 0;
    foreach (var _ in items) n++;
    return n;
  }
}

}  // namespace GeomPPTests
