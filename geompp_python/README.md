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


# Python Bindings

Python bindings for [geompp](https://github.com/amastrobera/geompp) — a C++ 2D/3D geometry library.

**[Changelog](https://github.com/amastrobera/geompp/blob/master/CHANGELOG.md)** — full release notes for every version.

## Install

```bash
pip install geompp
```

## Platform note

Pre-built wheels are available for:

| Platform | Python versions |
|---|---|
| Linux x86_64 | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |
| Windows x64  | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |
|||

If your platform or Python version is not in the table above, pip will compile
from source — you will need CMake ≥ 3.15 and a C++20-capable compiler.


## Test Coverage

This is the summary of the current test coverage. More on [test coverage](https://github.com/amastrobera/geompp/blob/master/test_coverage_report.md).

| Metric | Count | Notes |
|--------|-------|-------|
| Public methods (C++) | ~519 | Excl. ctors/dtors/operators. `geompp::maths`/`geompp::transformations` (templated/free-function, header-only) tracked separately, see test_coverage_report.md |
| C++ methods tested | ~499/519 | ~96% (1303 TEST cases, 1301 run, 2 disabled — incl. 37+17 for `geompp::maths` +6 for its own `detail::`, 30+6 for `geompp::transformations` (incl. `TransformBuilder2D/3D::Apply`), +40 direct `detail::`/`detail::view::` tests, +39 for the `polygonize()`/`merge()`/`Mesh2D/3D.Polygonize()`/`ConnectedMesh2D/3D.Polygonize()` family, +6 T-junction regression fix, +14 for the standalone `AdjacencyConformity`/`PolygonizationParams.conformity` feature) |
| Python methods tested | 478/498 | ~96% (949 pytest cases — incl. 27+6 for `geompp.maths`, 24+9+6 for `geompp.transformations` (incl. `TransformBuilder2D/3D.apply`), +4 for `distance_to(Point)` on `Polygon2D/3D`/`Triangle2D/3D`, +19 for polygonization, +6 T-junction regression fix, +15 for `AdjacencyConformity`/`PolygonizationParams.conformity`) |
| C# methods tested | 527/606 | ~87% (1049 harness tests — incl. 24+6 for `GeomPP.Maths`, 23+9+6 for `GeomPP.Transformations` (incl. `TransformBuilder2D/3D.Apply`), +6 for `DistanceTo(Point)` on `Polygon2D/3D`/`Triangle2D/3D`, +2 for `Triangle2D`-`Triangle2D` intersection parity, +19 for polygonization, +6 T-junction regression fix, +15 for `AdjacencyConformity`/`PolygonizationParams.Conformity`) |
| Stubs (not yet impl.) | 2 | `TriangulationParams::Strategy::MonotonePolygon`/`Delaunay` — intentional, see test_coverage_report.md |
||||


## How to use it

You can look at the [test suite](./tests/) to see detailed usage. 

A quick list of code examples per topic is provided here.

👉 [Visual Documentation and Code Examples on Github](https://github.com/amastrobera/geompp/blob/master/visual_doc_and_sample_code.md)


## What it provides

### Serialization

All primitives support:
- **WKT** (Well-Known Text) — `to_wkt()` / `from_wkt()` for standard text interchange
- **Binary file I/O** — `to_file()` / `from_file()` for compact storage

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
- **Intersection** — do two shapes strike through each other, and what is the resulting geometry? Also available as the free function `find_intersections()` on a free set of segments. The meaning of this operation changes from 2D to 3D — check the class docs.
- **Overlap** — do two shapes have a portion in common, and what is the resulting geometry? Meaning changes from 2D to 3D.
- **Touch** — do two shapes have a point in common, and which is it? Meaning changes from 2D to 3D.
- **Distance** — closest distance from a point to a shape.
- **Plane operations** — projection of a point from 3D to 2D, and re-projection from 2D to 3D, via the `Plane` class or the faster `View2D` (one of the 3 world planes XY/YZ/ZX, or a custom plane).
- **Interpolation / Location** — `lerp(p0, p1, t)` retrieves a point at parameter `t` between two points (not clamped); `interpolate(t)` does the same along a segment or polyline; the opposite operation finds the parameter `t` for a point already on a shape.
- **Area / Perimeter / Centroid** — geometric properties for closed shapes.
- **Signed area** — encodes orientation (clockwise vs. counter-clockwise in 2D, surface normal direction in 3D).
- **Simplicity / self-intersection** — `Polygon2D.is_simple()` and the free functions `has_intersections(segments)` (Shamos–Hoey, boolean) / `find_intersections(segments)` (Bentley–Ottmann, every crossing point).
- **Convex hull** — `convex_hull(points)` — Andrew's monotone chain, returns hull vertices in CCW order.
- **Bounding containers** — tight-fitting containers around point clouds: axis-aligned bounding box, bounding ball, minimal oriented rectangle, convex hull.
- **Polyline operations** — `Polyline.reduce()` (decimation) and `Polyline.expand()` (Bezier corner smoothing), or the underlying free functions (`dist_decimation()`/`rdp_decimation()`/`vw_decimation()`, `bezier_smoothing_2()`, `polyline_expansion()`) for a plain point list.
- **Polygon boolean operations** — `intersection()`, `union()`, `difference()`, `xor()` between two polygons (map-overlay method), or the free function `clip(clipper_loop, subject_loop)` for raw point loops without constructing a `Polygon` first.
- **Point cloud operations** — `principal_axes()` (PCA) finds the empirical 3 directive axes of a list of points in space.
- **Triangulation** — decomposition of a polygon into n-triangles, using several possible algorithms such as the _Ear Clip_, a _Best Fit Ear Clip_, _Monotone Polygon_ or _Constrained Delaunay_. 
- **Polygonization** — the reverse of triangulation: merges coplanar, edge-adjacent triangles back into polygons, via `Mesh2D/3D.polygonize()` / `ConnectedMesh2D/3D.polygonize()` or the free `polygonize(triangles, settings)`, under 3 strategies (`PlanarBoundaryExtraction` — O(n) external boundary of a triangle set, `PlanarQuads` — O(n) pairs of coplanar triangles into quads, `HertelMehlhorn` — merges coplanar triangles into convex n-gons). A related free function `merge(polygons)` welds a set of non-overlapping polygons that tile a plane (3D: grouped by plane first) into fewer, bigger polygons, including merging any of their holes that touch along the same seam.
- **Linear algebra** (`geompp.maths`) — a small fixed-size linear algebra submodule, independent of the geometry classes above: `Vector2`/`Vector3`/`Vector4`, `Matrix2`/`Matrix3`/`Matrix4`, and the `solve_gauss()` / `solve_cramer()` system solvers for `Ax = b`.
- **Affine transformations** (`geompp.transformations`) — `translate()`, `rotate()`, `scale()`, `shear()`, `reflect()` (fast, single-point, no matrix needed), and the general `transform(primitive, matrix)` for every primitive from `Point2D`/`Point3D` to `PolyMesh2D`/`PolyMesh3D`. Use `TransformBuilder2D`/`TransformBuilder3D` to fluently chain several transforms (e.g. `.translate(...).rotate(...).scale(...)`) into a single `Matrix3`/`Matrix4`, then apply it once with `.build()`/`transform()`.

Return values are `None` on no-intersection, and sometimes a `Point`/`list[LineSegment]`/`list[Polygon]` depending on
what the operation produced — check each method's docstring for the exact shape.

### Free functions

| Function | Description |
|---|---|
| `are_collinear(p1, p2, p3)` | Three points on the same line |
| `remove_consecutive_duplicates(points)` | Drop consecutive duplicate points |
| `remove_duplicates(points)` | Drop duplicate points |
| `remove_collinear(points)` | Drop collinear intermediate points |
| `linear_combination(points, weights)` | Weighted sum |
| `average(points)` | Arithmetic mean |
| `lerp(p0, p1, t)` | Linear interpolation between two points — `P0 + t*(P1-P0)`, not clamped |
| `centroid(points[, plane])` | Centroid of a polygon (3D: `plane` auto-detected if omitted) |
| `signed_area(points[, plane])` | Signed area of a polygon; positive = CCW, negative = CW |
| `are_ccw(points[, ref_plane])` | Counter-clockwise winding (2D or 3D) |
| `are_cw(points[, ref_plane])` | Clockwise winding (2D or 3D) |
| `are_coplanar(points)` | List of `Point3D` on the same plane |
| `closest_world_plane_to(points)` | XY / YZ / ZX plane nearest to the point cloud |
| `has_intersections(segments)` | Shamos–Hoey: `True` if any two segments in `list[LineSegment2D]` cross |
| `find_intersections(segments)` | Bentley–Ottmann: returns `list[Point2D]` — every crossing point, sorted left-to-right |
| `convex_hull(points)` | Andrew's monotone chain: convex hull of a `list[Point2D]`, returned in CCW order |
| `convex_hull(points, normal=None)` | Convex hull of a coplanar `list[Point3D]`; optional `Vector3D` normal (auto-detected if omitted) |
| `clip(clipper_loop, subject_loop)` | Set intersection of two point loops — `list[Point2D]` natively, `list[Point3D]` if coplanar (same map-overlay engine as `Polygon.intersection()`) |
| `dist_decimation(points, threshold)` | O(n) radial-distance point decimation |
| `rdp_decimation(points, threshold)` | Ramer–Douglas–Peucker point decimation |
| `vw_decimation(points, threshold)` | Visvalingam–Whyatt point decimation |
| `bezier_smoothing_2(p0, p1, p2, smoothness, min_distance\|num_segments, min_segment_length=...)` | Rounds one polyline corner with a quadratic Bezier arc |
| `polyline_expansion(points, settings)` | Rounds every inner corner of a point list and works with either fixed number of segmens or fixed min segment length (the engine behind `Polyline.expand()`) |
| `principal_axes(points)` | PCA on a `list[Point3D]`: returns `CoordinateFrame` (`.x` primary, `.y` secondary, `.z` best-fit normal) |
| `principal_normal(points)` | Best-fit plane normal of a `list[Point3D]` (PCA eigenvector with smallest eigenvalue) |
| `principal_direction(points)` | Dominant direction of a `list[Point3D]` (PCA eigenvector with largest eigenvalue) |
| `find_extreme_points(polygon, line)` | The two polygon vertices least/greatest projected along a line's direction |
| `distance_to(polygon, line)` | Distance from a polygon to a line (zero if they intersect) |
| `tangents_to(polygon, point_or_polygon)` | `PolygonTangents2D`/`PolygonTangents3D` (`.left`/`.right`) — tangent segments to a point, or common outer tangents to another polygon |
| `triangulate(polygons, settings)` | Returns a set of adjacent triangles replacing the surface of 1+ polygons (the engine behind `Polygon::Triangulate()` and `PolyMesh::Triangulate()`), and with a robust input validation |
| `maths.solve_gauss(a, b)` | Solve `Ax = b` via Gaussian elimination |
| `maths.solve_cramer(a, b)` | Solve `Ax = b` via Cramer's rule; raises `ValueError` if `a` is singular |
| `transformations.translate(primitive, offset)` | Translate a primitive by a vector |
| `transformations.rotate(primitive, angle_rad[, axis])` | Rotate about the origin — 3D takes an `axis` |
| `transformations.scale(primitive, factor \| sx, sy[, sz])` | Uniform or non-uniform scale about the origin |
| `transformations.shear(primitive, ...)` | Shear along one axis by a multiple of another |
| `transformations.reflect(primitive, normal)` | Reflect about a line/plane through the origin with the given normal |
| `transformations.transform(primitive, matrix)` | Apply an arbitrary `Matrix3`/`Matrix4` (homogeneous) to any primitive from `Point` to `PolyMesh` |
|||




