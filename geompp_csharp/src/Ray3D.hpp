#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <ray3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class Vector3D;
ref class Line3D;
ref class LineSegment3D;

public ref class Ray3D {
public:
    ~Ray3D();
    !Ray3D();

    // Factory method (private constructor — use Make)
    static Ray3D^ Make(Point3D^ origin, Vector3D^ dir);

    Point3D^  Origin();
    Vector3D^ Direction();

    bool    AlmostEquals(Ray3D^ other);
    bool    AlmostEquals(Ray3D^ other, double epsilon);
    bool    IsAhead(Point3D^ point);
    bool    IsBehind(Point3D^ point);
    Line3D^ ToLine();
    double  DistanceTo(Point3D^ point);

    // Distance: directed segment expressing min distance; nullptr if intersection/overlap
    LineSegment3D^ Distance(Line3D^ line);
    LineSegment3D^ Distance(Ray3D^ other);
    LineSegment3D^ Distance(LineSegment3D^ segment);

    // DistanceTo: scalar distance; 0 if intersection/overlap
    double DistanceTo(Line3D^ line);
    double DistanceTo(Ray3D^ other);
    double DistanceTo(LineSegment3D^ segment);

    System::String^ ToWkt();
    static Ray3D^   FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Ray3D^   FromFile(System::String^ path);

    bool Contains(Point3D^ point);

    // Intersects
    bool Intersects(Line3D^ line);
    bool Intersects(Ray3D^ other);
    bool Intersects(LineSegment3D^ segment);

    // Intersection — optional<variant<Point3D>> → Point3D^ (nullptr = no intersection)
    Point3D^ Intersection(Line3D^ line);
    Point3D^ Intersection(Ray3D^ other);
    Point3D^ Intersection(LineSegment3D^ segment);

    // Operator
    static bool operator==(Ray3D^ lhs, Ray3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Ray3D(geompp::Ray3D* native);
    geompp::Ray3D* _native;
};

}  // namespace GeomPP
