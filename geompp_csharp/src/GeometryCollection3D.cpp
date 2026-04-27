#include "GeometryCollection3D.hpp"
#include "Point3D.hpp"
#include "Line3D.hpp"
#include "Ray3D.hpp"
#include "LineSegment3D.hpp"
#include "Polyline3D.hpp"
#include "Triangle3D.hpp"
#include "Polygon3D.hpp"

#pragma managed(push, off)
#include <geometry_collection3d.hpp>
#include <line3d.hpp>
#include <line_segment3d.hpp>
#include <point3d.hpp>
#include <polygon3d.hpp>
#include <polyline3d.hpp>
#include <ray3d.hpp>
#include <triangle3d.hpp>
#pragma managed(pop)

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

GeometryCollection3D::GeometryCollection3D()
    : _native(new geompp::GeometryCollection3D()) {}

GeometryCollection3D::GeometryCollection3D(geompp::GeometryCollection3D* native)
    : _native(native) {}

GeometryCollection3D::~GeometryCollection3D() {
    delete _native;
    _native = nullptr;
}

GeometryCollection3D::!GeometryCollection3D() {
    delete _native;
    _native = nullptr;
}

// ── Size ──────────────────────────────────────────────────────────────────────

int GeometryCollection3D::Size() {
    return (int)_native->Size();
}

// ── Add ───────────────────────────────────────────────────────────────────────

void GeometryCollection3D::Add(Point3D^ point) {
    _native->Add(*point->_native);
}

void GeometryCollection3D::Add(Line3D^ line) {
    _native->Add(*line->_native);
}

void GeometryCollection3D::Add(LineSegment3D^ segment) {
    _native->Add(*segment->_native);
}

void GeometryCollection3D::Add(Ray3D^ ray) {
    _native->Add(*ray->_native);
}

void GeometryCollection3D::Add(Polyline3D^ polyline) {
    _native->Add(*polyline->_native);
}

void GeometryCollection3D::Add(Triangle3D^ triangle) {
    _native->Add(*triangle->_native);
}

void GeometryCollection3D::Add(Polygon3D^ polygon) {
    _native->Add(*polygon->_native);
}

void GeometryCollection3D::Add(GeometryCollection3D^ collection) {
    _native->Add(*collection->_native);
}

// ── Get ───────────────────────────────────────────────────────────────────────

System::Object^ GeometryCollection3D::Get(int index) {
    auto const& shape = _native->Get((std::size_t)index);
    if (std::holds_alternative<geompp::Point3D>(shape))
        return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(shape)));
    if (std::holds_alternative<geompp::Line3D>(shape))
        return gcnew Line3D(new geompp::Line3D(std::get<geompp::Line3D>(shape)));
    if (std::holds_alternative<geompp::Ray3D>(shape))
        return gcnew Ray3D(new geompp::Ray3D(std::get<geompp::Ray3D>(shape)));
    if (std::holds_alternative<geompp::LineSegment3D>(shape))
        return gcnew LineSegment3D(new geompp::LineSegment3D(std::get<geompp::LineSegment3D>(shape)));
    if (std::holds_alternative<geompp::Polyline3D>(shape))
        return gcnew Polyline3D(new geompp::Polyline3D(std::get<geompp::Polyline3D>(shape)));
    if (std::holds_alternative<geompp::Triangle3D>(shape))
        return gcnew Triangle3D(new geompp::Triangle3D(std::get<geompp::Triangle3D>(shape)));
    if (std::holds_alternative<geompp::Polygon3D>(shape))
        return gcnew Polygon3D(new geompp::Polygon3D(std::get<geompp::Polygon3D>(shape)));
    if (std::holds_alternative<geompp::GeometryCollection3D>(shape))
        return gcnew GeometryCollection3D(
            new geompp::GeometryCollection3D(std::get<geompp::GeometryCollection3D>(shape)));
    throw gcnew System::InvalidOperationException("Unknown geometry type");
}

// ── AlmostEquals ──────────────────────────────────────────────────────────────

bool GeometryCollection3D::AlmostEquals(GeometryCollection3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool GeometryCollection3D::AlmostEquals(GeometryCollection3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

// ── Serialization ─────────────────────────────────────────────────────────────

System::String^ GeometryCollection3D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

GeometryCollection3D^ GeometryCollection3D::FromWkt(System::String^ wkt) {
    return gcnew GeometryCollection3D(
        new geompp::GeometryCollection3D(
            geompp::GeometryCollection3D::FromWkt(marshal_as<std::string>(wkt))));
}

void GeometryCollection3D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

GeometryCollection3D^ GeometryCollection3D::FromFile(System::String^ path) {
    return gcnew GeometryCollection3D(
        new geompp::GeometryCollection3D(
            geompp::GeometryCollection3D::FromFile(marshal_as<std::string>(path))));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool GeometryCollection3D::operator==(GeometryCollection3D^ lhs, GeometryCollection3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ GeometryCollection3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
