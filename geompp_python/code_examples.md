[← back](./README.md)

## Examples

### Creation and I/O operations

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


### Containment

`triangle.contains(p)` and `polygon.contains(p)` test whether a point lies inside a shape using
barycentric coordinates and the winding number, respectively.
`line_segment.contains(p)` checks whether a point lies on the segment;
`location(p)` returns the parameter `t ∈ [0, 1]` for a point already on it, and `interpolate(t)` reverses the mapping.

```python
import geompp as g

# Triangle3D — barycentric containment
tri = g.Triangle3D.make(
    g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(0, 4, 0))
print(tri.contains(g.Point3D(1, 1, 0)))   # True  — inside
print(tri.contains(g.Point3D(3, 3, 0)))   # False — outside

# Polygon2D — winding-number containment
poly = g.Polygon2D.make([
    g.Point2D(0, 0), g.Point2D(6, 0),
    g.Point2D(6, 4), g.Point2D(0, 4),
])
print(poly.contains(g.Point2D(3, 2)))     # True
print(poly.contains(g.Point2D(7, 2)))     # False

# LineSegment2D — containment, location, and interpolation
seg = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(4, 0))
print(seg.contains(g.Point2D(2, 0)))      # True
print(seg.contains(g.Point2D(2, 1)))      # False

t   = seg.location(g.Point2D(2, 0))      # 0.5
mid = seg.interpolate(0.5)               # Point2D(2, 0)
print(f"t = {t:.1f}")
print(f"mid = {mid.to_wkt()}")
```

Output:
```
True
False
True
False
True
False
t = 0.5
mid = POINT (2 0)
```


### Intersections

Intersection methods return the resulting geometry or `None`. `find_intersections(segments)`
(Bentley–Ottmann) reports all crossing points across an arbitrary set of 2D segments, sorted left-to-right.

```python
import geompp as g

# Ray3D vs Triangle3D
tri = g.Triangle3D.make(
    g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(0, 4, 0))
ray = g.Ray3D.make(g.Point3D(1, 1, 3), g.Vector3D(0, 0, -1))
hit = tri.intersection(ray)
print(hit.to_wkt() if hit else None)      # POINT (1 1 0)

# LineSegment2D vs LineSegment2D
s1    = g.LineSegment2D.make(g.Point2D(0, 1), g.Point2D(4, 1))
s2    = g.LineSegment2D.make(g.Point2D(2, 0), g.Point2D(2, 4))
cross = s1.intersection(s2)
print(cross.to_wkt() if cross else None)  # POINT (2 1)

# find_intersections — all crossing points (Bentley–Ottmann)
segs = [
    g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(4, 4)),
    g.LineSegment2D.make(g.Point2D(0, 4), g.Point2D(4, 0)),
    g.LineSegment2D.make(g.Point2D(0, 2), g.Point2D(4, 2)),
]
for p in g.find_intersections(segs):
    print(p.to_wkt())
```

Output:
```
POINT (1 1 0)
POINT (2 1)
POINT (1 2)
POINT (2 2)
POINT (3 2)
```


### Planar operations

#### Coplanarity, orientation, and closest world plane

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


#### Projecting points onto a plane

`plane.project_onto(p)` returns the perpendicular projection in 3D world coordinates.
`plane.project_into(p)` maps the same projected point into the plane's local 2D frame.
`plane.evaluate(p2d)` is the inverse — local 2D coordinates back to world 3D.

```python
import geompp as g

# XY plane: normal (0, 0, 1), origin (0, 0, 0)
pl = g.Plane.xy()
p  = g.Point3D(3.0, 4.0, 7.0)

on   = pl.project_onto(p)    # Point3D(3, 4, 0) — projection in world 3D
into = pl.project_into(p)    # Point2D(3, 4)    — in the plane's local frame
back = pl.evaluate(into)     # Point3D(3, 4, 0) — local 2D back to world 3D

print(f"on_plane: {on.to_wkt()}")
print(f"in_plane: {into.to_wkt()}")
print(f"back:     {back.to_wkt()}")
```

Output:
```
on_plane: POINT (3 4 0)
in_plane: POINT (3 4)
back:     POINT (3 4 0)
```


#### Planar vs non-planar Polyline3D

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


### Bounding containers

#### Simple containers for quick rejection 

`BBox3D` gives the tight axis-aligned box; `BBall3D` (Ritter 1990) gives an approximate
minimum enclosing sphere — both accept any cloud of points.

```python
import geompp as g

g.set_decimal_precision(g.DP_THREE)

# Vertices of a rough L-shaped structure
verts = [
    g.Point3D(0, 0, 0), g.Point3D(6, 0, 0), g.Point3D(6, 2, 0),
    g.Point3D(2, 2, 0), g.Point3D(2, 4, 0), g.Point3D(0, 4, 0),
    g.Point3D(0, 0, 3), g.Point3D(6, 0, 3), g.Point3D(6, 2, 3),
    g.Point3D(2, 2, 3), g.Point3D(2, 4, 3), g.Point3D(0, 4, 3),
]

# Axis-aligned bounding box
box = g.BBox3D(g.Polyline3D.make(verts))
print(box.min.to_wkt())                      # POINT (0 0 0)
print(box.max.to_wkt())                      # POINT (6 4 3)
print(box.contains(g.Point3D(3, 1, 1)))      # True
print(box.contains(g.Point3D(7, 1, 1)))      # False

# Bounding ball — constructor takes the point cloud directly
ball = g.BBall3D(verts)
print(ball.center.to_wkt())
print(f"radius: {ball.radius:.3f}")
print(ball.contains(g.Point3D(3, 1, 1)))     # True

# All original vertices must be inside the ball
assert all(ball.contains(p) for p in verts)
```

Output:
```
POINT (0 0 0)
POINT (6 4 3)
True
False
POINT (3 ...)
radius: ...
True
```

#### Convex hulls

##### Convex hull of a point cloud

`convex_hull(points)` (Andrew's monotone chain) wraps any point cloud into its tightest convex polygon:

```python
import geompp as g

g.set_decimal_precision(g.DP_THREE)

star = [
    g.Point2D( 0,  5), g.Point2D( 4,  2),
    g.Point2D( 3, -3), g.Point2D(-2, -4), g.Point2D(-3,  1),
    g.Point2D( 2,  1), g.Point2D( 2, -1),
    g.Point2D( 0, -1), g.Point2D(-1, -1), g.Point2D(-1,  2),
]

hull = g.convex_hull(star)
print(f"hull has {len(hull)} vertices:")
for p in hull:
    print(" ", p.to_wkt())
```

```
hull has 5 vertices:
  POINT (3 -3)
  POINT (4 2)
  POINT (0 5)
  POINT (-3 1)
  POINT (-2 -4)
```

(CCW order, starting from the lexicographically smallest point)

##### Convex hull of a polygon

`Polygon2D` and `Polygon3D` expose a `convex_hull()` method:

```python
import geompp as g

star = g.Polygon3D.make([
    g.Point3D( 0,  5, 0), g.Point3D( 2,  1, 0),
    g.Point3D( 4,  2, 0), g.Point3D( 2, -1, 0),
    g.Point3D( 3, -3, 0), g.Point3D( 0, -1, 0),
    g.Point3D(-2, -4, 0), g.Point3D(-1, -1, 0),
    g.Point3D(-3,  1, 0), g.Point3D(-1,  2, 0),
])

print(f"star is convex: {star.is_convex()}")  # False
hull = star.convex_hull()   # Polygon3D with 5 vertices
print(f"hull is convex: {hull.is_convex()}")  # True
print(f"hull has {hull.size()} vertices")      # hull has 5 vertices
```

##### Convex hull of a simple polyline

`Polyline2D.convex_hull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `is_simple()` first.

```python
import geompp as g

path = g.Polyline2D.make([
    g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4),
    g.Point2D(2, 1), g.Point2D(0, 4),
])

if path.is_simple():
    hull = path.convex_hull()   # Polygon2D with 4 vertices
    print(f"hull has {hull.size()} vertices")  # hull has 4 vertices
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
