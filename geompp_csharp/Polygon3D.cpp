#include "Polygon3D.hpp"
#include "Point3D.hpp"

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

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polygon3D::operator==(Polygon3D^ lhs, Polygon3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polygon3D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
