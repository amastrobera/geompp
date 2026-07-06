#include "LineSegment3D.hpp"
#include "Point3D.hpp"
#include "Line3D.hpp"
#include "Polyline3D.hpp"
#include "Ray3D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

LineSegment3D::LineSegment3D(geompp::LineSegment3D* native)
    : _native(native) {}

LineSegment3D::~LineSegment3D() {
    delete _native;
    _native = nullptr;
}

LineSegment3D::!LineSegment3D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

LineSegment3D^ LineSegment3D::Make(Point3D^ p0, Point3D^ p1) {
    return gcnew LineSegment3D(
        new geompp::LineSegment3D(geompp::LineSegment3D::Make(*p0->_native, *p1->_native)));
}

// ── Properties ───────────────────────────────────────────────────────────────

Point3D^ LineSegment3D::First() {
    return gcnew Point3D(new geompp::Point3D(_native->First()));
}

Point3D^ LineSegment3D::Last() {
    return gcnew Point3D(new geompp::Point3D(_native->Last()));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool LineSegment3D::AlmostEquals(LineSegment3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool LineSegment3D::AlmostEquals(LineSegment3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

Line3D^ LineSegment3D::ToLine() {
    return gcnew Line3D(new geompp::Line3D(_native->ToLine()));
}

double LineSegment3D::Length() {
    return _native->Length();
}

LineSegment3D^ LineSegment3D::Reversed() {
    return gcnew LineSegment3D(new geompp::LineSegment3D(_native->Reversed()));
}

double LineSegment3D::DistanceTo(Point3D^ point) {
    return _native->DistanceTo(*point->_native);
}

double LineSegment3D::Location(Point3D^ point) {
    return _native->Location(*point->_native);
}

Point3D^ LineSegment3D::Interpolate(double pct) {
    return gcnew Point3D(new geompp::Point3D(_native->Interpolate(pct)));
}

// ── Distance / DistanceTo ────────────────────────────────────────────────────

LineSegment3D^ LineSegment3D::Distance(Line3D^ line) {
    auto result = _native->Distance(*line->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew LineSegment3D(new geompp::LineSegment3D(result.value()));
}

LineSegment3D^ LineSegment3D::Distance(Ray3D^ ray) {
    auto result = _native->Distance(*ray->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew LineSegment3D(new geompp::LineSegment3D(result.value()));
}

LineSegment3D^ LineSegment3D::Distance(LineSegment3D^ other) {
    auto result = _native->Distance(*other->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew LineSegment3D(new geompp::LineSegment3D(result.value()));
}

double LineSegment3D::DistanceTo(Line3D^ line) {
    return _native->DistanceTo(*line->_native);
}

double LineSegment3D::DistanceTo(Ray3D^ ray) {
    return _native->DistanceTo(*ray->_native);
}

double LineSegment3D::DistanceTo(LineSegment3D^ other) {
    return _native->DistanceTo(*other->_native);
}

System::String^ LineSegment3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

LineSegment3D^ LineSegment3D::FromWkt(System::String^ wkt) {
    return gcnew LineSegment3D(
        new geompp::LineSegment3D(geompp::LineSegment3D::FromWkt(marshal_as<std::string>(wkt))));
}

void LineSegment3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

LineSegment3D^ LineSegment3D::FromFile(System::String^ path) {
    return gcnew LineSegment3D(
        new geompp::LineSegment3D(geompp::LineSegment3D::FromFile(marshal_as<std::string>(path))));
}

bool LineSegment3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool LineSegment3D::Intersects(Line3D^ line) {
    return _native->Intersects(*line->_native);
}

bool LineSegment3D::Intersects(Ray3D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool LineSegment3D::Intersects(LineSegment3D^ other) {
    return _native->Intersects(*other->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point3D^ LineSegment3D::Intersection(Line3D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point3D(new geompp::Point3D(result.value()));
}

Point3D^ LineSegment3D::Intersection(Ray3D^ ray) {
    auto result = _native->Intersection(*ray->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point3D(new geompp::Point3D(result.value()));
}

Point3D^ LineSegment3D::Intersection(LineSegment3D^ other) {
    auto result = _native->Intersection(*other->_native);
    if (!result.has_value()) {
        return nullptr;
    }
    return gcnew Point3D(new geompp::Point3D(result.value()));
}

// ── Overlaps ─────────────────────────────────────────────────────────────────

bool LineSegment3D::Overlaps(Line3D^ line) {
    return _native->Overlaps(*line->_native);
}

bool LineSegment3D::Overlaps(Ray3D^ ray) {
    return _native->Overlaps(*ray->_native);
}

bool LineSegment3D::Overlaps(LineSegment3D^ other) {
    return _native->Overlaps(*other->_native);
}

// ── Overlap ───────────────────────────────────────────────────────────────────

LineSegment3D^ LineSegment3D::Overlap(Line3D^ line) {
    auto result = _native->Overlap(*line->_native);
    if (!result.has_value()) return nullptr;
    return gcnew LineSegment3D(new geompp::LineSegment3D(result.value()));
}

LineSegment3D^ LineSegment3D::Overlap(Ray3D^ ray) {
    auto result = _native->Overlap(*ray->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew LineSegment3D(new geompp::LineSegment3D(result.value()));
}

LineSegment3D^ LineSegment3D::Overlap(LineSegment3D^ other) {
    auto result = _native->Overlap(*other->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew LineSegment3D(new geompp::LineSegment3D(result.value()));
}

// ── Touches ───────────────────────────────────────────────────────────────────

bool LineSegment3D::Touches(Line3D^ line) { return _native->Touches(*line->_native); }
bool LineSegment3D::Touches(Ray3D^ ray) { return _native->Touches(*ray->_native); }
bool LineSegment3D::Touches(LineSegment3D^ other) { return _native->Touches(*other->_native); }

// ── Touch ─────────────────────────────────────────────────────────────────────

Point3D^ LineSegment3D::Touch(Line3D^ line) {
    auto result = _native->Touch(*line->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point3D(new geompp::Point3D(result.value()));
}

Point3D^ LineSegment3D::Touch(Ray3D^ ray) {
    auto result = _native->Touch(*ray->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point3D(new geompp::Point3D(result.value()));
}

Point3D^ LineSegment3D::Touch(LineSegment3D^ other) {
    auto result = _native->Touch(*other->_native);
    if (!result.has_value()) { return nullptr; }
    return gcnew Point3D(new geompp::Point3D(result.value()));
}

// ── Operator ──────────────────────────────────────────────────────────────────

// ── Polyline3D delegates ──────────────────────────────────────────────────────

bool LineSegment3D::Overlaps(Polyline3D^ polyline) { return _native->Overlaps(*polyline->_native); }
bool LineSegment3D::Touches(Polyline3D^ polyline) { return _native->Touches(*polyline->_native); }

array<LineSegment3D^>^ LineSegment3D::Overlap(Polyline3D^ polyline) {
    auto result = _native->Overlap(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& segs = result.value();
    auto arr = gcnew array<LineSegment3D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        arr[i] = gcnew LineSegment3D(new geompp::LineSegment3D(segs[i]));
    }
    return arr;
}

array<Point3D^>^ LineSegment3D::Touch(Polyline3D^ polyline) {
    auto result = _native->Touch(*polyline->_native);
    if (!result.has_value()) { return nullptr; }
    auto& pts = result.value();
    auto arr = gcnew array<Point3D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew Point3D(new geompp::Point3D(pts[i]));
    }
    return arr;
}

bool LineSegment3D::operator==(LineSegment3D^ lhs, LineSegment3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ LineSegment3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
