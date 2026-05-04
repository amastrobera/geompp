#include "Polygon2D.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Ray2D.hpp"
#include "LineSegment2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Polygon2D::Polygon2D(geompp::Polygon2D* native)
    : _native(native) {}

Polygon2D::~Polygon2D() {
    delete _native;
    _native = nullptr;
}

Polygon2D::!Polygon2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Polygon2D^ Polygon2D::Make(array<Point2D^>^ points) {
    std::vector<geompp::Point2D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point2D^ p in points)
        nativePoints.push_back(*p->_native);
    return gcnew Polygon2D(new geompp::Polygon2D(geompp::Polygon2D::Make(nativePoints)));
}

Polygon2D^ Polygon2D::Make(array<Point2D^>^ points, array<array<Point2D^>^>^ holes) {
    std::vector<geompp::Point2D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point2D^ p in points)
        nativePoints.push_back(*p->_native);

    std::vector<std::vector<geompp::Point2D>> nativeHoles;
    for each (array<Point2D^>^ hole in holes) {
        std::vector<geompp::Point2D> nativeHole;
        nativeHole.reserve(hole->Length);
        for each (Point2D^ p in hole)
            nativeHole.push_back(*p->_native);
        nativeHoles.push_back(nativeHole);
    }

    return gcnew Polygon2D(new geompp::Polygon2D(geompp::Polygon2D::Make(nativePoints, nativeHoles)));
}

// ── Methods ──────────────────────────────────────────────────────────────────

int Polygon2D::Size() {
    return (int)_native->Size();
}

Point2D^ Polygon2D::default::get(int i) {
    return gcnew Point2D(new geompp::Point2D((*_native)[i]));
}

bool Polygon2D::AlmostEquals(Polygon2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Polygon2D::AlmostEquals(Polygon2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

Point2D^ Polygon2D::Centroid() {
    return gcnew Point2D(new geompp::Point2D(_native->Centroid()));
}

double Polygon2D::Area() {
    return _native->Area();
}

double Polygon2D::Perimeter() {
    return _native->Perimeter();
}

double Polygon2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

bool Polygon2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

System::String^ Polygon2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Polygon2D^ Polygon2D::FromWkt(System::String^ wkt) {
    return gcnew Polygon2D(
        new geompp::Polygon2D(geompp::Polygon2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Polygon2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Polygon2D^ Polygon2D::FromFile(System::String^ path) {
    return gcnew Polygon2D(
        new geompp::Polygon2D(geompp::Polygon2D::FromFile(marshal_as<std::string>(path))));
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Polygon2D::Intersects(Line2D^ line) {
    return _native->Intersects(*line->_native);
}

bool Polygon2D::Intersects(Ray2D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Polygon2D::Intersects(LineSegment2D^ segment) {
    return _native->Intersects(*segment->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point2D^ Polygon2D::Intersection(Line2D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

Point2D^ Polygon2D::Intersection(Ray2D^ ray) {
    auto result = _native->Intersection(*ray->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

Point2D^ Polygon2D::Intersection(LineSegment2D^ segment) {
    auto result = _native->Intersection(*segment->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(result.value())));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polygon2D::operator==(Polygon2D^ lhs, Polygon2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polygon2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
