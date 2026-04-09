#include "Vector3D.hpp"
#include "Point3D.hpp"   // needed for ToPoint() and Vector+Point operator

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Vector3D::Vector3D(double x, double y, double z)
    : _native(new geompp::Vector3D(x, y, z)) {}

Vector3D::Vector3D(geompp::Vector3D* native)
    : _native(native) {}

Vector3D::~Vector3D() {
    delete _native;
    _native = nullptr;
}

Vector3D::!Vector3D() {
    delete _native;
    _native = nullptr;
}

// ── Properties ───────────────────────────────────────────────────────────────

double Vector3D::X::get() { return _native->x(); }
double Vector3D::Y::get() { return _native->y(); }
double Vector3D::Z::get() { return _native->z(); }

// ── Methods ──────────────────────────────────────────────────────────────────

double Vector3D::Length() { return _native->Length(); }

bool Vector3D::AlmostEquals(Vector3D^ other) {
    return _native->AlmostEquals(*other->_native);
}
bool Vector3D::AlmostEquals(Vector3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

System::String^ Vector3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Vector3D^ Vector3D::FromWkt(System::String^ wkt) {
    return gcnew Vector3D(
        new geompp::Vector3D(geompp::Vector3D::FromWkt(marshal_as<std::string>(wkt))));
}

void Vector3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Vector3D^ Vector3D::FromFile(System::String^ path) {
    return gcnew Vector3D(
        new geompp::Vector3D(geompp::Vector3D::FromFile(marshal_as<std::string>(path))));
}

double Vector3D::Dot(Vector3D^ other) {
    return _native->Dot(*other->_native);
}

Vector3D^ Vector3D::Cross(Vector3D^ other) {
    return gcnew Vector3D(new geompp::Vector3D(_native->Cross(*other->_native)));
}

Vector3D^ Vector3D::Perp() {
    return gcnew Vector3D(new geompp::Vector3D(_native->Perp()));
}

Vector3D^ Vector3D::Normalize() {
    return gcnew Vector3D(new geompp::Vector3D(_native->Normalize()));
}

bool Vector3D::IsParallel(Vector3D^ other) {
    return _native->IsParallel(*other->_native);
}

Point3D^ Vector3D::ToPoint() {
    return gcnew Point3D(new geompp::Point3D(_native->ToPoint()));
}

// ── Static factories ─────────────────────────────────────────────────────────

Vector3D^ Vector3D::BasisX() { return gcnew Vector3D(new geompp::Vector3D(geompp::Vector3D::BasisX())); }
Vector3D^ Vector3D::BasisY() { return gcnew Vector3D(new geompp::Vector3D(geompp::Vector3D::BasisY())); }
Vector3D^ Vector3D::BasisZ() { return gcnew Vector3D(new geompp::Vector3D(geompp::Vector3D::BasisZ())); }

// ── Operators ────────────────────────────────────────────────────────────────

bool Vector3D::operator==(Vector3D^ lhs, Vector3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

Vector3D^ Vector3D::operator+(Vector3D^ lhs, Vector3D^ rhs) {
    return gcnew Vector3D(new geompp::Vector3D(*lhs->_native + *rhs->_native));
}

Vector3D^ Vector3D::operator-(Vector3D^ lhs, Vector3D^ rhs) {
    return gcnew Vector3D(new geompp::Vector3D(*lhs->_native - *rhs->_native));
}

Vector3D^ Vector3D::operator*(Vector3D^ lhs, double a) {
    return gcnew Vector3D(new geompp::Vector3D(*lhs->_native * a));
}

Vector3D^ Vector3D::operator*(double a, Vector3D^ rhs) {
    return gcnew Vector3D(new geompp::Vector3D(a * *rhs->_native));
}

double Vector3D::operator*(Vector3D^ lhs, Vector3D^ rhs) {
    return *lhs->_native * *rhs->_native;   // native operator* returns double (dot product)
}

Vector3D^ Vector3D::operator/(Vector3D^ lhs, double a) {
    return gcnew Vector3D(new geompp::Vector3D(*lhs->_native / a));
}

Vector3D^ Vector3D::operator-(Vector3D^ v) {
    return gcnew Vector3D(new geompp::Vector3D(-(*v->_native)));
}

System::String^ Vector3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
