#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <maths/vector.hpp>
#pragma managed(pop)

namespace GeomPP { namespace Maths {

// Fixed-size linear-algebra vectors -- geompp::maths::Vector<double,N> (C++), mirroring
// geompp::maths as a real nested namespace (GeomPP.Maths.Vector2/3/4), not GeomPP.Vector2D/3D
// (a distinct, geometry-flavored vector type -- see maths/vector.hpp's own class docs for why).
// Templates can't cross the CLR boundary, so each size gets its own concrete ref class, same
// reasoning as Matrix2/3/4 below.

public ref class Vector2 sealed {
public:
    Vector2(double x, double y);
    ~Vector2();
    !Vector2();

    property double X { double get(); }
    property double Y { double get(); }
    property double default[int] { double get(int i); }

    double  Dot(Vector2^ other);
    double  Length();
    double  LengthSquared();
    Vector2^ Normalized();  // throws if zero-length

    static Vector2^ Zero();

    static bool     operator==(Vector2^ lhs, Vector2^ rhs);
    static Vector2^ operator+(Vector2^ lhs, Vector2^ rhs);
    static Vector2^ operator-(Vector2^ lhs, Vector2^ rhs);
    static Vector2^ operator-(Vector2^ v);
    static Vector2^ operator*(Vector2^ v, double s);
    static Vector2^ operator*(double s, Vector2^ v);
    static Vector2^ operator/(Vector2^ v, double s);

    virtual System::String^ ToString() override;

internal:
    Vector2(geompp::maths::Vector2* native);
    geompp::maths::Vector2* _native;
};

public ref class Vector3 sealed {
public:
    Vector3(double x, double y, double z);
    ~Vector3();
    !Vector3();

    property double X { double get(); }
    property double Y { double get(); }
    property double Z { double get(); }
    property double default[int] { double get(int i); }

    double  Dot(Vector3^ other);
    Vector3^ Cross(Vector3^ other);
    double  Length();
    double  LengthSquared();
    Vector3^ Normalized();  // throws if zero-length

    static Vector3^ Zero();

    static bool     operator==(Vector3^ lhs, Vector3^ rhs);
    static Vector3^ operator+(Vector3^ lhs, Vector3^ rhs);
    static Vector3^ operator-(Vector3^ lhs, Vector3^ rhs);
    static Vector3^ operator-(Vector3^ v);
    static Vector3^ operator*(Vector3^ v, double s);
    static Vector3^ operator*(double s, Vector3^ v);
    static Vector3^ operator/(Vector3^ v, double s);

    virtual System::String^ ToString() override;

internal:
    Vector3(geompp::maths::Vector3* native);
    geompp::maths::Vector3* _native;
};

public ref class Vector4 sealed {
public:
    Vector4(double x, double y, double z, double w);
    ~Vector4();
    !Vector4();

    property double X { double get(); }
    property double Y { double get(); }
    property double Z { double get(); }
    property double W { double get(); }
    property double default[int] { double get(int i); }

    double  Dot(Vector4^ other);
    double  Length();
    double  LengthSquared();
    Vector4^ Normalized();  // throws if zero-length

    static Vector4^ Zero();

    static bool     operator==(Vector4^ lhs, Vector4^ rhs);
    static Vector4^ operator+(Vector4^ lhs, Vector4^ rhs);
    static Vector4^ operator-(Vector4^ lhs, Vector4^ rhs);
    static Vector4^ operator-(Vector4^ v);
    static Vector4^ operator*(Vector4^ v, double s);
    static Vector4^ operator*(double s, Vector4^ v);
    static Vector4^ operator/(Vector4^ v, double s);

    virtual System::String^ ToString() override;

internal:
    Vector4(geompp::maths::Vector4* native);
    geompp::maths::Vector4* _native;
};

} }  // namespace GeomPP::Maths
