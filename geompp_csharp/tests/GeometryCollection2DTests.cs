using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class GeometryCollection2DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── GeometryCollection2D ──────────────────────────────────────────────────────
    Console.WriteLine("\nGeometryCollection2D");

    Test("DefaultConstructor_SizeZero", () => Eq(0, new GeometryCollection2D().Size(), 0));

    Test("AddPoint_SizeBecomesOne", () => {
      var gc = new GeometryCollection2D();
      gc.Add(new Point2D(1, 2));
      Eq(1, gc.Size(), 0);
    });

    Test("AddMultipleTypes_SizeIsCorrect", () => {
      var gc = new GeometryCollection2D();
      gc.Add(new Point2D(0, 0));
      gc.Add(LineSegment2D.Make(new Point2D(0, 0), new Point2D(1, 0)));
      Eq(2, gc.Size(), 0);
    });

    Test("Get_ReturnsCorrectType", () => {
      var gc = new GeometryCollection2D();
      gc.Add(new Point2D(3, 4));
      var pt = gc.Get(0) as Point2D;
      NotNull(pt, "expected Point2D");
      Eq(3.0, pt!.X);
      Eq(4.0, pt.Y);
    });

    Test("AlmostEquals_SameCollections", () => {
      var a = new GeometryCollection2D();
      a.Add(new Point2D(1, 2));
      var b = new GeometryCollection2D();
      b.Add(new Point2D(1, 2));
      IsTrue(a.AlmostEquals(b));
    });

    Test("AlmostEquals_DifferentCollections", () => {
      var a = new GeometryCollection2D();
      a.Add(new Point2D(1, 2));
      var b = new GeometryCollection2D();
      b.Add(new Point2D(9, 9));
      IsFalse(a.AlmostEquals(b));
    });

    Test("AlmostEquals_BothEmpty", () => {
      IsTrue(new GeometryCollection2D().AlmostEquals(new GeometryCollection2D()));
    });

    Test("ToWkt_Empty_ContainsEMPTY", () => {
      IsTrue(new GeometryCollection2D().ToWkt().Contains("EMPTY"));
    });

    Test("ToWkt_WithPoint_ContainsKeywords", () => {
      var gc = new GeometryCollection2D();
      gc.Add(new Point2D(1, 2));
      var wkt = gc.ToWkt();
      IsTrue(wkt.Contains("GEOMETRYCOLLECTION"), "missing GEOMETRYCOLLECTION");
      IsTrue(wkt.Contains("POINT"), "missing POINT");
    });

    Test("FromWkt_RoundTrip", () => {
      var gc = new GeometryCollection2D();
      gc.Add(new Point2D(1, 2));
      IsTrue(gc.AlmostEquals(GeometryCollection2D.FromWkt(gc.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var gc = new GeometryCollection2D();
      gc.Add(new Point2D(7, 8));
      var path = Path.GetTempFileName();
      gc.ToFile(path);
      IsTrue(gc.AlmostEquals(GeometryCollection2D.FromFile(path)));
      File.Delete(path);
    });

    // ── GeometryCollection2D (additional) ────────────────────────────────────────
    Console.WriteLine("\nGeometryCollection2D (additional)");

    Test("Add_Line2D_SizeIncreases", () => {
      var gc = new GeometryCollection2D();
      gc.Add(Line2D.Make(new Point2D(0,0), new Point2D(1,0)));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Ray2D_SizeIncreases", () => {
      var gc = new GeometryCollection2D();
      gc.Add(Ray2D.Make(new Point2D(0,0), new Vector2D(1,0)));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Polyline2D_SizeIncreases", () => {
      var gc = new GeometryCollection2D();
      gc.Add(Polyline2D.Make(new Point2D[] { new(0,0), new(1,0) }));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Polygon2D_SizeIncreases", () => {
      var gc = new GeometryCollection2D();
      gc.Add(Polygon2D.Make(new Point2D[] { new(0,0), new(1,0), new(1,1), new(0,1) }));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Triangle2D_SizeIncreases", () => {
      var gc = new GeometryCollection2D();
      gc.Add(Triangle2D.Make(new Point2D(0,0), new Point2D(1,0), new Point2D(0,1)));
      Eq(1, gc.Size(), 0);
    });

    Test("ToString_ContainsWkt", () => {
      var gc = new GeometryCollection2D();
      gc.Add(new Point2D(1, 2));
      var s = gc.ToString();
      IsTrue(s.Contains("GEOMETRYCOLLECTION"), "missing GEOMETRYCOLLECTION keyword");
    });

    Test("Equality_SameCollection_True", () => {
      var a = new GeometryCollection2D();
      a.Add(new Point2D(1, 2));
      var b = new GeometryCollection2D();
      b.Add(new Point2D(1, 2));
      IsTrue(a == b);
    });

    Test("Equality_DifferentCollection_False", () => {
      var a = new GeometryCollection2D();
      a.Add(new Point2D(1, 2));
      var b = new GeometryCollection2D();
      b.Add(new Point2D(9, 9));
      IsFalse(a == b);
    });
  }
}

}  // namespace GeomPPTests
