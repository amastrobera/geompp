#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <mesh3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Triangle3D;
ref class ConnectedMesh3D;
ref class PolyMesh3D;
ref class PolygonizationParams;

// A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple. No
// adjacency structure is stored to find a face's neighbors.
public ref class Mesh3D {
public:
    ~Mesh3D();
    !Mesh3D();

    // Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell
    // into a single shared vertex.
    static Mesh3D^ FromTriangles(array<Triangle3D^>^ triangles);

    int Size();
    double Area();
    property Triangle3D^ default[int] { Triangle3D^ get(int i); }

    // Same facets/vertices, plus precomputed per-facet edge adjacency (see ConnectedMesh3D).
    ConnectedMesh3D^ Connect();

    // Merges coplanar, edge-adjacent facets into polygons, per settings.Strategy.
    PolyMesh3D^ Polygonize(PolygonizationParams^ settings);

    virtual System::String^ ToString() override;

internal:
    Mesh3D(geompp::Mesh3D* native);
    geompp::Mesh3D* _native;
};

}  // namespace GeomPP
