#include "Ray2D.hpp"
#include "Point2D.hpp"
#include "Vector2D.hpp"
#include "Line2D.hpp"
#include "LineSegment2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Ray2D::Ray2D(geompp::Ray2D* native)
    : _native(native) {}

Ray2D::~Ray2D() {
    delete _native;
    _native = nullptr;
}

Ray2D::!Ray2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Ray2D^ Ray2D::Make(Point2D^ origin, Vector2D^ dir) {
    return gcnew Ray2D(new geompp::Ray2D(geompp::Ray2D::Make(*origin->_native, *dir->_native)));
}

// ── Properties ───────────────────────────────────────────────────────────────

Point2D^ Ray2D::Origin() {
    return gcnew Point2D(new geompp::Point2D(_native->Origin()));
}

Vector2D^ Ray2D::Direction() {
    return gcnew Vector2D(new geompp::Vector2D(_native->Direction()));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool Ray2D::AlmostEquals(Ray2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Ray2D::AlmostEquals(Ray2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool Ray2D::IsAhead(Point2D^ point) {
    return _native->IsAhead(*point->_native);
}

bool Ray2D::IsBehind(Point2D^ point) {
    return _native->IsBehind(*point->_native);
}

Line2D^ Ray2D::ToLine() {
    return gcnew Line2D(new geompp::Line2D(_native->ToLine()));
}

double Ray2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

System::String^ Ray2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Ray2D^ Ray2D::FromWkt(System::String^ wkt) {
    return gcnew Ray2D(new geompp::Ray2D(geompp::Ray2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Ray2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Ray2D^ Ray2D::FromFile(System::String^ path) {
    return gcnew Ray2D(new geompp::Ray2D(geompp::Ray2D::FromFile(marshal_as<std::string>(path))));
}

bool Ray2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Ray2D::Intersects(Line2D^ line) {
    return _native->Intersects(*line->_native);
}

bool Ray2D::Intersects(Ray2D^ other) {
    return _native->Intersects(*other->_native);
}

bool Ray2D::Intersects(LineSegment2D^ segment) {
    return _native->Intersects(*segment->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point2D^ Ray2D::Intersection(Line2D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

Point2D^ Ray2D::Intersection(Ray2D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

Point2D^ Ray2D::Intersection(LineSegment2D^ segment) {
    auto result = _native->Intersection(*segment->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Ray2D::operator==(Ray2D^ lhs, Ray2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Ray2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
