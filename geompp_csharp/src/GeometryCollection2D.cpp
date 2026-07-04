#include "GeometryCollection2D.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Ray2D.hpp"
#include "LineSegment2D.hpp"
#include "Polyline2D.hpp"
#include "Triangle2D.hpp"
#include "Polygon2D.hpp"

#pragma managed(push, off)
#include <geometry_collection2d.hpp>
#include <line2d.hpp>
#include <line_segment2d.hpp>
#include <point2d.hpp>
#include <polygon2d.hpp>
#include <polyline2d.hpp>
#include <ray2d.hpp>
#include <triangle2d.hpp>
#pragma managed(pop)

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

GeometryCollection2D::GeometryCollection2D()
    : _native(new geompp::GeometryCollection2D()) {}

GeometryCollection2D::GeometryCollection2D(geompp::GeometryCollection2D* native)
    : _native(native) {}

GeometryCollection2D::~GeometryCollection2D() {
    delete _native;
    _native = nullptr;
}

GeometryCollection2D::!GeometryCollection2D() {
    delete _native;
    _native = nullptr;
}

// ── Size ──────────────────────────────────────────────────────────────────────

int GeometryCollection2D::Size() {
    return (int)_native->Size();
}

// ── Add ───────────────────────────────────────────────────────────────────────

void GeometryCollection2D::Add(Point2D^ point) {
    _native->Add(*point->_native);
}

void GeometryCollection2D::Add(Line2D^ line) {
    _native->Add(*line->_native);
}

void GeometryCollection2D::Add(LineSegment2D^ segment) {
    _native->Add(*segment->_native);
}

void GeometryCollection2D::Add(Ray2D^ ray) {
    _native->Add(*ray->_native);
}

void GeometryCollection2D::Add(Polyline2D^ polyline) {
    _native->Add(*polyline->_native);
}

void GeometryCollection2D::Add(Triangle2D^ triangle) {
    _native->Add(*triangle->_native);
}

void GeometryCollection2D::Add(Polygon2D^ polygon) {
    _native->Add(*polygon->_native);
}

void GeometryCollection2D::Add(GeometryCollection2D^ collection) {
    _native->Add(*collection->_native);
}

// ── Get ───────────────────────────────────────────────────────────────────────

System::Object^ GeometryCollection2D::Get(int index) {
    auto const& shape = _native->Get((std::size_t)index);
    if (std::holds_alternative<geompp::Point2D>(shape)) {
        return gcnew Point2D(new geompp::Point2D(std::get<geompp::Point2D>(shape)));
    }
    if (std::holds_alternative<geompp::Line2D>(shape)) {
        return gcnew Line2D(new geompp::Line2D(std::get<geompp::Line2D>(shape)));
    }
    if (std::holds_alternative<geompp::Ray2D>(shape)) {
        return gcnew Ray2D(new geompp::Ray2D(std::get<geompp::Ray2D>(shape)));
    }
    if (std::holds_alternative<geompp::LineSegment2D>(shape)) {
        return gcnew LineSegment2D(new geompp::LineSegment2D(std::get<geompp::LineSegment2D>(shape)));
    }
    if (std::holds_alternative<geompp::Polyline2D>(shape)) {
        return gcnew Polyline2D(new geompp::Polyline2D(std::get<geompp::Polyline2D>(shape)));
    }
    if (std::holds_alternative<geompp::Triangle2D>(shape)) {
        return gcnew Triangle2D(new geompp::Triangle2D(std::get<geompp::Triangle2D>(shape)));
    }
    if (std::holds_alternative<geompp::Polygon2D>(shape)) {
        return gcnew Polygon2D(new geompp::Polygon2D(std::get<geompp::Polygon2D>(shape)));
    }
    if (std::holds_alternative<geompp::GeometryCollection2D>(shape)) {
        return gcnew GeometryCollection2D(
            new geompp::GeometryCollection2D(std::get<geompp::GeometryCollection2D>(shape)));
    }
    throw gcnew System::InvalidOperationException("Unknown geometry type");
}

// ── AlmostEquals ──────────────────────────────────────────────────────────────

bool GeometryCollection2D::AlmostEquals(GeometryCollection2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool GeometryCollection2D::AlmostEquals(GeometryCollection2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

// ── Serialization ─────────────────────────────────────────────────────────────

System::String^ GeometryCollection2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

GeometryCollection2D^ GeometryCollection2D::FromWkt(System::String^ wkt) {
    return gcnew GeometryCollection2D(
        new geompp::GeometryCollection2D(
            geompp::GeometryCollection2D::FromWkt(marshal_as<std::string>(wkt))));
}

void GeometryCollection2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

GeometryCollection2D^ GeometryCollection2D::FromFile(System::String^ path) {
    return gcnew GeometryCollection2D(
        new geompp::GeometryCollection2D(
            geompp::GeometryCollection2D::FromFile(marshal_as<std::string>(path))));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool GeometryCollection2D::operator==(GeometryCollection2D^ lhs, GeometryCollection2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ GeometryCollection2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
