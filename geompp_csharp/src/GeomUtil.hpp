#pragma once

#pragma managed(push, off)
#include <plane.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class Plane;
ref class LineSegment2D;
ref class IntersectionEvent2D;

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
    // HasIntersections — Shamos–Hoey: true if any two of the (closed-ring) segments intersect.
    static bool HasIntersections(System::Collections::Generic::List<LineSegment2D^>^ segments);
    // FindIntersections — Bentley–Ottmann: every intersection found among the segments.
    static System::Collections::Generic::IEnumerable<IntersectionEvent2D^>^ FindIntersections(
        System::Collections::Generic::List<LineSegment2D^>^ segments);
};

}  // namespace GeomPP
