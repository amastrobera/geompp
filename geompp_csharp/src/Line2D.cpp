#include "Line2D.hpp"
#include "Point2D.hpp"
#include "Polyline2D.hpp"
#include "Vector2D.hpp"
#include "Ray2D.hpp"
#include "LineSegment2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Line2D::Line2D(geompp::Line2D* native)
    : _native(native) {}

Line2D::~Line2D() {
    delete _native;
    _native = nullptr;
}

Line2D::!Line2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Line2D^ Line2D::Make(Point2D^ p0, Point2D^ p1) {
    return gcnew Line2D(new geompp::Line2D(geompp::Line2D::Make(*p0->_native, *p1->_native)));
}

Line2D^ Line2D::Make(Point2D^ origin, Vector2D^ dir) {
    return gcnew Line2D(new geompp::Line2D(geompp::Line2D::Make(*origin->_native, *dir->_native)));
}

// ── Properties ───────────────────────────────────────────────────────────────

Point2D^ Line2D::First() {
    return gcnew Point2D(new geompp::Point2D(_native->First()));
}

Point2D^ Line2D::Last() {
    return gcnew Point2D(new geompp::Point2D(_native->Last()));
}

Point2D^ Line2D::Origin() {
    return gcnew Point2D(new geompp::Point2D(_native->Origin()));
}

Vector2D^ Line2D::Direction() {
    return gcnew Vector2D(new geompp::Vector2D(_native->Direction()));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool Line2D::AlmostEquals(Line2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Line2D::AlmostEquals(Line2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

double Line2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

Point2D^ Line2D::ProjectOnto(Point2D^ point) {
    return gcnew Point2D(new geompp::Point2D(_native->ProjectOnto(*point->_native)));
}

System::String^ Line2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Line2D^ Line2D::FromWkt(System::String^ wkt) {
    return gcnew Line2D(new geompp::Line2D(geompp::Line2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Line2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Line2D^ Line2D::FromFile(System::String^ path) {
    return gcnew Line2D(new geompp::Line2D(geompp::Line2D::FromFile(marshal_as<std::string>(path))));
}

bool Line2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Line2D::Intersects(Line2D^ other) {
    return _native->Intersects(*other->_native);
}

bool Line2D::Intersects(Ray2D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Line2D::Intersects(LineSegment2D^ segment) {
    return _native->Intersects(*segment->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point2D^ Line2D::Intersection(Line2D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ Line2D::Intersection(Ray2D^ ray) {
    auto result = _native->Intersection(*ray->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ Line2D::Intersection(LineSegment2D^ segment) {
    auto result = _native->Intersection(*segment->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

// ── Overlaps ─────────────────────────────────────────────────────────────────

bool Line2D::Overlaps(Line2D^ other) {
    return _native->Overlaps(*other->_native);
}

bool Line2D::Overlaps(Ray2D^ ray) {
    return _native->Overlaps(*ray->_native);
}

bool Line2D::Overlaps(LineSegment2D^ segment) {
    return _native->Overlaps(*segment->_native);
}

// ── Overlap ───────────────────────────────────────────────────────────────────

Line2D^ Line2D::Overlap(Line2D^ other) {
    auto result = _native->Overlap(*other->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Line2D(new geompp::Line2D(result.value()));
}

Ray2D^ Line2D::Overlap(Ray2D^ ray) {
    auto result = _native->Overlap(*ray->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Ray2D(new geompp::Ray2D(result.value()));
}

LineSegment2D^ Line2D::Overlap(LineSegment2D^ segment) {
    auto result = _native->Overlap(*segment->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew LineSegment2D(new geompp::LineSegment2D(result.value()));
}

// ── Touches ───────────────────────────────────────────────────────────────────

bool Line2D::Touches(Ray2D^ ray) { return _native->Touches(*ray->_native); }
bool Line2D::Touches(LineSegment2D^ segment) { return _native->Touches(*segment->_native); }

// ── Touch ─────────────────────────────────────────────────────────────────────

Point2D^ Line2D::Touch(Ray2D^ ray) {
    auto result = _native->Touch(*ray->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ Line2D::Touch(LineSegment2D^ segment) {
    auto result = _native->Touch(*segment->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

// ── Operator ──────────────────────────────────────────────────────────────────

// ── Polyline2D delegates ──────────────────────────────────────────────────────

bool Line2D::Overlaps(Polyline2D^ polyline) { return _native->Overlaps(*polyline->_native); }
bool Line2D::Touches(Polyline2D^ polyline) { return _native->Touches(*polyline->_native); }

array<LineSegment2D^>^ Line2D::Overlap(Polyline2D^ polyline) {
    auto result = _native->Overlap(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& segs = result.value();
    auto arr = gcnew array<LineSegment2D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        arr[i] = gcnew LineSegment2D(new geompp::LineSegment2D(segs[i]));
    }
    return arr;
}

array<Point2D^>^ Line2D::Touch(Polyline2D^ polyline) {
    auto result = _native->Touch(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& pts = result.value();
    auto arr = gcnew array<Point2D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew Point2D(new geompp::Point2D(pts[i]));
    }
    return arr;
}

bool Line2D::operator==(Line2D^ lhs, Line2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Line2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
