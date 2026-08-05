#include "GridCell2D.hpp"
#include "Point2D.hpp"

namespace GeomPP {

GridCell2D::GridCell2D(geompp::GridCell2D* native) : _native(native) {}

GridCell2D::~GridCell2D() {
    delete _native;
    _native = nullptr;
}

GridCell2D::!GridCell2D() {
    delete _native;
    _native = nullptr;
}

GridCell2D^ GridCell2D::FromPoint(Point2D^ point) {
    return gcnew GridCell2D(new geompp::GridCell2D(geompp::GridCell2D::FromPoint(*point->_native)));
}

GridCell2D^ GridCell2D::FromPoint(Point2D^ point, double epsilon) {
    return gcnew GridCell2D(new geompp::GridCell2D(geompp::GridCell2D::FromPoint(*point->_native, epsilon)));
}

System::Int64 GridCell2D::X::get() { return _native->x; }
System::Int64 GridCell2D::Y::get() { return _native->y; }

bool GridCell2D::operator==(GridCell2D^ lhs, GridCell2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ GridCell2D::ToString() {
    std::string s = "GridCell2D[" + std::to_string(_native->x) + ", " + std::to_string(_native->y) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
