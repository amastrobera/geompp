#include "LineSegment2D.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Polyline2D.hpp"
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

LineSegment2D^ LineSegment2D::Reversed() {
    return gcnew LineSegment2D(new geompp::LineSegment2D(_native->Reversed()));
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
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ LineSegment2D::Intersection(Ray2D^ ray) {
    auto result = _native->Intersection(*ray->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ LineSegment2D::Intersection(LineSegment2D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

// ── Overlaps ─────────────────────────────────────────────────────────────────

bool LineSegment2D::Overlaps(Line2D^ line) {
    return _native->Overlaps(*line->_native);
}

bool LineSegment2D::Overlaps(Ray2D^ ray) {
    return _native->Overlaps(*ray->_native);
}

bool LineSegment2D::Overlaps(LineSegment2D^ other) {
    return _native->Overlaps(*other->_native);
}

// ── Overlap ───────────────────────────────────────────────────────────────────

LineSegment2D^ LineSegment2D::Overlap(Line2D^ line) {
    auto result = _native->Overlap(*line->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew LineSegment2D(new geompp::LineSegment2D(result.value()));
}

LineSegment2D^ LineSegment2D::Overlap(Ray2D^ ray) {
    auto result = _native->Overlap(*ray->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew LineSegment2D(new geompp::LineSegment2D(result.value()));
}

LineSegment2D^ LineSegment2D::Overlap(LineSegment2D^ other) {
    auto result = _native->Overlap(*other->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew LineSegment2D(new geompp::LineSegment2D(result.value()));
}

// ── Touches ───────────────────────────────────────────────────────────────────

bool LineSegment2D::Touches(Line2D^ line) { return _native->Touches(*line->_native); }
bool LineSegment2D::Touches(Ray2D^ ray) { return _native->Touches(*ray->_native); }
bool LineSegment2D::Touches(LineSegment2D^ other) { return _native->Touches(*other->_native); }

// ── Touch ─────────────────────────────────────────────────────────────────────

Point2D^ LineSegment2D::Touch(Line2D^ line) {
    auto result = _native->Touch(*line->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ LineSegment2D::Touch(Ray2D^ ray) {
    auto result = _native->Touch(*ray->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

Point2D^ LineSegment2D::Touch(LineSegment2D^ other) {
    auto result = _native->Touch(*other->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

// ── Operator ──────────────────────────────────────────────────────────────────

// ── Polyline2D delegates ──────────────────────────────────────────────────────

bool LineSegment2D::Overlaps(Polyline2D^ polyline) { return _native->Overlaps(*polyline->_native); }
bool LineSegment2D::Touches(Polyline2D^ polyline) { return _native->Touches(*polyline->_native); }

array<LineSegment2D^>^ LineSegment2D::Overlap(Polyline2D^ polyline) {
    auto result = _native->Overlap(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& segs = result.value();
    auto arr = gcnew array<LineSegment2D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        arr[i] = gcnew LineSegment2D(new geompp::LineSegment2D(segs[i]));
    }
    return arr;
}

array<Point2D^>^ LineSegment2D::Touch(Polyline2D^ polyline) {
    auto result = _native->Touch(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& pts = result.value();
    auto arr = gcnew array<Point2D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew Point2D(new geompp::Point2D(pts[i]));
    }
    return arr;
}

bool LineSegment2D::operator==(LineSegment2D^ lhs, LineSegment2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ LineSegment2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
