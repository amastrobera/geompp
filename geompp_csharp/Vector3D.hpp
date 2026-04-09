#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <vector3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;  // forward declaration — defined in Point3D.hpp

public ref class Vector3D {
public:
    Vector3D(double x, double y, double z);
    ~Vector3D();
    !Vector3D();

    property double X { double get(); }
    property double Y { double get(); }
    property double Z { double get(); }

    double        Length();
    bool          AlmostEquals(Vector3D^ other);
    bool          AlmostEquals(Vector3D^ other, double epsilon);
    System::String^ ToWkt();
    static Vector3D^ FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Vector3D^ FromFile(System::String^ path);

    double    Dot(Vector3D^ other);
    Vector3D^ Cross(Vector3D^ other);
    Vector3D^ Perp();
    Vector3D^ Normalize();
    bool      IsParallel(Vector3D^ other);

    Point3D^  ToPoint();

    static Vector3D^ BasisX();
    static Vector3D^ BasisY();
    static Vector3D^ BasisZ();

    // Operators
    static bool      operator==(Vector3D^ lhs, Vector3D^ rhs);
    static Vector3D^ operator+(Vector3D^ lhs, Vector3D^ rhs);
    static Vector3D^ operator-(Vector3D^ lhs, Vector3D^ rhs);
    static Vector3D^ operator*(Vector3D^ lhs, double a);
    static Vector3D^ operator*(double a, Vector3D^ rhs);
    static double    operator*(Vector3D^ lhs, Vector3D^ rhs);   // dot product
    static Vector3D^ operator/(Vector3D^ lhs, double a);
    static Vector3D^ operator-(Vector3D^ v);                    // unary negate

    virtual System::String^ ToString() override;

internal:
    // Used by Point3D to wrap values returned by native operations
    Vector3D(geompp::Vector3D* native);
    geompp::Vector3D* _native;
};

}  // namespace GeomPP
