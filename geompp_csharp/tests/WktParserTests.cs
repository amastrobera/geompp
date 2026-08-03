using GeomPP;
using Geompp.Extensions;
using System.IO;

namespace GeomPPTests {

public static class WktParserTests {
  public static void Run(TestHarness h) {
    void Test(string name, Action body) => h.Test(name, body);
    void Eq(double expected, double actual, int decimals = 3) => h.Eq(expected, actual, decimals);
    void IsTrue(bool value, string msg = "expected true") => h.IsTrue(value, msg);
    void IsFalse(bool value, string msg = "expected false") => h.IsFalse(value, msg);
    void NotNull(object? value, string msg = "expected non-null") => h.NotNull(value, msg);
    void IsNull(object? value, string msg = "expected null") => h.IsNull(value, msg);

    // ── WktParser ─────────────────────────────────────────────────────────────────
    Console.WriteLine("\nWktParser");

    string MakeTempWktFile(params string[] lines) {
      var path = Path.GetTempFileName();
      File.WriteAllLines(path, lines);
      return path;
    }

    Test("Open_ValidFile_NoThrow", () => {
      var path = MakeTempWktFile("POINT (1 2)");
      using (var parser = WktParser.Open(path)) { NotNull(parser); }
      File.Delete(path);
    });

    Test("GetFilePath_MatchesInput", () => {
      var path = MakeTempWktFile("POINT (1 2)");
      using (var parser = WktParser.Open(path)) {
        IsTrue(parser.GetFilePath() == path, "file path mismatch");
      }
      File.Delete(path);
    });

    Test("HasNext_TrueWhenFileHasContent", () => {
      var path = MakeTempWktFile("POINT (1 2)");
      using (var parser = WktParser.Open(path)) { IsTrue(parser.HasNext()); }
      File.Delete(path);
    });

    Test("Next_ReturnsPoint2D_WithCorrectCoords", () => {
      var path = MakeTempWktFile("POINT (3 4)");
      using (var parser = WktParser.Open(path)) {
        var pt = parser.Next() as Point2D;
        NotNull(pt, "expected Point2D");
        Eq(3.0, pt!.X);
        Eq(4.0, pt.Y);
      }
      File.Delete(path);
    });

    Test("Next_ReturnsPoint3D", () => {
      var path = MakeTempWktFile("POINT (1 2 3)");
      using (var parser = WktParser.Open(path)) {
        NotNull(parser.Next() as Point3D, "expected Point3D");
      }
      File.Delete(path);
    });

    Test("Next_ReturnsLineSegment2D", () => {
      var path = MakeTempWktFile("LINESTRING (0 0, 1 0)");
      using (var parser = WktParser.Open(path)) {
        NotNull(parser.Next() as LineSegment2D, "expected LineSegment2D");
      }
      File.Delete(path);
    });

    Test("Next_MultipleLines_InOrder", () => {
      var path = MakeTempWktFile("POINT (1 0)", "POINT (2 0)");
      using (var parser = WktParser.Open(path)) {
        var a = (parser.Next() as Point2D)!;
        var b = (parser.Next() as Point2D)!;
        Eq(1.0, a.X);
        Eq(2.0, b.X);
      }
      File.Delete(path);
    });

    Test("Next_ReturnsNullAfterExhausted", () => {
      var path = MakeTempWktFile("POINT (1 2)");
      using (var parser = WktParser.Open(path)) {
        parser.Next();
        IsNull(parser.Next(), "should return null when exhausted");
      }
      File.Delete(path);
    });

    Test("ToWkt_Point2D_ContainsCoordinates", () => {
      var pt = new Point2D(5.0, 6.0);
      var wkt = WktParser.ToWkt(pt);
      NotNull(wkt);
      IsTrue(wkt!.Contains("POINT"), "missing POINT keyword");
      IsTrue(wkt.Contains("5"), "missing x");
      IsTrue(wkt.Contains("6"), "missing y");
    });

    Test("ToString_ContainsFilePath", () => {
      var path = MakeTempWktFile("POINT (1 2)");
      using (var parser = WktParser.Open(path)) {
        var s = parser.ToString();
        IsTrue(s.Contains("WktParser"), "missing WktParser label");
        IsTrue(s.Contains(path), "missing file path");
      }
      File.Delete(path);
    });
  }
}

}  // namespace GeomPPTests
