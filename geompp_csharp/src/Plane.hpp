#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <plane.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Point3D;
ref class Vector3D;
ref class Line3D;
ref class Ray3D;
ref class LineSegment3D;
ref class Triangle3D;

public ref class Plane {
public:
    ~Plane();
    !Plane();

    // Factory methods (private constructor — use static factories)
    static Plane^ From3Points(Point3D^ p1, Point3D^ p2, Point3D^ p3);
    static Plane^ FromOriginAndAxes(Point3D^ origin, Vector3D^ u, Vector3D^ v);
    static Plane^ FromOriginAndNormal(Point3D^ origin, Vector3D^ normal);

    Point3D^  Origin();
    Vector3D^ Normal();
    Vector3D^ AxisU();
    Vector3D^ AxisV();

    bool AlmostEquals(Plane^ other);
    bool AlmostEquals(Plane^ other, double epsilon);

    double   SignedDistanceTo(Point3D^ point);
    double   DistanceTo(Point3D^ point);
    Point3D^ ProjectOnto(Point3D^ point);
    Point2D^ ProjectInto(Point3D^ point);
    Point3D^ Evaluate(Point2D^ point);

    bool Contains(Point3D^ point);

    // Intersection — optional<variant<...>> → managed object (nullptr = no intersection / not the active alternative)
    bool     Intersects(Line3D^ line);
    bool     Intersects(Ray3D^ ray);
    bool     Intersects(LineSegment3D^ segment);
    bool     Intersects(Plane^ other);
    bool     Intersects(Triangle3D^ triangle);
    Point3D^ Intersection(Line3D^ line);
    Point3D^ Intersection(Ray3D^ ray);
    Point3D^ Intersection(LineSegment3D^ segment);
    Line3D^  Intersection(Plane^ other);
    System::Object^ Intersection(Triangle3D^ triangle);  // Point3D^ or LineSegment3D^ (or null)

    bool IsParallel(Line3D^ line);
    bool IsParallel(Ray3D^ ray);
    bool IsParallel(LineSegment3D^ segment);
    bool IsCoplanar(Line3D^ line);
    bool IsCoplanar(Ray3D^ ray);
    bool IsCoplanar(LineSegment3D^ segment);

    // Standard planes
    static Plane^ XY();
    static Plane^ YZ();
    static Plane^ ZX();

    // Operator
    static bool operator==(Plane^ lhs, Plane^ rhs);

    virtual System::String^ ToString() override;

internal:
    Plane(geompp::Plane* native);
    geompp::Plane* _native;
};

}  // namespace GeomPP
