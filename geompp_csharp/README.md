[← back](../README.md)

# GeomPP — C# Bindings

C++/CLI bindings for [geompp](https://github.com/amastrobera/geompp), targeting **Windows x64**.

Available on NuGet as [`GeomPP`](https://www.nuget.org/packages/GeomPP).

Supports **.NET 8**, **.NET 9**, **.NET 10**, and **.NET Framework 4.8**.

**[Changelog](https://github.com/amastrobera/geompp/blob/master/CHANGELOG.md)** — full release notes for every version.

---

## Install

```bash
dotnet add package GeomPP
```

or in your `.csproj`:

```xml
<PackageReference Include="GeomPP" Version="0.8.0" />
```

---

## How to use it

You can look at the [test suite](./tests/) to see detailed usage. There also is a whole [set of code examples](./code_examples.md) in the next page.

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
| `BBall`           | ✓  | ✓  |
| `BRect2D`         | ✓  | —  |
| `BPrism3D`        | —  | ✓  |
| `Plane`           | —  | ✓  |
| `View2D`          | ✓  | —  |
| `GeometryCollection` | ✓ | ✓ |
| `WktParser`       | ✓  | ✓  |

All types expose `ToWkt()`, `FromWkt()`, `ToFile()`, `FromFile()`, `AlmostEquals()`, and the same
operators available in the C++ library.

Intersection methods return `object` (null when there is no intersection); use C# pattern matching
to extract the result type:

```csharp
var result = line.Intersection(segment);
if (result is G.Point2D p)       { /* point intersection */ }
if (result is G.LineSegment2D s) { /* overlap */ }
```

---

## Planar operations

`View2D` projects 3D points into 2D coordinates via `.X(point)` / `.Y(point)`. It is particularly
useful for streaming large containers of `Point3D` without allocating an intermediate list of
`Point2D` — each call reads one or two scalar coordinates directly.

```csharp
using G = GeomPP;

// axis-aligned views (fastest path)
var vXY = G.View2D.XY();   // x→x, y→y (drops z)
var vYZ = G.View2D.YZ();   // y→x, z→y (drops x)
var vZX = G.View2D.ZX();   // z→x, x→y (drops y)

// custom view onto any plane
var plane = G.Plane.FromOriginAndNormal(new G.Point3D(0, 0, 5), new G.Vector3D(0, 0, 1));
var vCustom = G.View2D.OnPlane(plane);

var pts3d = new[] { new G.Point3D(1, 2, 5), new G.Point3D(3, 4, 5), new G.Point3D(5, 6, 5) };

// stream 3D points to 2D without building a Point2D array
var xs = pts3d.Select(p => vXY.X(p)).ToArray();  // [1, 3, 5]
var ys = pts3d.Select(p => vXY.Y(p)).ToArray();  // [2, 4, 6]

Console.WriteLine(vXY.Type());  // XY
```

---

## Bounding containers

`BRect2D` — minimum oriented bounding rectangle (rotating calipers; requires ≥ 3 non-collinear points):

```csharp
using G = GeomPP;

var pts = new[] {
    new G.Point2D(0, 0), new G.Point2D(4, 0), new G.Point2D(4, 3),
    new G.Point2D(2, 4), new G.Point2D(0, 3),
};
var rect = new G.BRect2D(pts);
Console.WriteLine(rect.Center());                   // roughly (2.0, 1.75)
Console.WriteLine(rect.AxisU() + " " + rect.AxisV()); // orthonormal 2D frame
Console.WriteLine($"{rect.Width()} × {rect.Height()}  area={rect.Area()}");
var corners = rect.Corners();                        // array of 4 Point2D
Console.WriteLine(rect.Contains(new G.Point2D(2, 1))); // True
Console.WriteLine(rect.AlmostEquals(new G.BRect2D(pts))); // True
```

`BPrism3D` — minimum oriented bounding prism (PCA + rotating calipers; requires ≥ 3 non-collinear points):

```csharp
using G = GeomPP;

var pts = new[] {
    new G.Point3D(0, 0, 0), new G.Point3D(4, 0, 0),
    new G.Point3D(4, 3, 0), new G.Point3D(0, 3, 0),
    new G.Point3D(0, 0, 2), new G.Point3D(4, 0, 2),
    new G.Point3D(4, 3, 2), new G.Point3D(0, 3, 2),
};
var prism = new G.BPrism3D(pts);
Console.WriteLine(prism.Center());                  // roughly (2, 1.5, 1)
Console.WriteLine($"U={prism.AxisU()} V={prism.AxisV()} W={prism.AxisW()}");
Console.WriteLine($"{prism.Width()} × {prism.Height()} × {prism.Depth()}");
Console.WriteLine($"volume={prism.Volume()}");      // ~24.0
var corners = prism.Corners();                      // array of 8 Point3D
Console.WriteLine(prism.Contains(prism.Center()));  // True
Console.WriteLine(prism.AlmostEquals(new G.BPrism3D(pts))); // True
```

---

## Platform note

GeomPP is built with C++/CLI and is **Windows x64 only**. It will not run on Linux, macOS, or
32-bit processes.

---

## Build on Windows x64 manually

Build the C# DLL
```powershell
# from the main directory, geompp

# .NET 8 (LTS, supported until Nov 2026)
msbuild geompp_csharp\GeomPP_Net8.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# .NET 9 (STS)
msbuild geompp_csharp\GeomPP_Net9.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# .NET 10 (LTS)
msbuild geompp_csharp\GeomPP.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# .NET Framework 4.8
msbuild geompp_csharp\GeomPP_Net48.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# run smoke tests, after build from the main directory geompp
dotnet test geompp_csharp\tests\GeomPPTests.csproj [-p:GeomPPConfiguration=Release]
```
