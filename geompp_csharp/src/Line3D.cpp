#include "Line3D.hpp"
#include "Point3D.hpp"
#include "Vector3D.hpp"
#include "Ray3D.hpp"
#include "LineSegment3D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Line3D::Line3D(geompp::Line3D* native)
    : _native(native) {}

Line3D::~Line3D() {
    delete _native;
    _native = nullptr;
}

Line3D::!Line3D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Line3D^ Line3D::Make(Point3D^ p0, Point3D^ p1) {
    return gcnew Line3D(new geompp::Line3D(geompp::Line3D::Make(*p0->_native, *p1->_native)));
}

Line3D^ Line3D::Make(Point3D^ origin, Vector3D^ dir) {
    return gcnew Line3D(new geompp::Line3D(geompp::Line3D::Make(*origin->_native, *dir->_native)));
}

// ── Properties ───────────────────────────────────────────────────────────────

Point3D^ Line3D::First() {
    return gcnew Point3D(new geompp::Point3D(_native->First()));
}

Point3D^ Line3D::Last() {
    return gcnew Point3D(new geompp::Point3D(_native->Last()));
}

Point3D^ Line3D::Origin() {
    return gcnew Point3D(new geompp::Point3D(_native->Origin()));
}

Vector3D^ Line3D::Direction() {
    return gcnew Vector3D(new geompp::Vector3D(_native->Direction()));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool Line3D::AlmostEquals(Line3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Line3D::AlmostEquals(Line3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

double Line3D::DistanceTo(Point3D^ point) {
    return _native->DistanceTo(*point->_native);
}

Point3D^ Line3D::ProjectOnto(Point3D^ point) {
    return gcnew Point3D(new geompp::Point3D(_native->ProjectOnto(*point->_native)));
}

System::String^ Line3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Line3D^ Line3D::FromWkt(System::String^ wkt) {
    return gcnew Line3D(new geompp::Line3D(geompp::Line3D::FromWkt(marshal_as<std::string>(wkt))));
}

void Line3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Line3D^ Line3D::FromFile(System::String^ path) {
    return gcnew Line3D(new geompp::Line3D(geompp::Line3D::FromFile(marshal_as<std::string>(path))));
}

bool Line3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Line3D::Intersects(Line3D^ other) {
    return _native->Intersects(*other->_native);
}

bool Line3D::Intersects(Ray3D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Line3D::Intersects(LineSegment3D^ segment) {
    return _native->Intersects(*segment->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point3D^ Line3D::Intersection(Line3D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

Point3D^ Line3D::Intersection(Ray3D^ ray) {
    auto result = _native->Intersection(*ray->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

Point3D^ Line3D::Intersection(LineSegment3D^ segment) {
    auto result = _native->Intersection(*segment->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Line3D::operator==(Line3D^ lhs, Line3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Line3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
