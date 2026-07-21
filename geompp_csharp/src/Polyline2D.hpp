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

// Vertex-reduction strategy for PolylineDecimationParams / Polyline2D::Reduce() / Polyline3D::Reduce().
// All three interpret the same Threshold — meaning "how much noise to remove" — differently (a plain
// distance, a perpendicular chord-distance, or a triangle area), so the same threshold value produces
// different results across strategies.
public enum class PolylineDecimationStrategy {
    // O(n) brute-force pass: drop a vertex if it's closer than threshold to the last *kept* vertex.
    // Cheapest and least accurate — good as a fast noise-clustering pre-pass, not as the sole strategy
    // when shape fidelity matters.
    RadialDistance = 0,
    // O(n log n) to O(n^2): recursively drop vertices closer than threshold to the chord spanning
    // their segment. Given points P1, P2, P3, drops P2 when its perpendicular distance from the P1-P3
    // chord is below threshold; otherwise keeps P2 and recurses on both halves. Best general-purpose
    // choice — preserves the vertices that most define the polyline's shape.
    RamerDouglasPeucker = 1,
    // O(n log n) to O(n^2): repeatedly drop the vertex forming the smallest-area triangle with its
    // neighbors, while that area stays below threshold, then re-evaluate the neighbors. Tends to
    // preserve visually significant features (sharp spikes) better than RadialDistance while being
    // similarly simple to reason about.
    VisvalingamWhyatt = 2
};

// Bundles the vertex-decimation strategy and its threshold for Polyline2D::Reduce() / Polyline3D::Reduce().
// Defaults to RamerDouglasPeucker with Threshold=0.5, matching the parameterless Reduce() overload.
public ref class PolylineDecimationParams {
public:
    PolylineDecimationParams();
    PolylineDecimationParams(PolylineDecimationStrategy strategy, double threshold);

    property PolylineDecimationStrategy Strategy {
        PolylineDecimationStrategy get() { return _strategy; }
        void set(PolylineDecimationStrategy value) { _strategy = value; }
    }
    property double Threshold {
        double get() { return _threshold; }
        void set(double value) { _threshold = value; }
    }

internal:
    geompp::PolylineDecimationParams ToNative();

private:
    PolylineDecimationStrategy _strategy;
    double _threshold;
};

// How densely PolylineExpansionParams / Polyline2D::Expand() / Polyline3D::Expand() sample each
// corner's arc.
public enum class PolylineExpansionMode {
    // Sample an exact number of segments per corner — see SegmentsPerCorner.
    FixedSegments = 0,
    // Sample roughly MinDistance apart, however many points that takes per corner.
    MinDistance = 1
};

// Bundles the corner-rounding controls for Polyline2D::Expand() / Polyline3D::Expand(). Defaults to
// Smoothness=0.5, FixedSegments with SegmentsPerCorner=4, and MinSegmentLength=DOUBLE_EPSILON,
// matching the parameterless Expand() overload.
public ref class PolylineExpansionParams {
public:
    PolylineExpansionParams();
    PolylineExpansionParams(double smoothness, PolylineExpansionMode mode, int segmentsPerCorner,
                            double minDistance, double minSegmentLength);

    property double Smoothness {
        double get() { return _smoothness; }
        void set(double value) { _smoothness = value; }
    }
    property PolylineExpansionMode Mode {
        PolylineExpansionMode get() { return _mode; }
        void set(PolylineExpansionMode value) { _mode = value; }
    }
    property int SegmentsPerCorner {
        int get() { return _segmentsPerCorner; }
        void set(int value) { _segmentsPerCorner = value; }
    }
    property double MinDistance {
        double get() { return _minDistance; }
        void set(double value) { _minDistance = value; }
    }
    property double MinSegmentLength {
        double get() { return _minSegmentLength; }
        void set(double value) { _minSegmentLength = value; }
    }

internal:
    geompp::PolylineExpansionParams ToNative();

private:
    double _smoothness;
    PolylineExpansionMode _mode;
    int _segmentsPerCorner;
    double _minDistance;
    double _minSegmentLength;
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

    // Returns a copy of this polyline with fewer vertices, per the given PolylineDecimationParams.
    Polyline2D^ Reduce();
    Polyline2D^ Reduce(PolylineDecimationParams^ settings);

    // Returns a copy of this polyline with every inner corner rounded by a quadratic Bezier arc, per
    // the given PolylineExpansionParams. The inverse direction of Reduce(): adds vertices rather than
    // removing them.
    Polyline2D^ Expand();
    Polyline2D^ Expand(PolylineExpansionParams^ settings);

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
