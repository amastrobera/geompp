#pragma once

// Keep native headers out of managed compilation
#pragma managed(push, off)
#include <polyline2d.hpp>
#include <constants.hpp>
#pragma managed(pop)

namespace GeomPP {

ref class Point2D;
ref class Line2D;
ref class Ray2D;
ref class LineSegment2D;
ref class Polygon2D;

// Vertex-reduction strategy for Polyline2D::Reduce() / Polyline3D::Reduce().
public enum class PolylineDecimationStrategy {
    // O(n) brute-force pass: drop a vertex if it's closer than threshold to the last kept vertex.
    RadialDistance = 0,
    // O(n log n) to O(n^2): recursively drop vertices closer than threshold to the chord spanning
    // their segment.
    RamerDouglasPeucker = 1,
    // O(n log n) to O(n^2): repeatedly drop the vertex forming the smallest-area triangle with its
    // neighbors, while that area stays below threshold.
    VisvalingamWhyatt = 2
};

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
    // Requires the polyline to be simple (no self-intersections). Call IsSimple() first to verify;
    // behaviour is undefined on non-simple input.
    Polygon2D^ ConvexHull();

    // Returns a copy of this polyline with fewer vertices, per the given PolylineDecimationStrategy.
    Polyline2D^ Reduce();
    Polyline2D^ Reduce(PolylineDecimationStrategy strategy, double threshold);

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

    // Intersection — optional<vector<Point2D>> → System::Object^ (array<Point2D^>^ or nullptr)
    System::Object^ Intersection(Line2D^ line);
    System::Object^ Intersection(Ray2D^ ray);
    System::Object^ Intersection(LineSegment2D^ segment);
    System::Object^ Intersection(Polyline2D^ other);

    // Overlaps / Overlap — optional<vector<LineSegment2D>> → array<LineSegment2D^>^ or nullptr
    bool Overlaps(Line2D^ line);
    bool Overlaps(Ray2D^ ray);
    bool Overlaps(LineSegment2D^ segment);
    bool Overlaps(Polyline2D^ other);
    array<LineSegment2D^>^ Overlap(Line2D^ line);
    array<LineSegment2D^>^ Overlap(Ray2D^ ray);
    array<LineSegment2D^>^ Overlap(LineSegment2D^ segment);
    array<LineSegment2D^>^ Overlap(Polyline2D^ other);

    // Touches / Touch — optional<vector<Point2D>> → array<Point2D^>^ or nullptr
    bool Touches(Line2D^ line);
    bool Touches(Ray2D^ ray);
    bool Touches(LineSegment2D^ segment);
    bool Touches(Polyline2D^ other);
    array<Point2D^>^ Touch(Line2D^ line);
    array<Point2D^>^ Touch(Ray2D^ ray);
    array<Point2D^>^ Touch(LineSegment2D^ segment);
    array<Point2D^>^ Touch(Polyline2D^ other);

    // Operator
    static bool operator==(Polyline2D^ lhs, Polyline2D^ rhs);

    virtual System::String^ ToString() override;

internal:
    Polyline2D(geompp::Polyline2D* native);
    geompp::Polyline2D* _native;
};

}  // namespace GeomPP
