#include "Polygon2D.hpp"
#include "Point2D.hpp"
#include "Line2D.hpp"
#include "Ray2D.hpp"
#include "LineSegment2D.hpp"
#include "Triangle2D.hpp"

#include <msclr/marshal_cppstd.h>
using namespace msclr::interop;

#include <utility>

namespace GeomPP {

// ── Lifecycle ────────────────────────────────────────────────────────────────

Polygon2D::Polygon2D(geompp::Polygon2D* native)
    : _native(native) {}

Polygon2D::~Polygon2D() {
    delete _native;
    _native = nullptr;
}

Polygon2D::!Polygon2D() {
    delete _native;
    _native = nullptr;
}

// ── Factory ───────────────────────────────────────────────────────────────────

Polygon2D^ Polygon2D::Make(array<Point2D^>^ points) {
    // nativePoints is a fresh vector this function exclusively owns after marshalling — moving it
    // into Make(vector&&) avoids the extra copy Make(vector const&) would otherwise make internally.
    std::vector<geompp::Point2D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point2D^ p in points)
        nativePoints.push_back(*p->_native);
    return gcnew Polygon2D(new geompp::Polygon2D(geompp::Polygon2D::Make(std::move(nativePoints))));
}

Polygon2D^ Polygon2D::Make(array<Point2D^>^ points, array<array<Point2D^>^>^ holes) {
    std::vector<geompp::Point2D> nativePoints;
    nativePoints.reserve(points->Length);
    for each (Point2D^ p in points)
        nativePoints.push_back(*p->_native);

    std::vector<std::vector<geompp::Point2D>> nativeHoles;
    for each (array<Point2D^>^ hole in holes) {
        std::vector<geompp::Point2D> nativeHole;
        nativeHole.reserve(hole->Length);
        for each (Point2D^ p in hole)
            nativeHole.push_back(*p->_native);
        nativeHoles.push_back(std::move(nativeHole));
    }

    return gcnew Polygon2D(new geompp::Polygon2D(
        geompp::Polygon2D::Make(std::move(nativePoints), std::move(nativeHoles))));
}

// ── Methods ──────────────────────────────────────────────────────────────────

int Polygon2D::Size() {
    return (int)_native->Size();
}

Point2D^ Polygon2D::default::get(int i) {
    return gcnew Point2D(new geompp::Point2D((*_native)[(std::size_t)i]));
}

bool Polygon2D::AlmostEquals(Polygon2D^ other) {
    return _native->AlmostEquals(*other->_native);
}

bool Polygon2D::AlmostEquals(Polygon2D^ other, double epsilon) {
    return _native->AlmostEquals(*other->_native, epsilon);
}

Point2D^ Polygon2D::Centroid() {
    return gcnew Point2D(new geompp::Point2D(_native->Centroid()));
}

double Polygon2D::Area() {
    return _native->Area();
}

double Polygon2D::PerimeterSize() {
    return _native->PerimeterSize();
}

double Polygon2D::DistanceTo(Point2D^ point) {
    return _native->DistanceTo(*point->_native);
}

bool Polygon2D::Contains(Point2D^ point) {
    return _native->Contains(*point->_native);
}

bool Polygon2D::IsOnPerimeter(Point2D^ point) {
    return _native->IsOnPerimeter(*point->_native);
}

bool Polygon2D::IsSimple() {
    return _native->IsSimple();
}

bool Polygon2D::IsConvex() {
    return _native->IsConvex();
}

Polygon2D^ Polygon2D::ConvexHull() {
    return gcnew Polygon2D(new geompp::Polygon2D(_native->ConvexHull()));
}

array<Polygon2D^>^ Polygon2D::Simplify() {
    auto native = _native->Simplify();
    auto arr = gcnew array<Polygon2D^>(static_cast<int>(native.size()));
    for (int i = 0; i < static_cast<int>(native.size()); ++i) {
        arr[i] = gcnew Polygon2D(new geompp::Polygon2D(native[i]));
    }
    return arr;
}

array<Triangle2D^>^ Polygon2D::Triangulate() {
    return Triangulate(TriangulationStrategy::EarClipping);
}

array<Triangle2D^>^ Polygon2D::Triangulate(TriangulationStrategy strategy) {
    auto native = _native->Triangulate(static_cast<geompp::TriangulationParams::Strategy>(strategy));
    auto arr = gcnew array<Triangle2D^>(static_cast<int>(native.size()));
    for (int i = 0; i < static_cast<int>(native.size()); ++i) {
        arr[i] = gcnew Triangle2D(new geompp::Triangle2D(native[i]));
    }
    return arr;
}

array<Point2D^>^ Polygon2D::Perimeter() {
    auto const& native = _native->Perimeter();
    auto arr = gcnew array<Point2D^>(static_cast<int>(native.size()));
    for (int i = 0; i < static_cast<int>(native.size()); ++i) {
        arr[i] = gcnew Point2D(new geompp::Point2D(native[i]));
    }
    return arr;
}

bool Polygon2D::HasHoles() {
    return _native->HasHoles();
}

array<array<Point2D^>^>^ Polygon2D::Holes() {
    auto const& native = _native->Holes();
    auto arr = gcnew array<array<Point2D^>^>(static_cast<int>(native.size()));
    for (int i = 0; i < static_cast<int>(native.size()); ++i) {
        auto const& ring = native[i];
        auto inner = gcnew array<Point2D^>(static_cast<int>(ring.size()));
        for (int j = 0; j < static_cast<int>(ring.size()); ++j) {
            inner[j] = gcnew Point2D(new geompp::Point2D(ring[j]));
        }
        arr[i] = inner;
    }
    return arr;
}

System::String^ Polygon2D::ToWkt() {
    return gcnew System::String(_native->ToWkt().c_str());
}

Polygon2D^ Polygon2D::FromWkt(System::String^ wkt) {
    return gcnew Polygon2D(
        new geompp::Polygon2D(geompp::Polygon2D::FromWkt(marshal_as<std::string>(wkt))));
}

void Polygon2D::ToFile(System::String^ path) {
    _native->ToFile(marshal_as<std::string>(path));
}

Polygon2D^ Polygon2D::FromFile(System::String^ path) {
    return gcnew Polygon2D(
        new geompp::Polygon2D(geompp::Polygon2D::FromFile(marshal_as<std::string>(path))));
}

// ── Intersects ────────────────────────────────────────────────────────────────

bool Polygon2D::Intersects(Line2D^ line) {
    return _native->Intersects(*line->_native);
}

bool Polygon2D::Intersects(Ray2D^ ray) {
    return _native->Intersects(*ray->_native);
}

bool Polygon2D::Intersects(LineSegment2D^ segment) {
    return _native->Intersects(*segment->_native);
}

bool Polygon2D::Intersects(Polygon2D^ other) {
    return _native->Intersects(*other->_native);
}

// ── Intersection ──────────────────────────────────────────────────────────────

static array<LineSegment2D^>^ segs_to_managed(std::optional<std::vector<geompp::LineSegment2D>> const& result) {
    if (!result.has_value()) {
        return nullptr;
    }
    auto const& native_segs = result.value();
    auto arr = gcnew array<LineSegment2D^>(static_cast<int>(native_segs.size()));
    for (int i = 0; i < static_cast<int>(native_segs.size()); ++i) {
        arr[i] = gcnew LineSegment2D(new geompp::LineSegment2D(native_segs[i]));
    }
    return arr;
}

System::Object^ Polygon2D::Intersection(Line2D^ line) {
    return segs_to_managed(_native->Intersection(*line->_native));
}

System::Object^ Polygon2D::Intersection(Ray2D^ ray) {
    return segs_to_managed(_native->Intersection(*ray->_native));
}

System::Object^ Polygon2D::Intersection(LineSegment2D^ segment) {
    return segs_to_managed(_native->Intersection(*segment->_native));
}

// ── Boolean operations ────────────────────────────────────────────────────────

static array<Polygon2D^>^ polys_to_managed(std::vector<geompp::Polygon2D> const& native) {
    auto arr = gcnew array<Polygon2D^>(static_cast<int>(native.size()));
    for (int i = 0; i < static_cast<int>(native.size()); ++i) {
        arr[i] = gcnew Polygon2D(new geompp::Polygon2D(native[i]));
    }
    return arr;
}

array<Polygon2D^>^ Polygon2D::Intersection(Polygon2D^ other) {
    return polys_to_managed(_native->Intersection(*other->_native));
}

array<Polygon2D^>^ Polygon2D::Union(Polygon2D^ other) {
    return polys_to_managed(_native->Union(*other->_native));
}

array<Polygon2D^>^ Polygon2D::Difference(Polygon2D^ other) {
    return polys_to_managed(_native->Difference(*other->_native));
}

array<Polygon2D^>^ Polygon2D::Xor(Polygon2D^ other) {
    return polys_to_managed(_native->Xor(*other->_native));
}

// ── Operator ──────────────────────────────────────────────────────────────────

bool Polygon2D::operator==(Polygon2D^ lhs, Polygon2D^ rhs) {
    return *lhs->_native == *rhs->_native;
}

System::String^ Polygon2D::ToString() {
    return gcnew System::String(_native->ToWkt().c_str());
}

}  // namespace GeomPP
