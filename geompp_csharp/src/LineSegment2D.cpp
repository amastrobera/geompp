#include "LineSegment2D.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Ray2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

LineSegment2D::LineSegment2D(geompp::LineSegment2D* native)
    : _native(native) {}

LineSegment2D::~LineSegment2D() {
    delete _native;
    _native = nullptr;
}

LineSegment2D::!LineSegment2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

LineSegment2D^ LineSegment2D::Make(Point2D^ p0, Point2D^ p1) {
    return gcnew LineSegment2D(
        new geompp::LineSegment2D(geompp::LineSegment2D::Make(*p0->_native, *p1->_native)));
}

// ── Properties ───────────────────────────────────────────────────────────────

Point2D^ LineSegment2D::First() {
    return gcnew Point2D(new geompp::Point2D(_native->First()));
}

Point2D^ LineSegment2D::Last() {
    return gcnew Point2D(new geompp::Point2D(_native->Last()));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool LineSegment2D::AlmostEquals(LineSegment2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool LineSegment2D::AlmostEquals(LineSegment2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

Line2D^ LineSegment2D::ToLine() {
    return gcnew Line2D(new geompp::Line2D(_native->ToLine()));
}

double LineSegment2D::Length() {
    return _native->Length();
}

double LineSegment2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

double LineSegment2D::Location(Point2D^ point) {
    return _native->Location(*point->_native);
}

Point2D^ LineSegment2D::Interpolate(double pct) {
    return gcnew Point2D(new geompp::Point2D(_native->Interpolate(pct)));
}

System::String^ LineSegment2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

LineSegment2D^ LineSegment2D::FromWkt(System::String^ wkt) {
    return gcnew LineSegment2D(
        new geompp::LineSegment2D(geompp::LineSegment2D::FromWkt(marshal_as<std::string>(wkt))));
}

void LineSegment2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

LineSegment2D^ LineSegment2D::FromFile(System::String^ path) {
    return gcnew LineSegment2D(
        new geompp::LineSegment2D(geompp::LineSegment2D::FromFile(marshal_as<std::string>(path))));
}

bool LineSegment2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool LineSegment2D::Intersects(Line2D^ line) {
    return _native->Intersects(*line->_native);
}

bool LineSegment2D::Intersects(Ray2D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool LineSegment2D::Intersects(LineSegment2D^ other) {
    return _native->Intersects(*other->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point2D^ LineSegment2D::Intersection(Line2D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

Point2D^ LineSegment2D::Intersection(Ray2D^ ray) {
    auto result = _native->Intersection(*ray->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

Point2D^ LineSegment2D::Intersection(LineSegment2D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool LineSegment2D::operator==(LineSegment2D^ lhs, LineSegment2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ LineSegment2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
