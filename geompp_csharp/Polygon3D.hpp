#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "polygon3d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;

public ref class Polygon3D {
public:
    ~Polygon3D();
    !Polygon3D();

    // Factory method (private constructor — use Make)
    static Polygon3D^ Make(array<Point3D^>^ points);

    int Size();
    property Point3D^ default[int] { Point3D^ get(int i); }

    bool AlmostEquals(Polygon3D^ other);
    bool AlmostEquals(Polygon3D^ other, double epsilon);

    System::String^ ToWkt();
    static Polygon3D^ FromWkt(System::String^ wkt);
    void              ToFile(System::String^ path);
    static Polygon3D^ FromFile(System::String^ path);

    // Operator
    static bool operator==(Polygon3D^ lhs, Polygon3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polygon3D(geompp::Polygon3D* native);
    geompp::Polygon3D* _native;
};

}  // namespace GeomPP
