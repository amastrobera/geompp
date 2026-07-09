# GeomPP — Python Bindings

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
| `tangents_to(polygon, point_or_polygon)` | `PolygonTangents2D`/`PolygonTangents3D` (`.left`/`.right`) — tangent segments to a point, or common outer tangents to another polygon |


## How to use it

You can look at the [test suite](./tests/) to see detailed usage. 

A quick list of code examples per topic is provided here.

<details open><summary><b>Code Examples</b></summary>

<details open>
<summary><b> &nbsp; 1. Creation and I/O operations</b></summary>

<details open>
<summary><b> &nbsp; &nbsp; 1.1 Create geometries from classes</b></summary>

  ```python
  import geompp as g

  # This will be the precision used by all functions for this run of the program.
  g.set_decimal_precision(g.DP_THREE)

  # two line segments intersecting at (0,0,1)
  s1 = g.LineSegment3D.make(g.Point3D(1, 0, 0), g.Point3D(-1, 0, 2))
  s2 = g.LineSegment3D.make(g.Point3D(0, 1, 0), g.Point3D(0, -1, 2))

  print(f"s1 = {s1.to_wkt()}")
  print(f"s2 = {s2.to_wkt()}")

  if s1.intersects(s2):
      result = s1.intersection(s2)
      if result is not None:
          print(f"intersection found: {result.to_wkt()}")
          result.to_file("intersection.wkt")
          print("intersection written to intersection.wkt")
  else:
      print("no intersection found")
  ```

  will print out

  ```
  s1 = LINESTRING (1 0 0, -1 0 2)
  s2 = LINESTRING (0 1 0, 0 -1 2)
  intersection found: POINT (0 0 1)
  intersection written to intersection.wkt
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 1.2 Create geometries from text</b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # two line segments intersecting at (0,0,1)
  s1 = g.LineSegment3D.from_wkt("LINESTRING (1  0  0, -1 0 2)")
  s2 = g.LineSegment3D.from_wkt("LINESTRING (0 1  0, 0 -1 2)")

  print(f"s1 = {s1.to_wkt()}")
  print(f"s2 = {s2.to_wkt()}")

  if s1.intersects(s2):
      result = s1.intersection(s2)
      if result is not None:
          print(f"intersection found: {result.to_wkt()}")
          result.to_file("intersection.wkt")
          print("intersection written to intersection.wkt")
  else:
      print("no intersection found")
  ```

  will print out

  ```
  s1 = LINESTRING (1 0 0, -1 0 2)
  s2 = LINESTRING (0 1 0, 0 -1 2)
  intersection found: POINT (0 0 1)
  intersection written to intersection.wkt
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 1.3 Import geometries from a file</b></summary>

  ```python
  import geompp as g

  lsv_path = "sample_geometries.lsv"
  #   POINT (1 2 3)
  #   POINT (4 5 6)
  #   LINESTRING (0 0 0, 1 1 1)
  #   LINESTRING (2 0 0, 2 3 4)
  #   LINE (0 0 0, 1 0 0)
  #   RAY (0 0 0, 0 1 0)

  g.set_decimal_precision(g.DP_THREE)

  parser = g.WktParser.open(lsv_path)
  if not parser.has_next():
      print(f"no geometries found in file {lsv_path}")
  else:
      while parser.has_next():
          entry = parser.next()
          if entry is None:
              print("skipped unrecognised line")
              continue
          print(g.WktParser.to_wkt(entry))
  ```

  will print out exactly the list of geometries above.

</details>

</details>

<details open>
<summary><b> &nbsp; 2. Geometry Operations</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 2.1 Containment </b></summary>

`Triangle3D.contains(p)` and `Polygon2D/3D.contains(p)` test whether a point lies inside a shape
  using barycentric coordinates and the winding number, respectively.
  `LineSegment.contains(p)` checks whether a point lies on the segment;
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

</details>


<details closed>
<summary><b> &nbsp; &nbsp; 2.2 Intersection </b></summary>

  Every 2D primitive (`Line2D`, `Ray2D`, `LineSegment2D`, `Triangle2D`, `Polygon2D`) can intersect any
  other 2D primitive, and the same holds in 3D across `Line3D`, `Ray3D`, `LineSegment3D`, `Triangle3D`,
  and `Plane`. Results are the geometry object or `None`; polygon intersections return a
  `list[LineSegment2D]` because a line can produce multiple chords through a concave shape.
  `find_intersections(segments)` (Bentley–Ottmann) reports all crossing points across an arbitrary set
  of 2D segments, sorted left-to-right.

  ```python
  import geompp as g

  # LineSegment2D → Polygon2D: segment is clipped to the polygon interior
  square = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0),
      g.Point2D(4, 4), g.Point2D(0, 4),
  ])
  seg = g.LineSegment2D.make(g.Point2D(-1, 2), g.Point2D(5, 2))
  seg_chords = square.intersection(seg)
  for c in (seg_chords or []):
      print(c.to_wkt())   # LINESTRING (0 2, 4 2)

  # Ray2D → Polygon2D: ray entering from outside, clipped at the exit boundary
  ray2d = g.Ray2D.make(g.Point2D(-1, 2), g.Vector2D(1, 0))
  ray_chords = square.intersection(ray2d)
  for c in (ray_chords or []):
      print(c.to_wkt())   # LINESTRING (0 2, 4 2)

  # Line2D → concave Polygon2D: vertical line through a C-shape produces two chords
  cshape = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 1),
      g.Point2D(1, 1), g.Point2D(1, 3), g.Point2D(4, 3),
      g.Point2D(4, 4), g.Point2D(0, 4),
  ])
  line2d     = g.Line2D.make(g.Point2D(2, 0), g.Point2D(2, 1))
  lin_chords = cshape.intersection(line2d)
  for c in (lin_chords or []):
      print(c.to_wkt())
  # LINESTRING (2 0, 2 1)
  # LINESTRING (2 3, 2 4)

  # Ray3D → Triangle3D
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

  ```
  LINESTRING (0 2, 4 2)
  LINESTRING (0 2, 4 2)
  LINESTRING (2 0, 2 1)
  LINESTRING (2 3, 2 4)
  POINT (1 1 0)
  POINT (2 1)
  POINT (1 2)
  POINT (2 2)
  POINT (3 2)
  ```


<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 2.2.1 Split a complex polygon </b></summary>

  A **complex polygon** (also called a self-intersecting polygon) is a polygon whose edges cross
  each other. `simplify()` decomposes it into a list of simple (non-self-intersecting) polygons
  via planar-graph half-edge face tracing. Each returned polygon is guaranteed to satisfy
  `is_simple() == True`. If the input is already simple, `simplify()` returns a single-element
  list containing the original polygon.

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # 2D: a "bowtie" — edges B→C and D→A cross at (2,2)
  bowtie = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0),
      g.Point2D(1, 3), g.Point2D(3, 3)])
  print("is simple:", bowtie.is_simple())   # False

  parts = bowtie.simplify()
  print(f"{len(parts)} simple polygon(s)")
  for p in parts:
      print(f"  {p.to_wkt()}  area={p.area()}")

  # 3D: same bowtie lifted into the XY plane (z = 0)
  bowtie3d = g.Polygon3D.make([
      g.Point3D(0,0,0), g.Point3D(4,0,0),
      g.Point3D(1,3,0), g.Point3D(3,3,0)])
  parts3d = bowtie3d.simplify()
  print(f"{len(parts3d)} simple 3D polygon(s)")
  ```

  ```
  is simple: False
  2 simple polygon(s)
    POLYGON ((0 0, 4 0, 2 2, 0 0))  area=4.0
    POLYGON ((2 2, 1 3, 3 3, 2 2))  area=1.0
  2 simple 3D polygon(s)
  ```

</details>

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 2.3 Overlap </b></summary>

  `overlaps(other)` returns `True` when two primitives share a 1D region (more than a single point).
  `overlap(other)` returns the shared geometry, or `None` when they do not overlap or only touch at
  a single point. The return type mirrors the "smaller" of the two primitives: a `Line × Line`
  overlap yields a `Line`; `Ray × Ray` with opposite directions yields a `LineSegment`; all
  `Segment`-involving pairs yield a `LineSegment`.

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  # ── Line × Line ─────────────────────────────────────────────────────────────
  x_axis = g.Line2D.make(g.Point2D(0, 0), g.Vector2D(1, 0))
  x_same = g.Line2D.make(g.Point2D(5, 0), g.Point2D(8, 0))   # same infinite line
  x_off  = g.Line2D.make(g.Point2D(0, 1), g.Vector2D(1, 0))  # parallel, offset

  print(x_axis.overlaps(x_same))  # True
  print(x_axis.overlaps(x_off))   # False

  ov_ll = x_axis.overlap(x_same)  # Line2D or None
  if ov_ll is not None:
      print(ov_ll.to_wkt())        # LINE (0 0, 1 0)

  # ── Ray × Ray (same direction) ─────────────────────────────────────────────
  r1 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D(1, 0))
  r2 = g.Ray2D.make(g.Point2D(3, 0), g.Vector2D(1, 0))  # inside r1

  ov_rr = r1.overlap(r2)  # Ray2D or LineSegment2D or None
  if ov_rr is not None:
      print(ov_rr.to_wkt())  # RAY (3 0, 1 0)

  # ── Ray × Ray (anti-parallel) ──────────────────────────────────────────────
  r3 = g.Ray2D.make(g.Point2D(7, 0), g.Vector2D(-1, 0))  # heads toward r1

  ov_anti = r1.overlap(r3)
  if ov_anti is not None:
      print(ov_anti.to_wkt())
  # LINESTRING (0 0, 7 0)

  # ── Segment × Segment ──────────────────────────────────────────────────────
  a = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(5, 0))
  b = g.LineSegment2D.make(g.Point2D(3, 0), g.Point2D(8, 0))

  print(a.overlaps(b))  # True
  ov_ss = a.overlap(b)  # LineSegment2D or None
  if ov_ss is not None:
      print(ov_ss.to_wkt())  # LINESTRING (3 0, 5 0)

  # touch at a single endpoint → no overlap
  c = g.LineSegment2D.make(g.Point2D(5, 0), g.Point2D(9, 0))
  print(a.overlaps(c))          # False
  print(a.overlap(c) is None)   # True
  ```

  ```
  True
  False
  LINE (0 0, 1 0)
  RAY (3 0, 1 0)
  LINESTRING (0 0, 7 0)
  True
  LINESTRING (3 0, 5 0)
  False
  True
  ```

</details>


<details closed>
<summary><b> &nbsp; &nbsp; 2.4 Touch </b></summary>

  `touches(other)` returns `True` when two primitives share exactly one endpoint-contact point
  (not an interior crossing, not a shared segment). `touch(other)` returns that contact point,
  or `None` when there is no touch.

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  # Ray origin sits on a line → touch at origin
  line = g.Line2D.make(g.Point2D(0, 0), g.Point2D(1, 0))   # x-axis
  ray  = g.Ray2D.make(g.Point2D(3, 0), g.Vector2D(0, 1))   # vertical at x=3
  print(line.touches(ray))                    # True
  tp = line.touch(ray)                        # Point2D or None
  print(tp.to_wkt())                          # POINT (3 0)

  # Collinear ray → overlap, not touch
  ray_col = g.Ray2D.make(g.Point2D(1, 0), g.Vector2D(1, 0))
  print(line.touches(ray_col))               # False

  # Anti-parallel rays sharing only their common origin → touch
  r1 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D( 1, 0))
  r2 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D(-1, 0))
  print(r1.touches(r2))                      # True
  print(r1.touch(r2).to_wkt())              # POINT (0 0)

  # Anti-parallel rays overlapping → touch returns None
  r3 = g.Ray2D.make(g.Point2D(3, 0), g.Vector2D(-1, 0))
  print(r1.touches(r3))                      # False

  # Segment T-junction: endpoint of b lies on a (non-collinear)
  a = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(5, 0))
  b = g.LineSegment2D.make(g.Point2D(3, 0), g.Point2D(3, 3))
  print(a.touches(b))                        # True
  print(a.touch(b).to_wkt())                # POINT (3 0)

  # Collinear segments sharing exactly one endpoint → touch
  c = g.LineSegment2D.make(g.Point2D(5, 0), g.Point2D(8, 0))
  print(a.touches(c))                        # True
  print(a.touch(c).to_wkt())                # POINT (5 0)

  # Overlapping collinear segments → touch returns None
  d = g.LineSegment2D.make(g.Point2D(3, 0), g.Point2D(7, 0))
  print(a.touches(d))                        # False
  print(a.touch(d) is None)                 # True
  ```

  ```
  True
  POINT (3 0)
  False
  True
  POINT (0 0)
  False
  True
  POINT (3 0)
  True
  POINT (5 0)
  False
  True
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 2.5 Polyline Overlaps / Touches </b></summary>

  `Polyline2D` and `Polyline3D` iterate over their constituent segments to collect all
  overlapping sub-segments or all touch points. `overlap()` returns a list of
  `LineSegment2D`/`3D` or `None`; `touch()` returns a list of `Point2D`/`3D` or `None`.

  ```python
  import geompp as g

  # L-shaped polyline
  pl = g.Polyline2D.make([g.Point2D(0,0), g.Point2D(4,0), g.Point2D(4,3)])

  # x-axis line overlaps the horizontal leg
  x_axis = g.Line2D.make(g.Point2D(0,0), g.Point2D(1,0))
  print(pl.overlaps(line=x_axis))                    # True
  segs = pl.overlap(line=x_axis)
  for s in segs:
      print(s.to_wkt())                              # LINESTRING (0 0, 4 0)

  # T-junction: vertical arm touches a horizontal segment at (3,0)
  stem = g.Polyline2D.make([g.Point2D(3,0), g.Point2D(3,3)])
  bar  = g.LineSegment2D.make(g.Point2D(0,0), g.Point2D(5,0))
  print(stem.touches(segment=bar))                   # True
  pts = stem.touch(segment=bar)
  for p in pts:
      print(p.to_wkt())                              # POINT (3 0)

  # Two polylines sharing an endpoint
  pl1 = g.Polyline2D.make([g.Point2D(0,0), g.Point2D(3,0)])
  pl2 = g.Polyline2D.make([g.Point2D(3,0), g.Point2D(3,3)])
  print(pl1.touches(other=pl2))                      # True
  ```

  ```
  True
  LINESTRING (0 0, 4 0)
  True
  POINT (3 0)
  True
  ```

</details>

</details>


<details open>
<summary><b> &nbsp; 3. Planes </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 3.1 Coplanarity, winding order, and polygon with holes </b></summary>

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # Four points on the XY plane vs. a set that spans 3D space
  pts_flat = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(0,1,0), g.Point3D(1,1,0)]
  pts_3d   = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(0,1,0), g.Point3D(0,0,1)]

  print(g.are_coplanar(pts_flat))  # True  — all on the XY plane
  print(g.are_coplanar(pts_3d))    # False — spans 3D space

  # Which world-axis plane is closest to the cloud?
  plane = g.closest_world_plane_to(pts_flat)
  print(plane.normal)              # VECTOR (0 0 1)  → XY plane

  # Winding check
  ring = [g.Point3D(0,0,0), g.Point3D(1,0,0), g.Point3D(1,1,0), g.Point3D(0,1,0)]
  print(g.are_ccw(ring))           # True

  # Polygon3D with a rectangular hole (outer CCW, hole CW)
  outer = [g.Point3D(0,0,0), g.Point3D(4,0,0), g.Point3D(4,4,0), g.Point3D(0,4,0)]
  hole  = [g.Point3D(1,3,0), g.Point3D(3,3,0), g.Point3D(3,1,0), g.Point3D(1,1,0)]
  poly  = g.Polygon3D.make(outer, [hole])
  print(poly.to_wkt())
  ```

  will print out

  ```
  True
  False
  VECTOR (0 0 1)
  True
  POLYGON ((0 0 0, 4 0 0, 4 4 0, 0 4 0, 0 0 0), (1 3 0, 3 3 0, 3 1 0, 1 1 0, 1 3 0))
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 3.2 Projecting points onto a plane </b></summary>

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

  ```
  on_plane: POINT (3 4 0)
  in_plane: POINT (3 4)
  back:     POINT (3 4 0)
  ```

</details>


<details closed>
<summary><b> &nbsp; &nbsp; 3.3 Planar vs non-planar Polyline3D </b></summary>

  `Polyline3D.is_planar()` checks whether all knots lie in a common plane. Only planar polylines support
  `is_simple()`, `is_convex()`, `convex_hull()`, and `to_polygon()` — call `is_planar()` first.

  `convex_hull()` returns a `Polyline3D` (an open path). Call `to_polygon()` on it to close the boundary into a `Polygon3D` with area.

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

  will print out

  ```
  planar: True
  hull knots:   4
  polygon area: 16.000
  planar: False
  dominant direction: VECTOR (...)
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 3.4 View2D — streaming 3D points to 2D </b></summary>

  `View2D` maps 3D points to 2D scalars via `x()` / `y()` getters without allocating an intermediate
  `Point2D` list. Axis-aligned views (`xy`, `yz`, `zx`) are the fastest path — just a direct coordinate
  read. `on_plane` computes dot products against the plane's local axes.

  ```python
  from geompp import View2D, Plane, Point3D, Vector3D

  # Axis-aligned views (fast path — single coordinate read)
  v_xy = View2D.xy()   # x→x, y→y (drops z)
  v_yz = View2D.yz()   # y→x, z→y (drops x)
  v_zx = View2D.zx()   # z→x, x→y (drops y)

  # Custom view onto any plane
  plane    = Plane.from_origin_and_normal(Point3D(0, 0, 5), Vector3D(0, 0, 1))
  v_custom = View2D.on_plane(plane)

  pts3d = [Point3D(1, 2, 5), Point3D(3, 4, 5), Point3D(5, 6, 5)]

  # Stream 3D points to 2D without allocating a Point2D list
  for p in pts3d:
      print(f"({v_xy.x(p)}, {v_xy.y(p)})")
  ```

  ```
  (1, 2)
  (3, 4)
  (5, 6)
  ```

</details>

</details>


<details closed>
<summary><b> &nbsp; 4. PCA on a 3D point cloud </b></summary>

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

  will print out

  ```
  x (primary):   VECTOR (1 0 0)
  y (secondary): VECTOR (0 1 0)
  z (normal):    VECTOR (0 0 1)
  ```

</details>


<details open>
<summary><b> &nbsp; 5. Bounding containers </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 5.1 Simple containers for quick rejection </b></summary>

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

  # Axis-aligned bounding box (construct from a polyline spanning all verts)
  box = g.BBox3D(g.Polyline3D.make(verts))
  print(box.min.to_wkt())                      # POINT (0 0 0)
  print(box.max.to_wkt())                      # POINT (6 4 3)
  print(box.contains(g.Point3D(3, 1, 1)))      # True
  print(box.contains(g.Point3D(7, 1, 1)))      # False

  # Bounding ball — one constructor takes the point cloud directly
  ball = g.BBall3D(verts)
  print(ball.center.to_wkt())
  print(f"radius: {ball.radius:.3f}")
  print(ball.contains(g.Point3D(3, 1, 1)))     # True

  # All original vertices must be inside the ball
  assert all(ball.contains(p) for p in verts)
  ```

  will print out

  ```
  POINT (0 0 0)
  POINT (6 4 3)
  True
  False
  POINT (3 ...)
  radius: ...
  True
  ```

</details>

<details open>
<summary><b> &nbsp; &nbsp; 5.2 Convex hull </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.1 Convex hull of a point cloud </b></summary>

  `convex_hull(points)` (Andrew's monotone chain) wraps any point cloud into its tightest convex polygon:

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # An asymmetric 5-pointed star: 5 outer tips + 5 inner concave vertices.
  # The convex hull should be exactly the 5 outer tips.
  star = [
      g.Point2D( 0,  5), g.Point2D( 4,  2),
      g.Point2D( 3, -3), g.Point2D(-2, -4), g.Point2D(-3,  1),
      g.Point2D( 2,  1), g.Point2D( 2, -1),
      g.Point2D( 0, -1), g.Point2D(-1, -1), g.Point2D(-1,  2),
  ]

  hull = g.convex_hull(star)

  print(f"hull has {len(hull)} vertices:")
  for p in hull:
      print(f"  {p.to_wkt()}")
  ```

  ```
  hull has 5 vertices:
    POINT (3 -3)
    POINT (4 2)
    POINT (0 5)
    POINT (-3 1)
    POINT (-2 -4)
  ```

  For 3D point clouds, `convex_hull(points)` also works — points do **not** need to be perfectly
  coplanar. When no explicit normal is provided, the best-fit plane is estimated via PCA
  (Jacobi eigendecomposition), and the hull is computed on the projection onto that plane.
  You can also pass an explicit normal if known: `convex_hull(points, normal)`.

  ```python
  import geompp as g

  # Nearly-coplanar cloud with small Z jitter
  cloud = [
      g.Point3D(0, 0, 0.1), g.Point3D(4, 0, -0.1),
      g.Point3D(4, 4, 0.05), g.Point3D(0, 4, -0.05),
      g.Point3D(2, 2, 0.02),  # interior
  ]

  hull = g.convex_hull(cloud)  # PCA detects near-XY plane, projects, computes hull
  print(f"3D hull has {len(hull)} vertices")  # 4 — interior point excluded
  ```

  (CCW order, starting from the lexicographically smallest point)

</details>

<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.2 Convex hull of a polygon </b></summary>

  `Polygon2D` and `Polygon3D` expose a `convex_hull()` method that wraps the free function:

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

  hull = star.convex_hull()   # Polygon3D — 5-vertex pentagon

  print(f"star is convex: {star.is_convex()}")  # False — star has concavities
  print(f"hull is convex: {hull.is_convex()}")  # True

  print(f"hull has {hull.size()} vertices:")
  for i in range(hull.size()):
      print(f"  {hull[i].to_wkt()}")
  ```

  ```
  star is convex: False
  hull is convex: True
  hull has 5 vertices:
    POINT (3 -3 0)
    POINT (4 2 0)
    POINT (0 5 0)
    POINT (-3 1 0)
    POINT (-2 -4 0)
  ```

</details>


<details closed>
<summary><b> &nbsp; &nbsp; &nbsp; 5.2.3 Convex hull of a simple polyline </b></summary>

  `Polyline2D.convex_hull()` uses Melkman's O(n) algorithm. The polyline must be simple — call `is_simple()` first.

  ```python
  import geompp as g

  # Simple concave path: outer corners with an inner dip at (2,1)
  path = g.Polyline2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4),
      g.Point2D(2, 1), g.Point2D(0, 4),
  ])

  if path.is_simple():
      hull = path.convex_hull()  # Polygon2D — 4-vertex rectangle
      print(f"hull has {hull.size()} vertices")
  ```

  ```
  hull has 4 vertices
  ```

</details>

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 5.3 Oriented Minimum Bounding Rectangle </b></summary>

  `BRect2D` computes the **tightest** axis-aligned-to-input rectangle that encloses a point cloud.
  It is defined by a center point, two orthogonal unit axes (`axis_u`, `axis_v`), and two half-lengths
  (`half_len_u`, `half_len_v`).

  **Algorithm**: Freeman & Shapira (1975) / Toussaint (1983) rotating calipers.
  1. Compute the convex hull of the input cloud (Andrew's monotone chain, O(n log n)).
  2. For each hull edge, project all hull vertices onto the edge direction and its perpendicular.
  3. The rectangle aligned with that edge has width = max − min along the edge and height = max − min along the perpendicular.
  4. Track the edge orientation that minimises area; the center is the midpoint of the extents.

  ```python
  import geompp as g

  g.set_decimal_precision(g.DP_THREE)

  # An asymmetric pentagon
  pts = [
      g.Point2D(0, 0), g.Point2D(4, 0),
      g.Point2D(5, 2), g.Point2D(2, 4),
      g.Point2D(-1, 2),
  ]

  r = g.BRect2D(pts)

  print(r.center.to_wkt())      # center of the OBB
  print(r.axis_u.to_wkt())      # primary axis (unit vector, along a hull edge)
  print(r.axis_v.to_wkt())      # secondary axis (perpendicular, CCW rotation of axis_u)
  print(f"half_u: {r.half_len_u:.3f}")
  print(f"half_v: {r.half_len_v:.3f}")
  print(f"area:   {r.area:.3f}")
  print(r.contains(g.Point2D(2, 2)))   # True  — interior point
  print(r.contains(g.Point2D(9, 0)))   # False — outside

  corners = r.corners()         # list of 4 Point2D in CCW order
  for c in corners:
      print(c.to_wkt())
  ```

  `corners()` returns the four corners in CCW order; each is guaranteed to be `contains()`-true.

  The `contains()` test projects the query point onto the local axes and checks both projections against the half-lengths — O(1) per query.

  `BPrism3D` computes the minimum-volume oriented bounding prism via PCA + rotating calipers (requires ≥ 3 non-collinear points):

  ```python
  import geompp as g

  pts = [
      g.Point3D(0,0,0), g.Point3D(4,0,0), g.Point3D(4,3,0), g.Point3D(0,3,0),
      g.Point3D(0,0,2), g.Point3D(4,0,2), g.Point3D(4,3,2), g.Point3D(0,3,2),
  ]
  prism = g.BPrism3D(pts)
  print(prism.center.to_wkt())                 # roughly POINT (2 1.5 1)
  print(f"U: {prism.axis_u.to_wkt()}")
  print(f"V: {prism.axis_v.to_wkt()}")
  print(f"W: {prism.axis_w.to_wkt()}")
  print(f"{prism.width:.1f} × {prism.height:.1f} × {prism.depth:.1f}")   # 4 × 3 × 2
  print(f"volume: {prism.volume:.1f}")         # 24.0
  corners = prism.corners()                    # list of 8 Point3D corners
  print(prism.contains(prism.center))          # True
  ```

  
</details>

<details closed>
<summary><b> &nbsp; &nbsp; 5.4 Polygon extreme points </b></summary>

  `find_extreme_points(polygon, line)` returns the two vertices of a polygon that are **extreme** — the
  least and the greatest — when projected onto a line's direction (the "supporting vertices" along that
  axis). It is handy for collision broad-phase (SAT), rotating calipers, and directional clipping.

  The result is an `ExtremePoints2D` (or `ExtremePoints3D`) with `.min_point` / `.max_point`. When the
  polygon is **convex** it uses Daniel Sunday's O(log n) binary search; otherwise it falls back to an
  O(n) linear scan. Holes are ignored — only the outer ring participates.

  ```python
  import geompp as g

  # Convex diamond; project onto the X-axis to get the left / right tips
  diamond = g.Polygon2D.make([
      g.Point2D(2, 0), g.Point2D(4, 2), g.Point2D(2, 4), g.Point2D(0, 2)])
  x_axis = g.Line2D.make(g.Point2D(0, 0), g.Point2D(1, 0))

  ext = g.find_extreme_points(diamond, x_axis)   # convex → O(log n)
  print(f"min: {ext.min_point.to_wkt()}")         # POINT (0 2)
  print(f"max: {ext.max_point.to_wkt()}")         # POINT (4 2)

  # Works in 3D too — the polygon may lie in any plane
  para = g.Polygon3D.make([
      g.Point3D(0, 0, 0), g.Point3D(2, 0, 2),
      g.Point3D(2, 2, 2), g.Point3D(0, 2, 0)])
  d = g.Line3D.make(g.Point3D(0, 0, 0), g.Point3D(1, 1, 0))
  ext3 = g.find_extreme_points(para, d)
  print(f"{ext3.min_point.to_wkt()} .. {ext3.max_point.to_wkt()}")
  ```

  ```
  min: POINT (0 2)
  max: POINT (4 2)
  POINT (0 0 0) .. POINT (2 2 2)
  ```

</details>

</details>


<details open>
<summary><b> &nbsp; 6. Distance </b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 6.1 from Point</b></summary>

  Every core primitive implements `.distance_to(point)` — the perpendicular / nearest distance to a
  point, clamped to the primitive's own domain where relevant (a `Ray` only measures ahead of its
  origin, a `LineSegment`/`Polyline` clamps to its own bounded extent). `Point2D`/`Point3D` themselves
  just measure Euclidean distance to another point.

  `Polygon2D`/`Polygon3D.distance_to(point)` and `Triangle2D`/`Triangle3D.distance_to(point)` are
  declared but not yet implemented — they currently raise `RuntimeError`, so they're omitted from
  the example below.

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  p2 = g.Point2D(3, 4)
  p3 = g.Point3D(3, 4, 5)

  print(f"Point2D:       {g.Point2D(0, 0).distance_to(p2):.3f}")
  print(f"Point3D:       {g.Point3D(0, 0, 0).distance_to(p3):.3f}")

  line2 = g.Line2D.make(g.Point2D(0, 0), g.Point2D(1, 0))
  line3 = g.Line3D.make(g.Point3D(0, 0, 0), g.Point3D(1, 0, 0))
  print(f"Line2D:        {line2.distance_to(p2):.3f}")
  print(f"Line3D:        {line3.distance_to(p3):.3f}")

  ray2 = g.Ray2D.make(g.Point2D(0, 0), g.Vector2D(1, 0))
  ray3 = g.Ray3D.make(g.Point3D(0, 0, 0), g.Vector3D(1, 0, 0))
  print(f"Ray2D:         {ray2.distance_to(p2):.3f}")
  print(f"Ray3D:         {ray3.distance_to(p3):.3f}")

  seg2 = g.LineSegment2D.make(g.Point2D(0, 0), g.Point2D(6, 0))
  seg3 = g.LineSegment3D.make(g.Point3D(0, 0, 0), g.Point3D(6, 0, 0))
  print(f"LineSegment2D: {seg2.distance_to(p2):.3f}")
  print(f"LineSegment3D: {seg3.distance_to(p3):.3f}")

  pl2 = g.Polyline2D.make([g.Point2D(0, 0), g.Point2D(6, 0), g.Point2D(6, 6)])
  pl3 = g.Polyline3D.make([g.Point3D(0, 0, 0), g.Point3D(6, 0, 0), g.Point3D(6, 6, 0)])
  print(f"Polyline2D:    {pl2.distance_to(p2):.3f}")
  print(f"Polyline3D:    {pl3.distance_to(p3):.3f}")

  plane = g.Plane.xy()
  print(f"Plane:         {plane.distance_to(p3):.3f}")
  ```

  ```
  Point2D:       5.000
  Point3D:       7.071
  Line2D:        4.000
  Line3D:        6.403
  Ray2D:         4.000
  Ray3D:         6.403
  LineSegment2D: 4.000
  LineSegment3D: 6.403
  Polyline2D:    3.000
  Polyline3D:    5.831
  Plane:         5.000
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 6.2 from Other primitives</b></summary>

  `Line3D`, `Ray3D`, and `LineSegment3D` each expose `.distance_to(Line3D | Ray3D | LineSegment3D)`
  — pairwise distance between any two of the three (0 if they intersect, overlap, or one contains
  the other). This overload set is 3D-only: two 2D primitives are either parallel (a constant
  distance, rarely useful on its own) or they intersect (0), so `Line2D`/`Ray2D`/`LineSegment2D`
  don't expose it.

  If you need the actual closest-approach segment instead of just the scalar, use `.distance(...)`
  (note: no `_to`) — it returns a `LineSegment3D` or `None` when the two intersect or overlap
  (matching the zero case of `distance_to`).

  For polygon-to-line distance, use the free function `distance_to(polygon, line)` (see section 5.4
  "Polygon extreme points" for `find_extreme_points`, its sibling function) — zero if the line
  crosses the polygon; for `Polygon3D`/`Line3D` it also handles a line coplanar with, parallel to
  (fixed offset from), or skew to the polygon's plane.

  ```python
  lineA = g.Line3D.make(g.Point3D(0, 0, 0), g.Point3D(1, 0, 0))
  lineB = g.Line3D.make(g.Point3D(0, 1, 1), g.Point3D(1, 1, 1))  # parallel, offset sqrt(2)
  print(f"Line3D x Line3D:        {lineA.distance_to(lineB):.3f}")

  rayB = g.Ray3D.make(g.Point3D(0, 1, 1), g.Vector3D(1, 0, 0))
  print(f"Line3D x Ray3D:         {lineA.distance_to(rayB):.3f}")

  segB = g.LineSegment3D.make(g.Point3D(0, 1, 1), g.Point3D(1, 1, 1))
  print(f"Line3D x LineSegment3D: {lineA.distance_to(segB):.3f}")

  # the closest-approach connecting segment, instead of just the scalar
  connector = lineA.distance(lineB)
  print(f"Line3D.distance(Line3D): {connector.to_wkt()}")

  # Polygon2D / Polygon3D — distance to an infinite line (zero if the line crosses)
  square = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4), g.Point2D(0, 4)])
  far_line = g.Line2D.make(g.Point2D(6, -1), g.Point2D(6, 5))
  print(f"distance_to(Polygon2D, Line2D): {g.distance_to(square, far_line):.3f}")
  ```

  ```
  Line3D x Line3D:        1.414
  Line3D x Ray3D:         1.414
  Line3D x LineSegment3D: 1.414
  Line3D.distance(Line3D): LINESTRING (0 0 0, 0 1 1)
  distance_to(Polygon2D, Line2D): 2.000
  ```

</details>

</details>

<details open>
<summary><b> &nbsp; 7. Polygon tangents</b></summary>

<details closed>
<summary><b> &nbsp; &nbsp; 7.1 Point to Polygon</b></summary>

  `tangents_to(polygon, point)` returns `PolygonTangents2D`/`PolygonTangents3D` (`.left` / `.right`,
  each a `LineSegment`) — the two tangent segments from an external point to a polygon (the point's
  "line of sight" grazing the shape on either side, like a taut string pulled around it). Convex
  polygons use Daniel Sunday's O(log n) binary search; non-convex polygons are reduced to their
  convex hull first (a tangent point can only ever be a hull vertex — a reflex vertex always has the
  polygon on both sides of it, so it can never support a tangent line) and the result is mapped back
  to the original vertex.

  The point must be strictly outside the polygon and not equal to any of its vertices. For
  `Polygon3D`, a tangent is inherently a planar concept — unlike `distance_to`, there is no "skew"
  fallback — so the point must lie in the polygon's own plane, or the call raises `RuntimeError`.

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  square = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4), g.Point2D(0, 4)])
  t2 = g.tangents_to(square, g.Point2D(10, -2))
  print(f"Polygon2D left:  {t2.left.to_wkt()}")
  print(f"Polygon2D right: {t2.right.to_wkt()}")

  # Polygon3D requires the point to be coplanar with the polygon (here, the z=0 plane)
  square3 = g.Polygon3D.make([
      g.Point3D(0, 0, 0), g.Point3D(4, 0, 0), g.Point3D(4, 4, 0), g.Point3D(0, 4, 0)])
  t3 = g.tangents_to(square3, g.Point3D(10, -2, 0))
  print(f"Polygon3D left:  {t3.left.to_wkt()}")
  print(f"Polygon3D right: {t3.right.to_wkt()}")
  ```

  ```
  Polygon2D left:  LINESTRING (10 -2, 0 0)
  Polygon2D right: LINESTRING (10 -2, 4 4)
  Polygon3D left:  LINESTRING (10 -2 0, 0 0 0)
  Polygon3D right: LINESTRING (10 -2 0, 4 4 0)
  ```

</details>

<details closed>
<summary><b> &nbsp; &nbsp; 7.2 Polygon to Polygon</b></summary>

  `tangents_to(polygon, other)` returns the two common outer tangent segments between two polygons —
  the "belt around two pulleys" lines that touch both shapes without crossing either. Neither polygon
  needs to be convex: each is independently reduced to its convex hull when needed, same as the
  point overload above. For `Polygon3D`, both polygons must share the same plane (two polygons in
  general 3D position don't have a single well-defined common tangent line), or the call raises
  `RuntimeError`.

  ```python
  import geompp as g
  g.set_decimal_precision(g.DP_THREE)

  square_a = g.Polygon2D.make([
      g.Point2D(0, 0), g.Point2D(4, 0), g.Point2D(4, 4), g.Point2D(0, 4)])
  square_b = g.Polygon2D.make([
      g.Point2D(10, 1), g.Point2D(14, 1), g.Point2D(14, 5), g.Point2D(10, 5)])
  t2 = g.tangents_to(square_a, square_b)
  print(f"Polygon2D left:  {t2.left.to_wkt()}")
  print(f"Polygon2D right: {t2.right.to_wkt()}")
  ```

  ```
  Polygon2D left:  LINESTRING (0 4, 10 5)
  Polygon2D right: LINESTRING (4 0, 14 1)
  ```

</details>

</details>

</details>

