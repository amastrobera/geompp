#include "View2D.hpp"
#include "Plane.hpp"
#include "Point2D.hpp"
#include "Point3D.hpp"

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

View2D::View2D(geompp::View2D* native)
    : _native(native) {}

View2D::~View2D() {
    delete _native;
    _native = nullptr;
}

View2D::!View2D() {
    delete _native;
    _native = nullptr;
}

// ── Factories ─────────────────────────────────────────────────────────────────

View2D^ View2D::XY() {
    return gcnew View2D(new geompp::View2D(geompp::View2D::XY()));
}

View2D^ View2D::YZ() {
    return gcnew View2D(new geompp::View2D(geompp::View2D::YZ()));
}

View2D^ View2D::ZX() {
    return gcnew View2D(new geompp::View2D(geompp::View2D::ZX()));
}

View2D^ View2D::OnPlane(Plane^ plane) {
    return gcnew View2D(new geompp::View2D(geompp::View2D::OnPlane(*plane->_native)));
}

// ── Properties ────────────────────────────────────────────────────────────────

ProjectionType View2D::Type() {
    return static_cast<ProjectionType>(_native->type());
}

// ── Coordinate getters ────────────────────────────────────────────────────────

double View2D::X(Point2D^ point) {
    return _native->x(*point->_native);
}

double View2D::Y(Point2D^ point) {
    return _native->y(*point->_native);
}

double View2D::X(Point3D^ point) {
    return _native->x(*point->_native);
}

double View2D::Y(Point3D^ point) {
    return _native->y(*point->_native);
}

}  // namespace GeomPP
