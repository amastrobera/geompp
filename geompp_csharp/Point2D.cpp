#include "Point2D.hpp"
#include "Vector2D.hpp"   // needed for ToVector() and Point-Point = Vector operator

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Point2D::Point2D(double x, double y)
    : _native(new geompp::Point2D(x, y)) {}

Point2D::Point2D(geompp::Point2D* native)
    : _native(native) {}

Point2D::~Point2D() {
    delete _native;
    _native = nullptr;
}

Point2D::!Point2D() {
    delete _native;
    _native = nullptr;
}

// ── Properties ───────────────────────────────────────────────────────────────

double Point2D::X::get() { return _native->x(); }
double Point2D::Y::get() { return _native->y(); }

// ── Methods ──────────────────────────────────────────────────────────────────

Vector2D^ Point2D::ToVector() {
    return gcnew Vector2D(new geompp::Vector2D(_native->ToVector()));
}

bool Point2D::AlmostEquals(Point2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Point2D::AlmostEquals(Point2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

double Point2D::DistanceTo(Point2D^ other) {
    return _native->DistanceTo(*other->_native);
}

System::String^ Point2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Point2D^ Point2D::FromWkt(System::String^ wkt) {
    return gcnew Point2D(
        new geompp::Point2D(geompp::Point2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Point2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Point2D^ Point2D::FromFile(System::String^ path) {
    return gcnew Point2D(
        new geompp::Point2D(geompp::Point2D::FromFile(marshal_as<std::string>(path))));
}

Point2D^ Point2D::Zero() {
    return gcnew Point2D(new geompp::Point2D(geompp::Point2D::Zero()));
}

// ── Operators ────────────────────────────────────────────────────────────────

bool Point2D::operator==(Point2D^ lhs, Point2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

Point2D^ Point2D::operator+(Point2D^ lhs, Vector2D^ rhs) {
    return gcnew Point2D(new geompp::Point2D(*lhs->_native + *rhs->_native));
}

Vector2D^ Point2D::operator-(Point2D^ lhs, Point2D^ rhs) {
    return gcnew Vector2D(new geompp::Vector2D(*lhs->_native - *rhs->_native));
}

Point2D^ Point2D::operator-(Point2D^ lhs, Vector2D^ rhs) {
    return gcnew Point2D(new geompp::Point2D(*lhs->_native - *rhs->_native));
}

Point2D^ Point2D::operator*(Point2D^ lhs, double a) {
    return gcnew Point2D(new geompp::Point2D(*lhs->_native * a));
}

Point2D^ Point2D::operator*(double a, Point2D^ rhs) {
    return gcnew Point2D(new geompp::Point2D(a * *rhs->_native));
}

System::String^ Point2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
