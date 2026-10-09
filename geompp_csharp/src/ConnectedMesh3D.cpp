#include "ConnectedMesh3D.hpp"
#include "FaceView3D.hpp"
#include "GeomUtil.hpp"
#include "PolyMesh3D.hpp"
#include "Triangle3D.hpp"

namespace GeomPP {

ConnectedMesh3D::ConnectedMesh3D(geompp::ConnectedMesh3D* native) : _native(native) {}

ConnectedMesh3D::~ConnectedMesh3D() {
    delete _native;
    _native = nullptr;
}

ConnectedMesh3D::!ConnectedMesh3D() {
    delete _native;
    _native = nullptr;
}

ConnectedMesh3D^ ConnectedMesh3D::FromTriangles(array<Triangle3D^>^ triangles) {
    std::vector<geompp::Triangle3D> nativeTriangles;
    nativeTriangles.reserve(triangles->Length);
    for each (Triangle3D^ t in triangles)
        nativeTriangles.push_back(*t->_native);
    return gcnew ConnectedMesh3D(new geompp::ConnectedMesh3D(geompp::ConnectedMesh3D::FromTriangles(nativeTriangles)));
}

int ConnectedMesh3D::Size() {
    return (int)_native->Size();
}

double ConnectedMesh3D::Area() {
    return _native->Area();
}

FaceView3D^ ConnectedMesh3D::default::get(int i) {
    return gcnew FaceView3D(new geompp::ConnectedMesh3D::FaceView3D((*_native)[(std::size_t)i]), this);
}

PolyMesh3D^ ConnectedMesh3D::Polygonize(PolygonizationParams^ settings) {
    return gcnew PolyMesh3D(new geompp::PolyMesh3D(_native->Polygonize(settings->ToNative())));
}

System::String^ ConnectedMesh3D::ToString() {
    std::string s = "ConnectedMesh3D[size=" + std::to_string(_native->Size())
                  + ", area=" + std::to_string(_native->Area()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
