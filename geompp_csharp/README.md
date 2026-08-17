# GeomPP

A modern C++20 geometry library for 2D and 3D spatial computation — fast, mathematically correct,
thoroughly tested, and usable from C++, C# (.Net 8/9/10 or .Net Framework 4.8), and Python 3.

You may be a CAD or a Game developer using C#.Net, and you use APIs native to the platform you develop into. These native APIs are easy to get in, but may contain bugs that have not been fixed, or simply lack some functionalities. 
You may be a Data Scientist using Python on a GIS project, and having to import 3+ libraries, and covert from data-structure to data-structure to use it. 
You may be a C++ developer who wants to import a more lightweight library than those which already exist, and possibly more user friendly. 

This library was born a few years ago to solve all these problems. It was recently augmented with the aim of using the most modern algorithms to solve a variety of geometrical problems. 

The sources of these algorithms are to be found in several textbooks, such as 
- Practical Geometry Algorithms (Danniel Sunday)
- Computational Geometry in C (Joseph O'Rourke)
- Computational Geometry (Mark de Berg, Marc van Kreveld, Mark Overmars, Otfried Schwarzkopf)

Finally, the help of AI was used to validate algorithms (bug-free, guarantee the desired big-O), bind into other languages than C++, add edge cases to achieve a high test coverage, and build documentation. 


## C# Bindings

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
||||


## Platform note

GeomPP is built with C++/CLI and is **Windows x64 only**. It will not run on Linux, macOS, or
32-bit processes.

---


## Test Coverage

This is the summary of the current test coverage. More on [test coverage](https://github.com/amastrobera/geompp/blob/master/test_coverage_report.md).

| Metric | Count | Notes |
|--------|-------|-------|
| Public methods (C++) | ~519 | Excl. ctors/dtors/operators. `geompp::maths`/`geompp::transformations` (templated/free-function, header-only) tracked separately, see test_coverage_report.md |
| C++ methods tested | ~499/519 | ~96% (1260 TEST cases, 1258 run, 2 disabled — incl. 37+17 for `geompp::maths` +6 for its own `detail::`, 30 for `geompp::transformations`, +40 direct `detail::`/`detail::view::` tests, +39 for the `polygonize()`/`merge()`/`Mesh2D/3D.Polygonize()`/`ConnectedMesh2D/3D.Polygonize()` family, +6 T-junction regression fix) |
| Python methods tested | 473/481 | ~98% (928 pytest cases — incl. 27+6 for `geompp.maths`, 24+9 for `geompp.transformations`, +4 for `distance_to(Point)` on `Polygon2D/3D`/`Triangle2D/3D`, +19 for polygonization, +6 T-junction regression fix) |
| C# methods tested | 522/589 | ~89% (1028 harness tests — incl. 24+6 for `GeomPP.Maths`, 23+9 for `GeomPP.Transformations`, +6 for `DistanceTo(Point)` on `Polygon2D/3D`/`Triangle2D/3D`, +2 for `Triangle2D`-`Triangle2D` intersection parity, +19 for polygonization, +6 T-junction regression fix) |
| Stubs (not yet impl.) | 2 | `TriangulationParams::Strategy::MonotonePolygon`/`Delaunay` — intentional, see test_coverage_report.md |
||||


---

## How to use it

You can look at the [test suite](./tests/) to see detailed usage. 

A quick list of code examples per topic is provided here.

👉 [Visual Documentation and Code Examples on Github](https://github.com/amastrobera/geompp/blob/master/visual_doc_and_sample_code.md)


---

## What it provides

### Serialization

All primitives support:
- **WKT** (Well-Known Text) — `ToWkt()` / `FromWkt()` for standard text interchange
- **Binary file I/O** — `ToFile()` / `FromFile()` for compact storage

### Precision

Floating-point comparisons use a thread-local `DECIMAL_PRECISION` constant via `AlmostEquals()` methods, making the library robust against rounding errors while remaining configurable per thread.


### Classes

Where not explicitely specified, both 2D and 3D variants are available for all core types:

| Primitive        | Description                                              |
|------------------|----------------------------------------------------------|
| `Point`          | A coordinate in space                                    |
| `Vector`         | Direction and magnitude                                  |
| `Line`           | An infinite line through two points                      |
| `Ray`            | A semi-infinite line from an origin in one direction     |
| `LineSegment`    | A finite segment between two endpoints                   |
| `Polyline`       | A connected chain of segments                            |
| `Triangle`       | Three non-collinear points forming a closed face         |
| `Polygon`        | A closed polygon defined by an ordered list of vertices  |
| `BBox`           | Axis-aligned bounding box                                |
| `BBall`          | Minimum bounding sphere (Ritter's algorithm)             |
| `BRect2D`        | Minimum oriented bounding rectangle (rotating calipers) |
| `BPrism3D`       | Minimum oriented bounding prism (PCA + rotating calipers) |
| `Plane`          | A flat surface in 3D defined by a point and a normal     |
| `View2D`         | A class that converts a 3D point into 2D quicker than plane|
| `Mesh`           | A set of adjacent triangles that together make up a detailed 2D or 3D shape (**a surface or a solid**)|
| `ConnectedMesh`  | This one keeps track of the neighbors of each triangle, so that going from a facet to its 0-3 neighbors is very quick|
| `PolyMesh`       | Not just triangles, also polygons are allowed, in order to save on the number of vertices on the same planar regions of the surface|
|||


### Algorithm overview

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
- **Triangulation** — decomposition of a polygon into n-triangles, using several possible algorithms such as the _Ear Clip_, a _Best Fit Ear Clip_, _Monotone Polygon_ or _Constrained Delaunay_.
- **Polygonization** — the reverse of triangulation: merges coplanar, edge-adjacent triangles back into polygons, via `Mesh2D/3D.Polygonize()` / `ConnectedMesh2D/3D.Polygonize()` or `GeomUtil.Polygonize(triangles, settings)`, under 3 strategies (`PlanarBoundaryExtraction` — O(n) external boundary of a triangle set, `PlanarQuads` — O(n) pairs of coplanar triangles into quads, `HertelMehlhorn` — merges coplanar triangles into convex n-gons). A related `GeomUtil.Merge(polygons)` welds a set of non-overlapping polygons that tile a plane (3D: grouped by plane first) into fewer, bigger polygons, including merging any of their holes that touch along the same seam. 
- **Linear algebra** (`GeomPP.Maths`) — a small fixed-size linear algebra namespace, independent of the geometry classes above: `Vector2`/`Vector3`/`Vector4`, `Matrix2`/`Matrix3`/`Matrix4`, and the `Solvers.SolveGauss()` / `Solvers.SolveCramer()` system solvers for `Ax = b`.
- **Affine transformations** (`GeomPP.Transformations`) — `Transform.Translate()`, `.Rotate()`, `.Scale()`, `.Shear()`, `.Reflect()` (fast, single-`Point`, no matrix needed), and the general `Transform.Transform(primitive, matrix)` for every primitive from `Point2D`/`Point3D` to `PolyMesh2D`/`PolyMesh3D`. Use `TransformBuilder2D`/`TransformBuilder3D` to fluently chain several transforms (e.g. `.Translate(...).Rotate(...).Scale(...)`) into a single `Matrix3`/`Matrix4`, then apply it once with `.Build()`/`Transform.Transform()`.

Intersection-style methods return `object` (`null` on no intersection) — see the pattern-matching example above.

### Free functions

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
| `GeomUtil.Triangulate(polygons, settings)` | Returns a set of adjacent triangles replacing the surface of 1+ polygons (the engine behind `Polygon::Triangulate()` and `PolyMesh::Triangulate()`), and with a robust input validation |
| `GeomUtil.Polygonize(triangles, settings)` | Merges coplanar, edge-adjacent triangles into polygons (the engine behind `Mesh.Polygonize()` and `ConnectedMesh.Polygonize()`), under 3 strategies |
| `GeomUtil.Merge(polygons)` | Welds a set of non-overlapping, plane-tiling polygons into fewer, bigger polygons, merging touching holes along the way |
| `Maths.Solvers.SolveGauss(a, b)` | Solve `Ax = b` via Gaussian elimination |
| `Maths.Solvers.SolveCramer(a, b)` | Solve `Ax = b` via Cramer's rule; throws if `a` is singular |
| `Transformations.Transform.Translate(primitive, offset)` | Translate a primitive by a vector |
| `Transformations.Transform.Rotate(primitive, angleRad[, axis])` | Rotate about the origin — 3D takes an `axis` |
| `Transformations.Transform.Scale(primitive, factor \| sx, sy[, sz])` | Uniform or non-uniform scale about the origin |
| `Transformations.Transform.Shear(primitive, ...)` | Shear along one axis by a multiple of another |
| `Transformations.Transform.Reflect(primitive, normal)` | Reflect about a line/plane through the origin with the given normal |
| `Transformations.Transform.Transform(primitive, matrix)` | Apply an arbitrary `Matrix3`/`Matrix4` (homogeneous) to any primitive from `Point` to `PolyMesh` |
|||






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
msbuild geompp_csharp\GeomPP_Net10.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# .NET Framework 4.8
msbuild geompp_csharp\GeomPP_Net48.vcxproj /p:Platform=x64 /p:GeomppBuildRoot="$PWD\build_win" [/p:Configuration=Release]

# run smoke tests, after build from the main directory geompp
# (GeomPPTests.csproj is an Exe-type console harness, not a Test SDK project —
# `dotnet test` reports success but runs nothing; use `dotnet run`)
dotnet run --project geompp_csharp\tests\GeomPPTests.csproj [-p:GeomPPConfiguration=Release]
```
