#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polygon2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Line2D;
ref class Ray2D;
ref class LineSegment2D;

public ref class Polygon2D {
public:
    ~Polygon2D();
    !Polygon2D();

    // Factory methods (private constructor — use Make)
    static Polygon2D^ Make(array<Point2D^>^ points);
    static Polygon2D^ Make(array<Point2D^>^ points, array<array<Point2D^>^>^ holes);

    int Size();
    property Point2D^ default[int] { Point2D^ get(int i); }

    bool     AlmostEquals(Polygon2D^ other);
    bool     AlmostEquals(Polygon2D^ other, double epsilon);
    Point2D^ Centroid();
    double   Area();
    double   Perimeter();
    double   DistanceTo(Point2D^ point);
    bool     Contains(Point2D^ point);
    bool     IsOnBoundary(Point2D^ point);
    bool     IsSimple();  // no self-intersections, but holes are allowed
    Polygon2D^           ConvexHull();
    array<Point2D^>^     ToPoints();

    System::String^ ToWkt();
    static Polygon2D^ FromWkt(System::String^ wkt);
    void              ToFile(System::String^ path);
    static Polygon2D^ FromFile(System::String^ path);

    // Intersects
    bool Intersects(Line2D^ line);
    bool Intersects(Ray2D^ ray);
    bool Intersects(LineSegment2D^ segment);

    // Intersection — optional<variant<Point2D>> → Point2D^ (nullptr = no intersection)
    Point2D^ Intersection(Line2D^ line);
    Point2D^ Intersection(Ray2D^ ray);
    Point2D^ Intersection(LineSegment2D^ segment);

    // Operator
    static bool operator==(Polygon2D^ lhs, Polygon2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polygon2D(geompp::Polygon2D* native);
    geompp::Polygon2D* _native;
};

}  // namespace GeomPP
