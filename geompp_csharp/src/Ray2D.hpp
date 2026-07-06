#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <ray2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Polyline2D;
ref class Vector2D;
ref class Line2D;
ref class LineSegment2D;

public ref class Ray2D {
public:
    ~Ray2D();
    !Ray2D();

    // Factory method (private constructor — use Make)
    static Ray2D^ Make(Point2D^ origin, Vector2D^ dir);

    Point2D^  Origin();
    Vector2D^ Direction();

    bool    AlmostEquals(Ray2D^ other);
    bool    AlmostEquals(Ray2D^ other, double epsilon);
    bool    IsAhead(Point2D^ point);
    bool    IsBehind(Point2D^ point);
    Line2D^ ToLine();
    double  DistanceTo(Point2D^ point);

    System::String^ ToWkt();
    static Ray2D^   FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Ray2D^   FromFile(System::String^ path);

    bool Contains(Point2D^ point);

    // Intersects
    bool Intersects(Line2D^ line);
    bool Intersects(Ray2D^ other);
    bool Intersects(LineSegment2D^ segment);

    // Intersection — optional<variant<Point2D>> → Point2D^ (nullptr = no intersection)
    Point2D^ Intersection(Line2D^ line);
    Point2D^ Intersection(Ray2D^ other);
    Point2D^ Intersection(LineSegment2D^ segment);

    // Overlaps — true if the two primitives share a 1D region (not just a point)
    bool Overlaps(Line2D^ line);
    bool Overlaps(Ray2D^ other);
    bool Overlaps(LineSegment2D^ segment);

    // Overlap — returns the shared geometry, or nullptr on miss / single-point touch
    // Overlap(Ray2D): returns Ray2D^ or LineSegment2D^ depending on direction; use 'is' pattern
    Ray2D^              Overlap(Line2D^ line);
    System::Object^     Overlap(Ray2D^ other);
    LineSegment2D^      Overlap(LineSegment2D^ segment);

    // Touches — true if they share exactly one endpoint-contact point
    bool Touches(Line2D^ line);
    bool Touches(Ray2D^ other);
    bool Touches(LineSegment2D^ segment);

    // Touch — returns the contact point, or nullptr if no touch
    Point2D^ Touch(Line2D^ line);
    Point2D^ Touch(Ray2D^ other);
    Point2D^ Touch(LineSegment2D^ segment);

    // Overlaps / Overlap / Touches / Touch — Polyline2D
    bool Overlaps(Polyline2D^ polyline);
    array<LineSegment2D^>^ Overlap(Polyline2D^ polyline);
    bool Touches(Polyline2D^ polyline);
    array<Point2D^>^ Touch(Polyline2D^ polyline);

    // Operator
    static bool operator==(Ray2D^ lhs, Ray2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Ray2D(geompp::Ray2D* native);
    geompp::Ray2D* _native;
};

}  // namespace GeomPP
