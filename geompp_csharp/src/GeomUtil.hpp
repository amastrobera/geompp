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
ref class CoordinateFrame;
ref class Vector3D;
ref class Polygon2D;
ref class Polygon3D;
ref class Line2D;
ref class Line3D;

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

// Static utility class — wraps the geompp free functions that operate on point collections.
// Consumed directly or via the Geompp.Extensions extension methods.
public ref class GeomUtil abstract sealed {
public:
    static bool   AreCoplanar(System::Collections::Generic::List<Point3D^>^ points);
    static Plane^ ClosestWorldPlaneTo(System::Collections::Generic::List<Point3D^>^ points);

    // refPlane == nullptr → auto-detect from the points (closest world axis)
    static bool AreCCW(System::Collections::Generic::List<Point3D^>^ points, Plane^ refPlane);
    static bool AreCW (System::Collections::Generic::List<Point3D^>^ points, Plane^ refPlane);

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
};

}  // namespace GeomPP
