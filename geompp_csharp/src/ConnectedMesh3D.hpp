#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <connected_mesh3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Triangle3D;
ref class FaceView3D;
ref class PolyMesh3D;
ref class PolygonizationParams;

// A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.
// Per-facet edge adjacency is precomputed internally and exposed via the FaceView3D returned by
// indexing: FaceView3D::Neighbor()/NeighborEntryEdge().
public ref class ConnectedMesh3D {
public:
    ~ConnectedMesh3D();
    !ConnectedMesh3D();

    // Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell
    // into a single shared vertex.
    static ConnectedMesh3D^ FromTriangles(array<Triangle3D^>^ triangles);

    int Size();
    double Area();
    property FaceView3D^ default[int] { FaceView3D^ get(int i); }

    // Merges coplanar, edge-adjacent facets into polygons, per settings.Strategy.
    PolyMesh3D^ Polygonize(PolygonizationParams^ settings);

    virtual System::String^ ToString() override;

internal:
    ConnectedMesh3D(geompp::ConnectedMesh3D* native);
    geompp::ConnectedMesh3D* _native;
};

}  // namespace GeomPP
