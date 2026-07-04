#pragma once

#pragma managed(push, off)
#include <view2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Plane;
ref class Point2D;
ref class Point3D;

public enum class ProjectionType {
    XY     = 0,
    YZ     = 1,
    ZX     = 2,
    Custom = 3
};

public ref class View2D {
public:
    ~View2D();
    !View2D();

    static View2D^ XY();
    static View2D^ YZ();
    static View2D^ ZX();
    static View2D^ OnPlane(Plane^ plane);

    ProjectionType Type();

    double X(Point2D^ point);
    double Y(Point2D^ point);

    double X(Point3D^ point);
    double Y(Point3D^ point);

internal:
    View2D(geompp::View2D* native);
    geompp::View2D* _native;
};

}  // namespace GeomPP
