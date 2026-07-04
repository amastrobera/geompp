#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <bball2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;

public ref class BBall2D {
public:
    BBall2D(Point2D^ center, double radius);
    BBall2D(array<Point2D^>^ points);
    ~BBall2D();
    !BBall2D();

    Point2D^ Center();
    double   Radius();

    bool AlmostEquals(BBall2D^ other);
    bool AlmostEquals(BBall2D^ other, double epsilon);
    bool Contains(Point2D^ point);

    // Operator
    static bool operator==(BBall2D^ lhs, BBall2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    BBall2D(geompp::BBall2D* native);
    geompp::BBall2D* _native;
};

}  // namespace GeomPP
