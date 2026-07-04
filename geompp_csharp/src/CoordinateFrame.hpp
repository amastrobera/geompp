#pragma once

#pragma managed(push, off)
#include <vector3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Vector3D;

public ref class CoordinateFrame {
public:
    ~CoordinateFrame();
    !CoordinateFrame();

    property Vector3D^ X { Vector3D^ get(); }
    property Vector3D^ Y { Vector3D^ get(); }
    property Vector3D^ Z { Vector3D^ get(); }

    virtual System::String^ ToString() override;

internal:
    CoordinateFrame(geompp::Vector3D* x, geompp::Vector3D* y, geompp::Vector3D* z);
    geompp::Vector3D* _x;
    geompp::Vector3D* _y;
    geompp::Vector3D* _z;
};

}  // namespace GeomPP
