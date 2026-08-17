#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <connected_mesh2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Triangle2D;
ref class FaceView2D;
ref class PolyMesh2D;
ref class PolygonizationParams;

// A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.
// Per-facet edge adjacency is precomputed internally and exposed via the FaceView2D returned by
// indexing: FaceView2D::Neighbor()/NeighborEntryEdge().
public ref class ConnectedMesh2D {
public:
    ~ConnectedMesh2D();
    !ConnectedMesh2D();

    // Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell
    // into a single shared vertex.
    static ConnectedMesh2D^ FromTriangles(array<Triangle2D^>^ triangles);

    int Size();
    double Area();
    property FaceView2D^ default[int] { FaceView2D^ get(int i); }

    // Merges coplanar, edge-adjacent facets into polygons, per settings.Strategy.
    PolyMesh2D^ Polygonize(PolygonizationParams^ settings);

    virtual System::String^ ToString() override;

internal:
    ConnectedMesh2D(geompp::ConnectedMesh2D* native);
    geompp::ConnectedMesh2D* _native;
};

}  // namespace GeomPP
