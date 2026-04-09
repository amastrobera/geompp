#include "Point3D.hpp"
#include "Vector3D.hpp"   // needed for ToVector() and Point-Point = Vector operator

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Point3D::Point3D(double x, double y, double z)
    : _native(new geompp::Point3D(x, y, z)) {}

Point3D::Point3D(geompp::Point3D* native)
    : _native(native) {}

Point3D::~Point3D() {
    delete _native;
    _native = nullptr;
}

Point3D::!Point3D() {
    delete _native;
    _native = nullptr;
}

// ── Properties ───────────────────────────────────────────────────────────────

double Point3D::X::get() { return _native->x(); }
double Point3D::Y::get() { return _native->y(); }
double Point3D::Z::get() { return _native->z(); }

// ── Methods ──────────────────────────────────────────────────────────────────

Vector3D^ Point3D::ToVector() {
    return gcnew Vector3D(new geompp::Vector3D(_native->ToVector()));
}

bool Point3D::AlmostEquals(Point3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Point3D::AlmostEquals(Point3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

double Point3D::DistanceTo(Point3D^ other) {
    return _native->DistanceTo(*other->_native);
}

System::String^ Point3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Point3D^ Point3D::FromWkt(System::String^ wkt) {
    return gcnew Point3D(
        new geompp::Point3D(geompp::Point3D::FromWkt(marshal_as<std::string>(wkt))));
}

void Point3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Point3D^ Point3D::FromFile(System::String^ path) {
    return gcnew Point3D(
        new geompp::Point3D(geompp::Point3D::FromFile(marshal_as<std::string>(path))));
}

Point3D^ Point3D::Zero() {
    return gcnew Point3D(new geompp::Point3D(geompp::Point3D::Zero()));
}

// ── Operators ────────────────────────────────────────────────────────────────

bool Point3D::operator==(Point3D^ lhs, Point3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

Point3D^ Point3D::operator+(Point3D^ lhs, Vector3D^ rhs) {
    return gcnew Point3D(new geompp::Point3D(*lhs->_native + *rhs->_native));
}

Vector3D^ Point3D::operator-(Point3D^ lhs, Point3D^ rhs) {
    return gcnew Vector3D(new geompp::Vector3D(*lhs->_native - *rhs->_native));
}

Point3D^ Point3D::operator-(Point3D^ lhs, Vector3D^ rhs) {
    return gcnew Point3D(new geompp::Point3D(*lhs->_native - *rhs->_native));
}

Point3D^ Point3D::operator*(Point3D^ lhs, double a) {
    return gcnew Point3D(new geompp::Point3D(*lhs->_native * a));
}

Point3D^ Point3D::operator*(double a, Point3D^ rhs) {
    return gcnew Point3D(new geompp::Point3D(a * *rhs->_native));
}

System::String^ Point3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
