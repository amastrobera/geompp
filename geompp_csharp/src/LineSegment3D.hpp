#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "line_segment3d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class Line3D;
ref class Ray3D;

public ref class LineSegment3D {
public:
    ~LineSegment3D();
    !LineSegment3D();

    // Factory method (private constructor — use Make)
    static LineSegment3D^ Make(Point3D^ p0, Point3D^ p1);

    Point3D^ First();
    Point3D^ Last();

    bool             AlmostEquals(LineSegment3D^ other);
    bool             AlmostEquals(LineSegment3D^ other, double epsilon);
    Line3D^          ToLine();
    double           Length();
    double           DistanceTo(Point3D^ point);
    double           Location(Point3D^ point);
    Point3D^         Interpolate(double pct);

    // Distance: directed segment expressing min distance; nullptr if intersection/overlap
    LineSegment3D^ Distance(Line3D^ line);
    LineSegment3D^ Distance(Ray3D^ ray);
    LineSegment3D^ Distance(LineSegment3D^ other);

    // DistanceTo: scalar distance; 0 if intersection/overlap
    double DistanceTo(Line3D^ line);
    double DistanceTo(Ray3D^ ray);
    double DistanceTo(LineSegment3D^ other);

    System::String^       ToWkt();
    static LineSegment3D^ FromWkt(System::String^ wkt);
    void                  ToFile(System::String^ path);
    static LineSegment3D^ FromFile(System::String^ path);

    bool Contains(Point3D^ point);

    // Intersects
    bool Intersects(Line3D^ line);
    bool Intersects(Ray3D^ ray);
    bool Intersects(LineSegment3D^ other);

    // Intersection — optional<variant<Point3D>> → Point3D^ (nullptr = no intersection)
    Point3D^ Intersection(Line3D^ line);
    Point3D^ Intersection(Ray3D^ ray);
    Point3D^ Intersection(LineSegment3D^ other);

    // Operator
    static bool operator==(LineSegment3D^ lhs, LineSegment3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    LineSegment3D(geompp::LineSegment3D* native);
    geompp::LineSegment3D* _native;
};

}  // namespace GeomPP
