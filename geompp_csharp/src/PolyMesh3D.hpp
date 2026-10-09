#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polymesh3d.hpp>
#pragma managed(pop)

#include "GeomUtil.hpp"  // TriangulationStrategy

namespace GeomPP {

ref class Polygon3D;
ref class Mesh3D;

// A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer. No
// adjacency structure is stored to find a face's neighbors. Each facet has no holes.
public ref class PolyMesh3D {
public:
    ~PolyMesh3D();
    !PolyMesh3D();

    // Builds a mesh from a set of hole-free polygons, welding vertices that land in the same spatial
    // grid cell into a single shared vertex. Throws if any polygon has holes.
    static PolyMesh3D^ FromPolygons(array<Polygon3D^>^ polygons);
    // conformity (see AdjacencyConformity) decides how a mesh-conformity violation (a T-junction or
    // non-manifold edge) is handled: Assert raises, Enforce auto-repairs every T-junction via
    // FixAdjacency() (raising instead if any facet has holes), Guaranteed skips the check.
    static PolyMesh3D^ FromPolygons(array<Polygon3D^>^ polygons, AdjacencyConformity conformity);

    int Size();
    double Area();
    property Polygon3D^ default[int] { Polygon3D^ get(int i); }

    // Returns a mesh of triangles instead of n-gons — every facet is triangulated independently
    // (already simple/CCW/collinear-free by construction, so no re-validation) and combined into one
    // Mesh3D.
    Mesh3D^ Triangulate();
    Mesh3D^ Triangulate(TriangulationStrategy strategy);

    virtual System::String^ ToString() override;

internal:
    PolyMesh3D(geompp::PolyMesh3D* native);
    geompp::PolyMesh3D* _native;
};

}  // namespace GeomPP
