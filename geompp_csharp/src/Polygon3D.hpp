#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polygon3d.hpp>
#pragma managed(pop)

#include "GeomUtil.hpp"  // TriangulationStrategy

namespace GeomPP {

ref class Plane;
ref class Point3D;
ref class Line3D;
ref class Ray3D;
ref class LineSegment3D;
ref class Triangle3D;

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
    double   PerimeterSize();
    bool     IsSimple();
    bool     IsConvex();
    double   DistanceTo(Point3D^ point);
    bool     Contains(Point3D^ point);
    bool     IsOnPerimeter(Point3D^ point);
    Polygon3D^                ConvexHull();
    array<Polygon3D^>^        Simplify();
    // Breaks the polygon (outer ring only, holes are ignored) down into an array of Triangle3D, in the
    // polygon's own plane. Make() already guarantees the outer ring is simple/CCW/collinear-free, so
    // this never re-validates.
    array<Triangle3D^>^ Triangulate();
    array<Triangle3D^>^ Triangulate(TriangulationStrategy strategy);
    array<Point3D^>^          Perimeter();
    bool                      HasHoles();
    array<array<Point3D^>^>^  Holes();

    System::String^ ToWkt();
    static Polygon3D^ FromWkt(System::String^ wkt);
    void              ToFile(System::String^ path);
    static Polygon3D^ FromFile(System::String^ path);

    // Intersects
    bool Intersects(Line3D^ line);
    bool Intersects(Ray3D^ ray);
    bool Intersects(LineSegment3D^ segment);
    // Area overlap when coplanar, or a genuine strike-through (shared segment on the two planes'
    // common line) when not.
    bool Intersects(Polygon3D^ other);

    // Intersection — optional<variant<Point3D>> → Point3D^ (nullptr = no intersection)
    Point3D^ Intersection(Line3D^ line);
    Point3D^ Intersection(Ray3D^ ray);
    Point3D^ Intersection(LineSegment3D^ segment);

    // Intersection — optional<variant<vector<Polygon3D>, vector<LineSegment3D>>>
    //   null                  → no shared point
    //   array<Polygon3D^>^    → coplanar: set intersection of the two areas
    //   array<LineSegment3D^> → planes cross: the chord(s) where both bounded regions cover the shared line
    System::Object^ Intersection(Polygon3D^ other);

    // Boolean operations — coplanar only; throws if GetPlane() doesn't match other's.
    array<Polygon3D^>^ Union(Polygon3D^ other);
    array<Polygon3D^>^ Difference(Polygon3D^ other);
    array<Polygon3D^>^ Xor(Polygon3D^ other);

    // Operator
    static bool operator==(Polygon3D^ lhs, Polygon3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polygon3D(geompp::Polygon3D* native);
    geompp::Polygon3D* _native;
};

}  // namespace GeomPP
