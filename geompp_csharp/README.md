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

Decimal precision for all `<=>` or `AlmostEquals()` or `compare(a,b, epsilon)` functions are managed by a global, _thread local_ variable. It is best to set it up at the beginning of your program (in your _main_). The defalt is 3 decimals, or 0.001 tolorance. 

```csharp
G.Precision.DecimalPrecision = 6; // default: G.Precision.DP_THREE
```

Intersection methods return `object` (null when there is no intersection); use C# pattern matching
to extract the result type:

```csharp
var result = line.Intersection(segment);
if (result is G.Point2D p)       { /* point intersection */ }
if (result is G.LineSegment2D s) { /* overlap */ }
```

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
