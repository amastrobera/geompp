#include "Polyline3D.hpp"
#include "Point3D.hpp"
#include "Line3D.hpp"
#include "Ray3D.hpp"
#include "LineSegment3D.hpp"

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
    for (int i = 0; i < (int)segs.size(); ++i)
        result[i] = gcnew LineSegment3D(new geompp::LineSegment3D(segs[i]));
    return result;
}

double Polyline3D::Length() {
    return _native->Length();
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

// Helper to convert optional<variant<Point3D, vector<Point3D>>> to System::Object^
static System::Object^ ConvertPolyline3DIntersection(
    const geompp::Polyline3D::ReturnSet& result) {
    if (!result.has_value()) return nullptr;

    auto& val = result.value();
    if (std::holds_alternative<geompp::Point3D>(val))
        return gcnew GeomPP::Point3D(new geompp::Point3D(std::get<geompp::Point3D>(val)));

    // vector<Point3D>
    auto& pts = std::get<geompp::Polyline3D::MultiPoint>(val);
    auto arr = gcnew array<GeomPP::Point3D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i)
        arr[i] = gcnew GeomPP::Point3D(new geompp::Point3D(pts[i]));
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

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polyline3D::operator==(Polyline3D^ lhs, Polyline3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polyline3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
