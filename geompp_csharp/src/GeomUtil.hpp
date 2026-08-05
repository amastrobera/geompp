#pragma once

#pragma managed(push, off)
#include <plane.hpp>
#include <calc_utils3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Point3D;
ref class Plane;
ref class LineSegment2D;
ref class LineSegment3D;
ref class CoordinateFrame;
ref class Vector3D;
ref class Polygon2D;
ref class Polygon3D;
ref class Line2D;
ref class Line3D;
ref class PolylineExpansionParams;
ref class Triangle2D;
ref class Triangle3D;

// The two vertices of a 2D shape extreme (least / greatest projection) along a direction.
public ref class ExtremePoints2D sealed {
public:
    property Point2D^ MinPoint { Point2D^ get() { return _min; } }
    property Point2D^ MaxPoint { Point2D^ get() { return _max; } }
internal:
    ExtremePoints2D(Point2D^ mn, Point2D^ mx) : _min(mn), _max(mx) {}
private:
    Point2D^ _min;
    Point2D^ _max;
};

// The two vertices of a 3D shape extreme (least / greatest projection) along a direction.
public ref class ExtremePoints3D sealed {
public:
    property Point3D^ MinPoint { Point3D^ get() { return _min; } }
    property Point3D^ MaxPoint { Point3D^ get() { return _max; } }
internal:
    ExtremePoints3D(Point3D^ mn, Point3D^ mx) : _min(mn), _max(mx) {}
private:
    Point3D^ _min;
    Point3D^ _max;
};

// The left and right tangent segments from a point or polygon to a 2D polygon.
public ref class PolygonTangents2D sealed {
public:
    property LineSegment2D^ Left { LineSegment2D^ get() { return _left; } }
    property LineSegment2D^ Right { LineSegment2D^ get() { return _right; } }
internal:
    PolygonTangents2D(LineSegment2D^ l, LineSegment2D^ r) : _left(l), _right(r) {}
private:
    LineSegment2D^ _left;
    LineSegment2D^ _right;
};

// The left and right tangent segments from a point or polygon to a 3D polygon.
public ref class PolygonTangents3D sealed {
public:
    property LineSegment3D^ Left { LineSegment3D^ get() { return _left; } }
    property LineSegment3D^ Right { LineSegment3D^ get() { return _right; } }
internal:
    PolygonTangents3D(LineSegment3D^ l, LineSegment3D^ r) : _left(l), _right(r) {}
private:
    LineSegment3D^ _left;
    LineSegment3D^ _right;
};

// Which triangulation algorithm to run — see TriangulationParams. Values must stay in the same order
// as geompp::TriangulationParams::Strategy: ToNative() converts via a raw static_cast by ordinal, not
// by name, so inserting or reordering a value here without matching the C++ enum silently corrupts
// every other value after it.
public enum class TriangulationStrategy {
    // Clips the first valid ear found in scan order. O(n^2) worst case, but often close to O(n) in
    // practice. Doesn't optimize triangle shape, so it can produce a visually thin sliver purely from
    // scan order, even on ordinary input.
    EarClipping = 0,
    // Clips the best-scoring (least sliver-prone) valid ear every step instead of the first one found.
    // Same termination guarantee as EarClipping, but unconditionally ~O(n^2) -- a full rescan of the
    // current ring on every single clip, not just worst case. Default.
    EarClippingBestFit = 1,
    // O(n log n) worst case; requires a monotone polygon (or a decomposition into monotone pieces).
    // Not yet implemented.
    MonotonePolygon = 2,
    // O(n log n) worst case; maximizes the minimum angle across all triangles (avoids skinny slivers).
    // Not yet implemented.
    Delaunay = 3
};

// How GeomUtil.Triangulate() handles a possibly self-intersecting input ring.
public enum class TriangulationSimplicity {
    // No check is carried out (runs at your own risk).
    Guaranteed = 0,
    // Throws if the input isn't simple.
    Assert = 1,
    // Decomposes non-simple input into simple pieces (via simplify_rings) before triangulating.
    Enforce = 2
};

// How GeomUtil.Triangulate() handles input that may not be wound counter-clockwise (CCW).
public enum class TriangulationWinding {
    // No check is carried out (runs at your own risk).
    Guaranteed = 0,
    // Throws if the input isn't CCW.
    Assert = 1,
    // Reverses the input if it's CW, before triangulating.
    Enforce = 2
};

// How GeomUtil.Triangulate() handles collinear points. A duplicate consecutive point is just the
// degenerate case of three collinear points, so this covers both.
public enum class TriangulationCollinearity {
    // No check is carried out (runs at your own risk).
    Guaranteed = 0,
    // Throws if the input has collinear (or duplicate) points.
    Assert = 1,
    // Removes collinear/duplicate points before triangulating.
    Enforce = 2
};

// Bundles the triangulation strategy and how to handle non-simple / non-CCW / collinear input for
// GeomUtil.Triangulate(). Defaults to EarClipping, and Enforce for all three input-quality checks —
// matching the native triangulate()'s own defaults. Polygon2D.Triangulate() / Polygon3D.Triangulate() /
// PolyMesh2D.Triangulate() / PolyMesh3D.Triangulate() take just a TriangulationStrategy instead: their
// input is already guaranteed simple/CCW/collinear-free by construction, so the other three checks
// aren't exposed there.
public ref class TriangulationParams {
public:
    TriangulationParams();
    TriangulationParams(TriangulationStrategy strategy, TriangulationSimplicity simplicity,
                        TriangulationWinding ccwWinding, TriangulationCollinearity collinearity);

    property TriangulationStrategy Strategy {
        TriangulationStrategy get() { return _strategy; }
        void set(TriangulationStrategy value) { _strategy = value; }
    }
    property TriangulationSimplicity Simplicity {
        TriangulationSimplicity get() { return _simplicity; }
        void set(TriangulationSimplicity value) { _simplicity = value; }
    }
    property TriangulationWinding CcwWinding {
        TriangulationWinding get() { return _ccwWinding; }
        void set(TriangulationWinding value) { _ccwWinding = value; }
    }
    property TriangulationCollinearity Collinearity {
        TriangulationCollinearity get() { return _collinearity; }
        void set(TriangulationCollinearity value) { _collinearity = value; }
    }

internal:
    geompp::TriangulationParams ToNative();

private:
    TriangulationStrategy _strategy;
    TriangulationSimplicity _simplicity;
    TriangulationWinding _ccwWinding;
    TriangulationCollinearity _collinearity;
};

// How to handle a batch of facets that violate "every edge has at most 1 neighbor" -- no facet vertex
// may lie in the interior of another facet's edge, only exactly at that edge's own start/end vertex.
// Known elsewhere as: no "hanging nodes" (FEM), no "T-junctions" (graphics), a valid PSLG (mesh
// generation). GeomUtil.ValidateAdjacency() / FixAdjacency() do the actual checking/repair;
// Mesh2D/3D.FromTriangles, PolyMesh2D/3D.FromPolygons, and ConnectedMesh2D/3D.FromTriangles always
// Assert this at construction time. Values must stay in the same order as geompp::AdjacencyConformity
// (converted via a raw static_cast by ordinal, same reasoning as TriangulationStrategy above).
public enum class AdjacencyConformity {
    // No check is carried out (runs at your own risk).
    Guaranteed = 0,
    // Throws if any violation (T-junction or non-manifold edge) is found.
    Assert = 1,
    // Auto-repairs every T-junction via FixAdjacency(); still throws on a non-manifold edge (a full
    // edge shared by 3+ facets) -- there's no principled automatic fix for that one.
    Enforce = 2
};

// One "more than 1 neighbor" violation found by GeomUtil.ValidateAdjacency() across a batch of 2D
// facets. A T-junction (a vertex partially overlapping an edge) is fixable -- see OnVertex; a
// non-manifold edge (a full edge shared by 3+ facets) is not, since there's no principled way to pick
// which 2 of the 3+ facets are "the real pair".
public ref class AdjacencyViolation2D sealed {
public:
    property Point2D^ EdgeP0 { Point2D^ get() { return _edgeP0; } }
    property Point2D^ EdgeP1 { Point2D^ get() { return _edgeP1; } }
    property System::Collections::Generic::IEnumerable<int>^ FacetIndices { System::Collections::Generic::IEnumerable<int>^ get() { return _facetIndices; } }
    property bool IsNonManifold { bool get() { return _isNonManifold; } }
    property Point2D^ OnVertex { Point2D^ get() { return _onVertex; } }
internal:
    AdjacencyViolation2D(Point2D^ edgeP0, Point2D^ edgeP1, System::Collections::Generic::List<int>^ facetIndices,
                         bool isNonManifold, Point2D^ onVertex)
        : _edgeP0(edgeP0), _edgeP1(edgeP1), _facetIndices(facetIndices), _isNonManifold(isNonManifold), _onVertex(onVertex) {}
private:
    Point2D^ _edgeP0;
    Point2D^ _edgeP1;
    System::Collections::Generic::List<int>^ _facetIndices;
    bool _isNonManifold;
    Point2D^ _onVertex;
};

// 3D counterpart of AdjacencyViolation2D -- same fields, operating on Point3D. Native 3D
// collinearity/betweenness, not View2D-projected (a shared 2D projection would be wrong for a general
// 3D mesh whose facets aren't all coplanar).
public ref class AdjacencyViolation3D sealed {
public:
    property Point3D^ EdgeP0 { Point3D^ get() { return _edgeP0; } }
    property Point3D^ EdgeP1 { Point3D^ get() { return _edgeP1; } }
    property System::Collections::Generic::IEnumerable<int>^ FacetIndices { System::Collections::Generic::IEnumerable<int>^ get() { return _facetIndices; } }
    property bool IsNonManifold { bool get() { return _isNonManifold; } }
    property Point3D^ OnVertex { Point3D^ get() { return _onVertex; } }
internal:
    AdjacencyViolation3D(Point3D^ edgeP0, Point3D^ edgeP1, System::Collections::Generic::List<int>^ facetIndices,
                         bool isNonManifold, Point3D^ onVertex)
        : _edgeP0(edgeP0), _edgeP1(edgeP1), _facetIndices(facetIndices), _isNonManifold(isNonManifold), _onVertex(onVertex) {}
private:
    Point3D^ _edgeP0;
    Point3D^ _edgeP1;
    System::Collections::Generic::List<int>^ _facetIndices;
    bool _isNonManifold;
    Point3D^ _onVertex;
};

// Static utility class — wraps the geompp free functions that operate on point collections.
// Consumed directly or via the Geompp.Extensions extension methods.
public ref class GeomUtil abstract sealed {
public:
    static bool   AreCoplanar(System::Collections::Generic::List<Point3D^>^ points);
    static Plane^ ClosestWorldPlaneTo(System::Collections::Generic::List<Point3D^>^ points);

    // refPlane == nullptr → auto-detect from the points (closest world axis)
    static bool AreCCW(System::Collections::Generic::List<Point3D^>^ points, Plane^ refPlane);
    static bool AreCW (System::Collections::Generic::List<Point3D^>^ points, Plane^ refPlane);

    // Lerp — linear interpolation between two points: p0 + t * (p1 - p0). Not clamped — t outside
    // [0, 1] extrapolates past p0/p1.
    static Point2D^ Lerp(Point2D^ p0, Point2D^ p1, double t);
    static Point3D^ Lerp(Point3D^ p0, Point3D^ p1, double t);

    // Clip — set intersection of clipperLoop and subjectLoop (raw point loops, no holes, last point
    // != first — implicitly closed). Returns every ring of the result (CCW outers and CW holes mixed
    // in one flat list — even hole-less input can produce a holed intersection). The Point3D overload
    // requires both loops to be coplanar (plane fitted from subjectLoop's first three points); throws
    // otherwise.
    static System::Collections::Generic::IEnumerable<System::Collections::Generic::List<Point2D^>^>^ Clip(
        System::Collections::Generic::List<Point2D^>^ clipperLoop,
        System::Collections::Generic::List<Point2D^>^ subjectLoop);
    static System::Collections::Generic::IEnumerable<System::Collections::Generic::List<Point3D^>^>^ Clip(
        System::Collections::Generic::List<Point3D^>^ clipperLoop,
        System::Collections::Generic::List<Point3D^>^ subjectLoop);

    // Segment-set intersection.
    // HasIntersections — Shamos–Hoey: true if any two of the segments intersect.
    static bool HasIntersections(System::Collections::Generic::List<LineSegment2D^>^ segments);
    // FindIntersections — Bentley–Ottmann: every crossing point among the segments.
    static System::Collections::Generic::IEnumerable<Point2D^>^ FindIntersections(
        System::Collections::Generic::List<LineSegment2D^>^ segments);

    // ConvexHull — Andrew's monotone chain: convex hull of a 2D point cloud, returned in CCW order.
    static System::Collections::Generic::IEnumerable<Point2D^>^ ConvexHull(
        System::Collections::Generic::List<Point2D^>^ points);

    // ConvexHull — Andrew's monotone chain: convex hull of 3D points, returned in CCW order.
    // Points do not need to be coplanar — the best-fit plane is estimated via PCA.
    static System::Collections::Generic::IEnumerable<Point3D^>^ ConvexHull(
        System::Collections::Generic::List<Point3D^>^ points);

    // Lower-level building blocks behind Polyline2D.Reduce() / Polyline3D.Reduce() — operate
    // directly on a list of points instead of a Polyline object.
    //
    // DistDecimation — O(n) radial-distance decimation: drop a point if it's closer than
    // threshold to the last kept point.
    static System::Collections::Generic::IEnumerable<Point2D^>^ DistDecimation(
        System::Collections::Generic::List<Point2D^>^ points, double threshold);
    static System::Collections::Generic::IEnumerable<Point3D^>^ DistDecimation(
        System::Collections::Generic::List<Point3D^>^ points, double threshold);

    // RdpDecimation — Ramer-Douglas-Peucker: recursively drop points closer than threshold to
    // the chord spanning their segment.
    static System::Collections::Generic::IEnumerable<Point2D^>^ RdpDecimation(
        System::Collections::Generic::List<Point2D^>^ points, double threshold);
    static System::Collections::Generic::IEnumerable<Point3D^>^ RdpDecimation(
        System::Collections::Generic::List<Point3D^>^ points, double threshold);

    // VwDecimation — Visvalingam-Whyatt: repeatedly drop the point forming the smallest-area
    // triangle with its neighbors, while that area stays below threshold.
    static System::Collections::Generic::IEnumerable<Point2D^>^ VwDecimation(
        System::Collections::Generic::List<Point2D^>^ points, double threshold);
    static System::Collections::Generic::IEnumerable<Point3D^>^ VwDecimation(
        System::Collections::Generic::List<Point3D^>^ points, double threshold);

    // BezierSmoothing2 — rounds the corner at p1 with a quadratic Bezier arc tangent to p0-p1 and
    // p1-p2. smoothness in [0,1] controls how much of the shorter adjacent edge is trimmed into
    // the tangent points. Two overloads: one samples roughly minDistance apart, the other takes
    // an exact numSegments (+1 points in the result either way). The 4-arg forms use the native
    // default (DOUBLE_EPSILON) for minSegmentLength; the 6-arg forms let the caller override it —
    // an adjacent edge at or below it isn't trimmed into (both at or below: the corner stays sharp).
    static System::Collections::Generic::IEnumerable<Point2D^>^ BezierSmoothing2(
        Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, double minDistance);
    static System::Collections::Generic::IEnumerable<Point3D^>^ BezierSmoothing2(
        Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, double minDistance);
    static System::Collections::Generic::IEnumerable<Point2D^>^ BezierSmoothing2(
        Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, int numSegments);
    static System::Collections::Generic::IEnumerable<Point3D^>^ BezierSmoothing2(
        Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, int numSegments);
    static System::Collections::Generic::IEnumerable<Point2D^>^ BezierSmoothing2(
        Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, double minDistance, double minSegmentLength);
    static System::Collections::Generic::IEnumerable<Point3D^>^ BezierSmoothing2(
        Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, double minDistance, double minSegmentLength);
    static System::Collections::Generic::IEnumerable<Point2D^>^ BezierSmoothing2(
        Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, int numSegments, double minSegmentLength);
    static System::Collections::Generic::IEnumerable<Point3D^>^ BezierSmoothing2(
        Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, int numSegments, double minSegmentLength);

    // PolylineExpansion — the lower-level building block behind Polyline2D.Expand() / Polyline3D.Expand():
    // rounds every inner corner of a raw point list with a quadratic Bezier arc, per the given
    // PolylineExpansionParams. Call directly when you want to round a point list without constructing
    // a Polyline first.
    static System::Collections::Generic::IEnumerable<Point2D^>^ PolylineExpansion(
        System::Collections::Generic::List<Point2D^>^ points, PolylineExpansionParams^ settings);
    static System::Collections::Generic::IEnumerable<Point3D^>^ PolylineExpansion(
        System::Collections::Generic::List<Point3D^>^ points, PolylineExpansionParams^ settings);

    // PCA — principal axes of a 3D point cloud via Jacobi eigendecomposition.
    static CoordinateFrame^ PrincipalAxes(System::Collections::Generic::List<Point3D^>^ points);
    static Vector3D^        PrincipalNormal(System::Collections::Generic::List<Point3D^>^ points);
    static Vector3D^        PrincipalDirection(System::Collections::Generic::List<Point3D^>^ points);

    // FindExtremePoints — the polygon's outer-ring vertices extreme (least / greatest projection) along the
    // line's direction. Daniel Sunday's O(log n) binary search when convex, else O(n). Holes are ignored.
    static ExtremePoints2D^ FindExtremePoints(Polygon2D^ polygon, Line2D^ line);
    static ExtremePoints3D^ FindExtremePoints(Polygon3D^ polygon, Line3D^ line);

    // DistanceTo — distance between the polygon and an infinite line (zero if they cross). Holes are
    // ignored. The 3D overload handles coplanar, parallel-offset, and skew lines.
    static double DistanceTo(Polygon2D^ polygon, Line2D^ line);
    static double DistanceTo(Polygon3D^ polygon, Line3D^ line);

    // TangentsTo — left/right tangent segments from a point to a polygon, or the two common outer
    // tangent segments between two polygons. Uses Daniel Sunday's O(log n) binary search when convex,
    // else reduces to the convex hull first. The 3D overloads require coplanar inputs (point in the
    // polygon's plane, or both polygons sharing a plane) and throw otherwise.
    static PolygonTangents2D^ TangentsTo(Polygon2D^ polygon, Point2D^ point);
    static PolygonTangents2D^ TangentsTo(Polygon2D^ polygon, Polygon2D^ other);
    static PolygonTangents3D^ TangentsTo(Polygon3D^ polygon, Point3D^ point);
    static PolygonTangents3D^ TangentsTo(Polygon3D^ polygon, Polygon3D^ other);

    // Triangulate — breaks a simple polygon's outer loop (no holes) down into triangles, per the given
    // TriangulationParams. 2D is native; the 3D overloads assume flat/coplanar input, projected via the
    // given plane normal, or (when normal is omitted) one fitted via PCA (PrincipalNormal).
    static System::Collections::Generic::IEnumerable<Triangle2D^>^ Triangulate(
        System::Collections::Generic::List<Point2D^>^ points, TriangulationParams^ settings);
    static System::Collections::Generic::IEnumerable<Triangle3D^>^ Triangulate(
        System::Collections::Generic::List<Point3D^>^ points, Vector3D^ normal, TriangulationParams^ settings);
    static System::Collections::Generic::IEnumerable<Triangle3D^>^ Triangulate(
        System::Collections::Generic::List<Point3D^>^ points, TriangulationParams^ settings);

    // ValidateAdjacency — checks a batch of facets for "every edge has at most 1 neighbor". Returns
    // every violation found (both T-junctions and non-manifold edges); empty if conforming.
    static System::Collections::Generic::IEnumerable<AdjacencyViolation2D^>^ ValidateAdjacency(array<Polygon2D^>^ facets);
    static System::Collections::Generic::IEnumerable<AdjacencyViolation2D^>^ ValidateAdjacency(array<Triangle2D^>^ facets);
    static System::Collections::Generic::IEnumerable<AdjacencyViolation3D^>^ ValidateAdjacency(array<Polygon3D^>^ facets);
    static System::Collections::Generic::IEnumerable<AdjacencyViolation3D^>^ ValidateAdjacency(array<Triangle3D^>^ facets);

    // FixAdjacency — repairs every T-junction ValidateAdjacency() would report, by splicing the
    // foreign vertex into the coarse edge's facet. Throws on a non-manifold edge (not fixable). Returns
    // raw point rings, NOT Polygon2D/3D -- Polygon2D/3D.Make() unconditionally strips collinear points,
    // which would undo the splice.
    static array<array<Point2D^>^>^ FixAdjacency(array<Polygon2D^>^ facets);
    static array<array<Point3D^>^>^ FixAdjacency(array<Polygon3D^>^ facets);

    // FixAdjacency (Triangle overload) — unlike a Polygon2D/3D facet, a triangle can't just absorb a
    // spliced-in vertex and stay a triangle, so a repaired facet is re-triangulated into 2+ triangles
    // covering the same area as the original one. An unaffected facet passes through unchanged. Throws
    // on a non-manifold edge (not fixable).
    static array<Triangle2D^>^ FixAdjacency(array<Triangle2D^>^ facets);
    static array<Triangle3D^>^ FixAdjacency(array<Triangle3D^>^ facets);

    // Triangulate (batch) — batch-triangulates a set of 2D polygon facets together, the free-function
    // equivalent of PolyMesh2D.FromPolygons(polygons).Triangulate(). Unlike PolyMesh2D.FromPolygons
    // (which always throws on bad adjacency), conformity is typically Enforce: auto-repairs a
    // T-junction, still throws on a non-manifold edge.
    static System::Collections::Generic::IEnumerable<Triangle2D^>^ Triangulate(
        array<Polygon2D^>^ polygons, AdjacencyConformity conformity, TriangulationParams^ settings);
};

}  // namespace GeomPP
