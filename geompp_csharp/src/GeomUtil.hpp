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
};

}  // namespace GeomPP
