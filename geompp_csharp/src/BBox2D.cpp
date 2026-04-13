#include "BBox2D.hpp"
#include "Point2D.hpp"
#include "LineSegment2D.hpp"
#include "Polyline2D.hpp"
#include "Polygon2D.hpp"
#include "Triangle2D.hpp"

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

BBox2D::BBox2D(geompp::BBox2D* native)
    : _native(native) {}

BBox2D::BBox2D(Point2D^ min, Point2D^ max)
    : _native(new geompp::BBox2D(*min->_native, *max->_native)) {}

BBox2D::BBox2D(LineSegment2D^ s)
    : _native(new geompp::BBox2D(*s->_native)) {}

BBox2D::BBox2D(Polyline2D^ s)
    : _native(new geompp::BBox2D(*s->_native)) {}

BBox2D::BBox2D(Polygon2D^ s)
    : _native(new geompp::BBox2D(*s->_native)) {}

BBox2D::BBox2D(Triangle2D^ s)
    : _native(new geompp::BBox2D(*s->_native)) {}

BBox2D::~BBox2D() {
    delete _native;
    _native = nullptr;
}

BBox2D::!BBox2D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

Point2D^ BBox2D::Min() {
    return gcnew Point2D(new geompp::Point2D(_native->min()));
}

Point2D^ BBox2D::Max() {
    return gcnew Point2D(new geompp::Point2D(_native->max()));
}

bool BBox2D::AlmostEquals(BBox2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool BBox2D::AlmostEquals(BBox2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool BBox2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool BBox2D::operator==(BBox2D^ lhs, BBox2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ BBox2D::ToString() {
    return gcnew System::String(
        ("BBox2D[" + _native->min().ToWkt() + ", " + _native->max().ToWkt() + "]").c_str());
}

}  // namespace GeomPP
