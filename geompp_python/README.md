# geompp

Python bindings for [geompp](https://github.com/amastrobera/geompp) — a C++ 2D/3D geometry library.

## Install from source

```bash
pip install ./geompp_python
```

Requires CMake ≥ 3.15, a C++20-capable compiler, and Python ≥ 3.8.

## Quick start

```python
import geompp

# Points & vectors
p = geompp.Point2D(1.0, 2.0)
v = geompp.Vector2D(3.0, 0.0)
q = p + v                       # Point2D(4, 2)
diff = q - p                    # Vector2D(3, 0)

# Lines and intersection
l1 = geompp.Line2D.make(geompp.Point2D(0,0), geompp.Point2D(1,0))
l2 = geompp.Line2D.make(geompp.Point2D(0.5,-1), geompp.Point2D(0.5,1))
hit = l1.intersection(l2)       # Point2D(0.5, 0) or None

# 3D
p3 = geompp.Point3D(1, 2, 3)
plane = geompp.Plane.xy()
proj = plane.project_onto(p3)   # Point3D(1, 2, 0)

# Precision
geompp.set_decimal_precision(geompp.DP_SIX)

# File parser
parser = geompp.LVSParser.open("geometry.lsv")
while parser.has_next():
    item = parser.next()
    if item is not None:
        print(geompp.LVSParser.to_wkt(item))
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
