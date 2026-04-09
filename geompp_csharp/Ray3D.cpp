#include "Ray3D.hpp"
#include "Point3D.hpp"
#include "Vector3D.hpp"
#include "Line3D.hpp"
#include "LineSegment3D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Ray3D::Ray3D(geompp::Ray3D* native)
    : _native(native) {}

Ray3D::~Ray3D() {
    delete _native;
    _native = nullptr;
}

Ray3D::!Ray3D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Ray3D^ Ray3D::Make(Point3D^ origin, Vector3D^ dir) {
    return gcnew Ray3D(new geompp::Ray3D(geompp::Ray3D::Make(*origin->_native, *dir->_native)));
}

// ── Properties ───────────────────────────────────────────────────────────────

Point3D^ Ray3D::Origin() {
    return gcnew Point3D(new geompp::Point3D(_native->Origin()));
}

Vector3D^ Ray3D::Direction() {
    return gcnew Vector3D(new geompp::Vector3D(_native->Direction()));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool Ray3D::AlmostEquals(Ray3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Ray3D::AlmostEquals(Ray3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool Ray3D::IsAhead(Point3D^ point) {
    return _native->IsAhead(*point->_native);
}

bool Ray3D::IsBehind(Point3D^ point) {
    return _native->IsBehind(*point->_native);
}

Line3D^ Ray3D::ToLine() {
    return gcnew Line3D(new geompp::Line3D(_native->ToLine()));
}

double Ray3D::DistanceTo(Point3D^ point) {
    return _native->DistanceTo(*point->_native);
}

System::String^ Ray3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Ray3D^ Ray3D::FromWkt(System::String^ wkt) {
    return gcnew Ray3D(new geompp::Ray3D(geompp::Ray3D::FromWkt(marshal_as<std::string>(wkt))));
}

void Ray3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Ray3D^ Ray3D::FromFile(System::String^ path) {
    return gcnew Ray3D(new geompp::Ray3D(geompp::Ray3D::FromFile(marshal_as<std::string>(path))));
}

bool Ray3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Ray3D::Intersects(Line3D^ line) {
    return _native->Intersects(*line->_native);
}

bool Ray3D::Intersects(Ray3D^ other) {
    return _native->Intersects(*other->_native);
}

bool Ray3D::Intersects(LineSegment3D^ segment) {
    return _native->Intersects(*segment->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point3D^ Ray3D::Intersection(Line3D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

Point3D^ Ray3D::Intersection(Ray3D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

Point3D^ Ray3D::Intersection(LineSegment3D^ segment) {
    auto result = _native->Intersection(*segment->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Ray3D::operator==(Ray3D^ lhs, Ray3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Ray3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
