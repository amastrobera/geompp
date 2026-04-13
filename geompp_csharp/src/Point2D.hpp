#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <point2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Vector2D;  // forward declaration — defined in Vector2D.hpp

public ref class Point2D {
public:
    Point2D(double x, double y);
    ~Point2D();
    !Point2D();

    property double X { double get(); }
    property double Y { double get(); }

    Vector2D^       ToVector();
    bool            AlmostEquals(Point2D^ other);
    bool            AlmostEquals(Point2D^ other, double epsilon);
    double          DistanceTo(Point2D^ other);
    System::String^ ToWkt();
    static Point2D^ FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Point2D^ FromFile(System::String^ path);

    static Point2D^ Zero();

    // Operators
    static bool     operator==(Point2D^ lhs, Point2D^ rhs);
    static Point2D^ operator+(Point2D^ lhs, Vector2D^ rhs);    // Point + Vector = Point
    static Vector2D^ operator-(Point2D^ lhs, Point2D^ rhs);    // Point - Point  = Vector
    static Point2D^ operator-(Point2D^ lhs, Vector2D^ rhs);    // Point - Vector = Point
    static Point2D^ operator*(Point2D^ lhs, double a);
    static Point2D^ operator*(double a, Point2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Point2D(geompp::Point2D* native);
    geompp::Point2D* _native;
};

}  // namespace GeomPP
