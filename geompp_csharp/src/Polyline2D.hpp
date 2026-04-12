#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polyline2d.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Line2D;
ref class Ray2D;
ref class LineSegment2D;

public ref class Polyline2D {
public:
    ~Polyline2D();
    !Polyline2D();

    // Factory method (private constructor — use Make)
    static Polyline2D^ Make(array<Point2D^>^ points);

    int Size();
    property Point2D^ default[int] { Point2D^ get(int i); }

    bool AlmostEquals(Polyline2D^ other);
    bool AlmostEquals(Polyline2D^ other, double epsilon);

    array<LineSegment2D^>^ ToSegments();
    double   Length();
    double   DistanceTo(Point2D^ point);
    double   Location(Point2D^ point);
    Point2D^ Interpolate(double pct);

    System::String^  ToWkt();
    static Polyline2D^ FromWkt(System::String^ wkt);
    void               ToFile(System::String^ path);
    static Polyline2D^ FromFile(System::String^ path);

    bool Contains(Point2D^ point);

    // Intersects
    bool Intersects(Line2D^ line);
    bool Intersects(Ray2D^ ray);
    bool Intersects(LineSegment2D^ segment);
    bool Intersects(Polyline2D^ other);

    // Intersection — optional<variant<Point2D, vector<Point2D>>> → System::Object^
    // (Point2D^ or array<Point2D^>^ or nullptr)
    System::Object^ Intersection(Line2D^ line);
    System::Object^ Intersection(Ray2D^ ray);
    System::Object^ Intersection(LineSegment2D^ segment);
    System::Object^ Intersection(Polyline2D^ other);

    // Operator
    static bool operator==(Polyline2D^ lhs, Polyline2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polyline2D(geompp::Polyline2D* native);
    geompp::Polyline2D* _native;
};

}  // namespace GeomPP
