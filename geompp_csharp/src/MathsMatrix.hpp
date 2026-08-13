#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <maths/matrix.hpp>
#pragma managed(pop)

#include "MathsVector.hpp"

namespace GeomPP { namespace Maths {

// Fixed-size square linear-algebra matrices -- geompp::maths::Matrix<double,N,N> (C++). Only the
// square N x N aliases (Matrix2/3/4) are bound; the generic Rows != Cols template stays C++-only,
// same convention as detail:: internals never crossing the language boundary.

public ref class Matrix2 sealed {
public:
    Matrix2();  // zero matrix
    Matrix2(double m00, double m01, double m10, double m11);  // row-major
    ~Matrix2();
    !Matrix2();

    double  Get(int row, int col);
    void    Set(int row, int col, double value);

    Matrix2^ Transpose();
    double   Determinant();
    Matrix2^ Inverse();  // throws if singular

    static Matrix2^ Zero();
    static Matrix2^ Identity();

    static bool     operator==(Matrix2^ lhs, Matrix2^ rhs);
    static Matrix2^ operator+(Matrix2^ lhs, Matrix2^ rhs);
    static Matrix2^ operator-(Matrix2^ lhs, Matrix2^ rhs);
    static Matrix2^ operator-(Matrix2^ m);
    static Matrix2^ operator*(Matrix2^ m, double s);
    static Matrix2^ operator*(double s, Matrix2^ m);
    static Matrix2^ operator/(Matrix2^ m, double s);
    static Matrix2^ operator*(Matrix2^ lhs, Matrix2^ rhs);   // matrix product
    static Vector2^ operator*(Matrix2^ m, Vector2^ v);       // matrix * column vector

    virtual System::String^ ToString() override;

internal:
    Matrix2(geompp::maths::Matrix2* native);
    geompp::maths::Matrix2* _native;
};

public ref class Matrix3 sealed {
public:
    Matrix3();  // zero matrix
    Matrix3(double m00, double m01, double m02,
            double m10, double m11, double m12,
            double m20, double m21, double m22);  // row-major
    ~Matrix3();
    !Matrix3();

    double  Get(int row, int col);
    void    Set(int row, int col, double value);

    Matrix3^ Transpose();
    double   Determinant();
    Matrix3^ Inverse();  // throws if singular

    static Matrix3^ Zero();
    static Matrix3^ Identity();

    // Homogeneous 3x3 affine-transform factories (2D) -- what GeomPP.Transformations' 2D transform()
    // overloads expect.
    static Matrix3^ Translation(Vector2^ offset);
    static Matrix3^ Rotation(double angleRad);
    static Matrix3^ Scale(double factor);
    static Matrix3^ Scale(double sx, double sy);

    static bool     operator==(Matrix3^ lhs, Matrix3^ rhs);
    static Matrix3^ operator+(Matrix3^ lhs, Matrix3^ rhs);
    static Matrix3^ operator-(Matrix3^ lhs, Matrix3^ rhs);
    static Matrix3^ operator-(Matrix3^ m);
    static Matrix3^ operator*(Matrix3^ m, double s);
    static Matrix3^ operator*(double s, Matrix3^ m);
    static Matrix3^ operator/(Matrix3^ m, double s);
    static Matrix3^ operator*(Matrix3^ lhs, Matrix3^ rhs);
    static Vector3^ operator*(Matrix3^ m, Vector3^ v);

    virtual System::String^ ToString() override;

internal:
    Matrix3(geompp::maths::Matrix3* native);
    geompp::maths::Matrix3* _native;
};

public ref class Matrix4 sealed {
public:
    Matrix4();  // zero matrix
    Matrix4(double m00, double m01, double m02, double m03,
            double m10, double m11, double m12, double m13,
            double m20, double m21, double m22, double m23,
            double m30, double m31, double m32, double m33);  // row-major
    ~Matrix4();
    !Matrix4();

    double  Get(int row, int col);
    void    Set(int row, int col, double value);

    Matrix4^ Transpose();
    double   Determinant();
    Matrix4^ Inverse();  // throws if singular

    static Matrix4^ Zero();
    static Matrix4^ Identity();

    // Homogeneous 4x4 affine-transform factories -- what GeomPP.Transformations.TransformBuilder
    // composes via plain Matrix4 multiplication.
    static Matrix4^ Translation(Vector3^ offset);
    static Matrix4^ Rotation(double angleRad, Vector3^ axis);  // throws if axis is zero-length
    static Matrix4^ Scale(double factor);
    static Matrix4^ Scale(double sx, double sy, double sz);

    static bool     operator==(Matrix4^ lhs, Matrix4^ rhs);
    static Matrix4^ operator+(Matrix4^ lhs, Matrix4^ rhs);
    static Matrix4^ operator-(Matrix4^ lhs, Matrix4^ rhs);
    static Matrix4^ operator-(Matrix4^ m);
    static Matrix4^ operator*(Matrix4^ m, double s);
    static Matrix4^ operator*(double s, Matrix4^ m);
    static Matrix4^ operator/(Matrix4^ m, double s);
    static Matrix4^ operator*(Matrix4^ lhs, Matrix4^ rhs);
    static Vector4^ operator*(Matrix4^ m, Vector4^ v);

    virtual System::String^ ToString() override;

internal:
    Matrix4(geompp::maths::Matrix4* native);
    geompp::maths::Matrix4* _native;
};

// Solves the square linear system a*x = b. SolveGauss uses Gauss-Jordan elimination with partial
// pivoting; SolveCramer uses Cramer's rule (a determinant ratio per unknown). Both throw if `a` is
// singular (no unique solution).
public ref class Solvers abstract sealed {
public:
    static Vector2^ SolveGauss(Matrix2^ a, Vector2^ b);
    static Vector3^ SolveGauss(Matrix3^ a, Vector3^ b);
    static Vector4^ SolveGauss(Matrix4^ a, Vector4^ b);

    static Vector2^ SolveCramer(Matrix2^ a, Vector2^ b);
    static Vector3^ SolveCramer(Matrix3^ a, Vector3^ b);
    static Vector4^ SolveCramer(Matrix4^ a, Vector4^ b);
};

} }  // namespace GeomPP::Maths
