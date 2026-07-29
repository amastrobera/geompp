#include "ConnectedMesh2D.hpp"
#include "Triangle2D.hpp"
#include "FaceView2D.hpp"

namespace GeomPP {

ConnectedMesh2D::ConnectedMesh2D(geompp::ConnectedMesh2D* native) : _native(native) {}

ConnectedMesh2D::~ConnectedMesh2D() {
    delete _native;
    _native = nullptr;
}

ConnectedMesh2D::!ConnectedMesh2D() {
    delete _native;
    _native = nullptr;
}

ConnectedMesh2D^ ConnectedMesh2D::FromTriangles(array<Triangle2D^>^ triangles) {
    std::vector<geompp::Triangle2D> nativeTriangles;
    nativeTriangles.reserve(triangles->Length);
    for each (Triangle2D^ t in triangles)
        nativeTriangles.push_back(*t->_native);
    return gcnew ConnectedMesh2D(new geompp::ConnectedMesh2D(geompp::ConnectedMesh2D::FromTriangles(nativeTriangles)));
}

int ConnectedMesh2D::Size() {
    return (int)_native->Size();
}

double ConnectedMesh2D::Area() {
    return _native->Area();
}

FaceView2D^ ConnectedMesh2D::default::get(int i) {
    return gcnew FaceView2D(new geompp::ConnectedMesh2D::FaceView2D((*_native)[(std::size_t)i]), this);
}

System::String^ ConnectedMesh2D::ToString() {
    std::string s = "ConnectedMesh2D[size=" + std::to_string(_native->Size())
                  + ", area=" + std::to_string(_native->Area()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
