#include "Polygon3D.hpp"
#include "Plane.hpp"
#include "Point3D.hpp"
#include "Line3D.hpp"
#include "Ray3D.hpp"
#include "LineSegment3D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Polygon3D::Polygon3D(geompp::Polygon3D* native)
    : _native(native) {}

Polygon3D::~Polygon3D() {
    delete _native;
    _native = nullptr;
}

Polygon3D::!Polygon3D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Polygon3D^ Polygon3D::Make(array<Point3D^>^ points) {
    std::vector<geompp::Point3D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point3D^ p in points)
        nativePoints.push_back(*p->_native);
    return gcnew Polygon3D(new geompp::Polygon3D(geompp::Polygon3D::Make(nativePoints)));
}

Polygon3D^ Polygon3D::Make(array<Point3D^>^ points, array<array<Point3D^>^>^ holes) {
    std::vector<geompp::Point3D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point3D^ p in points)
        nativePoints.push_back(*p->_native);

    std::vector<std::vector<geompp::Point3D>> nativeHoles;
    for each (array<Point3D^>^ hole in holes) {
        std::vector<geompp::Point3D> nativeHole;
        nativeHole.reserve(hole->Length);
        for each (Point3D^ p in hole)
            nativeHole.push_back(*p->_native);
        nativeHoles.push_back(nativeHole);
    }

    return gcnew Polygon3D(new geompp::Polygon3D(geompp::Polygon3D::Make(nativePoints, nativeHoles)));
}

// ── Methods ──────────────────────────────────────────────────────────────────

int Polygon3D::Size() {
    return (int)_native->Size();
}

Point3D^ Polygon3D::default::get(int i) {
    return gcnew Point3D(new geompp::Point3D((*_native)[i]));
}

bool Polygon3D::AlmostEquals(Polygon3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Polygon3D::AlmostEquals(Polygon3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

Plane^ Polygon3D::GetPlane() {
    return gcnew Plane(new geompp::Plane(_native->GetPlane()));
}

Point3D^ Polygon3D::Centroid() {
    return gcnew Point3D(new geompp::Point3D(_native->Centroid()));
}

double Polygon3D::Area() {
    return _native->Area();
}

double Polygon3D::Perimeter() {
    return _native->Perimeter();
}

double Polygon3D::DistanceTo(Point3D^ point) {
    return _native->DistanceTo(*point->_native);
}

bool Polygon3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

bool Polygon3D::IsOnBoundary(Point3D^ point) {
    return _native->IsOnBoundary(*point->_native);
}

bool Polygon3D::IsSimple() {
    return _native->IsSimple();
}

bool Polygon3D::IsConvex() {
    return _native->IsConvex();
}

Polygon3D^ Polygon3D::ConvexHull() {
    return gcnew Polygon3D(new geompp::Polygon3D(_native->ConvexHull()));
}

array<Point3D^>^ Polygon3D::ToPoints() {
    auto native = _native->ToPoints();
    auto arr = gcnew array<Point3D^>(static_cast<int>(native.size()));
    for (int i = 0; i < static_cast<int>(native.size()); ++i)
        arr[i] = gcnew Point3D(new geompp::Point3D(native[i]));
    return arr;
}

System::String^ Polygon3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Polygon3D^ Polygon3D::FromWkt(System::String^ wkt) {
    return gcnew Polygon3D(
        new geompp::Polygon3D(geompp::Polygon3D::FromWkt(marshal_as<std::string>(wkt))));
}

void Polygon3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Polygon3D^ Polygon3D::FromFile(System::String^ path) {
    return gcnew Polygon3D(
        new geompp::Polygon3D(geompp::Polygon3D::FromFile(marshal_as<std::string>(path))));
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Polygon3D::Intersects(Line3D^ line) {
    return _native->Intersects(*line->_native);
}

bool Polygon3D::Intersects(Ray3D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Polygon3D::Intersects(LineSegment3D^ segment) {
    return _native->Intersects(*segment->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

Point3D^ Polygon3D::Intersection(Line3D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

Point3D^ Polygon3D::Intersection(Ray3D^ ray) {
    auto result = _native->Intersection(*ray->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

Point3D^ Polygon3D::Intersection(LineSegment3D^ segment) {
    auto result = _native->Intersection(*segment->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polygon3D::operator==(Polygon3D^ lhs, Polygon3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polygon3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
