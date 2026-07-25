#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polymesh3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Polygon3D;

// A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer. No
// adjacency structure is stored to find a face's neighbors. Each facet has no holes.
public ref class PolyMesh3D {
public:
    ~PolyMesh3D();
    !PolyMesh3D();

    // Builds a mesh from a set of hole-free polygons, welding vertices that land in the same spatial
    // grid cell into a single shared vertex. Throws if any polygon has holes.
    static PolyMesh3D^ FromPolygons(array<Polygon3D^>^ polygons);

    int Size();
    double Area();
    property Polygon3D^ default[int] { Polygon3D^ get(int i); }

    virtual System::String^ ToString() override;

internal:
    PolyMesh3D(geompp::PolyMesh3D* native);
    geompp::PolyMesh3D* _native;
};

}  // namespace GeomPP
