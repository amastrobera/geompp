#include "Mesh2D.hpp"
#include "Triangle2D.hpp"
#include "ConnectedMesh2D.hpp"

namespace GeomPP {

Mesh2D::Mesh2D(geompp::Mesh2D* native) : _native(native) {}

Mesh2D::~Mesh2D() {
    delete _native;
    _native = nullptr;
}

Mesh2D::!Mesh2D() {
    delete _native;
    _native = nullptr;
}

Mesh2D^ Mesh2D::FromTriangles(array<Triangle2D^>^ triangles) {
    std::vector<geompp::Triangle2D> nativeTriangles;
    nativeTriangles.reserve(triangles->Length);
    for each (Triangle2D^ t in triangles)
        nativeTriangles.push_back(*t->_native);
    return gcnew Mesh2D(new geompp::Mesh2D(geompp::Mesh2D::FromTriangles(nativeTriangles)));
}

int Mesh2D::Size() {
    return (int)_native->Size();
}

double Mesh2D::Area() {
    return _native->Area();
}

Triangle2D^ Mesh2D::default::get(int i) {
    return gcnew Triangle2D(new geompp::Triangle2D((*_native)[(std::size_t)i]));
}

ConnectedMesh2D^ Mesh2D::Connect() {
    return gcnew ConnectedMesh2D(new geompp::ConnectedMesh2D(_native->Connect()));
}

System::String^ Mesh2D::ToString() {
    std::string s = "Mesh2D[size=" + std::to_string(_native->Size())
                  + ", area=" + std::to_string(_native->Area()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
