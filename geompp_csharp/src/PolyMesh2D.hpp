#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polymesh2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Polygon2D;

// A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer. No
// adjacency structure is stored to find a face's neighbors. Each facet has no holes.
public ref class PolyMesh2D {
public:
    ~PolyMesh2D();
    !PolyMesh2D();

    // Builds a mesh from a set of hole-free polygons, welding vertices that land in the same spatial
    // grid cell into a single shared vertex. Throws if any polygon has holes.
    static PolyMesh2D^ FromPolygons(array<Polygon2D^>^ polygons);

    int Size();
    double Area();
    property Polygon2D^ default[int] { Polygon2D^ get(int i); }

    virtual System::String^ ToString() override;

internal:
    PolyMesh2D(geompp::PolyMesh2D* native);
    geompp::PolyMesh2D* _native;
};

}  // namespace GeomPP
