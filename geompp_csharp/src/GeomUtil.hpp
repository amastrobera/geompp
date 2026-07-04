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
};

}  // namespace GeomPP
