#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polyline3d.hpp>
#pragma managed(pop)

// Reuses the PolylineDecimationStrategy enum declared alongside Polyline2D.
#include "Polyline2D.hpp"

namespace GeomPP {

ref class Point3D;
ref class Line3D;
ref class Ray3D;
ref class LineSegment3D;
ref class Polygon3D;

public ref class Polyline3D {
public:
    ~Polyline3D();
    !Polyline3D();

    // Factory method (private constructor — use Make)
    static Polyline3D^ Make(array<Point3D^>^ points);

    int Size();
    property Point3D^ default[int] { Point3D^ get(int i); }

    bool AlmostEquals(Polyline3D^ other);
    bool AlmostEquals(Polyline3D^ other, double epsilon);

    array<LineSegment3D^>^ ToSegments();
    double   Length();
    bool     IsPlanar();
    bool     IsSimple();
    bool     IsConvex();
    Polyline3D^ ConvexHull();

    // Returns a copy of this polyline with fewer vertices, per the given PolylineDecimationStrategy.
    Polyline3D^ Reduce();
    Polyline3D^ Reduce(PolylineDecimationStrategy strategy, double threshold);

    Polygon3D^  ToPolygon();
    double   DistanceTo(Point3D^ point);
    double   Location(Point3D^ point);
    Point3D^ Interpolate(double pct);

    System::String^  ToWkt();
    static Polyline3D^ FromWkt(System::String^ wkt);
    void               ToFile(System::String^ path);
    static Polyline3D^ FromFile(System::String^ path);

    bool Contains(Point3D^ point);

    // Intersects
    bool Intersects(Line3D^ line);
    bool Intersects(Ray3D^ ray);
    bool Intersects(LineSegment3D^ segment);
    bool Intersects(Polyline3D^ other);

    // Intersection — optional<vector<Point3D>> → System::Object^ (array<Point3D^>^ or nullptr)
    System::Object^ Intersection(Line3D^ line);
    System::Object^ Intersection(Ray3D^ ray);
    System::Object^ Intersection(LineSegment3D^ segment);
    System::Object^ Intersection(Polyline3D^ other);

    // Overlaps / Overlap — optional<vector<LineSegment3D>> → array<LineSegment3D^>^ or nullptr
    bool Overlaps(Line3D^ line);
    bool Overlaps(Ray3D^ ray);
    bool Overlaps(LineSegment3D^ segment);
    bool Overlaps(Polyline3D^ other);
    array<LineSegment3D^>^ Overlap(Line3D^ line);
    array<LineSegment3D^>^ Overlap(Ray3D^ ray);
    array<LineSegment3D^>^ Overlap(LineSegment3D^ segment);
    array<LineSegment3D^>^ Overlap(Polyline3D^ other);

    // Touches / Touch — optional<vector<Point3D>> → array<Point3D^>^ or nullptr
    bool Touches(Line3D^ line);
    bool Touches(Ray3D^ ray);
    bool Touches(LineSegment3D^ segment);
    bool Touches(Polyline3D^ other);
    array<Point3D^>^ Touch(Line3D^ line);
    array<Point3D^>^ Touch(Ray3D^ ray);
    array<Point3D^>^ Touch(LineSegment3D^ segment);
    array<Point3D^>^ Touch(Polyline3D^ other);

    // Operator
    static bool operator==(Polyline3D^ lhs, Polyline3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polyline3D(geompp::Polyline3D* native);
    geompp::Polyline3D* _native;
};

}  // namespace GeomPP
