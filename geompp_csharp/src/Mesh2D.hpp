#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <mesh2d.hpp>
#pragma managed(pop)

#include "GeomUtil.hpp"  // AdjacencyConformity

namespace GeomPP {

ref class Triangle2D;
ref class ConnectedMesh2D;
ref class PolyMesh2D;
ref class PolygonizationParams;

// A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple. No
// adjacency structure is stored to find a face's neighbors.
public ref class Mesh2D {
public:
    ~Mesh2D();
    !Mesh2D();

    // Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell
    // into a single shared vertex.
    static Mesh2D^ FromTriangles(array<Triangle2D^>^ triangles);
    // conformity (see AdjacencyConformity) decides how a mesh-conformity violation (a T-junction or
    // non-manifold edge) is handled: Assert raises, Enforce auto-repairs every T-junction via
    // FixAdjacency(), Guaranteed skips the check.
    static Mesh2D^ FromTriangles(array<Triangle2D^>^ triangles, AdjacencyConformity conformity);

    int Size();
    double Area();
    property Triangle2D^ default[int] { Triangle2D^ get(int i); }

    // Same facets/vertices, plus precomputed per-facet edge adjacency (see ConnectedMesh2D).
    ConnectedMesh2D^ Connect();

    // Merges coplanar, edge-adjacent facets into polygons, per settings.Strategy.
    PolyMesh2D^ Polygonize(PolygonizationParams^ settings);

    virtual System::String^ ToString() override;

internal:
    Mesh2D(geompp::Mesh2D* native);
    geompp::Mesh2D* _native;
};

}  // namespace GeomPP
