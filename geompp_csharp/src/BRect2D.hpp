#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <brect2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Vector2D;

public ref class BRect2D {
public:
    BRect2D(array<Point2D^>^ points);
    ~BRect2D();
    !BRect2D();

    Point2D^  Center();
    Vector2D^ AxisU();
    Vector2D^ AxisV();
    double    HalfLenU();
    double    HalfLenV();
    double    Width();
    double    Height();
    double    Area();

    array<Point2D^>^ Corners();

    bool AlmostEquals(BRect2D^ other);
    bool AlmostEquals(BRect2D^ other, double epsilon);
    bool Contains(Point2D^ point);

    // Operator
    static bool operator==(BRect2D^ lhs, BRect2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    BRect2D(geompp::BRect2D* native);
    geompp::BRect2D* _native;
};

}  // namespace GeomPP
