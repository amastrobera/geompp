[← back](../README.md)

# geompp

Python bindings for [geompp](https://github.com/amastrobera/geompp) — a C++ 2D/3D geometry library.

**[Changelog](https://github.com/amastrobera/geompp/blob/master/CHANGELOG.md)** — full release notes for every version.

## Install

```bash
pip install geompp
```

Pre-built wheels are available for:

| Platform | Python versions |
|---|---|
| Linux x86_64 | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 |
| Windows x64  | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 |

If your platform or Python version is not in the table above, pip will compile
from source — you will need CMake ≥ 3.15 and a C++20-capable compiler.


## How to use it

You can look at the [test suite](./tests/) to see detailed usage. There also is a whole [set of code examples](./code_examples.md) in the next page.


## Classes

| 2D | 3D |
|----|----|
| Point2D | Point3D |
| Vector2D | Vector3D |
| Line2D | Line3D |
| Ray2D | Ray3D |
| LineSegment2D | LineSegment3D |
| Polyline2D | Polyline3D |
| Triangle2D | Triangle3D |
| Polygon2D | Polygon3D |
| BBox2D | BBox3D |
| BBall2D | BBall3D |
| GeometryCollection2D | GeometryCollection3D |
| | Plane |

## Free functions

| Function | Description |
|---|---|
| `are_collinear(p1, p2, p3)` | Three points on the same line |
| `are_coplanar(points)` | List of `Point3D` on the same plane |
| `closest_world_plane_to(points)` | XY / YZ / ZX plane nearest to the point cloud |
| `are_ccw(points[, ref_plane])` | Counter-clockwise winding (2D or 3D) |
| `are_cw(points[, ref_plane])` | Clockwise winding (2D or 3D) |
| `remove_collinear(points)` | Drop collinear intermediate points |
| `remove_duplicates(points)` | Drop duplicate points |
| `average(points)` | Arithmetic mean |
| `linear_combination(points, weights)` | Weighted sum |
| `has_intersections(segments)` | Shamos–Hoey: `True` if any two segments in `list[LineSegment2D]` cross |
| `find_intersections(segments)` | Bentley–Ottmann: returns `list[Point2D]` — every crossing point, sorted left-to-right |
| `convex_hull(points)` | Andrew's monotone chain: convex hull of a `list[Point2D]`, returned in CCW order |
| `convex_hull(points, normal=None)` | Convex hull of a coplanar `list[Point3D]`; optional `Vector3D` normal (auto-detected if omitted) |
| `principal_axes(points)` | PCA on a `list[Point3D]`: returns `CoordinateFrame` (`.x` primary, `.y` secondary, `.z` best-fit normal) |
| `principal_normal(points)` | Best-fit plane normal of a `list[Point3D]` (PCA eigenvector with smallest eigenvalue) |
| `principal_direction(points)` | Dominant direction of a `list[Point3D]` (PCA eigenvector with largest eigenvalue) |
