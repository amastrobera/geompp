#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <triangle2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Vector2D;
ref class Line2D;
ref class LineSegment2D;
ref class Polygon2D;

public ref class Triangle2D {
public:
    ~Triangle2D();
    !Triangle2D();

    // Factory method (private constructor — use Make)
    static Triangle2D^ Make(Point2D^ p0, Point2D^ p1, Point2D^ p2);

    System::Tuple<Point2D^, Point2D^, Point2D^>^ Vertices();

    bool            AlmostEquals(Triangle2D^ other);
    bool            AlmostEquals(Triangle2D^ other, double epsilon);
    Point2D^        Centroid();
    Polygon2D^      ToPolygon();
    double          SignedArea();
    bool            IsCCW();
    double          Area();
    double          Perimeter();
    double          DistanceTo(Point2D^ point);
    System::Tuple<Vector2D^, Vector2D^>^ ToAxis();
    System::Tuple<double, double>^       Location(Point2D^ point);
    Point2D^                             Interpolate(double s, double t);  // nullptr if outside

    System::String^   ToWkt();
    static Triangle2D^ FromWkt(System::String^ wkt);
    void               ToFile(System::String^ path);
    static Triangle2D^ FromFile(System::String^ path);

    bool Contains(Point2D^ point);

    // Intersects
    bool Intersects(Line2D^ line);

    // Intersection — optional<variant<Point2D,LineSegment2D,Triangle2D,Polygon2D>> → System::Object^
    System::Object^ Intersection(Line2D^ line);

    // Operator
    static bool operator==(Triangle2D^ lhs, Triangle2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Triangle2D(geompp::Triangle2D* native);
    geompp::Triangle2D* _native;
};

}  // namespace GeomPP
