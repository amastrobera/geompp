#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polygon2d.hpp>
#pragma managed(pop)

#include "GeomUtil.hpp"  // TriangulationStrategy

namespace GeomPP {

ref class Point2D;
ref class Line2D;
ref class Ray2D;
ref class LineSegment2D;
ref class Triangle2D;

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
    double   PerimeterSize();
    double   DistanceTo(Point2D^ point);
    bool     Contains(Point2D^ point);
    bool     IsOnPerimeter(Point2D^ point);
    bool     IsSimple();  // no self-intersections, but holes are allowed
    bool     IsConvex();
    Polygon2D^                ConvexHull();
    array<Polygon2D^>^        Simplify();
    // Breaks the polygon (outer ring only, holes are ignored) down into an array of Triangle2D. Make()
    // already guarantees the outer ring is simple/CCW/collinear-free, so this never re-validates.
    array<Triangle2D^>^ Triangulate();
    array<Triangle2D^>^ Triangulate(TriangulationStrategy strategy);
    array<Point2D^>^          Perimeter();
    bool                      HasHoles();
    array<array<Point2D^>^>^  Holes();

    System::String^ ToWkt();
    static Polygon2D^ FromWkt(System::String^ wkt);
    void              ToFile(System::String^ path);
    static Polygon2D^ FromFile(System::String^ path);

    // Intersects
    bool Intersects(Line2D^ line);
    bool Intersects(Ray2D^ ray);
    bool Intersects(LineSegment2D^ segment);
    // True if this polygon shares any area (or boundary) with other.
    bool Intersects(Polygon2D^ other);

    // Intersection — optional<variant<Point2D, vector<LineSegment2D>>>
    //   null                  → no intersection
    //   Point2D^              → single tangent point
    //   array<LineSegment2D^> → one or more chord segments
    System::Object^ Intersection(Line2D^ line);
    System::Object^ Intersection(Ray2D^ ray);
    System::Object^ Intersection(LineSegment2D^ segment);

    // Boolean operations — general map-overlay ops, holes and self-intersecting operands tolerant.
    // Each returns zero or more disjoint result polygons (a disjoint pair yields more than one).
    array<Polygon2D^>^ Intersection(Polygon2D^ other);
    array<Polygon2D^>^ Union(Polygon2D^ other);
    array<Polygon2D^>^ Difference(Polygon2D^ other);
    array<Polygon2D^>^ Xor(Polygon2D^ other);

    // Operator
    static bool operator==(Polygon2D^ lhs, Polygon2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polygon2D(geompp::Polygon2D* native);
    geompp::Polygon2D* _native;
};

}  // namespace GeomPP
