#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <bprism3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class Vector3D;

public ref class BPrism3D {
public:
    BPrism3D(array<Point3D^>^ points);
    ~BPrism3D();
    !BPrism3D();

    Point3D^  Center();
    Vector3D^ AxisU();
    Vector3D^ AxisV();
    Vector3D^ AxisW();
    double    HalfLenU();
    double    HalfLenV();
    double    HalfLenW();
    double    Width();
    double    Height();
    double    Depth();
    double    Volume();

    array<Point3D^>^ Corners();

    bool AlmostEquals(BPrism3D^ other);
    bool AlmostEquals(BPrism3D^ other, double epsilon);
    bool Contains(Point3D^ point);

    // Operator
    static bool operator==(BPrism3D^ lhs, BPrism3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    BPrism3D(geompp::BPrism3D* native);
    geompp::BPrism3D* _native;
};

}  // namespace GeomPP
