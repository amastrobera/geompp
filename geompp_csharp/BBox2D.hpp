#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "bbox2d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class LineSegment2D;
ref class Polyline2D;
ref class Polygon2D;
ref class Triangle2D;

public ref class BBox2D {
public:
    BBox2D(Point2D^ min, Point2D^ max);
    BBox2D(LineSegment2D^ s);
    BBox2D(Polyline2D^ s);
    BBox2D(Polygon2D^ s);
    BBox2D(Triangle2D^ s);
    ~BBox2D();
    !BBox2D();

    Point2D^ Min();
    Point2D^ Max();

    bool AlmostEquals(BBox2D^ other);
    bool AlmostEquals(BBox2D^ other, double epsilon);
    bool Contains(Point2D^ point);

    // Operator
    static bool operator==(BBox2D^ lhs, BBox2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    BBox2D(geompp::BBox2D* native);
    geompp::BBox2D* _native;
};

}  // namespace GeomPP
