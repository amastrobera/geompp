#include "MathsMatrix.hpp"

#include <maths/solvers.hpp>

namespace GeomPP { namespace Maths {

// ── Matrix2 ──────────────────────────────────────────────────────────────────

Matrix2::Matrix2() : _native(new geompp::maths::Matrix2()) {}
Matrix2::Matrix2(double m00, double m01, double m10, double m11)
    : _native(new geompp::maths::Matrix2(m00, m01, m10, m11)) {}
Matrix2::Matrix2(geompp::maths::Matrix2* native) : _native(native) {}
Matrix2::~Matrix2() { delete _native; _native = nullptr; }
Matrix2::!Matrix2() { delete _native; _native = nullptr; }

double Matrix2::Get(int row, int col) { return _native->At(static_cast<std::size_t>(row), static_cast<std::size_t>(col)); }
void   Matrix2::Set(int row, int col, double value) { _native->At(static_cast<std::size_t>(row), static_cast<std::size_t>(col)) = value; }

Matrix2^ Matrix2::Transpose() { return gcnew Matrix2(new geompp::maths::Matrix2(_native->Transpose())); }
double   Matrix2::Determinant() { return _native->Determinant(); }
Matrix2^ Matrix2::Inverse() { return gcnew Matrix2(new geompp::maths::Matrix2(_native->Inverse())); }

Matrix2^ Matrix2::Zero() { return gcnew Matrix2(new geompp::maths::Matrix2(geompp::maths::Matrix2::Zero())); }
Matrix2^ Matrix2::Identity() { return gcnew Matrix2(new geompp::maths::Matrix2(geompp::maths::Matrix2::Identity())); }

bool     Matrix2::operator==(Matrix2^ lhs, Matrix2^ rhs) { return *lhs->_native == *rhs->_native; }
Matrix2^ Matrix2::operator+(Matrix2^ lhs, Matrix2^ rhs) { return gcnew Matrix2(new geompp::maths::Matrix2(*lhs->_native + *rhs->_native)); }
Matrix2^ Matrix2::operator-(Matrix2^ lhs, Matrix2^ rhs) { return gcnew Matrix2(new geompp::maths::Matrix2(*lhs->_native - *rhs->_native)); }
Matrix2^ Matrix2::operator-(Matrix2^ m) { return gcnew Matrix2(new geompp::maths::Matrix2(-(*m->_native))); }
Matrix2^ Matrix2::operator*(Matrix2^ m, double s) { return gcnew Matrix2(new geompp::maths::Matrix2(*m->_native * s)); }
Matrix2^ Matrix2::operator*(double s, Matrix2^ m) { return gcnew Matrix2(new geompp::maths::Matrix2(s * *m->_native)); }
Matrix2^ Matrix2::operator/(Matrix2^ m, double s) { return gcnew Matrix2(new geompp::maths::Matrix2(*m->_native / s)); }
Matrix2^ Matrix2::operator*(Matrix2^ lhs, Matrix2^ rhs) { return gcnew Matrix2(new geompp::maths::Matrix2(*lhs->_native * *rhs->_native)); }
Vector2^ Matrix2::operator*(Matrix2^ m, Vector2^ v) { return gcnew Vector2(new geompp::maths::Vector2(*m->_native * *v->_native)); }

System::String^ Matrix2::ToString() { return gcnew System::String(_native->ToString().c_str()); }

// ── Matrix3 ──────────────────────────────────────────────────────────────────

Matrix3::Matrix3() : _native(new geompp::maths::Matrix3()) {}
Matrix3::Matrix3(double m00, double m01, double m02, double m10, double m11, double m12, double m20, double m21, double m22)
    : _native(new geompp::maths::Matrix3(m00, m01, m02, m10, m11, m12, m20, m21, m22)) {}
Matrix3::Matrix3(geompp::maths::Matrix3* native) : _native(native) {}
Matrix3::~Matrix3() { delete _native; _native = nullptr; }
Matrix3::!Matrix3() { delete _native; _native = nullptr; }

double Matrix3::Get(int row, int col) { return _native->At(static_cast<std::size_t>(row), static_cast<std::size_t>(col)); }
void   Matrix3::Set(int row, int col, double value) { _native->At(static_cast<std::size_t>(row), static_cast<std::size_t>(col)) = value; }

Matrix3^ Matrix3::Transpose() { return gcnew Matrix3(new geompp::maths::Matrix3(_native->Transpose())); }
double   Matrix3::Determinant() { return _native->Determinant(); }
Matrix3^ Matrix3::Inverse() { return gcnew Matrix3(new geompp::maths::Matrix3(_native->Inverse())); }

Matrix3^ Matrix3::Zero() { return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Zero())); }
Matrix3^ Matrix3::Identity() { return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Identity())); }

Matrix3^ Matrix3::Translation(Vector2^ offset) {
    return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Translation(*offset->_native)));
}
Matrix3^ Matrix3::Rotation(double angleRad) {
    return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Rotation(angleRad)));
}
Matrix3^ Matrix3::Scale(double factor) {
    return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Scale(factor)));
}
Matrix3^ Matrix3::Scale(double sx, double sy) {
    return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Scale(sx, sy)));
}
Matrix3^ Matrix3::Shear(double shx, double shy) {
    return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Shear(shx, shy)));
}
Matrix3^ Matrix3::Reflection(Vector2^ normal) {
    return gcnew Matrix3(new geompp::maths::Matrix3(geompp::maths::Matrix3::Reflection(*normal->_native)));
}

bool     Matrix3::operator==(Matrix3^ lhs, Matrix3^ rhs) { return *lhs->_native == *rhs->_native; }
Matrix3^ Matrix3::operator+(Matrix3^ lhs, Matrix3^ rhs) { return gcnew Matrix3(new geompp::maths::Matrix3(*lhs->_native + *rhs->_native)); }
Matrix3^ Matrix3::operator-(Matrix3^ lhs, Matrix3^ rhs) { return gcnew Matrix3(new geompp::maths::Matrix3(*lhs->_native - *rhs->_native)); }
Matrix3^ Matrix3::operator-(Matrix3^ m) { return gcnew Matrix3(new geompp::maths::Matrix3(-(*m->_native))); }
Matrix3^ Matrix3::operator*(Matrix3^ m, double s) { return gcnew Matrix3(new geompp::maths::Matrix3(*m->_native * s)); }
Matrix3^ Matrix3::operator*(double s, Matrix3^ m) { return gcnew Matrix3(new geompp::maths::Matrix3(s * *m->_native)); }
Matrix3^ Matrix3::operator/(Matrix3^ m, double s) { return gcnew Matrix3(new geompp::maths::Matrix3(*m->_native / s)); }
Matrix3^ Matrix3::operator*(Matrix3^ lhs, Matrix3^ rhs) { return gcnew Matrix3(new geompp::maths::Matrix3(*lhs->_native * *rhs->_native)); }
Vector3^ Matrix3::operator*(Matrix3^ m, Vector3^ v) { return gcnew Vector3(new geompp::maths::Vector3(*m->_native * *v->_native)); }

System::String^ Matrix3::ToString() { return gcnew System::String(_native->ToString().c_str()); }

// ── Matrix4 ──────────────────────────────────────────────────────────────────

Matrix4::Matrix4() : _native(new geompp::maths::Matrix4()) {}
Matrix4::Matrix4(double m00, double m01, double m02, double m03,
                 double m10, double m11, double m12, double m13,
                 double m20, double m21, double m22, double m23,
                 double m30, double m31, double m32, double m33)
    : _native(new geompp::maths::Matrix4(m00, m01, m02, m03, m10, m11, m12, m13,
                                         m20, m21, m22, m23, m30, m31, m32, m33)) {}
Matrix4::Matrix4(geompp::maths::Matrix4* native) : _native(native) {}
Matrix4::~Matrix4() { delete _native; _native = nullptr; }
Matrix4::!Matrix4() { delete _native; _native = nullptr; }

double Matrix4::Get(int row, int col) { return _native->At(static_cast<std::size_t>(row), static_cast<std::size_t>(col)); }
void   Matrix4::Set(int row, int col, double value) { _native->At(static_cast<std::size_t>(row), static_cast<std::size_t>(col)) = value; }

Matrix4^ Matrix4::Transpose() { return gcnew Matrix4(new geompp::maths::Matrix4(_native->Transpose())); }
double   Matrix4::Determinant() { return _native->Determinant(); }
Matrix4^ Matrix4::Inverse() { return gcnew Matrix4(new geompp::maths::Matrix4(_native->Inverse())); }

Matrix4^ Matrix4::Zero() { return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Zero())); }
Matrix4^ Matrix4::Identity() { return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Identity())); }

Matrix4^ Matrix4::Translation(Vector3^ offset) {
    return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Translation(*offset->_native)));
}
Matrix4^ Matrix4::Rotation(double angleRad, Vector3^ axis) {
    return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Rotation(angleRad, *axis->_native)));
}
Matrix4^ Matrix4::Scale(double factor) {
    return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Scale(factor)));
}
Matrix4^ Matrix4::Scale(double sx, double sy, double sz) {
    return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Scale(sx, sy, sz)));
}
Matrix4^ Matrix4::Shear(double xy, double xz, double yx, double yz, double zx, double zy) {
    return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Shear(xy, xz, yx, yz, zx, zy)));
}
Matrix4^ Matrix4::Reflection(Vector3^ normal) {
    return gcnew Matrix4(new geompp::maths::Matrix4(geompp::maths::Matrix4::Reflection(*normal->_native)));
}

bool     Matrix4::operator==(Matrix4^ lhs, Matrix4^ rhs) { return *lhs->_native == *rhs->_native; }
Matrix4^ Matrix4::operator+(Matrix4^ lhs, Matrix4^ rhs) { return gcnew Matrix4(new geompp::maths::Matrix4(*lhs->_native + *rhs->_native)); }
Matrix4^ Matrix4::operator-(Matrix4^ lhs, Matrix4^ rhs) { return gcnew Matrix4(new geompp::maths::Matrix4(*lhs->_native - *rhs->_native)); }
Matrix4^ Matrix4::operator-(Matrix4^ m) { return gcnew Matrix4(new geompp::maths::Matrix4(-(*m->_native))); }
Matrix4^ Matrix4::operator*(Matrix4^ m, double s) { return gcnew Matrix4(new geompp::maths::Matrix4(*m->_native * s)); }
Matrix4^ Matrix4::operator*(double s, Matrix4^ m) { return gcnew Matrix4(new geompp::maths::Matrix4(s * *m->_native)); }
Matrix4^ Matrix4::operator/(Matrix4^ m, double s) { return gcnew Matrix4(new geompp::maths::Matrix4(*m->_native / s)); }
Matrix4^ Matrix4::operator*(Matrix4^ lhs, Matrix4^ rhs) { return gcnew Matrix4(new geompp::maths::Matrix4(*lhs->_native * *rhs->_native)); }
Vector4^ Matrix4::operator*(Matrix4^ m, Vector4^ v) { return gcnew Vector4(new geompp::maths::Vector4(*m->_native * *v->_native)); }

System::String^ Matrix4::ToString() { return gcnew System::String(_native->ToString().c_str()); }

// ── Solvers ──────────────────────────────────────────────────────────────────

Vector2^ Solvers::SolveGauss(Matrix2^ a, Vector2^ b) { return gcnew Vector2(new geompp::maths::Vector2(geompp::maths::solve_gauss(*a->_native, *b->_native))); }
Vector3^ Solvers::SolveGauss(Matrix3^ a, Vector3^ b) { return gcnew Vector3(new geompp::maths::Vector3(geompp::maths::solve_gauss(*a->_native, *b->_native))); }
Vector4^ Solvers::SolveGauss(Matrix4^ a, Vector4^ b) { return gcnew Vector4(new geompp::maths::Vector4(geompp::maths::solve_gauss(*a->_native, *b->_native))); }

Vector2^ Solvers::SolveCramer(Matrix2^ a, Vector2^ b) { return gcnew Vector2(new geompp::maths::Vector2(geompp::maths::solve_cramer(*a->_native, *b->_native))); }
Vector3^ Solvers::SolveCramer(Matrix3^ a, Vector3^ b) { return gcnew Vector3(new geompp::maths::Vector3(geompp::maths::solve_cramer(*a->_native, *b->_native))); }
Vector4^ Solvers::SolveCramer(Matrix4^ a, Vector4^ b) { return gcnew Vector4(new geompp::maths::Vector4(geompp::maths::solve_cramer(*a->_native, *b->_native))); }

} }  // namespace GeomPP::Maths
