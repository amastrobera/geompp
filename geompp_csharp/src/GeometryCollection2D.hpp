#pragma once

#pragma managed(push, off)
#include <geometry_collection2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Line2D;
ref class Ray2D;
ref class LineSegment2D;
ref class Polyline2D;
ref class Triangle2D;
ref class Polygon2D;

public ref class GeometryCollection2D {
public:
    GeometryCollection2D();
    ~GeometryCollection2D();
    !GeometryCollection2D();

    int Size();

    void Add(Point2D^ point);
    void Add(Line2D^ line);
    void Add(LineSegment2D^ segment);
    void Add(Ray2D^ ray);
    void Add(Polyline2D^ polyline);
    void Add(Triangle2D^ triangle);
    void Add(Polygon2D^ polygon);
    void Add(GeometryCollection2D^ collection);

    System::Object^ Get(int index);

    bool AlmostEquals(GeometryCollection2D^ other);
    bool AlmostEquals(GeometryCollection2D^ other, double epsilon);

    System::String^ ToWkt();
    static GeometryCollection2D^ FromWkt(System::String^ wkt);
    void ToFile(System::String^ path);
    static GeometryCollection2D^ FromFile(System::String^ path);

    static bool operator==(GeometryCollection2D^ lhs, GeometryCollection2D^ rhs);
    virtual System::String^ ToString() override;

internal:
    GeometryCollection2D(geompp::GeometryCollection2D* native);
    geompp::GeometryCollection2D* _native;
};

}  // namespace GeomPP
