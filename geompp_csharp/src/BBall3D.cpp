#include "BBall3D.hpp"
#include "Point3D.hpp"

#include <string>

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

BBall3D::BBall3D(geompp::BBall3D* native)
    : _native(native) {}

BBall3D::BBall3D(Point3D^ center, double radius)
    : _native(new geompp::BBall3D(*center->_native, radius)) {}

BBall3D::BBall3D(array<Point3D^>^ points) {
    std::vector<geompp::Point3D> native_pts;
    native_pts.reserve(points->Length);
    for each (Point3D^ p in points)
        native_pts.push_back(*p->_native);
    _native = new geompp::BBall3D(native_pts);
}

BBall3D::~BBall3D() {
    delete _native;
    _native = nullptr;
}

BBall3D::!BBall3D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

Point3D^ BBall3D::Center() {
    return gcnew Point3D(new geompp::Point3D(_native->center()));
}

double BBall3D::Radius() {
    return _native->radius();
}

bool BBall3D::AlmostEquals(BBall3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool BBall3D::AlmostEquals(BBall3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool BBall3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool BBall3D::operator==(BBall3D^ lhs, BBall3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ BBall3D::ToString() {
    std::string s = "BBall3D[center=" + _native->center().ToWkt()
                  + ", radius=" + std::to_string(_native->radius()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
