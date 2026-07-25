#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <grid_cell3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;

// Axis-aligned spatial hash bucket: the integer grid cell a Point3D falls into at a given resolution.
// Used internally to weld near-duplicate mesh vertices (see Mesh3D/PolyMesh3D) — exposed for
// completeness, not typically constructed directly by user code.
public ref class GridCell3D {
public:
    ~GridCell3D();
    !GridCell3D();

    static GridCell3D^ FromPoint(Point3D^ point);
    static GridCell3D^ FromPoint(Point3D^ point, double epsilon);

    property System::Int64 X { System::Int64 get(); }
    property System::Int64 Y { System::Int64 get(); }
    property System::Int64 Z { System::Int64 get(); }

    static bool operator==(GridCell3D^ lhs, GridCell3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    GridCell3D(geompp::GridCell3D* native);
    geompp::GridCell3D* _native;
};

}  // namespace GeomPP
