#include "MathsVector.hpp"

namespace GeomPP { namespace Maths {

// ── Vector2 ──────────────────────────────────────────────────────────────────

Vector2::Vector2(double x, double y) : _native(new geompp::maths::Vector2(x, y)) {}
Vector2::Vector2(geompp::maths::Vector2* native) : _native(native) {}
Vector2::~Vector2() { delete _native; _native = nullptr; }
Vector2::!Vector2() { delete _native; _native = nullptr; }

double Vector2::X::get() { return _native->x(); }
double Vector2::Y::get() { return _native->y(); }
double Vector2::default::get(int i) { return _native->At(static_cast<std::size_t>(i)); }

double Vector2::Dot(Vector2^ other) { return _native->Dot(*other->_native); }
double Vector2::Length() { return _native->Length(); }
double Vector2::LengthSquared() { return _native->LengthSquared(); }
Vector2^ Vector2::Normalized() { return gcnew Vector2(new geompp::maths::Vector2(_native->Normalized())); }

Vector2^ Vector2::Zero() { return gcnew Vector2(new geompp::maths::Vector2(geompp::maths::Vector2::Zero())); }

bool Vector2::operator==(Vector2^ lhs, Vector2^ rhs) { return *lhs->_native == *rhs->_native; }
Vector2^ Vector2::operator+(Vector2^ lhs, Vector2^ rhs) { return gcnew Vector2(new geompp::maths::Vector2(*lhs->_native + *rhs->_native)); }
Vector2^ Vector2::operator-(Vector2^ lhs, Vector2^ rhs) { return gcnew Vector2(new geompp::maths::Vector2(*lhs->_native - *rhs->_native)); }
Vector2^ Vector2::operator-(Vector2^ v) { return gcnew Vector2(new geompp::maths::Vector2(-(*v->_native))); }
Vector2^ Vector2::operator*(Vector2^ v, double s) { return gcnew Vector2(new geompp::maths::Vector2(*v->_native * s)); }
Vector2^ Vector2::operator*(double s, Vector2^ v) { return gcnew Vector2(new geompp::maths::Vector2(s * *v->_native)); }
Vector2^ Vector2::operator/(Vector2^ v, double s) { return gcnew Vector2(new geompp::maths::Vector2(*v->_native / s)); }

System::String^ Vector2::ToString() { return gcnew System::String(_native->ToString().c_str()); }

// ── Vector3 ──────────────────────────────────────────────────────────────────

Vector3::Vector3(double x, double y, double z) : _native(new geompp::maths::Vector3(x, y, z)) {}
Vector3::Vector3(geompp::maths::Vector3* native) : _native(native) {}
Vector3::~Vector3() { delete _native; _native = nullptr; }
Vector3::!Vector3() { delete _native; _native = nullptr; }

double Vector3::X::get() { return _native->x(); }
double Vector3::Y::get() { return _native->y(); }
double Vector3::Z::get() { return _native->z(); }
double Vector3::default::get(int i) { return _native->At(static_cast<std::size_t>(i)); }

double Vector3::Dot(Vector3^ other) { return _native->Dot(*other->_native); }
Vector3^ Vector3::Cross(Vector3^ other) { return gcnew Vector3(new geompp::maths::Vector3(_native->Cross(*other->_native))); }
double Vector3::Length() { return _native->Length(); }
double Vector3::LengthSquared() { return _native->LengthSquared(); }
Vector3^ Vector3::Normalized() { return gcnew Vector3(new geompp::maths::Vector3(_native->Normalized())); }

Vector3^ Vector3::Zero() { return gcnew Vector3(new geompp::maths::Vector3(geompp::maths::Vector3::Zero())); }

bool Vector3::operator==(Vector3^ lhs, Vector3^ rhs) { return *lhs->_native == *rhs->_native; }
Vector3^ Vector3::operator+(Vector3^ lhs, Vector3^ rhs) { return gcnew Vector3(new geompp::maths::Vector3(*lhs->_native + *rhs->_native)); }
Vector3^ Vector3::operator-(Vector3^ lhs, Vector3^ rhs) { return gcnew Vector3(new geompp::maths::Vector3(*lhs->_native - *rhs->_native)); }
Vector3^ Vector3::operator-(Vector3^ v) { return gcnew Vector3(new geompp::maths::Vector3(-(*v->_native))); }
Vector3^ Vector3::operator*(Vector3^ v, double s) { return gcnew Vector3(new geompp::maths::Vector3(*v->_native * s)); }
Vector3^ Vector3::operator*(double s, Vector3^ v) { return gcnew Vector3(new geompp::maths::Vector3(s * *v->_native)); }
Vector3^ Vector3::operator/(Vector3^ v, double s) { return gcnew Vector3(new geompp::maths::Vector3(*v->_native / s)); }

System::String^ Vector3::ToString() { return gcnew System::String(_native->ToString().c_str()); }

// ── Vector4 ──────────────────────────────────────────────────────────────────

Vector4::Vector4(double x, double y, double z, double w) : _native(new geompp::maths::Vector4(x, y, z, w)) {}
Vector4::Vector4(geompp::maths::Vector4* native) : _native(native) {}
Vector4::~Vector4() { delete _native; _native = nullptr; }
Vector4::!Vector4() { delete _native; _native = nullptr; }

double Vector4::X::get() { return _native->x(); }
double Vector4::Y::get() { return _native->y(); }
double Vector4::Z::get() { return _native->z(); }
double Vector4::W::get() { return _native->w(); }
double Vector4::default::get(int i) { return _native->At(static_cast<std::size_t>(i)); }

double Vector4::Dot(Vector4^ other) { return _native->Dot(*other->_native); }
double Vector4::Length() { return _native->Length(); }
double Vector4::LengthSquared() { return _native->LengthSquared(); }
Vector4^ Vector4::Normalized() { return gcnew Vector4(new geompp::maths::Vector4(_native->Normalized())); }

Vector4^ Vector4::Zero() { return gcnew Vector4(new geompp::maths::Vector4(geompp::maths::Vector4::Zero())); }

bool Vector4::operator==(Vector4^ lhs, Vector4^ rhs) { return *lhs->_native == *rhs->_native; }
Vector4^ Vector4::operator+(Vector4^ lhs, Vector4^ rhs) { return gcnew Vector4(new geompp::maths::Vector4(*lhs->_native + *rhs->_native)); }
Vector4^ Vector4::operator-(Vector4^ lhs, Vector4^ rhs) { return gcnew Vector4(new geompp::maths::Vector4(*lhs->_native - *rhs->_native)); }
Vector4^ Vector4::operator-(Vector4^ v) { return gcnew Vector4(new geompp::maths::Vector4(-(*v->_native))); }
Vector4^ Vector4::operator*(Vector4^ v, double s) { return gcnew Vector4(new geompp::maths::Vector4(*v->_native * s)); }
Vector4^ Vector4::operator*(double s, Vector4^ v) { return gcnew Vector4(new geompp::maths::Vector4(s * *v->_native)); }
Vector4^ Vector4::operator/(Vector4^ v, double s) { return gcnew Vector4(new geompp::maths::Vector4(*v->_native / s)); }

System::String^ Vector4::ToString() { return gcnew System::String(_native->ToString().c_str()); }

} }  // namespace GeomPP::Maths
