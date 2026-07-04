#include "BBall2D.hpp"
#include "Point2D.hpp"

#include <string>

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

BBall2D::BBall2D(geompp::BBall2D* native)
    : _native(native) {}

BBall2D::BBall2D(Point2D^ center, double radius)
    : _native(new geompp::BBall2D(*center->_native, radius)) {}

BBall2D::BBall2D(array<Point2D^>^ points) {
    std::vector<geompp::Point2D> native_pts;
    native_pts.reserve(points->Length);
    for each (Point2D^ p in points)
        native_pts.push_back(*p->_native);
    _native = new geompp::BBall2D(native_pts);
}

BBall2D::~BBall2D() {
    delete _native;
    _native = nullptr;
}

BBall2D::!BBall2D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

Point2D^ BBall2D::Center() {
    return gcnew Point2D(new geompp::Point2D(_native->center()));
}

double BBall2D::Radius() {
    return _native->radius();
}

bool BBall2D::AlmostEquals(BBall2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool BBall2D::AlmostEquals(BBall2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool BBall2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool BBall2D::operator==(BBall2D^ lhs, BBall2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ BBall2D::ToString() {
    std::string s = "BBall2D[center=" + _native->center().ToWkt()
                  + ", radius=" + std::to_string(_native->radius()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
