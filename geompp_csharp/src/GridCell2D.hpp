#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <grid_cell2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;

// Axis-aligned spatial hash bucket: the integer grid cell a Point2D falls into at a given resolution.
// Used internally to weld near-duplicate mesh vertices (see Mesh2D/PolyMesh2D) — exposed for
// completeness, not typically constructed directly by user code.
public ref class GridCell2D {
public:
    ~GridCell2D();
    !GridCell2D();

    static GridCell2D^ FromPoint(Point2D^ point);
    static GridCell2D^ FromPoint(Point2D^ point, double epsilon);

    property System::Int64 X { System::Int64 get(); }
    property System::Int64 Y { System::Int64 get(); }

    static bool operator==(GridCell2D^ lhs, GridCell2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    GridCell2D(geompp::GridCell2D* native);
    geompp::GridCell2D* _native;
};

}  // namespace GeomPP
