#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "vector2d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;  // forward declaration — defined in Point2D.hpp

public ref class Vector2D {
public:
    Vector2D(double x, double y);
    ~Vector2D();
    !Vector2D();

    property double X { double get(); }
    property double Y { double get(); }

    double        Length();
    bool          AlmostEquals(Vector2D^ other);
    bool          AlmostEquals(Vector2D^ other, double epsilon);
    System::String^ ToWkt();
    static Vector2D^ FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Vector2D^ FromFile(System::String^ path);

    double    Dot(Vector2D^ other);
    double    Cross(Vector2D^ other);
    Vector2D^ Perp();
    Vector2D^ Normalize();

    Point2D^  ToPoint();

    static Vector2D^ BasisX();
    static Vector2D^ BasisY();

    // Operators
    static bool      operator==(Vector2D^ lhs, Vector2D^ rhs);
    static Vector2D^ operator+(Vector2D^ lhs, Vector2D^ rhs);
    static Point2D^  operator+(Vector2D^ lhs, Point2D^ rhs);   // Vector + Point = Point
    static Vector2D^ operator-(Vector2D^ lhs, Vector2D^ rhs);
    static Vector2D^ operator*(Vector2D^ lhs, double a);
    static Vector2D^ operator*(double a, Vector2D^ rhs);
    static double    operator*(Vector2D^ lhs, Vector2D^ rhs);  // dot product
    static Vector2D^ operator/(Vector2D^ lhs, double a);
    static Vector2D^ operator-(Vector2D^ v);                   // unary negate

    virtual System::String^ ToString() override;

internal:
    Vector2D(geompp::Vector2D* native);
    geompp::Vector2D* _native;
};

}  // namespace GeomPP
