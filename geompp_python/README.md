[← back](../README.md)

# geompp

Python bindings for [geompp](https://github.com/amastrobera/geompp) — a C++ 2D/3D geometry library.

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

# Precision
g.set_decimal_precision(g.DP_SIX)

# File parser
parser = g.WktParser.open("geometry.lsv")
while parser.has_next():
    item = parser.next()
    if item is not None:
        print(g.WktParser.to_wkt(item))
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
| | Plane |



