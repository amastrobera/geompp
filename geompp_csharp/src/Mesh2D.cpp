#include "Mesh2D.hpp"
#include "ConnectedMesh2D.hpp"
#include "GeomUtil.hpp"
#include "PolyMesh2D.hpp"
#include "Triangle2D.hpp"

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
    return FromTriangles(triangles, AdjacencyConformity::Assert);
}

Mesh2D^ Mesh2D::FromTriangles(array<Triangle2D^>^ triangles, AdjacencyConformity conformity) {
    std::vector<geompp::Triangle2D> nativeTriangles;
    nativeTriangles.reserve(triangles->Length);
    for each (Triangle2D^ t in triangles)
        nativeTriangles.push_back(*t->_native);
    auto nativeConformity = static_cast<geompp::AdjacencyConformity>(conformity);
    return gcnew Mesh2D(new geompp::Mesh2D(geompp::Mesh2D::FromTriangles(nativeTriangles, nativeConformity)));
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

PolyMesh2D^ Mesh2D::Polygonize(PolygonizationParams^ settings) {
    return gcnew PolyMesh2D(new geompp::PolyMesh2D(_native->Polygonize(settings->ToNative())));
}

System::String^ Mesh2D::ToString() {
    std::string s = "Mesh2D[size=" + std::to_string(_native->Size())
                  + ", area=" + std::to_string(_native->Area()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
