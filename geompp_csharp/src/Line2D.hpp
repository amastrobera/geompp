#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <line2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Polyline2D;
ref class Vector2D;
ref class Ray2D;
ref class LineSegment2D;

public ref class Line2D {
public:
    ~Line2D();
    !Line2D();

    // Factory methods (private constructor — use Make)
    static Line2D^ Make(Point2D^ p0, Point2D^ p1);
    static Line2D^ Make(Point2D^ origin, Vector2D^ dir);

    Point2D^  First();
    Point2D^  Last();
    Point2D^  Origin();
    Vector2D^ Direction();

    bool            AlmostEquals(Line2D^ other);
    bool            AlmostEquals(Line2D^ other, double epsilon);
    double          DistanceTo(Point2D^ point);
    Point2D^        ProjectOnto(Point2D^ point);

    System::String^ ToWkt();
    static Line2D^  FromWkt(System::String^ wkt);
    void            ToFile(System::String^ path);
    static Line2D^  FromFile(System::String^ path);

    bool            Contains(Point2D^ point);

    // Intersects
    bool Intersects(Line2D^ other);
    bool Intersects(Ray2D^ ray);
    bool Intersects(LineSegment2D^ segment);

    // Intersection — optional<variant<Point2D>> → Point2D^ (nullptr = no intersection)
    Point2D^ Intersection(Line2D^ other);
    Point2D^ Intersection(Ray2D^ ray);
    Point2D^ Intersection(LineSegment2D^ segment);

    // Overlaps — true if the two primitives share a 1D region (not just a point)
    bool Overlaps(Line2D^ other);
    bool Overlaps(Ray2D^ ray);
    bool Overlaps(LineSegment2D^ segment);

    // Overlap — returns the shared geometry, or nullptr on miss / single-point touch
    Line2D^        Overlap(Line2D^ other);
    Ray2D^         Overlap(Ray2D^ ray);
    LineSegment2D^ Overlap(LineSegment2D^ segment);

    // Touches — true if they share exactly one endpoint-contact point
    bool Touches(Ray2D^ ray);
    bool Touches(LineSegment2D^ segment);

    // Touch — returns the contact point, or nullptr if no touch
    Point2D^ Touch(Ray2D^ ray);
    Point2D^ Touch(LineSegment2D^ segment);

    // Overlaps / Overlap / Touches / Touch — Polyline2D
    bool Overlaps(Polyline2D^ polyline);
    array<LineSegment2D^>^ Overlap(Polyline2D^ polyline);
    bool Touches(Polyline2D^ polyline);
    array<Point2D^>^ Touch(Polyline2D^ polyline);

    // Operator
    static bool operator==(Line2D^ lhs, Line2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Line2D(geompp::Line2D* native);
    geompp::Line2D* _native;
};

}  // namespace GeomPP
