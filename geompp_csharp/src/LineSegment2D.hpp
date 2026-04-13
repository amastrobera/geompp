#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include "line_segment2d.hpp"
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Line2D;
ref class Ray2D;

public ref class LineSegment2D {
public:
    ~LineSegment2D();
    !LineSegment2D();

    // Factory method (private constructor — use Make)
    static LineSegment2D^ Make(Point2D^ p0, Point2D^ p1);

    Point2D^ First();
    Point2D^ Last();

    bool             AlmostEquals(LineSegment2D^ other);
    bool             AlmostEquals(LineSegment2D^ other, double epsilon);
    Line2D^          ToLine();
    double           Length();
    double           DistanceTo(Point2D^ point);
    double           Location(Point2D^ point);
    Point2D^         Interpolate(double pct);

    System::String^       ToWkt();
    static LineSegment2D^ FromWkt(System::String^ wkt);
    void                  ToFile(System::String^ path);
    static LineSegment2D^ FromFile(System::String^ path);

    bool Contains(Point2D^ point);

    // Intersects
    bool Intersects(Line2D^ line);
    bool Intersects(Ray2D^ ray);
    bool Intersects(LineSegment2D^ other);

    // Intersection — optional<variant<Point2D>> → Point2D^ (nullptr = no intersection)
    Point2D^ Intersection(Line2D^ line);
    Point2D^ Intersection(Ray2D^ ray);
    Point2D^ Intersection(LineSegment2D^ other);

    // Operator
    static bool operator==(LineSegment2D^ lhs, LineSegment2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    LineSegment2D(geompp::LineSegment2D* native);
    geompp::LineSegment2D* _native;
};

}  // namespace GeomPP
