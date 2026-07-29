#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <connected_mesh2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Triangle2D;
ref class ConnectedMesh2D;

// Which of a triangular facet's 3 edges to cross via FaceView2D::Neighbor()/NeighborEntryEdge().
// Invalid is only ever returned (never passed in) — it marks a boundary edge with no twin.
public enum class TriangleEdge {
    Invalid = 0,
    First = 1,
    Second = 2,
    Third = 3
};

// A lightweight, chainable view onto one facet of a ConnectedMesh2D: its geometry, plus topology
// navigation across its 3 edges via Neighbor()/NeighborEntryEdge(). Holds a reference to the
// ConnectedMesh2D it came from, so the mesh is kept alive for as long as any FaceView2D from it is.
public ref class FaceView2D {
public:
    ~FaceView2D();
    !FaceView2D();

    int Id();
    Triangle2D^ Geometry();

    // Chainable topology navigation, e.g. face->Neighbor(TriangleEdge::Third)->Geometry().
    // Returns nullptr if edge is a boundary edge (no twin).
    FaceView2D^ Neighbor(TriangleEdge edge);

    // Which edge of Neighbor(edge) was entered through, or TriangleEdge::Invalid at a boundary.
    TriangleEdge NeighborEntryEdge(TriangleEdge edge);

    virtual System::String^ ToString() override;

internal:
    FaceView2D(geompp::ConnectedMesh2D::FaceView2D* native, ConnectedMesh2D^ parentMesh);
    geompp::ConnectedMesh2D::FaceView2D* _native;
    ConnectedMesh2D^ _parentMesh;  // keeps the native mesh this face points into alive
};

}  // namespace GeomPP
