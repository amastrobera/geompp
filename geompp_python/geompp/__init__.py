"""
geompp — Python bindings for the geompp C++ geometry library.

Classes
-------
2D primitives:
    Point2D, Vector2D, Line2D, Ray2D, LineSegment2D,
    Polyline2D, Triangle2D, Polygon2D, BBox2D, BBall2D, BRect2D, GeometryCollection2D

3D primitives:
    Point3D, Vector3D, Line3D, Ray3D, LineSegment3D,
    Polyline3D, Triangle3D, Polygon3D, BBox3D, BBall3D, BPrism3D, Plane, GeometryCollection3D

Planar operations:
    ProjectionType, View2D

Parser:
    WktParser

Meshes (WIP — faces cannot have holes):
    GridCell2D, GridCell3D (internal vertex-welding grid bucket, exposed for completeness)
    Mesh2D, Mesh3D (triangle faces, no adjacency structure)
    PolyMesh2D, PolyMesh3D (arbitrary-sided polygon faces, no adjacency structure)
    ConnectedMesh2D, ConnectedMesh3D (triangle faces; per-facet edge adjacency precomputed
                     internally and exposed via the FaceView2D/FaceView3D objects returned by
                     indexing/iteration)
    FaceView2D, FaceView3D (a facet of a ConnectedMesh2D/3D: .geometry(), .neighbor(edge),
                     .neighbor_entry_edge(edge))
    TriangleEdge (FIRST, SECOND, THIRD — which of a facet's 3 edges to cross; INVALID marks a
                  boundary edge with no twin; shared by ConnectedMesh2D and ConnectedMesh3D)

Free functions
--------------
    are_collinear(p1, p2, p3)
    are_coplanar(points)
    closest_world_plane_to(points)
    are_ccw(points[, ref_plane])
    are_cw(points[, ref_plane])
    remove_duplicates(points)
    remove_consecutive_duplicates(points)
    remove_collinear(points)
    linear_combination(points, weights)
    average(points)
    lerp(p0, p1, t)
    clip(clipper_loop, subject_loop)
    signed_area(points[, plane])
    centroid(points[, plane])
    find_extreme_points(polygon, line)
    distance_to(polygon, line)
    tangents_to(polygon, point_or_polygon)
    dist_decimation(points, threshold)
    rdp_decimation(points, threshold)
    vw_decimation(points, threshold)
    bezier_smoothing_2(p0, p1, p2, smoothness, min_distance_or_num_segments, min_segment_length=DOUBLE_EPSILON)
        (a float dispatches to the min_distance behavior, an int to the num_segments behavior)

Polyline decimation:
    PolylineDecimationStrategy (RadialDistance, RamerDouglasPeucker, VisvalingamWhyatt)
    PolylineDecimationParams(strategy=RamerDouglasPeucker, threshold=0.5)

Polyline expansion (corner rounding — the inverse of decimation):
    PolylineExpansionMode (FixedSegments, MinDistance)
    PolylineExpansionParams(smoothness=0.5, mode=FixedSegments, segments_per_corner=4,
                             min_distance=0.1, min_segment_length=DOUBLE_EPSILON)
    polyline_expansion(points, settings)

Triangulation (2D or 3D, 3D input is assumed flat/planar):
    TriangulationStrategy (EarClipping; MonotonePolygon and Delaunay not yet implemented)
    TriangulationSimplicity, TriangulationWinding, TriangulationCollinearity
        (each Guaranteed/Assert/Enforce — how to handle non-simple/non-CCW/collinear input)
    TriangulationParams(strategy=EarClipping, simplicity=Enforce, ccw_winding=Enforce,
                         collinearity=Enforce)
    triangulate(points, settings) -> list[Triangle2D]
    triangulate(points, normal, settings) -> list[Triangle3D]
    triangulate(points, settings) -> list[Triangle3D]              # normal fitted via PCA
    Polygon2D.triangulate(strategy), Polygon3D.triangulate(strategy) -> list[Triangle2D/3D]
    PolyMesh2D.triangulate(strategy), PolyMesh3D.triangulate(strategy) -> Mesh2D/Mesh3D

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

geompp.maths submodule
-----------------------
Fixed-size linear algebra, independent of the geometry classes above:
    Vector2, Vector3, Vector4 -- indexing, x/y/z/w, dot/cross (Vector3 only), length,
                                  normalized(), arithmetic operators
    Matrix2, Matrix3, Matrix4 -- element access via m(r, c)/.set(r, c, v), zero()/identity(),
                                  transpose(), determinant(), inverse(), arithmetic operators,
                                  @ for matrix@matrix and matrix@vector
    Matrix4.translation(offset), .rotation(angle_rad, axis), .scale(factor|sx, sy, sz)
                                  -- homogeneous 4x4 affine-transform factories
    solve_gauss(a, b), solve_cramer(a, b) -- solve a square linear system a*x = b
Import as ``from geompp import maths`` or ``import geompp.maths``.

geompp.transformations submodule
----------------------------------
Affine transforms for the classes above, built on geompp.maths:
    translate(point, offset), rotate(point, angle_rad[, axis]), scale(point, factor|sx, sy[, sz])
                                  -- fast direct arithmetic on a single Point2D/Point3D, no matrix
    transform(obj, matrix)       -- every primitive from Point2D/3D to PolyMesh2D/3D, via a 3x3
                                     (2D) or 4x4 (3D) homogeneous matrix
    TransformBuilder()           -- fluent Matrix4 composer: .translate(offset).rotate(angle_rad, axis)
                                     .scale(...), each applied after every op already chained; .get()/
                                     .build() returns the composed geompp.maths.Matrix4
Import as ``from geompp import transformations`` or ``import geompp.transformations``.
"""

import sys as _sys

from . import _geompp

# geompp::maths is a real (non-inline) C++ namespace, so it gets a real Python submodule rather than a
# flat set of names -- register it under both spellings so `from geompp import maths` and
# `import geompp.maths` both work, matching how a normal Python package submodule behaves.
maths = _geompp.maths
_sys.modules[__name__ + ".maths"] = maths

transformations = _geompp.transformations
_sys.modules[__name__ + ".transformations"] = transformations

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
    BBall2D,
    BRect2D,
    GeometryCollection2D,
    # 3D
    Axis,
    Point3D,
    Vector3D,
    Line3D,
    Ray3D,
    LineSegment3D,
    Polyline3D,
    Triangle3D,
    Polygon3D,
    BBox3D,
    BBall3D,
    BPrism3D,
    Plane,
    GeometryCollection3D,
    # planar operations
    ProjectionType,
    View2D,
    # parser
    WktParser,
    # meshes (WIP)
    GridCell2D,
    GridCell3D,
    Mesh2D,
    Mesh3D,
    PolyMesh2D,
    PolyMesh3D,
    ConnectedMesh2D,
    ConnectedMesh3D,
    FaceView2D,
    FaceView3D,
    TriangleEdge,
    # free functions
    are_collinear,
    are_coplanar,
    closest_world_plane_to,
    are_ccw,
    are_cw,
    remove_duplicates,
    remove_consecutive_duplicates,
    remove_collinear,
    linear_combination,
    average,
    lerp,
    clip,
    signed_area,
    centroid,
    has_intersections,
    find_intersections,
    convex_hull,
    CoordinateFrame,
    principal_axes,
    principal_normal,
    principal_direction,
    ExtremePoints2D,
    ExtremePoints3D,
    find_extreme_points,
    distance_to,
    PolygonTangents2D,
    PolygonTangents3D,
    tangents_to,
    # polyline decimation
    PolylineDecimationStrategy,
    PolylineDecimationParams,
    dist_decimation,
    rdp_decimation,
    vw_decimation,
    bezier_smoothing_2,
    # polyline expansion
    PolylineExpansionMode,
    PolylineExpansionParams,
    polyline_expansion,
    # triangulation
    TriangulationStrategy,
    TriangulationSimplicity,
    TriangulationWinding,
    TriangulationCollinearity,
    TriangulationParams,
    triangulate,
    # mesh-conformity checking ("every edge has at most 1 neighbor")
    AdjacencyConformity,
    AdjacencyViolation2D,
    AdjacencyViolation3D,
    validate_adjacency,
    fix_adjacency,
)

__version__ = "1.0.0"

__all__ = [
    "DP_THREE", "DP_SIX", "DP_NINE",
    "set_decimal_precision", "get_decimal_precision",
    "Point2D", "Vector2D", "Line2D", "Ray2D", "LineSegment2D",
    "Polyline2D", "Triangle2D", "Polygon2D", "BBox2D", "BBall2D", "BRect2D", "GeometryCollection2D",
    "Point3D", "Vector3D", "Line3D", "Ray3D", "LineSegment3D",
    "Polyline3D", "Triangle3D", "Polygon3D", "BBox3D", "BBall3D", "BPrism3D", "Plane", "GeometryCollection3D",
    "WktParser",
    "GridCell2D", "GridCell3D", "Mesh2D", "Mesh3D", "PolyMesh2D", "PolyMesh3D",
    "ConnectedMesh2D", "ConnectedMesh3D", "FaceView2D", "FaceView3D", "TriangleEdge",
    "Axis",
    "are_collinear", "are_coplanar", "closest_world_plane_to", "are_ccw", "are_cw",
    "remove_duplicates", "remove_consecutive_duplicates",
    "remove_collinear", "linear_combination", "average", "lerp", "clip",
    "signed_area", "centroid",
    "has_intersections", "find_intersections", "convex_hull",
    "CoordinateFrame", "principal_axes", "principal_normal", "principal_direction",
    "ExtremePoints2D", "ExtremePoints3D", "find_extreme_points", "distance_to",
    "PolygonTangents2D", "PolygonTangents3D", "tangents_to",
    "ProjectionType", "View2D",
    "PolylineDecimationStrategy", "PolylineDecimationParams", "dist_decimation", "rdp_decimation", "vw_decimation",
    "bezier_smoothing_2",
    "PolylineExpansionMode", "PolylineExpansionParams", "polyline_expansion",
    "TriangulationStrategy", "TriangulationSimplicity", "TriangulationWinding", "TriangulationCollinearity",
    "TriangulationParams", "triangulate",
    "AdjacencyConformity", "AdjacencyViolation2D", "AdjacencyViolation3D", "validate_adjacency", "fix_adjacency",
    "maths", "transformations",
]
