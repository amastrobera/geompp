#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "line3d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class Vector3D;
ref class Ray3D;
ref class LineSegment3D;

public ref class Line3D {
public:
    ~Line3D();
    !Line3D();

    // Factory methods (private constructor — use Make)
    static Line3D^ Make(Point3D^ p0, Point3D^ p1);
    static Line3D^ Make(Point3D^ origin, Vector3D^ dir);

    Point3D^  First();
    Point3D^  Last();
    Point3D^  Origin();
    Vector3D^ Direction();

    bool            AlmostEquals(Line3D^ other);
    bool            AlmostEquals(Line3D^ other, double epsilon);
    double          DistanceTo(Point3D^ point);
    Point3D^        ProjectOnto(Point3D^ point);
    double          Location(Point3D^ point);

    System::String^ ToWkt();
    static Line3D^  FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Line3D^  FromFile(System::String^ path);

    bool            Contains(Point3D^ point);

    // Intersects
    bool Intersects(Line3D^ other);
    bool Intersects(Ray3D^ ray);
    bool Intersects(LineSegment3D^ segment);

    // Intersection — optional<variant<Point3D>> → Point3D^ (nullptr = no intersection)
    Point3D^ Intersection(Line3D^ other);
    Point3D^ Intersection(Ray3D^ ray);
    Point3D^ Intersection(LineSegment3D^ segment);

    // Operator
    static bool operator==(Line3D^ lhs, Line3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Line3D(geompp::Line3D* native);
    geompp::Line3D* _native;
};

}  // namespace GeomPP
