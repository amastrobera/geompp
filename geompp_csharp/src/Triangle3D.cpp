#include "Triangle3D.hpp"
#include "Point3D.hpp"
#include "Vector3D.hpp"
#include "Line3D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Triangle3D::Triangle3D(geompp::Triangle3D* native)
    : _native(native) {}

Triangle3D::~Triangle3D() {
    delete _native;
    _native = nullptr;
}

Triangle3D::!Triangle3D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Triangle3D^ Triangle3D::Make(Point3D^ p0, Point3D^ p1, Point3D^ p2) {
    return gcnew Triangle3D(
        new geompp::Triangle3D(geompp::Triangle3D::Make(*p0->_native, *p1->_native, *p2->_native)));
}

// ── Methods ──────────────────────────────────────────────────────────────────

System::Tuple<Point3D^, Point3D^, Point3D^>^ Triangle3D::Vertices() {
    auto [p0, p1, p2] = _native->Vertices();
    return gcnew System::Tuple<Point3D^, Point3D^, Point3D^>(
        gcnew Point3D(new geompp::Point3D(p0)),
        gcnew Point3D(new geompp::Point3D(p1)),
        gcnew Point3D(new geompp::Point3D(p2)));
}

bool Triangle3D::AlmostEquals(Triangle3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Triangle3D::AlmostEquals(Triangle3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

Point3D^ Triangle3D::Centroid() {
    return gcnew Point3D(new geompp::Point3D(_native->Centroid()));
}

double Triangle3D::SignedArea() {
    return _native->SignedArea();
}

double Triangle3D::Area() {
    return _native->Area();
}

double Triangle3D::Perimeter() {
    return _native->Perimeter();
}

double Triangle3D::DistanceTo(Point3D^ point) {
    return _native->DistanceTo(*point->_native);
}

System::Tuple<Vector3D^, Vector3D^>^ Triangle3D::ToAxis() {
    auto [u, v] = _native->ToAxis();
    return gcnew System::Tuple<Vector3D^, Vector3D^>(
        gcnew Vector3D(new geompp::Vector3D(u)),
        gcnew Vector3D(new geompp::Vector3D(v)));
}

System::Tuple<double, double>^ Triangle3D::Location(Point3D^ point) {
    auto [s, t] = _native->Location(*point->_native);
    return gcnew System::Tuple<double, double>(s, t);
}

Point3D^ Triangle3D::Interpolate(double s, double t) {
    auto result = _native->Interpolate(s, t);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(result.value()));
}

System::String^ Triangle3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Triangle3D^ Triangle3D::FromWkt(System::String^ wkt) {
    return gcnew Triangle3D(
        new geompp::Triangle3D(geompp::Triangle3D::FromWkt(marshal_as<std::string>(wkt))));
}

void Triangle3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Triangle3D^ Triangle3D::FromFile(System::String^ path) {
    return gcnew Triangle3D(
        new geompp::Triangle3D(geompp::Triangle3D::FromFile(marshal_as<std::string>(path))));
}

bool Triangle3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Triangle3D::Intersects(Line3D^ line) {
    return _native->Intersects(*line->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

System::Object^ Triangle3D::Intersection(Line3D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;

    auto& val = result.value();
    if (std::holds_alternative<geompp::Point3D>(val))
        return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(val)));
    if (std::holds_alternative<geompp::Triangle3D>(val))
        return gcnew Triangle3D(new geompp::Triangle3D(std::get<geompp::Triangle3D>(val)));
    return nullptr;
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Triangle3D::operator==(Triangle3D^ lhs, Triangle3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Triangle3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
