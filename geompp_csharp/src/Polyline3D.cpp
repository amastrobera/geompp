#include "Polyline3D.hpp"
#include "Point3D.hpp"
#include "Line3D.hpp"
#include "Ray3D.hpp"
#include "LineSegment3D.hpp"
#include "Polygon3D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Polyline3D::Polyline3D(geompp::Polyline3D* native)
    : _native(native) {}

Polyline3D::~Polyline3D() {
    delete _native;
    _native = nullptr;
}

Polyline3D::!Polyline3D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Polyline3D^ Polyline3D::Make(array<Point3D^>^ points) {
    std::vector<geompp::Point3D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point3D^ p in points)
        nativePoints.push_back(*p->_native);
    return gcnew Polyline3D(new geompp::Polyline3D(geompp::Polyline3D::Make(nativePoints)));
}

// ── Methods ──────────────────────────────────────────────────────────────────

int Polyline3D::Size() {
    return _native->Size();
}

Point3D^ Polyline3D::default::get(int i) {
    return gcnew Point3D(new geompp::Point3D((*_native)[(std::size_t)i]));
}

bool Polyline3D::AlmostEquals(Polyline3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Polyline3D::AlmostEquals(Polyline3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

array<LineSegment3D^>^ Polyline3D::ToSegments() {
    auto segs = _native->ToSegments();
    auto result = gcnew array<LineSegment3D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        result[i] = gcnew LineSegment3D(new geompp::LineSegment3D(segs[i]));
    }
    return result;
}

double Polyline3D::Length() {
    return _native->Length();
}

bool Polyline3D::IsPlanar()  { return _native->IsPlanar(); }
bool Polyline3D::IsSimple()  { return _native->IsSimple(); }
bool Polyline3D::IsConvex()  { return _native->IsConvex(); }

Polyline3D^ Polyline3D::ConvexHull() {
    return gcnew Polyline3D(new geompp::Polyline3D(_native->ConvexHull()));
}

Polygon3D^ Polyline3D::ToPolygon() {
    return gcnew Polygon3D(new geompp::Polygon3D(_native->ToPolygon()));
}

double Polyline3D::DistanceTo(Point3D^ point) {
    return _native->DistanceTo(*point->_native);
}

double Polyline3D::Location(Point3D^ point) {
    return _native->Location(*point->_native);
}

Point3D^ Polyline3D::Interpolate(double pct) {
    return gcnew Point3D(new geompp::Point3D(_native->Interpolate(pct)));
}

System::String^ Polyline3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Polyline3D^ Polyline3D::FromWkt(System::String^ wkt) {
    return gcnew Polyline3D(
        new geompp::Polyline3D(geompp::Polyline3D::FromWkt(marshal_as<std::string>(wkt))));
}

void Polyline3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Polyline3D^ Polyline3D::FromFile(System::String^ path) {
    return gcnew Polyline3D(
        new geompp::Polyline3D(geompp::Polyline3D::FromFile(marshal_as<std::string>(path))));
}

bool Polyline3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Polyline3D::Intersects(Line3D^ line) {
    return _native->Intersects(*line->_native);
}

bool Polyline3D::Intersects(Ray3D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Polyline3D::Intersects(LineSegment3D^ segment) {
    return _native->Intersects(*segment->_native);
}

bool Polyline3D::Intersects(Polyline3D^ other) {
    return _native->Intersects(*other->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

// Helper to convert optional<vector<Point3D>> to System::Object^ (null on miss, array otherwise)
static array<GeomPP::Point3D^>^ ConvertPolyline3DIntersection(
    std::optional<std::vector<geompp::Point3D>> const& result) {
    if (!result.has_value()) {
        return nullptr;
    }
    auto const& pts = result.value();
    auto arr = gcnew array<GeomPP::Point3D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew GeomPP::Point3D(new geompp::Point3D(pts[i]));
    }
    return arr;
}

System::Object^ Polyline3D::Intersection(Line3D^ line) {
    return ConvertPolyline3DIntersection(_native->Intersection(*line->_native));
}

System::Object^ Polyline3D::Intersection(Ray3D^ ray) {
    return ConvertPolyline3DIntersection(_native->Intersection(*ray->_native));
}

System::Object^ Polyline3D::Intersection(LineSegment3D^ segment) {
    return ConvertPolyline3DIntersection(_native->Intersection(*segment->_native));
}

System::Object^ Polyline3D::Intersection(Polyline3D^ other) {
    return ConvertPolyline3DIntersection(_native->Intersection(*other->_native));
}

// ── Overlaps / Overlap ────────────────────────────────────────────────────────

static array<LineSegment3D^>^ ToManagedSegArray3D(const std::optional<std::vector<geompp::LineSegment3D>>& opt) {
    if (!opt.has_value()) { return nullptr; }
    auto& segs = opt.value();
    auto arr = gcnew array<LineSegment3D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        arr[i] = gcnew LineSegment3D(new geompp::LineSegment3D(segs[i]));
    }
    return arr;
}

static array<Point3D^>^ ToManagedPtArray3D(const std::optional<std::vector<geompp::Point3D>>& opt) {
    if (!opt.has_value()) { return nullptr; }
    auto& pts = opt.value();
    auto arr = gcnew array<Point3D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew Point3D(new geompp::Point3D(pts[i]));
    }
    return arr;
}

bool Polyline3D::Overlaps(Line3D^ line) { return _native->Overlaps(*line->_native); }
bool Polyline3D::Overlaps(Ray3D^ ray) { return _native->Overlaps(*ray->_native); }
bool Polyline3D::Overlaps(LineSegment3D^ segment) { return _native->Overlaps(*segment->_native); }
bool Polyline3D::Overlaps(Polyline3D^ other) { return _native->Overlaps(*other->_native); }

array<LineSegment3D^>^ Polyline3D::Overlap(Line3D^ line) { return ToManagedSegArray3D(_native->Overlap(*line->_native)); }
array<LineSegment3D^>^ Polyline3D::Overlap(Ray3D^ ray) { return ToManagedSegArray3D(_native->Overlap(*ray->_native)); }
array<LineSegment3D^>^ Polyline3D::Overlap(LineSegment3D^ segment) { return ToManagedSegArray3D(_native->Overlap(*segment->_native)); }
array<LineSegment3D^>^ Polyline3D::Overlap(Polyline3D^ other) { return ToManagedSegArray3D(_native->Overlap(*other->_native)); }

// ── Touches / Touch ───────────────────────────────────────────────────────────

bool Polyline3D::Touches(Line3D^ line) { return _native->Touches(*line->_native); }
bool Polyline3D::Touches(Ray3D^ ray) { return _native->Touches(*ray->_native); }
bool Polyline3D::Touches(LineSegment3D^ segment) { return _native->Touches(*segment->_native); }
bool Polyline3D::Touches(Polyline3D^ other) { return _native->Touches(*other->_native); }

array<Point3D^>^ Polyline3D::Touch(Line3D^ line) { return ToManagedPtArray3D(_native->Touch(*line->_native)); }
array<Point3D^>^ Polyline3D::Touch(Ray3D^ ray) { return ToManagedPtArray3D(_native->Touch(*ray->_native)); }
array<Point3D^>^ Polyline3D::Touch(LineSegment3D^ segment) { return ToManagedPtArray3D(_native->Touch(*segment->_native)); }
array<Point3D^>^ Polyline3D::Touch(Polyline3D^ other) { return ToManagedPtArray3D(_native->Touch(*other->_native)); }

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polyline3D::operator==(Polyline3D^ lhs, Polyline3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polyline3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
