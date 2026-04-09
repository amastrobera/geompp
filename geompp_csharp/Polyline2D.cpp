#include "Polyline2D.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Ray2D.hpp"
#include "LineSegment2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Polyline2D::Polyline2D(geompp::Polyline2D* native)
    : _native(native) {}

Polyline2D::~Polyline2D() {
    delete _native;
    _native = nullptr;
}

Polyline2D::!Polyline2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Polyline2D^ Polyline2D::Make(array<Point2D^>^ points) {
    std::vector<geompp::Point2D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point2D^ p in points)
        nativePoints.push_back(*p->_native);
    return gcnew Polyline2D(new geompp::Polyline2D(geompp::Polyline2D::Make(nativePoints)));
}

// ── Methods ──────────────────────────────────────────────────────────────────

int Polyline2D::Size() {
    return _native->Size();
}

Point2D^ Polyline2D::default::get(int i) {
    return gcnew Point2D(new geompp::Point2D((*_native)[(size_t)i]));
}

bool Polyline2D::AlmostEquals(Polyline2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Polyline2D::AlmostEquals(Polyline2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

array<LineSegment2D^>^ Polyline2D::ToSegments() {
    auto segs = _native->ToSegments();
    auto result = gcnew array<LineSegment2D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i)
        result[i] = gcnew LineSegment2D(new geompp::LineSegment2D(segs[i]));
    return result;
}

double Polyline2D::Length() {
    return _native->Length();
}

double Polyline2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

double Polyline2D::Location(Point2D^ point) {
    return _native->Location(*point->_native);
}

Point2D^ Polyline2D::Interpolate(double pct) {
    return gcnew Point2D(new geompp::Point2D(_native->Interpolate(pct)));
}

System::String^ Polyline2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Polyline2D^ Polyline2D::FromWkt(System::String^ wkt) {
    return gcnew Polyline2D(
        new geompp::Polyline2D(geompp::Polyline2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Polyline2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Polyline2D^ Polyline2D::FromFile(System::String^ path) {
    return gcnew Polyline2D(
        new geompp::Polyline2D(geompp::Polyline2D::FromFile(marshal_as<std::string>(path))));
}

bool Polyline2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Polyline2D::Intersects(Line2D^ line) {
    return _native->Intersects(*line->_native);
}

bool Polyline2D::Intersects(Ray2D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Polyline2D::Intersects(LineSegment2D^ segment) {
    return _native->Intersects(*segment->_native);
}

bool Polyline2D::Intersects(Polyline2D^ other) {
    return _native->Intersects(*other->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

// Helper to convert optional<variant<Point2D, vector<Point2D>>> to System::Object^
static System::Object^ ConvertPolyline2DIntersection(
    const geompp::Polyline2D::ReturnSet& result) {
    if (!result.has_value()) return nullptr;

    auto& val = result.value();
    if (std::holds_alternative<geompp::Point2D>(val))
        return gcnew GeomPP::Point2D(new geompp::Point2D(std::get<geompp::Point2D>(val)));

    // vector<Point2D>
    auto& pts = std::get<geompp::Polyline2D::MultiPoint>(val);
    auto arr = gcnew array<GeomPP::Point2D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i)
        arr[i] = gcnew GeomPP::Point2D(new geompp::Point2D(pts[i]));
    return arr;
}

System::Object^ Polyline2D::Intersection(Line2D^ line) {
    return ConvertPolyline2DIntersection(_native->Intersection(*line->_native));
}

System::Object^ Polyline2D::Intersection(Ray2D^ ray) {
    return ConvertPolyline2DIntersection(_native->Intersection(*ray->_native));
}

System::Object^ Polyline2D::Intersection(LineSegment2D^ segment) {
    return ConvertPolyline2DIntersection(_native->Intersection(*segment->_native));
}

System::Object^ Polyline2D::Intersection(Polyline2D^ other) {
    return ConvertPolyline2DIntersection(_native->Intersection(*other->_native));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polyline2D::operator==(Polyline2D^ lhs, Polyline2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polyline2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
