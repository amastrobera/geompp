#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "bbox3d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;

public ref class BBox3D {
public:
    BBox3D(Point3D^ min, Point3D^ max);
    ~BBox3D();
    !BBox3D();

    Point3D^ Min();
    Point3D^ Max();

    bool AlmostEquals(BBox3D^ other);
    bool AlmostEquals(BBox3D^ other, double epsilon);
    bool Contains(Point3D^ point);

    // Operator
    static bool operator==(BBox3D^ lhs, BBox3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    BBox3D(geompp::BBox3D* native);
    geompp::BBox3D* _native;
};

}  // namespace GeomPP
