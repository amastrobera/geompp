#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polygon3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Plane;
ref class Point3D;
ref class Line3D;
ref class Ray3D;
ref class LineSegment3D;

public ref class Polygon3D {
public:
    ~Polygon3D();
    !Polygon3D();

    // Factory methods (private constructor — use Make)
    static Polygon3D^ Make(array<Point3D^>^ points);
    static Polygon3D^ Make(array<Point3D^>^ points, array<array<Point3D^>^>^ holes);

    int Size();
    property Point3D^ default[int] { Point3D^ get(int i); }
    Plane^   GetPlane();

    bool     AlmostEquals(Polygon3D^ other);
    bool     AlmostEquals(Polygon3D^ other, double epsilon);
    Point3D^ Centroid();
    double   Area();
    double   Perimeter();
    double   DistanceTo(Point3D^ point);
    double   Location(Point3D^ point);
    Point3D^ Interpolate(double pct);
    bool     Contains(Point3D^ point);

    System::String^ ToWkt();
    static Polygon3D^ FromWkt(System::String^ wkt);
    void              ToFile(System::String^ path);
    static Polygon3D^ FromFile(System::String^ path);

    // Intersects
    bool Intersects(Line3D^ line);
    bool Intersects(Ray3D^ ray);
    bool Intersects(LineSegment3D^ segment);

    // Intersection — optional<variant<Point3D>> → Point3D^ (nullptr = no intersection)
    Point3D^ Intersection(Line3D^ line);
    Point3D^ Intersection(Ray3D^ ray);
    Point3D^ Intersection(LineSegment3D^ segment);

    // Operator
    static bool operator==(Polygon3D^ lhs, Polygon3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polygon3D(geompp::Polygon3D* native);
    geompp::Polygon3D* _native;
};

}  // namespace GeomPP
