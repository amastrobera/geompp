#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <calc_utils2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;

// Managed wrapper around geompp::IntersectionEvent2D — an intersection found among a set of segments:
// the crossing point and the indices of the two segments involved.
public ref class IntersectionEvent2D {
public:
    ~IntersectionEvent2D();
    !IntersectionEvent2D();

    property Point2D^ Point   { Point2D^ get(); }
    property int      SegmentId1 { int get(); }
    property int      SegmentId2 { int get(); }

    virtual System::String^ ToString() override;

internal:
    IntersectionEvent2D(geompp::IntersectionEvent2D* native);
    geompp::IntersectionEvent2D* _native;
};

}  // namespace GeomPP
