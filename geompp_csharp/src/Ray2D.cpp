#include "Ray2D.hpp"
#include "Point2D.hpp"
#include "Polyline2D.hpp"
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
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ Ray2D::Intersection(Ray2D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ Ray2D::Intersection(LineSegment2D^ segment) {
    auto result = _native->Intersection(*segment->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

// ── Overlaps ─────────────────────────────────────────────────────────────────

bool Ray2D::Overlaps(Line2D^ line) {
    return _native->Overlaps(*line->_native);
}

bool Ray2D::Overlaps(Ray2D^ other) {
    return _native->Overlaps(*other->_native);
}

bool Ray2D::Overlaps(LineSegment2D^ segment) {
    return _native->Overlaps(*segment->_native);
}

// ── Overlap ───────────────────────────────────────────────────────────────────

Ray2D^ Ray2D::Overlap(Line2D^ line) {
    auto result = _native->Overlap(*line->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Ray2D(new geompp::Ray2D(result.value()));
}

System::Object^ Ray2D::Overlap(Ray2D^ other) {
    auto result = _native->Overlap(*other->_native);
    if (!result.has_value()) { return nullptr; }
    auto& var = result.value();
    if (std::holds_alternative<geompp::Ray2D>(var)) {
        return gcnew Ray2D(new geompp::Ray2D(std::get<geompp::Ray2D>(var)));
    }
    return gcnew LineSegment2D(new geompp::LineSegment2D(std::get<geompp::LineSegment2D>(var)));
}

LineSegment2D^ Ray2D::Overlap(LineSegment2D^ segment) {
    auto result = _native->Overlap(*segment->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew LineSegment2D(new geompp::LineSegment2D(result.value()));
}

// ── Touches ───────────────────────────────────────────────────────────────────

bool Ray2D::Touches(Line2D^ line) { return _native->Touches(*line->_native); }
bool Ray2D::Touches(Ray2D^ other) { return _native->Touches(*other->_native); }
bool Ray2D::Touches(LineSegment2D^ segment) { return _native->Touches(*segment->_native); }

// ── Touch ─────────────────────────────────────────────────────────────────────

Point2D^ Ray2D::Touch(Line2D^ line) {
    auto result = _native->Touch(*line->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ Ray2D::Touch(Ray2D^ other) {
    auto result = _native->Touch(*other->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ Ray2D::Touch(LineSegment2D^ segment) {
    auto result = _native->Touch(*segment->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

// ── Operator ──────────────────────────────────────────────────────────────────

// ── Polyline2D delegates ──────────────────────────────────────────────────────

bool Ray2D::Overlaps(Polyline2D^ polyline) { return _native->Overlaps(*polyline->_native); }
bool Ray2D::Touches(Polyline2D^ polyline) { return _native->Touches(*polyline->_native); }

array<LineSegment2D^>^ Ray2D::Overlap(Polyline2D^ polyline) {
    auto result = _native->Overlap(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& segs = result.value();
    auto arr = gcnew array<LineSegment2D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        arr[i] = gcnew LineSegment2D(new geompp::LineSegment2D(segs[i]));
    }
    return arr;
}

array<Point2D^>^ Ray2D::Touch(Polyline2D^ polyline) {
    auto result = _native->Touch(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& pts = result.value();
    auto arr = gcnew array<Point2D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew Point2D(new geompp::Point2D(pts[i]));
    }
    return arr;
}

bool Ray2D::operator==(Ray2D^ lhs, Ray2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Ray2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
