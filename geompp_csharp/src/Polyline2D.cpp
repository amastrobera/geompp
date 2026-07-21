#include "Polyline2D.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Polygon2D.hpp"
#include "Ray2D.hpp"
#include "LineSegment2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Polyline2D::Polyline2D(geompp::Polyline2D* native)
    : _native(native) {}

Polyline2D::~Polyline2D() {
    delete _native;
    _native = nullptr;
}

Polyline2D::!Polyline2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Polyline2D^ Polyline2D::Make(array<Point2D^>^ points) {
    std::vector<geompp::Point2D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point2D^ p in points)
        nativePoints.push_back(*p->_native);
    return gcnew Polyline2D(new geompp::Polyline2D(geompp::Polyline2D::Make(nativePoints)));
}

// ── Methods ──────────────────────────────────────────────────────────────────

int Polyline2D::Size() {
    return _native->Size();
}

Point2D^ Polyline2D::default::get(int i) {
    return gcnew Point2D(new geompp::Point2D((*_native)[(std::size_t)i]));
}

bool Polyline2D::AlmostEquals(Polyline2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Polyline2D::AlmostEquals(Polyline2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

array<LineSegment2D^>^ Polyline2D::ToSegments() {
    auto segs = _native->ToSegments();
    auto result = gcnew array<LineSegment2D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        result[i] = gcnew LineSegment2D(new geompp::LineSegment2D(segs[i]));
    }
    return result;
}

double Polyline2D::Length() {
    return _native->Length();
}

Polygon2D^ Polyline2D::ConvexHull() {
    return gcnew Polygon2D(new geompp::Polygon2D(_native->ConvexHull()));
}

PolylineDecimationParams::PolylineDecimationParams()
    : _strategy(PolylineDecimationStrategy::RamerDouglasPeucker), _threshold(0.5) {}

PolylineDecimationParams::PolylineDecimationParams(PolylineDecimationStrategy strategy, double threshold)
    : _strategy(strategy), _threshold(threshold) {}

geompp::PolylineDecimationParams PolylineDecimationParams::ToNative() {
    geompp::PolylineDecimationParams native;
    native.strategy = static_cast<geompp::PolylineDecimationParams::Strategy>(_strategy);
    native.threshold = _threshold;
    return native;
}

Polyline2D^ Polyline2D::Reduce() {
    return gcnew Polyline2D(new geompp::Polyline2D(_native->Reduce()));
}

Polyline2D^ Polyline2D::Reduce(PolylineDecimationParams^ settings) {
    return gcnew Polyline2D(new geompp::Polyline2D(_native->Reduce(settings->ToNative())));
}

PolylineExpansionParams::PolylineExpansionParams()
    : _smoothness(0.5), _mode(PolylineExpansionMode::FixedSegments), _segmentsPerCorner(4),
      _minDistance(0.1), _minSegmentLength(static_cast<double>(geompp::DOUBLE_EPSILON)) {}

PolylineExpansionParams::PolylineExpansionParams(double smoothness, PolylineExpansionMode mode,
                                                  int segmentsPerCorner, double minDistance,
                                                  double minSegmentLength)
    : _smoothness(smoothness), _mode(mode), _segmentsPerCorner(segmentsPerCorner),
      _minDistance(minDistance), _minSegmentLength(minSegmentLength) {}

geompp::PolylineExpansionParams PolylineExpansionParams::ToNative() {
    geompp::PolylineExpansionParams native;
    native.smoothness = _smoothness;
    native.mode = static_cast<geompp::PolylineExpansionParams::Mode>(_mode);
    native.segments_per_corner = _segmentsPerCorner;
    native.min_distance = _minDistance;
    native.min_segment_length = _minSegmentLength;
    return native;
}

Polyline2D^ Polyline2D::Expand() {
    return gcnew Polyline2D(new geompp::Polyline2D(_native->Expand()));
}

Polyline2D^ Polyline2D::Expand(PolylineExpansionParams^ settings) {
    return gcnew Polyline2D(new geompp::Polyline2D(_native->Expand(settings->ToNative())));
}

double Polyline2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

double Polyline2D::Location(Point2D^ point) {
    return _native->Location(*point->_native);
}

Point2D^ Polyline2D::Interpolate(double pct) {
    return gcnew Point2D(new geompp::Point2D(_native->Interpolate(pct)));
}

System::String^ Polyline2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Polyline2D^ Polyline2D::FromWkt(System::String^ wkt) {
    return gcnew Polyline2D(
        new geompp::Polyline2D(geompp::Polyline2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Polyline2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Polyline2D^ Polyline2D::FromFile(System::String^ path) {
    return gcnew Polyline2D(
        new geompp::Polyline2D(geompp::Polyline2D::FromFile(marshal_as<std::string>(path))));
}

bool Polyline2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Polyline2D::Intersects(Line2D^ line) {
    return _native->Intersects(*line->_native);
}

bool Polyline2D::Intersects(Ray2D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Polyline2D::Intersects(LineSegment2D^ segment) {
    return _native->Intersects(*segment->_native);
}

bool Polyline2D::Intersects(Polyline2D^ other) {
    return _native->Intersects(*other->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

// Helper to convert optional<vector<Point2D>> to System::Object^ (null on miss, array otherwise)
static array<GeomPP::Point2D^>^ ConvertPolyline2DIntersection(
    std::optional<std::vector<geompp::Point2D>> const& result) {
    if (!result.has_value()) {
        return nullptr;
    }
    auto const& pts = result.value();
    auto arr = gcnew array<GeomPP::Point2D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew GeomPP::Point2D(new geompp::Point2D(pts[i]));
    }
    return arr;
}

System::Object^ Polyline2D::Intersection(Line2D^ line) {
    return ConvertPolyline2DIntersection(_native->Intersection(*line->_native));
}

System::Object^ Polyline2D::Intersection(Ray2D^ ray) {
    return ConvertPolyline2DIntersection(_native->Intersection(*ray->_native));
}

System::Object^ Polyline2D::Intersection(LineSegment2D^ segment) {
    return ConvertPolyline2DIntersection(_native->Intersection(*segment->_native));
}

System::Object^ Polyline2D::Intersection(Polyline2D^ other) {
    return ConvertPolyline2DIntersection(_native->Intersection(*other->_native));
}

// ── Overlaps / Overlap ────────────────────────────────────────────────────────

static array<LineSegment2D^>^ ToManagedSegArray(const std::optional<std::vector<geompp::LineSegment2D>>& opt) {
    if (!opt.has_value()) { return nullptr; }
    auto& segs = opt.value();
    auto arr = gcnew array<LineSegment2D^>((int)segs.size());
    for (int i = 0; i < (int)segs.size(); ++i) {
        arr[i] = gcnew LineSegment2D(new geompp::LineSegment2D(segs[i]));
    }
    return arr;
}

static array<Point2D^>^ ToManagedPtArray(const std::optional<std::vector<geompp::Point2D>>& opt) {
    if (!opt.has_value()) { return nullptr; }
    auto& pts = opt.value();
    auto arr = gcnew array<Point2D^>((int)pts.size());
    for (int i = 0; i < (int)pts.size(); ++i) {
        arr[i] = gcnew Point2D(new geompp::Point2D(pts[i]));
    }
    return arr;
}

bool Polyline2D::Overlaps(Line2D^ line) { return _native->Overlaps(*line->_native); }
bool Polyline2D::Overlaps(Ray2D^ ray) { return _native->Overlaps(*ray->_native); }
bool Polyline2D::Overlaps(LineSegment2D^ segment) { return _native->Overlaps(*segment->_native); }
bool Polyline2D::Overlaps(Polyline2D^ other) { return _native->Overlaps(*other->_native); }

array<LineSegment2D^>^ Polyline2D::Overlap(Line2D^ line) { return ToManagedSegArray(_native->Overlap(*line->_native)); }
array<LineSegment2D^>^ Polyline2D::Overlap(Ray2D^ ray) { return ToManagedSegArray(_native->Overlap(*ray->_native)); }
array<LineSegment2D^>^ Polyline2D::Overlap(LineSegment2D^ segment) { return ToManagedSegArray(_native->Overlap(*segment->_native)); }
array<LineSegment2D^>^ Polyline2D::Overlap(Polyline2D^ other) { return ToManagedSegArray(_native->Overlap(*other->_native)); }

// ── Touches / Touch ───────────────────────────────────────────────────────────

bool Polyline2D::Touches(Line2D^ line) { return _native->Touches(*line->_native); }
bool Polyline2D::Touches(Ray2D^ ray) { return _native->Touches(*ray->_native); }
bool Polyline2D::Touches(LineSegment2D^ segment) { return _native->Touches(*segment->_native); }
bool Polyline2D::Touches(Polyline2D^ other) { return _native->Touches(*other->_native); }

array<Point2D^>^ Polyline2D::Touch(Line2D^ line) { return ToManagedPtArray(_native->Touch(*line->_native)); }
array<Point2D^>^ Polyline2D::Touch(Ray2D^ ray) { return ToManagedPtArray(_native->Touch(*ray->_native)); }
array<Point2D^>^ Polyline2D::Touch(LineSegment2D^ segment) { return ToManagedPtArray(_native->Touch(*segment->_native)); }
array<Point2D^>^ Polyline2D::Touch(Polyline2D^ other) { return ToManagedPtArray(_native->Touch(*other->_native)); }

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polyline2D::operator==(Polyline2D^ lhs, Polyline2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polyline2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
