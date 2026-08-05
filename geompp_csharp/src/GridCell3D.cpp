#include "GridCell3D.hpp"
#include "Point3D.hpp"

namespace GeomPP {

GridCell3D::GridCell3D(geompp::GridCell3D* native) : _native(native) {}

GridCell3D::~GridCell3D() {
    delete _native;
    _native = nullptr;
}

GridCell3D::!GridCell3D() {
    delete _native;
    _native = nullptr;
}

GridCell3D^ GridCell3D::FromPoint(Point3D^ point) {
    return gcnew GridCell3D(new geompp::GridCell3D(geompp::GridCell3D::FromPoint(*point->_native)));
}

GridCell3D^ GridCell3D::FromPoint(Point3D^ point, double epsilon) {
    return gcnew GridCell3D(new geompp::GridCell3D(geompp::GridCell3D::FromPoint(*point->_native, epsilon)));
}

System::Int64 GridCell3D::X::get() { return _native->x; }
System::Int64 GridCell3D::Y::get() { return _native->y; }
System::Int64 GridCell3D::Z::get() { return _native->z; }

bool GridCell3D::operator==(GridCell3D^ lhs, GridCell3D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ GridCell3D::ToString() {
    std::string s = "GridCell3D[" + std::to_string(_native->x) + ", " + std::to_string(_native->y)
                  + ", " + std::to_string(_native->z) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
