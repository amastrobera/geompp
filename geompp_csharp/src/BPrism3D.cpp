#include "BPrism3D.hpp"
#include "Point3D.hpp"
#include "Vector3D.hpp"

#include <string>

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

BPrism3D::BPrism3D(geompp::BPrism3D* native)
    : _native(native) {}

BPrism3D::BPrism3D(array<Point3D^>^ points) {
    std::vector<geompp::Point3D> native_pts;
    native_pts.reserve(points->Length);
    for each (Point3D^ p in points)
        native_pts.push_back(*p->_native);
    _native = new geompp::BPrism3D(native_pts);
}

BPrism3D::~BPrism3D() {
    delete _native;
    _native = nullptr;
}

BPrism3D::!BPrism3D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

Point3D^ BPrism3D::Center() {
    return gcnew Point3D(new geompp::Point3D(_native->center()));
}

Vector3D^ BPrism3D::AxisU() {
    return gcnew Vector3D(new geompp::Vector3D(_native->axis_u()));
}

Vector3D^ BPrism3D::AxisV() {
    return gcnew Vector3D(new geompp::Vector3D(_native->axis_v()));
}

Vector3D^ BPrism3D::AxisW() {
    return gcnew Vector3D(new geompp::Vector3D(_native->axis_w()));
}

double BPrism3D::HalfLenU() { return _native->half_len_u(); }
double BPrism3D::HalfLenV() { return _native->half_len_v(); }
double BPrism3D::HalfLenW() { return _native->half_len_w(); }
double BPrism3D::Width()    { return _native->width(); }
double BPrism3D::Height()   { return _native->height(); }
double BPrism3D::Depth()    { return _native->depth(); }
double BPrism3D::Volume()   { return _native->volume(); }

array<Point3D^>^ BPrism3D::Corners() {
    auto c = _native->Corners();
    auto arr = gcnew array<Point3D^>(8);
    for (int i = 0; i < 8; ++i) {
        arr[i] = gcnew Point3D(new geompp::Point3D(c[i]));
    }
    return arr;
}

bool BPrism3D::AlmostEquals(BPrism3D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool BPrism3D::AlmostEquals(BPrism3D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool BPrism3D::Contains(Point3D^ point) {
    return _native->Contains(*point->_native);
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool BPrism3D::operator==(BPrism3D^ lhs, BPrism3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ BPrism3D::ToString() {
    auto c = _native->center();
    std::string s = "BPrism3D[center=" + c.ToWkt()
                  + ", half_u=" + std::to_string(_native->half_len_u())
                  + ", half_v=" + std::to_string(_native->half_len_v())
                  + ", half_w=" + std::to_string(_native->half_len_w()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
