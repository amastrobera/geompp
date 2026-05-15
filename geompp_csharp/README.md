[← back](../README.md)

# GeomPP — C# Bindings

C++/CLI bindings for [geompp](https://github.com/amastrobera/geompp), targeting **Windows x64**.

Available on NuGet as [`GeomPP`](https://www.nuget.org/packages/GeomPP).

Supports **.NET 8** (and later) and **.NET Framework 4.8**.

**[Changelog](https://github.com/amastrobera/geompp/blob/master/CHANGELOG.md)** — full release notes for every version.

---

## Install

```bash
dotnet add package GeomPP
```

or in your `.csproj`:

```xml
<PackageReference Include="GeomPP" Version="0.7.0" />
```

---

## Quick start

### Example 1 — LineSegment3D intersection

The code: 
  ```csharp
using G = GeomPP;

G.Precision.DecimalPrecision = G.Precision.DP_THREE;

var s1 = G.LineSegment3D.Make(new G.Point3D(1, 0, 0), new G.Point3D(-1, 0, 2));
var s2 = G.LineSegment3D.Make(new G.Point3D(0, 1, 0), new G.Point3D(0, -1, 2));

Console.WriteLine($"s1 = {s1}");
Console.WriteLine($"s2 = {s2}");

var result = s1.Intersection(s2);
if (result is G.Point3D p) {
    Console.WriteLine("OK: " + p.ToWkt());  // expects POINT (0 0 1)
} else {
    Console.WriteLine("FAIL: no intersection");
}
```

Output:
```bash
s1 = LINESTRING (1 0 0, -1 0 2)
s2 = LINESTRING (0 1 0, 0 -1 2)
intersection found: POINT (0 0 1)
intersection written to intersection.wkt
```

---

### Example 2 — Load geometries from an .lsv file

An `.lsv` file is a plain-text list of WKT geometries, one per line:

```bash
POINT (1 2 3)
POINT (4 5 6)
LINESTRING (0 0 0, 1 1 1)
LINESTRING (2 0 0, 2 3 4)
LINE (0 0 0, 1 0 0)
RAY (0 0 0, 0 1 0)
```

```csharp
using G = GeomPP;

G.Precision.DecimalPrecision = G.Precision.DP_THREE;

var parser = G.WktParser.Open("sample_geometries.lsv");

if (!parser.HasNext()) {
    Console.WriteLine("no geometries found");
    return;
}

while (parser.HasNext()) {
    var item = parser.Next();
    if (item == null) {
        Console.WriteLine("skipped unrecognised line");
        continue;
    }
    Console.WriteLine(G.WktParser.ToWkt(item));
}
```

Output:

```bash
POINT (1 2 3)
POINT (4 5 6)
LINESTRING (0 0 0, 1 1 1)
LINESTRING (2 0 0, 2 3 4)
LINE (0 0 0, 1 0 0)
RAY (0 0 0, 0 1 0)
```

---

### Example 3 — `are_coplanar`, winding order, and polygon with holes

```csharp
using GeomPP;
using Geompp.Extensions;

Precision.DecimalPrecision = Precision.DP_THREE;

var flat = new List<Point3D> {
    new Point3D(0,0,0), new Point3D(1,0,0),
    new Point3D(0,1,0), new Point3D(1,1,0)
};
var skew = new List<Point3D> {
    new Point3D(0,0,0), new Point3D(1,0,0),
    new Point3D(0,1,0), new Point3D(0,0,1)
};

Console.WriteLine(flat.AreCoplanar());   // True  — all on the XY plane
Console.WriteLine(skew.AreCoplanar());   // False — spans 3D space

// Closest world-axis plane and winding check
var plane = flat.ClosestWorldPlaneTo();
Console.WriteLine(plane.Normal());       // (0, 0, 1)  → XY plane

Console.WriteLine(flat.AreCCW());        // True  — CCW on the XY plane
Console.WriteLine(flat.AreCW());         // False

// Polygon3D requires CCW outer ring and CW holes
var outer = new List<Point3D> {
    new Point3D(0,0,0), new Point3D(4,0,0),
    new Point3D(4,4,0), new Point3D(0,4,0)
};
var hole = new List<Point3D> {
    new Point3D(1,3,0), new Point3D(3,3,0),
    new Point3D(3,1,0), new Point3D(1,1,0)
};
var poly = Polygon3D.Make(outer, new List<List<Point3D>> { hole });
Console.WriteLine(poly.Size());          // 4
```

Output:
```
True
False
VECTOR (0 0 1)
True
False
4
```

---

## Precision

All floating-point comparisons go through a thread-local precision setting:

```csharp
G.Precision.DecimalPrecision = G.Precision.DP_THREE;   // 3 decimal places (default)
G.Precision.DecimalPrecision = G.Precision.DP_SIX;     // 6 decimal places
G.Precision.DecimalPrecision = G.Precision.DP_NINE;    // 9 decimal places

double eps = G.Precision.Epsilon;  // current epsilon (10^-N)
```

---

## Supported types

| Type              | 2D | 3D |
|-------------------|----|----|
| `Point`           | ✓  | ✓  |
| `Vector`          | ✓  | ✓  |
| `Line`            | ✓  | ✓  |
| `Ray`             | ✓  | ✓  |
| `LineSegment`     | ✓  | ✓  |
| `Polyline`        | ✓  | ✓  |
| `Triangle`        | ✓  | ✓  |
| `Polygon`         | ✓  | ✓  |
| `BBox`            | ✓  | ✓  |
| `Plane`           | —  | ✓  |
| `GeometryCollection` | ✓ | ✓ |
| `WktParser`       | ✓  | ✓  |

All types expose `ToWkt()`, `FromWkt()`, `ToFile()`, `FromFile()`, `AlmostEquals()`, and the same
operators available in the C++ library.

Intersection methods return `object` (null when there is no intersection); use C# pattern matching
to extract the result type:

```csharp
var result = line.Intersection(segment);
if (result is G.Point2D p)      { /* point intersection */ }
if (result is G.LineSegment2D s) { /* overlap */ }
```

`Plane` exposes intersections, parallel / coplanar checks, and convenience constructors:

```csharp
var pl  = G.Plane.XY();
var ray = G.Ray3D.Make(new G.Point3D(5, 3, 4), new G.Vector3D(0, 0, -1));
var hit = pl.Intersection(ray);            // Point3D(5, 3, 0)  or null
var axisY = pl.Intersection(G.Plane.YZ()); // Line3D along the Y-axis  or null

pl.IsParallel(ray);                        // false (ray crosses the plane)
pl.IsCoplanar(G.Line3D.Make(new G.Point3D(0,0,0), new G.Point3D(1,1,0))); // true

// Implicit Vector → Point construction
var pFromV = new G.Point3D(new G.Vector3D(1, 2, 3));  // = Point3D(1, 2, 3)

// Triangle3D intersection with Line / Ray / Segment / Plane / Triangle
var tri    = G.Triangle3D.Make(new G.Point3D(0,0,0), new G.Point3D(4,0,0), new G.Point3D(0,4,0));
var hitL   = tri.Intersection(G.Line3D.Make(new G.Point3D(1, 1, -1), new G.Point3D(1, 1, 1)))             as G.Point3D;
var hitR   = tri.Intersection(G.Ray3D.Make(new G.Point3D(1, 1, 4),  new G.Vector3D(0, 0, -1)))            as G.Point3D;
var hitS   = tri.Intersection(G.LineSegment3D.Make(new G.Point3D(1, 1, -2), new G.Point3D(1, 1, 3)))      as G.Point3D;
var y1     = G.Plane.FromOriginAndNormal(new G.Point3D(0, 1, 0), new G.Vector3D(0, 1, 0));
var hitP   = tri.Intersection(y1)                                                                          as G.LineSegment3D;  // (0,1,0)→(3,1,0)
var other  = G.Triangle3D.Make(new G.Point3D(1,1,-1), new G.Point3D(1,1,1), new G.Point3D(3,1,0));
var hitT   = tri.Intersection(other)                                                                       as G.LineSegment3D;  // (1,1,0)→(3,1,0)
```

---

## Platform note

GeomPP is built with C++/CLI and is **Windows x64 only**. It will not run on Linux, macOS, or
32-bit processes.


## Build on Windows x64 manually

If you want to built from this source files, use these commands


Build the C# DLL
```powershell
# from the main directory, geompp

# if you want to build for .Net 8
msbuild geompp_csharp\GeomPP.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# if you want to build for .Net Framework 4.8
msbuild geompp_csharp\GeomPP_Net48.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# run smoke tests, after build from the main directory geompp
dotnet test geompp_csharp\tests\GeomPPTests.csproj [-p:GeomPPConfiguration=Release]
```