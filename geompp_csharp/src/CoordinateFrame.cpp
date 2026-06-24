#include "CoordinateFrame.hpp"
#include "Vector3D.hpp"

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

CoordinateFrame::CoordinateFrame(geompp::Vector3D* x, geompp::Vector3D* y, geompp::Vector3D* z)
    : _x(x), _y(y), _z(z) {}

CoordinateFrame::~CoordinateFrame() {
    delete _x; _x = nullptr;
    delete _y; _y = nullptr;
    delete _z; _z = nullptr;
}

CoordinateFrame::!CoordinateFrame() {
    delete _x; _x = nullptr;
    delete _y; _y = nullptr;
    delete _z; _z = nullptr;
}

// ── Properties ───────────────────────────────────────────────────────────────

Vector3D^ CoordinateFrame::X::get() {
    return gcnew Vector3D(new geompp::Vector3D(*_x));
}

Vector3D^ CoordinateFrame::Y::get() {
    return gcnew Vector3D(new geompp::Vector3D(*_y));
}

Vector3D^ CoordinateFrame::Z::get() {
    return gcnew Vector3D(new geompp::Vector3D(*_z));
}

// ── ToString ─────────────────────────────────────────────────────────────────

System::String^ CoordinateFrame::ToString() {
    std::string s = "CoordinateFrame[X=" + _x->ToWkt() +
                    ", Y=" + _y->ToWkt() +
                    ", Z=" + _z->ToWkt() + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
