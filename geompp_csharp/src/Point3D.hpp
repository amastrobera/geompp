#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <point3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Vector3D;  // forward declaration — defined in Vector3D.hpp

public ref class Point3D {
public:
    Point3D(double x, double y, double z);
    ~Point3D();
    !Point3D();

    property double X { double get(); }
    property double Y { double get(); }
    property double Z { double get(); }

    Vector3D^       ToVector();
    bool            AlmostEquals(Point3D^ other);
    bool            AlmostEquals(Point3D^ other, double epsilon);
    double          DistanceTo(Point3D^ other);
    System::String^ ToWkt();
    static Point3D^ FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Point3D^ FromFile(System::String^ path);

    static Point3D^ Zero();

    // Operators
    static bool     operator==(Point3D^ lhs, Point3D^ rhs);
    static Point3D^ operator+(Point3D^ lhs, Vector3D^ rhs);   // Point + Vector = Point
    static Vector3D^ operator-(Point3D^ lhs, Point3D^ rhs);   // Point - Point  = Vector
    static Point3D^ operator-(Point3D^ lhs, Vector3D^ rhs);   // Point - Vector = Point
    static Point3D^ operator*(Point3D^ lhs, double a);
    static Point3D^ operator*(double a, Point3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    // Used by Vector3D to wrap values returned by native operations
    Point3D(geompp::Point3D* native);
    geompp::Point3D* _native;
};

}  // namespace GeomPP
