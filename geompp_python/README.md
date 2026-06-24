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

## Quick start

```python
import geompp as g

# Points & vectors
p = g.Point2D(1.0, 2.0)
v = g.Vector2D(3.0, 0.0)
q = p + v                       # Point2D(4, 2)
diff = q - p                    # Vector2D(3, 0)

# Lines and intersection
l1 = g.Line2D.make(g.Point2D(0,0), g.Point2D(1,0))
l2 = g.Line2D.make(g.Point2D(0.5,-1), g.Point2D(0.5,1))
hit = l1.intersection(l2)       # Point2D(0.5, 0) or None

# 3D
p3 = g.Point3D(1, 2, 3)
plane = g.Plane.xy()
proj = plane.project_onto(p3)   # Point3D(1, 2, 0)

# Plane intersections (Line / Ray / Segment / Plane / Triangle)
ray = g.Ray3D.make(g.Point3D(5, 3, 4), g.Vector3D(0, 0, -1))
hit = plane.intersection(ray)              # Point3D(5, 3, 0)
axis_y = plane.intersection(g.Plane.yz())  # Line3D along the Y-axis

# Parallel / coplanar tests
plane.is_parallel(ray)                     # False (ray crosses the plane)
plane.is_coplanar(g.Line3D.make(g.Point3D(0,0,0), g.Vector3D(1, 1, 0)))  # True

# Implicit Vector → Point construction
p_from_v = g.Point3D(g.Vector3D(1, 2, 3))  # = Point3D(1, 2, 3)

# Triangle3D intersection with Line / Ray / Segment / Plane / Triangle
tri = g.Triangle3D.make(g.Point3D(0,0,0), g.Point3D(4,0,0), g.Point3D(0,4,0))
hit_line  = tri.intersection(
    g.Line3D.make(g.Point3D(1, 1, -1), g.Point3D(1, 1, 1)))                # Point3D(1, 1, 0)
hit_ray   = tri.intersection(
    g.Ray3D.make(g.Point3D(1, 1, 4), g.Vector3D(0, 0, -1)))                # Point3D(1, 1, 0)
hit_seg   = tri.intersection(
    g.LineSegment3D.make(g.Point3D(1, 1, -2), g.Point3D(1, 1, 3)))         # Point3D(1, 1, 0)
y1        = g.Plane.from_origin_and_normal(g.Point3D(0,1,0), g.Vector3D(0,1,0))
hit_plane = tri.intersection(y1)                                            # LineSegment3D (0,1,0)→(3,1,0)
other     = g.Triangle3D.make(g.Point3D(1,1,-1), g.Point3D(1,1,1), g.Point3D(3,1,0))
hit_tri   = tri.intersection(other)                                         # LineSegment3D (1,1,0)→(3,1,0)

# Precision
g.set_decimal_precision(g.DP_SIX)

# File parser
parser = g.WktParser.open("geometry.lsv")
while parser.has_next():
    item = parser.next()
    if item is not None:
        print(g.WktParser.to_wkt(item))
```

## Checking coplanarity, orientation, and closest world plane

```python
import geompp as g

pts_flat = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(0,1,0), g.Point3D(1,1,0)]
pts_3d   = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(0,1,0), g.Point3D(0,0,1)]

print(g.are_coplanar(pts_flat))  # True  — all on the XY plane
print(g.are_coplanar(pts_3d))    # False — spans 3D space

# Find which world axis plane is closest to the point cloud
plane = g.closest_world_plane_to(pts_flat)
print(plane.normal)              # Vector3D(0, 0, 1)  → XY plane

# Check / require CCW winding
ring = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(1,1,0), g.Point3D(0,1,0)]
print(g.are_ccw(ring))           # True
print(g.are_cw(ring))            # False

# Polygon3D requires CCW outer ring and CW holes
outer = [g.Point3D(0,0,0), g.Point3D(4,0,0), g.Point3D(4,4,0), g.Point3D(0,4,0)]
hole  = [g.Point3D(1,3,0), g.Point3D(3,3,0), g.Point3D(3,1,0), g.Point3D(1,1,0)]
poly  = g.Polygon3D.make(outer, [hole])
print(poly.size())               # 4
```

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

## Convex hull

```python
import geompp as g

g.set_decimal_precision(g.DP_THREE)

# An asymmetric 5-pointed star: 5 outer tips + 5 inner concave vertices.
# The convex hull should be exactly the 5 outer tips.
star = [
    # outer tips
    g.Point2D( 0,  5), g.Point2D( 4,  2),
    g.Point2D( 3, -3), g.Point2D(-2, -4), g.Point2D(-3,  1),
    # inner concave vertices (will be excluded from the hull)
    g.Point2D( 2,  1), g.Point2D( 2, -1),
    g.Point2D( 0, -1), g.Point2D(-1, -1), g.Point2D(-1,  2),
]

hull = g.convex_hull(star)
print(f"hull has {len(hull)} vertices:")
for p in hull:
    print(" ", p.to_wkt())
# hull has 5 vertices:
#   POINT (3 -3)
#   POINT (4 2)
#   POINT (0 5)
#   POINT (-3 1)
#   POINT (-2 -4)
```

### Convex hull of a polygon

`Polygon2D` and `Polygon3D` expose a `convex_hull()` method:

```python
import geompp as g

# 3D star polygon (10 vertices, coplanar, CCW)
star = g.Polygon3D.make([
    g.Point3D( 0,  5, 0), g.Point3D( 2,  1, 0),
    g.Point3D( 4,  2, 0), g.Point3D( 2, -1, 0),
    g.Point3D( 3, -3, 0), g.Point3D( 0, -1, 0),
    g.Point3D(-2, -4, 0), g.Point3D(-1, -1, 0),
    g.Point3D(-3,  1, 0), g.Point3D(-1,  2, 0),
])

hull = star.convex_hull()   # Polygon3D with 5 vertices
print(f"hull has {hull.size()} vertices")
# hull has 5 vertices
```

### Convex hull of a simple polyline

`Polyline2D.convex_hull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `is_simple()` first.

```python
import geompp as g

# Simple concave path: outer corners with an inner dip at (2,1)
path = g.Polyline2D.make([
    g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4),
    g.Point2D(2, 1), g.Point2D(0, 4),
])

if path.is_simple():
    hull = path.convex_hull()   # Polygon2D with 4 vertices
    print(f"hull has {hull.size()} vertices")
# hull has 4 vertices
```

### Planar vs non-planar Polyline3D

`Polyline3D.is_planar()` checks whether all knots lie in a common plane. Only planar polylines support
`is_simple()`, `is_convex()`, `convex_hull()`, and `to_polygon()`.

`convex_hull()` returns a `Polyline3D` (open path). Call `to_polygon()` on it to close the boundary into a `Polygon3D` with area.

```python
import geompp as g

# Planar star-like path in the XY plane
planar = g.Polyline3D.make([
    g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(2, 2, 0),
    g.Point3D(4, 4, 0), g.Point3D(0, 4, 0),
])

print(f"planar: {planar.is_planar()}")     # True

hull    = planar.convex_hull()             # Polyline3D — open hull
polygon = hull.to_polygon()               # Polygon3D  — closed region with area

print(f"hull knots:   {hull.size()}")
print(f"polygon area: {polygon.area():.3f}")

# Non-planar path: each point rises out of the XY plane
rising = g.Polyline3D.make([
    g.Point3D(0, 0, 0), g.Point3D(1, 0, 0),
    g.Point3D(1, 1, 1), g.Point3D(0, 1, 2),
])

print(f"planar: {rising.is_planar()}")     # False

pts = [rising[i] for i in range(rising.size())]
direction = g.principal_direction(pts)
print(f"dominant direction: {direction}")
```

Output:
```
planar: True
hull knots:   4
polygon area: 16.000
planar: False
dominant direction: VECTOR (...)
```

### PCA on a 3D point cloud

`principal_axes(points)` runs PCA (Jacobi eigendecomposition on the 3×3 covariance matrix) and returns
a `CoordinateFrame` — three orthonormal axes sorted by variance: `x` is the direction of most spread,
`y` the secondary, and `z` the best-fit plane normal (least variance).

```python
import geompp as g

# 8 points flat in the XY plane, elongated along X
cloud = [
    g.Point3D(0, 0,   0), g.Point3D(1, 0,   0),
    g.Point3D(2, 0,   0), g.Point3D(3, 0,   0),
    g.Point3D(0, 0.1, 0), g.Point3D(1, 0.1, 0),
    g.Point3D(2, 0.1, 0), g.Point3D(3, 0.1, 0),
]

frame = g.principal_axes(cloud)

print(f"x (primary):   {frame.x}")   # ≈ (1, 0, 0)
print(f"y (secondary): {frame.y}")   # ≈ (0, 1, 0)
print(f"z (normal):    {frame.z}")   # ≈ (0, 0, 1)

# Convenience wrappers
normal    = g.principal_normal(cloud)     # == frame.z
direction = g.principal_direction(cloud)  # == frame.x
```

Output:
```
x (primary):   VECTOR (1 0 0)
y (secondary): VECTOR (0 1 0)
z (normal):    VECTOR (0 0 1)
```


