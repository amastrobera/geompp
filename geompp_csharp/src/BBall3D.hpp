#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <bball3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;

public ref class BBall3D {
public:
    BBall3D(Point3D^ center, double radius);
    BBall3D(array<Point3D^>^ points);
    ~BBall3D();
    !BBall3D();

    Point3D^ Center();
    double   Radius();

    bool AlmostEquals(BBall3D^ other);
    bool AlmostEquals(BBall3D^ other, double epsilon);
    bool Contains(Point3D^ point);

    // Operator
    static bool operator==(BBall3D^ lhs, BBall3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    BBall3D(geompp::BBall3D* native);
    geompp::BBall3D* _native;
};

}  // namespace GeomPP
