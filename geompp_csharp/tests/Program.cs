using GeomPPTests;

// GeomPP C# binding test suite -- entry point.
//
// This used to be one ~6800-line top-level-statements file. It's now split into one file per topic
// (Point2DTests.cs, Polygon2DTests.cs, TriangulateTests.cs, ...), each a `public static class XTests`
// with a `Run(TestHarness h)` method, since C# only allows ONE file per project to use top-level
// statements -- this file is that one file, and it's reduced to pure orchestration.
//
// Run via: dotnet run --project geompp_csharp\tests\GeomPPTests.csproj -c Debug
// (NOT `dotnet test` -- GeomPPTests.csproj is OutputType=Exe, a console harness, not a Test SDK
// project; `dotnet test` would report success without running anything.)

var h = new TestHarness();

PrecisionTests.Run(h);
Point2DTests.Run(h);
Point3DTests.Run(h);
Vector2DTests.Run(h);
Vector3DTests.Run(h);
Line2DTests.Run(h);
Line3DTests.Run(h);
Ray2DTests.Run(h);
Ray3DTests.Run(h);
LineSegment2DTests.Run(h);
LineSegment3DTests.Run(h);
Polyline2DTests.Run(h);
Polyline3DTests.Run(h);
Triangle2DTests.Run(h);
Triangle3DTests.Run(h);
Polygon2DTests.Run(h);
Polygon3DTests.Run(h);
BBoxTests.Run(h);
BBallTests.Run(h);
BRect2DTests.Run(h);
BPrism3DTests.Run(h);
PlaneTests.Run(h);
WktParserTests.Run(h);
GeometryCollection2DTests.Run(h);
GeometryCollection3DTests.Run(h);
ConvexHullTests.Run(h);
TriangulateTests.Run(h);
CurveSmoothingTests.Run(h);
DecimationTests.Run(h);
PolylineExpansionTests.Run(h);
GeometryQueriesTests.Run(h);
PointFreeFunctionTests.Run(h);
GridCellTests.Run(h);
MeshTests.Run(h);
ConnectedMeshTests.Run(h);
PolyMeshTests.Run(h);
MathsTests.Run(h);
TransformationsTests.Run(h);

h.PrintSummary();
