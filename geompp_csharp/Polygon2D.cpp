#include "Polygon2D.hpp"
#include "Point2D.hpp"

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

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polygon2D::operator==(Polygon2D^ lhs, Polygon2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polygon2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
