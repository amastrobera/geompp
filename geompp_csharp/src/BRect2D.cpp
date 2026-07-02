#include "BRect2D.hpp"
#include "Point2D.hpp"
#include "Vector2D.hpp"

#include <string>

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

BRect2D::BRect2D(geompp::BRect2D* native)
    : _native(native) {}

BRect2D::BRect2D(array<Point2D^>^ points) {
    std::vector<geompp::Point2D> native_pts;
    native_pts.reserve(points->Length);
    for each (Point2D^ p in points)
        native_pts.push_back(*p->_native);
    _native = new geompp::BRect2D(native_pts);
}

BRect2D::~BRect2D() {
    delete _native;
    _native = nullptr;
}

BRect2D::!BRect2D() {
    delete _native;
    _native = nullptr;
}

// ── Methods ──────────────────────────────────────────────────────────────────

Point2D^ BRect2D::Center() {
    return gcnew Point2D(new geompp::Point2D(_native->center()));
}

Vector2D^ BRect2D::AxisU() {
    return gcnew Vector2D(new geompp::Vector2D(_native->axis_u()));
}

Vector2D^ BRect2D::AxisV() {
    return gcnew Vector2D(new geompp::Vector2D(_native->axis_v()));
}

double BRect2D::HalfLenU() { return _native->half_len_u(); }
double BRect2D::HalfLenV() { return _native->half_len_v(); }
double BRect2D::Width()    { return _native->width(); }
double BRect2D::Height()   { return _native->height(); }
double BRect2D::Area()     { return _native->area(); }

array<Point2D^>^ BRect2D::Corners() {
    auto c = _native->Corners();
    auto arr = gcnew array<Point2D^>(4);
    for (int i = 0; i < 4; ++i)
        arr[i] = gcnew Point2D(new geompp::Point2D(c[i]));
    return arr;
}

bool BRect2D::AlmostEquals(BRect2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool BRect2D::AlmostEquals(BRect2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

bool BRect2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool BRect2D::operator==(BRect2D^ lhs, BRect2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ BRect2D::ToString() {
    auto c = _native->center();
    std::string s = "BRect2D[center=" + c.ToWkt()
                  + ", half_u=" + std::to_string(_native->half_len_u())
                  + ", half_v=" + std::to_string(_native->half_len_v()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
