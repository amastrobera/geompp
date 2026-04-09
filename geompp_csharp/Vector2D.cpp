#include "Vector2D.hpp"
#include "Point2D.hpp"   // needed for ToPoint() and Vector+Point operator

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Vector2D::Vector2D(double x, double y)
    : _native(new geompp::Vector2D(x, y)) {}

Vector2D::Vector2D(geompp::Vector2D* native)
    : _native(native) {}

Vector2D::~Vector2D() {
    delete _native;
    _native = nullptr;
}

Vector2D::!Vector2D() {
    delete _native;
    _native = nullptr;
}

// ── Properties ───────────────────────────────────────────────────────────────

double Vector2D::X::get() { return _native->x(); }
double Vector2D::Y::get() { return _native->y(); }

// ── Methods ──────────────────────────────────────────────────────────────────

double Vector2D::Length() { return _native->Length(); }

bool Vector2D::AlmostEquals(Vector2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Vector2D::AlmostEquals(Vector2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

System::String^ Vector2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Vector2D^ Vector2D::FromWkt(System::String^ wkt) {
    return gcnew Vector2D(
        new geompp::Vector2D(geompp::Vector2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Vector2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Vector2D^ Vector2D::FromFile(System::String^ path) {
    return gcnew Vector2D(
        new geompp::Vector2D(geompp::Vector2D::FromFile(marshal_as<std::string>(path))));
}

double Vector2D::Dot(Vector2D^ other) {
    return _native->Dot(*other->_native);
}

double Vector2D::Cross(Vector2D^ other) {
    return _native->Cross(*other->_native);
}

Vector2D^ Vector2D::Perp() {
    return gcnew Vector2D(new geompp::Vector2D(_native->Perp()));
}

Vector2D^ Vector2D::Normalize() {
    return gcnew Vector2D(new geompp::Vector2D(_native->Normalize()));
}

Point2D^ Vector2D::ToPoint() {
    return gcnew Point2D(new geompp::Point2D(_native->ToPoint()));
}

// ── Static factories ─────────────────────────────────────────────────────────

Vector2D^ Vector2D::BasisX() { return gcnew Vector2D(new geompp::Vector2D(geompp::Vector2D::BasisX())); }
Vector2D^ Vector2D::BasisY() { return gcnew Vector2D(new geompp::Vector2D(geompp::Vector2D::BasisY())); }

// ── Operators ────────────────────────────────────────────────────────────────

bool Vector2D::operator==(Vector2D^ lhs, Vector2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

Vector2D^ Vector2D::operator+(Vector2D^ lhs, Vector2D^ rhs) {
    return gcnew Vector2D(new geompp::Vector2D(*lhs->_native + *rhs->_native));
}

Point2D^ Vector2D::operator+(Vector2D^ lhs, Point2D^ rhs) {
    return gcnew Point2D(new geompp::Point2D(*lhs->_native + *rhs->_native));
}

Vector2D^ Vector2D::operator-(Vector2D^ lhs, Vector2D^ rhs) {
    return gcnew Vector2D(new geompp::Vector2D(*lhs->_native - *rhs->_native));
}

Vector2D^ Vector2D::operator*(Vector2D^ lhs, double a) {
    return gcnew Vector2D(new geompp::Vector2D(*lhs->_native * a));
}

Vector2D^ Vector2D::operator*(double a, Vector2D^ rhs) {
    return gcnew Vector2D(new geompp::Vector2D(a * *rhs->_native));
}

double Vector2D::operator*(Vector2D^ lhs, Vector2D^ rhs) {
    return *lhs->_native * *rhs->_native;   // native operator* returns double (dot product)
}

Vector2D^ Vector2D::operator/(Vector2D^ lhs, double a) {
    return gcnew Vector2D(new geompp::Vector2D(*lhs->_native / a));
}

Vector2D^ Vector2D::operator-(Vector2D^ v) {
    return gcnew Vector2D(new geompp::Vector2D(-(*v->_native)));
}

System::String^ Vector2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
