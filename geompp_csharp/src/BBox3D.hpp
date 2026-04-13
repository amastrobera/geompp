#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <bbox3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class LineSegment3D;
ref class Polyline3D;
ref class Polygon3D;
ref class Triangle3D;

public ref class BBox3D {
public:
    BBox3D(Point3D^ min, Point3D^ max);
    BBox3D(LineSegment3D^ s);
    BBox3D(Polyline3D^ s);
    BBox3D(Polygon3D^ s);
    BBox3D(Triangle3D^ s);
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
