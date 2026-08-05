using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class GeometryCollection3DTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── GeometryCollection3D ──────────────────────────────────────────────────────
    Console.WriteLine("\nGeometryCollection3D");

    Test("DefaultConstructor_SizeZero", () => Eq(0, new GeometryCollection3D().Size(), 0));

    Test("AddPoint_SizeBecomesOne", () => {
      var gc = new GeometryCollection3D();
      gc.Add(new Point3D(1, 2, 3));
      Eq(1, gc.Size(), 0);
    });

    Test("AddMultipleTypes_SizeIsCorrect", () => {
      var gc = new GeometryCollection3D();
      gc.Add(new Point3D(0, 0, 0));
      gc.Add(LineSegment3D.Make(new Point3D(0, 0, 0), new Point3D(1, 0, 0)));
      Eq(2, gc.Size(), 0);
    });

    Test("Get_ReturnsCorrectType", () => {
      var gc = new GeometryCollection3D();
      gc.Add(new Point3D(1, 2, 3));
      var pt = gc.Get(0) as Point3D;
      NotNull(pt, "expected Point3D");
      Eq(1.0, pt!.X);
      Eq(2.0, pt.Y);
      Eq(3.0, pt.Z);
    });

    Test("AlmostEquals_SameCollections", () => {
      var a = new GeometryCollection3D();
      a.Add(new Point3D(1, 2, 3));
      var b = new GeometryCollection3D();
      b.Add(new Point3D(1, 2, 3));
      IsTrue(a.AlmostEquals(b));
    });

    Test("AlmostEquals_DifferentCollections", () => {
      var a = new GeometryCollection3D();
      a.Add(new Point3D(1, 2, 3));
      var b = new GeometryCollection3D();
      b.Add(new Point3D(9, 9, 9));
      IsFalse(a.AlmostEquals(b));
    });

    Test("AlmostEquals_BothEmpty", () => {
      IsTrue(new GeometryCollection3D().AlmostEquals(new GeometryCollection3D()));
    });

    Test("ToWkt_Empty_ContainsEMPTY", () => {
      IsTrue(new GeometryCollection3D().ToWkt().Contains("EMPTY"));
    });

    Test("ToWkt_WithPoint_ContainsKeywords", () => {
      var gc = new GeometryCollection3D();
      gc.Add(new Point3D(1, 2, 3));
      var wkt = gc.ToWkt();
      IsTrue(wkt.Contains("GEOMETRYCOLLECTION"), "missing GEOMETRYCOLLECTION");
      IsTrue(wkt.Contains("POINT"), "missing POINT");
    });

    Test("FromWkt_RoundTrip", () => {
      var gc = new GeometryCollection3D();
      gc.Add(new Point3D(1, 2, 3));
      IsTrue(gc.AlmostEquals(GeometryCollection3D.FromWkt(gc.ToWkt())));
    });

    Test("ToFile_FromFile_RoundTrip", () => {
      var gc = new GeometryCollection3D();
      gc.Add(new Point3D(7, 8, 9));
      var path = Path.GetTempFileName();
      gc.ToFile(path);
      IsTrue(gc.AlmostEquals(GeometryCollection3D.FromFile(path)));
      File.Delete(path);
    });

    // ── GeometryCollection3D (additional) ────────────────────────────────────────
    Console.WriteLine("\nGeometryCollection3D (additional)");

    Test("Add_Line3D_SizeIncreases", () => {
      var gc = new GeometryCollection3D();
      gc.Add(Line3D.Make(new Point3D(0,0,0), new Point3D(1,0,0)));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Ray3D_SizeIncreases", () => {
      var gc = new GeometryCollection3D();
      gc.Add(Ray3D.Make(new Point3D(0,0,0), new Vector3D(1,0,0)));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Polyline3D_SizeIncreases", () => {
      var gc = new GeometryCollection3D();
      gc.Add(Polyline3D.Make(new Point3D[] { new(0,0,0), new(1,0,0) }));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Polygon3D_SizeIncreases", () => {
      var gc = new GeometryCollection3D();
      gc.Add(Polygon3D.Make(new Point3D[] { new(0,0,0), new(1,0,0), new(1,1,0), new(0,1,0) }));
      Eq(1, gc.Size(), 0);
    });

    Test("Add_Triangle3D_SizeIncreases", () => {
      var gc = new GeometryCollection3D();
      gc.Add(Triangle3D.Make(new Point3D(0,0,0), new Point3D(1,0,0), new Point3D(0,1,0)));
      Eq(1, gc.Size(), 0);
    });

    Test("ToString_ContainsWkt", () => {
      var gc = new GeometryCollection3D();
      gc.Add(new Point3D(1, 2, 3));
      var s = gc.ToString();
      IsTrue(s.Contains("GEOMETRYCOLLECTION"), "missing GEOMETRYCOLLECTION keyword");
    });

    Test("Equality_SameCollection_True", () => {
      var a = new GeometryCollection3D();
      a.Add(new Point3D(1, 2, 3));
      var b = new GeometryCollection3D();
      b.Add(new Point3D(1, 2, 3));
      IsTrue(a == b);
    });

    Test("Equality_DifferentCollection_False", () => {
      var a = new GeometryCollection3D();
      a.Add(new Point3D(1, 2, 3));
      var b = new GeometryCollection3D();
      b.Add(new Point3D(9, 9, 9));
      IsFalse(a == b);
    });
  }
}

}  // namespace GeomPPTests
