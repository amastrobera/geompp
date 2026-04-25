#pragma once

#pragma managed(push, off)
#include <geometry_collection3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class Line3D;
ref class Ray3D;
ref class LineSegment3D;
ref class Polyline3D;
ref class Triangle3D;
ref class Polygon3D;

public ref class GeometryCollection3D {
public:
    GeometryCollection3D();
    ~GeometryCollection3D();
    !GeometryCollection3D();

    int Size();

    void Add(Point3D^ point);
    void Add(Line3D^ line);
    void Add(LineSegment3D^ segment);
    void Add(Ray3D^ ray);
    void Add(Polyline3D^ polyline);
    void Add(Triangle3D^ triangle);
    void Add(Polygon3D^ polygon);
    void Add(GeometryCollection3D^ collection);

    System::Object^ Get(int index);

    bool AlmostEquals(GeometryCollection3D^ other);
    bool AlmostEquals(GeometryCollection3D^ other, double epsilon);

    System::String^ ToWkt();
    static GeometryCollection3D^ FromWkt(System::String^ wkt);
    void ToFile(System::String^ path);
    static GeometryCollection3D^ FromFile(System::String^ path);

    static bool operator==(GeometryCollection3D^ lhs, GeometryCollection3D^ rhs);
    virtual System::String^ ToString() override;

internal:
    GeometryCollection3D(geompp::GeometryCollection3D* native);
    geompp::GeometryCollection3D* _native;
};

}  // namespace GeomPP
