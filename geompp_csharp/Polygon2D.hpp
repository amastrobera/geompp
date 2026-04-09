#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "polygon2d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;

public ref class Polygon2D {
public:
    ~Polygon2D();
    !Polygon2D();

    // Factory method (private constructor — use Make)
    static Polygon2D^ Make(array<Point2D^>^ points);

    int Size();
    property Point2D^ default[int] { Point2D^ get(int i); }

    bool AlmostEquals(Polygon2D^ other);
    bool AlmostEquals(Polygon2D^ other, double epsilon);

    System::String^ ToWkt();
    static Polygon2D^ FromWkt(System::String^ wkt);
    void              ToFile(System::String^ path);
    static Polygon2D^ FromFile(System::String^ path);

    // Operator
    static bool operator==(Polygon2D^ lhs, Polygon2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polygon2D(geompp::Polygon2D* native);
    geompp::Polygon2D* _native;
};

}  // namespace GeomPP
