#include "BBox3D.hpp"
#include "Point3D.hpp"

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

BBox3D::BBox3D(geompp::BBox3D* native)
    : _native(native) {}

BBox3D::BBox3D(Point3D^ min, Point3D^ max)
    : _native(new geompp::BBox3D(*min->_native, *max->_native)) {}

BBox3D::~BBox3D() {
    delete _native;
    _native = nullptr;
}

BBox3D::!BBox3D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

Point3D^ BBox3D::Min() {
    return gcnew Point3D(new geompp::Point3D(_native->min()));
}

Point3D^ BBox3D::Max() {
    return gcnew Point3D(new geompp::Point3D(_native->max()));
}

bool BBox3D::AlmostEquals(BBox3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool BBox3D::AlmostEquals(BBox3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool BBox3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool BBox3D::operator==(BBox3D^ lhs, BBox3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ BBox3D::ToString() {
    return gcnew System::String(
        ("BBox3D[" + _native->min().ToWkt() + ", " + _native->max().ToWkt() + "]").c_str());
}

}  // namespace GeomPP
