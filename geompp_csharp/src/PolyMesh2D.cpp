#include "PolyMesh2D.hpp"
#include "Polygon2D.hpp"
#include "Mesh2D.hpp"

namespace GeomPP {

PolyMesh2D::PolyMesh2D(geompp::PolyMesh2D* native) : _native(native) {}

PolyMesh2D::~PolyMesh2D() {
    delete _native;
    _native = nullptr;
}

PolyMesh2D::!PolyMesh2D() {
    delete _native;
    _native = nullptr;
}

PolyMesh2D^ PolyMesh2D::FromPolygons(array<Polygon2D^>^ polygons) {
    return FromPolygons(polygons, AdjacencyConformity::Assert);
}

PolyMesh2D^ PolyMesh2D::FromPolygons(array<Polygon2D^>^ polygons, AdjacencyConformity conformity) {
    std::vector<geompp::Polygon2D> nativePolygons;
    nativePolygons.reserve(polygons->Length);
    for each (Polygon2D^ p in polygons)
        nativePolygons.push_back(*p->_native);
    auto nativeConformity = static_cast<geompp::AdjacencyConformity>(conformity);
    return gcnew PolyMesh2D(new geompp::PolyMesh2D(geompp::PolyMesh2D::FromPolygons(nativePolygons, nativeConformity)));
}

int PolyMesh2D::Size() {
    return (int)_native->Size();
}

double PolyMesh2D::Area() {
    return _native->Area();
}

Polygon2D^ PolyMesh2D::default::get(int i) {
    return gcnew Polygon2D(new geompp::Polygon2D((*_native)[(std::size_t)i]));
}

Mesh2D^ PolyMesh2D::Triangulate() {
    return Triangulate(TriangulationStrategy::EarClippingBestFit);
}

Mesh2D^ PolyMesh2D::Triangulate(TriangulationStrategy strategy) {
    return gcnew Mesh2D(new geompp::Mesh2D(
        _native->Triangulate(static_cast<geompp::TriangulationParams::Strategy>(strategy))));
}

System::String^ PolyMesh2D::ToString() {
    std::string s = "PolyMesh2D[size=" + std::to_string(_native->Size())
                  + ", area=" + std::to_string(_native->Area()) + "]";
    return gcnew System::String(s.c_str());
}

}  // namespace GeomPP
