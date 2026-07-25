#include "PolyMesh3D.hpp"
#include "Polygon3D.hpp"

namespace GeomPP {

PolyMesh3D::PolyMesh3D(geompp::PolyMesh3D* native) : _native(native) {}

PolyMesh3D::~PolyMesh3D() {
    delete _native;
    _native = nullptr;
}

PolyMesh3D::!PolyMesh3D() {
    delete _native;
    _native = nullptr;
}

PolyMesh3D^ PolyMesh3D::FromPolygons(array<Polygon3D^>^ polygons) {
    std::vector<geompp::Polygon3D> nativePolygons;
    nativePolygons.reserve(polygons->Length);
    for each (Polygon3D^ p in polygons)
        nativePolygons.push_back(*p->_native);
    return gcnew PolyMesh3D(new geompp::PolyMesh3D(geompp::PolyMesh3D::FromPolygons(nativePolygons)));
}

int PolyMesh3D::Size() {
    return (int)_native->Size();
}

double PolyMesh3D::Area() {
    return _native->Area();
}

Polygon3D^ PolyMesh3D::default::get(int i) {
    return gcnew Polygon3D(new geompp::Polygon3D((*_native)[(std::size_t)i]));
}

System::String^ PolyMesh3D::ToString() {
    std::string s = "PolyMesh3D[size=" + std::to_string(_native->Size())
                  + ", area=" + std::to_string(_native->Area()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
