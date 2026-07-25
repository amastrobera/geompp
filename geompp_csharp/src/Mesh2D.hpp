#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <mesh2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Triangle2D;

// A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple. No
// adjacency structure is stored to find a face's neighbors.
public ref class Mesh2D {
public:
    ~Mesh2D();
    !Mesh2D();

    // Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell
    // into a single shared vertex.
    static Mesh2D^ FromTriangles(array<Triangle2D^>^ triangles);

    int Size();
    double Area();
    property Triangle2D^ default[int] { Triangle2D^ get(int i); }

    virtual System::String^ ToString() override;

internal:
    Mesh2D(geompp::Mesh2D* native);
    geompp::Mesh2D* _native;
};

}  // namespace GeomPP
