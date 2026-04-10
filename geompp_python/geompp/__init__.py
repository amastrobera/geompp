"""
geompp — Python bindings for the geompp C++ geometry library.

Classes
-------
2D primitives:
    Point2D, Vector2D, Line2D, Ray2D, LineSegment2D,
    Polyline2D, Triangle2D, Polygon2D, BBox2D

3D primitives:
    Point3D, Vector3D, Line3D, Ray3D, LineSegment3D,
    Polyline3D, Triangle3D, Polygon3D, BBox3D, Plane

Parser:
    LVSParser

Free functions
--------------
    are_collinear(p1, p2, p3)
    remove_duplicates(points)
    remove_duplicates_from_sorted_list(points)
    remove_collinear(points)
    linear_combination(points, weights)
    average(points)

Precision
---------
    set_decimal_precision(dp)   # e.g. DP_THREE, DP_SIX, DP_NINE
    get_decimal_precision() -> int
    DP_THREE, DP_SIX, DP_NINE

Intersection return values
--------------------------
``intersection()`` methods return ``None`` when there is no intersection, or
the geometry object of the intersection otherwise.  For polylines and triangles
the return can be a single point *or* a list of points / a segment / a polygon
depending on the geometry involved — use ``isinstance()`` to discriminate.
"""

from ._geompp import (  # noqa: F401
    # precision
    DP_THREE,
    DP_SIX,
    DP_NINE,
    set_decimal_precision,
    get_decimal_precision,
    # 2D
    Point2D,
    Vector2D,
    Line2D,
    Ray2D,
    LineSegment2D,
    Polyline2D,
    Triangle2D,
    Polygon2D,
    BBox2D,
    # 3D
    Point3D,
    Vector3D,
    Line3D,
    Ray3D,
    LineSegment3D,
    Polyline3D,
    Triangle3D,
    Polygon3D,
    BBox3D,
    Plane,
    # parser
    LVSParser,
    # free functions
    are_collinear,
    remove_duplicates,
    remove_duplicates_from_sorted_list,
    remove_collinear,
    linear_combination,
    average,
)

__version__ = "1.0.0"

__all__ = [
    "DP_THREE", "DP_SIX", "DP_NINE",
    "set_decimal_precision", "get_decimal_precision",
    "Point2D", "Vector2D", "Line2D", "Ray2D", "LineSegment2D",
    "Polyline2D", "Triangle2D", "Polygon2D", "BBox2D",
    "Point3D", "Vector3D", "Line3D", "Ray3D", "LineSegment3D",
    "Polyline3D", "Triangle3D", "Polygon3D", "BBox3D", "Plane",
    "LVSParser",
    "are_collinear", "remove_duplicates", "remove_duplicates_from_sorted_list",
    "remove_collinear", "linear_combination", "average",
]
