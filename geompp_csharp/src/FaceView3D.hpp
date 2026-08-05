#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <connected_mesh3d.hpp>
#pragma managed(pop)

#include "FaceView2D.hpp"  // reuses the TriangleEdge enum, shared between ConnectedMesh2D/3D

namespace GeomPP {

ref class Triangle3D;
ref class ConnectedMesh3D;

// A lightweight, chainable view onto one facet of a ConnectedMesh3D: its geometry, plus topology
// navigation across its 3 edges via Neighbor()/NeighborEntryEdge(). Holds a reference to the
// ConnectedMesh3D it came from, so the mesh is kept alive for as long as any FaceView3D from it is.
public ref class FaceView3D {
public:
    ~FaceView3D();
    !FaceView3D();

    int Id();
    Triangle3D^ Geometry();

    // Chainable topology navigation, e.g. face->Neighbor(TriangleEdge::Third)->Geometry().
    // Returns nullptr if edge is a boundary edge (no twin).
    FaceView3D^ Neighbor(TriangleEdge edge);

    // Which edge of Neighbor(edge) was entered through, or TriangleEdge::Invalid at a boundary.
    TriangleEdge NeighborEntryEdge(TriangleEdge edge);

    virtual System::String^ ToString() override;

internal:
    FaceView3D(geompp::ConnectedMesh3D::FaceView3D* native, ConnectedMesh3D^ parentMesh);
    geompp::ConnectedMesh3D::FaceView3D* _native;
    ConnectedMesh3D^ _parentMesh;  // keeps the native mesh this face points into alive
};

}  // namespace GeomPP
