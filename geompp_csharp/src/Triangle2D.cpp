#include "Triangle2D.hpp"
#include "Point2D.hpp"
#include "Vector2D.hpp"
#include "Line2D.hpp"
#include "LineSegment2D.hpp"
#include "Polygon2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Triangle2D::Triangle2D(geompp::Triangle2D* native)
    : _native(native) {}

Triangle2D::~Triangle2D() {
    delete _native;
    _native = nullptr;
}

Triangle2D::!Triangle2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Triangle2D^ Triangle2D::Make(Point2D^ p0, Point2D^ p1, Point2D^ p2) {
    return gcnew Triangle2D(
        new geompp::Triangle2D(geompp::Triangle2D::Make(*p0->_native, *p1->_native, *p2->_native)));
}

// ── Methods ──────────────────────────────────────────────────────────────────

System::Tuple<Point2D^, Point2D^, Point2D^>^ Triangle2D::Vertices() {
    auto [p0, p1, p2] = _native->Vertices();
    return gcnew System::Tuple<Point2D^, Point2D^, Point2D^>(
        gcnew Point2D(new geompp::Point2D(p0)),
        gcnew Point2D(new geompp::Point2D(p1)),
        gcnew Point2D(new geompp::Point2D(p2)));
}

bool Triangle2D::AlmostEquals(Triangle2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Triangle2D::AlmostEquals(Triangle2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

Point2D^ Triangle2D::Centroid() {
    return gcnew Point2D(new geompp::Point2D(_native->Centroid()));
}

Polygon2D^ Triangle2D::ToPolygon() {
    return gcnew Polygon2D(new geompp::Polygon2D(_native->ToPolygon()));
}

double Triangle2D::SignedArea() {
    return _native->SignedArea();
}

bool Triangle2D::IsCCW() {
    return _native->IsCCW();
}

double Triangle2D::Area() {
    return _native->Area();
}

double Triangle2D::Perimeter() {
    return _native->Perimeter();
}

double Triangle2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

System::Tuple<Vector2D^, Vector2D^>^ Triangle2D::ToAxis() {
    auto [u, v] = _native->ToAxis();
    return gcnew System::Tuple<Vector2D^, Vector2D^>(
        gcnew Vector2D(new geompp::Vector2D(u)),
        gcnew Vector2D(new geompp::Vector2D(v)));
}

System::Tuple<double, double>^ Triangle2D::Location(Point2D^ point) {
    auto [s, t] = _native->Location(*point->_native);
    return gcnew System::Tuple<double, double>(s, t);
}

Point2D^ Triangle2D::Interpolate(double s, double t) {
    auto result = _native->Interpolate(s, t);
    if (!result.has_value()) return nullptr;
    return gcnew Point2D(new geompp::Point2D(result.value()));
}

System::String^ Triangle2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Triangle2D^ Triangle2D::FromWkt(System::String^ wkt) {
    return gcnew Triangle2D(
        new geompp::Triangle2D(geompp::Triangle2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Triangle2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Triangle2D^ Triangle2D::FromFile(System::String^ path) {
    return gcnew Triangle2D(
        new geompp::Triangle2D(geompp::Triangle2D::FromFile(marshal_as<std::string>(path))));
}

bool Triangle2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Triangle2D::Intersects(Line2D^ line) {
    return _native->Intersects(*line->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

System::Object^ Triangle2D::Intersection(Line2D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;

    auto& val = result.value();
    if (std::holds_alternative<geompp::Point2D>(val))
        return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(val)));
    if (std::holds_alternative<geompp::LineSegment2D>(val))
        return gcnew LineSegment2D(new geompp::LineSegment2D(std::get<geompp::LineSegment2D>(val)));
    if (std::holds_alternative<geompp::Triangle2D>(val))
        return gcnew Triangle2D(new geompp::Triangle2D(std::get<geompp::Triangle2D>(val)));
    if (std::holds_alternative<geompp::Polygon2D>(val))
        return gcnew Polygon2D(new geompp::Polygon2D(std::get<geompp::Polygon2D>(val)));
    return nullptr;
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Triangle2D::operator==(Triangle2D^ lhs, Triangle2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Triangle2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
