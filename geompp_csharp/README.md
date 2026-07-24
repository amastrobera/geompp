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

| Platform | .Net | .Net Framework |
|---|---|---|
| Windows x64  | 8 · 9 · 10 | 4.8 |


## Platform note

GeomPP is built with C++/CLI and is **Windows x64 only**. It will not run on Linux, macOS, or
32-bit processes.

---


## Classes

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


## Algorithm overview

Each class supports a consistent set of spatial operations where applicable:

- **Containment** — does a shape contain a given point?
- **Intersection** — do two shapes strike through each other, and what is the resulting geometry? Also available as `GeomUtil.FindIntersections()` on a free set of segments. The meaning of this operation changes from 2D to 3D — check the class docs.
- **Overlap** — do two shapes have a portion in common, and what is the resulting geometry? Meaning changes from 2D to 3D.
- **Touch** — do two shapes have a point in common, and which is it? Meaning changes from 2D to 3D.
- **Distance** — closest distance from a point to a shape.
- **Plane operations** — projection of a point from 3D to 2D, and re-projection from 2D to 3D, via the `Plane` class or the faster `View2D` (one of the 3 world planes XY/YZ/ZX, or a custom plane).
- **Interpolation / Location** — `GeomUtil.Lerp(p0, p1, t)` retrieves a point at parameter `t` between two points (not clamped); `Interpolate(t)` does the same along a segment or polyline; the opposite operation finds the parameter `t` for a point already on a shape.
- **Area / Perimeter / Centroid** — geometric properties for closed shapes.
- **Signed area** — encodes orientation (clockwise vs. counter-clockwise in 2D, surface normal direction in 3D).
- **Simplicity / self-intersection** — `Polygon2D.IsSimple()` and `GeomUtil.HasIntersections(segments)` (Shamos–Hoey, boolean) / `GeomUtil.FindIntersections(segments)` (Bentley–Ottmann, every crossing point).
- **Convex hull** — `GeomUtil.ConvexHull(points)` — Andrew's monotone chain, returns hull vertices in CCW order.
- **Bounding containers** — tight-fitting containers around point clouds: axis-aligned bounding box, bounding ball, minimal oriented rectangle, convex hull.
- **Polyline operations** — `Polyline.Reduce()` (decimation) and `Polyline.Expand()` (Bezier corner smoothing), or the underlying `GeomUtil.DistDecimation()`/`RdpDecimation()`/`VwDecimation()`/`BezierSmoothing2()`/`PolylineExpansion()` for a plain point list.
- **Polygon boolean operations** — `Intersection()`, `Union()`, `Difference()`, `Xor()` between two polygons (map-overlay method), or `GeomUtil.Clip(clipperLoop, subjectLoop)` for raw point loops without constructing a `Polygon` first.
- **Point cloud operations** — `GeomUtil.PrincipalAxes()` (PCA) finds the empirical 3 directive axes of a list of points in space.

Intersection-style methods return `object` (`null` on no intersection) — see the pattern-matching example above.

## Free functions

`GeomUtil` is a static class wrapping the geompp free functions that operate on point/segment lists
directly, without needing a class instance first:

| Function | Description |
|---|---|
| `GeomUtil.AreCoplanar(points)` | List of `Point3D` on the same plane |
| `GeomUtil.ClosestWorldPlaneTo(points)` | XY / YZ / ZX plane nearest to the point cloud |
| `GeomUtil.AreCCW(points, refPlane)` | Counter-clockwise winding (3D; `refPlane = null` auto-detects) |
| `GeomUtil.AreCW(points, refPlane)` | Clockwise winding (3D; `refPlane = null` auto-detects) |
| `GeomUtil.Lerp(p0, p1, t)` | Linear interpolation between two points — `p0 + t*(p1-p0)`, not clamped |
| `GeomUtil.Clip(clipperLoop, subjectLoop)` | Set intersection of two point loops — `Point2D` natively, `Point3D` if coplanar (same map-overlay engine as `Polygon.Intersection()`) |
| `GeomUtil.HasIntersections(segments)` | Shamos–Hoey: `true` if any two segments in a `LineSegment2D` list cross |
| `GeomUtil.FindIntersections(segments)` | Bentley–Ottmann: every crossing point among a set of segments, sorted left-to-right |
| `GeomUtil.ConvexHull(points)` | Andrew's monotone chain: convex hull, returned in CCW order (3D overload auto-detects the normal) |
| `GeomUtil.DistDecimation(points, threshold)` | O(n) radial-distance point decimation |
| `GeomUtil.RdpDecimation(points, threshold)` | Ramer–Douglas–Peucker point decimation |
| `GeomUtil.VwDecimation(points, threshold)` | Visvalingam–Whyatt point decimation |
| `GeomUtil.BezierSmoothing2(p0, p1, p2, smoothness, minDistance\|numSegments[, minSegmentLength])` | Rounds one polyline corner with a quadratic Bezier arc |
| `GeomUtil.PolylineExpansion(points, settings)` | Rounds every inner corner of a point list and works with either fixed number of segmens or fixed min segment length (the engine behind `Polyline.Expand()`) |
| `GeomUtil.PrincipalAxes(points)` | PCA on a point cloud: returns a `CoordinateFrame` (`.X` primary, `.Y` secondary, `.Z` best-fit normal) |
| `GeomUtil.PrincipalNormal(points)` | Best-fit plane normal (PCA eigenvector with smallest eigenvalue) |
| `GeomUtil.PrincipalDirection(points)` | Dominant direction (PCA eigenvector with largest eigenvalue) |
| `GeomUtil.FindExtremePoints(polygon, line)` | The two polygon vertices least/greatest projected along a line's direction |
| `GeomUtil.DistanceTo(polygon, line)` | Distance from a polygon to a line (zero if they intersect) |
| `GeomUtil.TangentsTo(polygon, pointOrPolygon)` | Tangent segments from a point to a polygon, or common outer tangents between two polygons |


## Serialization

All primitives support:
- **WKT** (Well-Known Text) — `ToWkt()` / `FromWkt()` for standard text interchange
- **Binary file I/O** — `ToFile()` / `FromFile()` for compact storage

## Precision

Floating-point comparisons use a thread-local `DECIMAL_PRECISION` constant via `AlmostEquals()` methods, making the library robust against rounding errors while remaining configurable per thread.


---

## How to use it

You can look at the [test suite](./tests/) to see detailed usage. 

A quick list of code examples per topic is provided here.

👉 [Visual Documentation and Code Examples on Github](https://github.com/amastrobera/geompp/blob/master/visual_doc_and_sample_code.md)




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
