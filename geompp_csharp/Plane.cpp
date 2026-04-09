#include "Plane.hpp"
#include "Point2D.hpp"
#include "Point3D.hpp"
#include "Vector3D.hpp"
#include "Line3D.hpp"

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Plane::Plane(geompp::Plane* native)
    : _native(native) {}

Plane::~Plane() {
    delete _native;
    _native = nullptr;
}

Plane::!Plane() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Plane^ Plane::From3Points(Point3D^ p1, Point3D^ p2, Point3D^ p3) {
    return gcnew Plane(
        new geompp::Plane(geompp::Plane::From3Points(*p1->_native, *p2->_native, *p3->_native)));
}

Plane^ Plane::FromOriginAndAxes(Point3D^ origin, Vector3D^ u, Vector3D^ v) {
    return gcnew Plane(
        new geompp::Plane(geompp::Plane::FromOriginAndAxes(*origin->_native, *u->_native, *v->_native)));
}

Plane^ Plane::FromOriginAndNormal(Point3D^ origin, Vector3D^ normal) {
    return gcnew Plane(
        new geompp::Plane(geompp::Plane::FromOriginAndNormal(*origin->_native, *normal->_native)));
}

// ── Properties ───────────────────────────────────────────────────────────────

Point3D^ Plane::Origin() {
    return gcnew Point3D(new geompp::Point3D(_native->origin()));
}

Vector3D^ Plane::Normal() {
    return gcnew Vector3D(new geompp::Vector3D(_native->normal()));
}

Vector3D^ Plane::AxisU() {
    return gcnew Vector3D(new geompp::Vector3D(_native->axis_u()));
}

Vector3D^ Plane::AxisV() {
    return gcnew Vector3D(new geompp::Vector3D(_native->axis_v()));
}

// ── Methods ──────────────────────────────────────────────────────────────────

bool Plane::AlmostEquals(Plane^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Plane::AlmostEquals(Plane^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

double Plane::SignedDistanceTo(Point3D^ point) {
    return _native->SignedDistanceTo(*point->_native);
}

double Plane::DistanceTo(Point3D^ point) {
    return _native->DistanceTo(*point->_native);
}

Point3D^ Plane::ProjectOnto(Point3D^ point) {
    return gcnew Point3D(new geompp::Point3D(_native->ProjectOnto(*point->_native)));
}

Point2D^ Plane::ProjectInto(Point3D^ point) {
    return gcnew Point2D(new geompp::Point2D(_native->ProjectInto(*point->_native)));
}

Point3D^ Plane::Evaluate(Point2D^ point) {
    return gcnew Point3D(new geompp::Point3D(_native->Evaluate(*point->_native)));
}

bool Plane::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

bool Plane::Intersects(Line3D^ line) {
    return _native->Intersects(*line->_native);
}

Point3D^ Plane::Intersection(Line3D^ line) {
    auto result = _native->Intersection(*line->_native);
    if (!result.has_value()) return nullptr;
    return gcnew Point3D(new geompp::Point3D(std::get<geompp::Point3D>(result.value())));
}

// ── Standard planes ───────────────────────────────────────────────────────────

Plane^ Plane::XY() {
    return gcnew Plane(new geompp::Plane(geompp::Plane::XY()));
}

Plane^ Plane::YZ() {
    return gcnew Plane(new geompp::Plane(geompp::Plane::YZ()));
}

Plane^ Plane::ZX() {
    return gcnew Plane(new geompp::Plane(geompp::Plane::ZX()));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Plane::operator==(Plane^ lhs, Plane^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Plane::ToString() {
    return gcnew System::String(
        ("Plane[origin=" + _native->origin().ToWkt() +
         ", normal=" + _native->normal().ToWkt() + "]").c_str());
}

}  // namespace GeomPP
