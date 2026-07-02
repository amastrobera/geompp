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
| Linux x86_64 | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |
| Windows x64  | 3.8 · 3.9 · 3.10 · 3.11 · 3.12 · 3.13 · 3.14 |

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
| BRect2D | |
| | BPrism3D |
| GeometryCollection2D | GeometryCollection3D |
| | Plane |
| View2D | |

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

## Planar operations

`View2D` projects 3D points into 2D coordinates via `.x(point)` / `.y(point)`. It is particularly
useful for streaming large containers of `Point3D` without allocating an intermediate list of
`Point2D` — each call reads one or two scalar coordinates directly.

```python
from geompp import View2D, ProjectionType, Plane, Point3D, Vector3D

# axis-aligned views (fastest path)
v_xy = View2D.xy()   # x→x, y→y (drops z)
v_yz = View2D.yz()   # y→x, z→y (drops x)
v_zx = View2D.zx()   # z→x, x→y (drops y)

# custom view onto any plane
plane = Plane.from_origin_and_normal(Point3D(0, 0, 5), Vector3D(0, 0, 1))
v_custom = View2D.on_plane(plane)

pts3d = [Point3D(1, 2, 5), Point3D(3, 4, 5), Point3D(5, 6, 5)]

# stream 3D points to 2D without building a Point2D list
xs = [v_xy.x(p) for p in pts3d]  # [1.0, 3.0, 5.0]
ys = [v_xy.y(p) for p in pts3d]  # [2.0, 4.0, 6.0]

print(v_xy.type)  # ProjectionType.XY
```

## Bounding containers

`BRect2D` — minimum oriented bounding rectangle (rotating calipers; requires ≥ 3 non-collinear points):

```python
import geompp

pts = [geompp.Point2D(0, 0), geompp.Point2D(4, 0), geompp.Point2D(4, 3),
       geompp.Point2D(2, 4), geompp.Point2D(0, 3)]
rect = geompp.BRect2D(pts)
print(rect.center)                       # Point2D(2.0, 1.75)
print(rect.axis_u, rect.axis_v)          # unit vectors along the OBB edges
print(rect.width, rect.height)
print(rect.area)
corners = rect.corners()                 # list of 4 Point2D
print(rect.contains(geompp.Point2D(2, 1)))  # True
```

`BPrism3D` — minimum oriented bounding prism (PCA + rotating calipers; requires ≥ 3 non-collinear points):

```python
import geompp

pts = [
    geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
    geompp.Point3D(4, 3, 0), geompp.Point3D(0, 3, 0),
    geompp.Point3D(0, 0, 2), geompp.Point3D(4, 0, 2),
    geompp.Point3D(4, 3, 2), geompp.Point3D(0, 3, 2),
]
prism = geompp.BPrism3D(pts)
print(prism.center)                      # roughly Point3D(2, 1.5, 1)
print(prism.axis_u, prism.axis_v, prism.axis_w)  # orthonormal frame
print(prism.width, prism.height, prism.depth)     # 4.0, 3.0, 2.0
print(prism.volume)                      # ~24.0
corners = prism.corners()                # list of 8 Point3D
print(prism.contains(geompp.Point3D(2, 1.5, 1)))  # True
print(prism.almost_equals(geompp.BPrism3D(pts)))   # True
```
