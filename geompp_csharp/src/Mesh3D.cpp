#include "Mesh3D.hpp"
#include "ConnectedMesh3D.hpp"
#include "GeomUtil.hpp"
#include "PolyMesh3D.hpp"
#include "Triangle3D.hpp"

namespace GeomPP {

Mesh3D::Mesh3D(geompp::Mesh3D* native) : _native(native) {}

Mesh3D::~Mesh3D() {
    delete _native;
    _native = nullptr;
}

Mesh3D::!Mesh3D() {
    delete _native;
    _native = nullptr;
}

Mesh3D^ Mesh3D::FromTriangles(array<Triangle3D^>^ triangles) {
    return FromTriangles(triangles, AdjacencyConformity::Assert);
}

Mesh3D^ Mesh3D::FromTriangles(array<Triangle3D^>^ triangles, AdjacencyConformity conformity) {
    std::vector<geompp::Triangle3D> nativeTriangles;
    nativeTriangles.reserve(triangles->Length);
    for each (Triangle3D^ t in triangles)
        nativeTriangles.push_back(*t->_native);
    auto nativeConformity = static_cast<geompp::AdjacencyConformity>(conformity);
    return gcnew Mesh3D(new geompp::Mesh3D(geompp::Mesh3D::FromTriangles(nativeTriangles, nativeConformity)));
}

int Mesh3D::Size() {
    return (int)_native->Size();
}

double Mesh3D::Area() {
    return _native->Area();
}

Triangle3D^ Mesh3D::default::get(int i) {
    return gcnew Triangle3D(new geompp::Triangle3D((*_native)[(std::size_t)i]));
}

ConnectedMesh3D^ Mesh3D::Connect() {
    return gcnew ConnectedMesh3D(new geompp::ConnectedMesh3D(_native->Connect()));
}

PolyMesh3D^ Mesh3D::Polygonize(PolygonizationParams^ settings) {
    return gcnew PolyMesh3D(new geompp::PolyMesh3D(_native->Polygonize(settings->ToNative())));
}

System::String^ Mesh3D::ToString() {
    std::string s = "Mesh3D[size=" + std::to_string(_native->Size())
                  + ", area=" + std::to_string(_native->Area()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
