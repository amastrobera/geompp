#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <triangle3d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point3D;
ref class Vector3D;
ref class Line3D;
ref class Ray3D;
ref class LineSegment3D;
ref class Plane;
ref class Polygon3D;

public ref class Triangle3D {
public:
    ~Triangle3D();
    !Triangle3D();

    // Factory method (private constructor — use Make)
    static Triangle3D^ Make(Point3D^ p0, Point3D^ p1, Point3D^ p2);

    System::Tuple<Point3D^, Point3D^, Point3D^>^ Vertices();

    bool      AlmostEquals(Triangle3D^ other);
    bool      AlmostEquals(Triangle3D^ other, double epsilon);
    Point3D^  Centroid();
    Polygon3D^ ToPolygon();
    Plane^     ToPlane();
    Vector3D^  AreaVector();
    double     SignedArea(Vector3D^ ref_normal);
    bool       IsCCW(Vector3D^ ref_normal);
    double     Area();
    double     Perimeter();
    double     DistanceTo(Point3D^ point);
    System::Tuple<Vector3D^, Vector3D^>^ ToAxis();
    System::Tuple<double, double>^       Location(Point3D^ point);
    Point3D^                             Interpolate(double s, double t);  // nullptr if outside

    System::String^   ToWkt();
    static Triangle3D^ FromWkt(System::String^ wkt);
    void               ToFile(System::String^ path);
    static Triangle3D^ FromFile(System::String^ path);

    bool Contains(Point3D^ point);

    // Intersects — bool overloads
    bool Intersects(Line3D^ line);
    bool Intersects(Ray3D^ ray);
    bool Intersects(LineSegment3D^ segment);
    // Triangle3D overload returns ReturnSet, not bool
    System::Object^ Intersects(Triangle3D^ other);

    // Intersection — optional<variant<Point3D,LineSegment3D,Triangle3D,Polygon3D>> → System::Object^
    System::Object^ Intersection(Line3D^ line);
    System::Object^ Intersection(Ray3D^ ray);
    System::Object^ Intersection(LineSegment3D^ other);
    System::Object^ Intersection(Triangle3D^ other);

    // Operator
    static bool operator==(Triangle3D^ lhs, Triangle3D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Triangle3D(geompp::Triangle3D* native);
    geompp::Triangle3D* _native;
};

}  // namespace GeomPP
