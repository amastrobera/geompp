#pragma managed(push, off)
#include <calc_utils3d.hpp>
#pragma managed(pop)

#include "GeomUtil.hpp"
#include "Point2D.hpp"
#include "Point3D.hpp"
#include "Plane.hpp"
#include "LineSegment2D.hpp"
#include "LineSegment3D.hpp"
#include "CoordinateFrame.hpp"
#include "Vector3D.hpp"
#include "Polygon2D.hpp"
#include "Polygon3D.hpp"
#include "Line2D.hpp"
#include "Line3D.hpp"
#include "Polyline2D.hpp"
#include "Triangle2D.hpp"
#include "Triangle3D.hpp"

namespace GeomPP {

// File-local helper: managed List<LineSegment2D^> → native std::vector<geompp::LineSegment2D>
static std::vector<geompp::LineSegment2D> ToNativeSegments(
    System::Collections::Generic::List<LineSegment2D^>^ segments) {
    std::vector<geompp::LineSegment2D> native;
    native.reserve(segments->Count);
    for each (LineSegment2D^ s in segments)
        native.push_back(*s->_native);
    return native;
}

// File-local helper: managed List<Point2D^> → native std::vector<geompp::Point2D>
static std::vector<geompp::Point2D> ToNativePoints2D(
    System::Collections::Generic::List<Point2D^>^ points) {
    std::vector<geompp::Point2D> native;
    native.reserve(points->Count);
    for each (Point2D^ p in points)
        native.push_back(*p->_native);
    return native;
}

// File-local helper: managed List<Point3D^> → native std::vector<geompp::Point3D>
static std::vector<geompp::Point3D> ToNative(System::Collections::Generic::List<Point3D^>^ points) {
    std::vector<geompp::Point3D> native;
    native.reserve(points->Count);
    for each (Point3D^ p in points)
        native.push_back(*p->_native);
    return native;
}

bool GeomUtil::AreCoplanar(System::Collections::Generic::List<Point3D^>^ points) {
    return geompp::are_coplanar(ToNative(points));
}

Plane^ GeomUtil::ClosestWorldPlaneTo(System::Collections::Generic::List<Point3D^>^ points) {
    return gcnew Plane(new geompp::Plane(geompp::closest_world_plane_to(ToNative(points))));
}

bool GeomUtil::AreCCW(System::Collections::Generic::List<Point3D^>^ points, Plane^ refPlane) {
    auto native = ToNative(points);
    std::optional<geompp::Plane> opt = refPlane != nullptr
        ? std::optional<geompp::Plane>(*refPlane->_native)
        : std::nullopt;
    return geompp::are_ccw(native, opt);
}

bool GeomUtil::AreCW(System::Collections::Generic::List<Point3D^>^ points, Plane^ refPlane) {
    auto native = ToNative(points);
    std::optional<geompp::Plane> opt = refPlane != nullptr
        ? std::optional<geompp::Plane>(*refPlane->_native)
        : std::nullopt;
    return geompp::are_cw(native, opt);
}

Point2D^ GeomUtil::Lerp(Point2D^ p0, Point2D^ p1, double t) {
    return gcnew Point2D(new geompp::Point2D(geompp::lerp(*p0->_native, *p1->_native, t)));
}

Point3D^ GeomUtil::Lerp(Point3D^ p0, Point3D^ p1, double t) {
    return gcnew Point3D(new geompp::Point3D(geompp::lerp(*p0->_native, *p1->_native, t)));
}

System::Collections::Generic::IEnumerable<System::Collections::Generic::List<Point2D^>^>^ GeomUtil::Clip(
    System::Collections::Generic::List<Point2D^>^ clipperLoop,
    System::Collections::Generic::List<Point2D^>^ subjectLoop) {
    auto rings = geompp::clip(ToNativePoints2D(clipperLoop), ToNativePoints2D(subjectLoop));

    auto result = gcnew System::Collections::Generic::List<System::Collections::Generic::List<Point2D^>^>();
    for (auto const& ring : rings) {
        auto managedRing = gcnew System::Collections::Generic::List<Point2D^>();
        for (auto const& p : ring) {
            managedRing->Add(gcnew Point2D(new geompp::Point2D(p)));
        }
        result->Add(managedRing);
    }
    return result;
}

System::Collections::Generic::IEnumerable<System::Collections::Generic::List<Point3D^>^>^ GeomUtil::Clip(
    System::Collections::Generic::List<Point3D^>^ clipperLoop,
    System::Collections::Generic::List<Point3D^>^ subjectLoop) {
    auto rings = geompp::clip(ToNative(clipperLoop), ToNative(subjectLoop));

    auto result = gcnew System::Collections::Generic::List<System::Collections::Generic::List<Point3D^>^>();
    for (auto const& ring : rings) {
        auto managedRing = gcnew System::Collections::Generic::List<Point3D^>();
        for (auto const& p : ring) {
            managedRing->Add(gcnew Point3D(new geompp::Point3D(p)));
        }
        result->Add(managedRing);
    }
    return result;
}

bool GeomUtil::HasIntersections(System::Collections::Generic::List<LineSegment2D^>^ segments) {
    return geompp::has_intersections(ToNativeSegments(segments));
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::FindIntersections(
    System::Collections::Generic::List<LineSegment2D^>^ segments) {
    auto native = geompp::find_intersections(ToNativeSegments(segments));
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::ConvexHull(
    System::Collections::Generic::List<Point2D^>^ points) {
    auto native = geompp::convex_hull(ToNativePoints2D(points));
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::ConvexHull(
    System::Collections::Generic::List<Point3D^>^ points) {
    auto native = geompp::convex_hull(ToNative(points));
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::DistDecimation(
    System::Collections::Generic::List<Point2D^>^ points, double threshold) {
    auto native = geompp::dist_decimation(ToNativePoints2D(points), threshold);
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::DistDecimation(
    System::Collections::Generic::List<Point3D^>^ points, double threshold) {
    auto native = geompp::dist_decimation(ToNative(points), threshold);
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::RdpDecimation(
    System::Collections::Generic::List<Point2D^>^ points, double threshold) {
    auto native = geompp::rdp_decimation(ToNativePoints2D(points), threshold);
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::RdpDecimation(
    System::Collections::Generic::List<Point3D^>^ points, double threshold) {
    auto native = geompp::rdp_decimation(ToNative(points), threshold);
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::VwDecimation(
    System::Collections::Generic::List<Point2D^>^ points, double threshold) {
    auto native = geompp::vw_decimation(ToNativePoints2D(points), threshold);
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::VwDecimation(
    System::Collections::Generic::List<Point3D^>^ points, double threshold) {
    auto native = geompp::vw_decimation(ToNative(points), threshold);
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::BezierSmoothing2(
    Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, double minDistance) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, minDistance);
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::BezierSmoothing2(
    Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, double minDistance) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, minDistance);
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::BezierSmoothing2(
    Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, int numSegments) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, numSegments);
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::BezierSmoothing2(
    Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, int numSegments) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, numSegments);
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::BezierSmoothing2(
    Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, double minDistance, double minSegmentLength) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, minDistance,
                                             minSegmentLength);
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::BezierSmoothing2(
    Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, double minDistance, double minSegmentLength) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, minDistance,
                                             minSegmentLength);
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::BezierSmoothing2(
    Point2D^ p0, Point2D^ p1, Point2D^ p2, double smoothness, int numSegments, double minSegmentLength) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, numSegments,
                                             minSegmentLength);
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::BezierSmoothing2(
    Point3D^ p0, Point3D^ p1, Point3D^ p2, double smoothness, int numSegments, double minSegmentLength) {
    auto native = geompp::bezier_smoothing_2(*p0->_native, *p1->_native, *p2->_native, smoothness, numSegments,
                                             minSegmentLength);
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::PolylineExpansion(
    System::Collections::Generic::List<Point2D^>^ points, PolylineExpansionParams^ settings) {
    auto native = geompp::polyline_expansion(ToNativePoints2D(points), settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Point3D^>^ GeomUtil::PolylineExpansion(
    System::Collections::Generic::List<Point3D^>^ points, PolylineExpansionParams^ settings) {
    auto native = geompp::polyline_expansion(ToNative(points), settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Point3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Point3D(new geompp::Point3D(p)));
    }
    return list;
}

CoordinateFrame^ GeomUtil::PrincipalAxes(System::Collections::Generic::List<Point3D^>^ points) {
    auto native = geompp::principal_axes(ToNative(points));
    return gcnew CoordinateFrame(
        new geompp::Vector3D(native.X),
        new geompp::Vector3D(native.Y),
        new geompp::Vector3D(native.Z));
}

Vector3D^ GeomUtil::PrincipalNormal(System::Collections::Generic::List<Point3D^>^ points) {
    return gcnew Vector3D(new geompp::Vector3D(geompp::principal_normal(ToNative(points))));
}

Vector3D^ GeomUtil::PrincipalDirection(System::Collections::Generic::List<Point3D^>^ points) {
    return gcnew Vector3D(new geompp::Vector3D(geompp::principal_direction(ToNative(points))));
}

ExtremePoints2D^ GeomUtil::FindExtremePoints(Polygon2D^ polygon, Line2D^ line) {
    auto ex = geompp::find_extreme_points(*polygon->_native, *line->_native);
    return gcnew ExtremePoints2D(
        gcnew Point2D(new geompp::Point2D(ex.min_point)),
        gcnew Point2D(new geompp::Point2D(ex.max_point)));
}

ExtremePoints3D^ GeomUtil::FindExtremePoints(Polygon3D^ polygon, Line3D^ line) {
    auto ex = geompp::find_extreme_points(*polygon->_native, *line->_native);
    return gcnew ExtremePoints3D(
        gcnew Point3D(new geompp::Point3D(ex.min_point)),
        gcnew Point3D(new geompp::Point3D(ex.max_point)));
}

double GeomUtil::DistanceTo(Polygon2D^ polygon, Line2D^ line) {
    return geompp::distance_to(*polygon->_native, *line->_native);
}

double GeomUtil::DistanceTo(Polygon3D^ polygon, Line3D^ line) {
    return geompp::distance_to(*polygon->_native, *line->_native);
}

PolygonTangents2D^ GeomUtil::TangentsTo(Polygon2D^ polygon, Point2D^ point) {
    auto t = geompp::tangents_to(*polygon->_native, *point->_native);
    return gcnew PolygonTangents2D(
        gcnew LineSegment2D(new geompp::LineSegment2D(t.left)),
        gcnew LineSegment2D(new geompp::LineSegment2D(t.right)));
}

PolygonTangents2D^ GeomUtil::TangentsTo(Polygon2D^ polygon, Polygon2D^ other) {
    auto t = geompp::tangents_to(*polygon->_native, *other->_native);
    return gcnew PolygonTangents2D(
        gcnew LineSegment2D(new geompp::LineSegment2D(t.left)),
        gcnew LineSegment2D(new geompp::LineSegment2D(t.right)));
}

PolygonTangents3D^ GeomUtil::TangentsTo(Polygon3D^ polygon, Point3D^ point) {
    auto t = geompp::tangents_to(*polygon->_native, *point->_native);
    return gcnew PolygonTangents3D(
        gcnew LineSegment3D(new geompp::LineSegment3D(t.left)),
        gcnew LineSegment3D(new geompp::LineSegment3D(t.right)));
}

PolygonTangents3D^ GeomUtil::TangentsTo(Polygon3D^ polygon, Polygon3D^ other) {
    auto t = geompp::tangents_to(*polygon->_native, *other->_native);
    return gcnew PolygonTangents3D(
        gcnew LineSegment3D(new geompp::LineSegment3D(t.left)),
        gcnew LineSegment3D(new geompp::LineSegment3D(t.right)));
}

TriangulationParams::TriangulationParams()
    : _strategy(TriangulationStrategy::EarClippingBestFit), _simplicity(TriangulationSimplicity::Enforce),
      _ccwWinding(TriangulationWinding::Enforce), _collinearity(TriangulationCollinearity::Enforce),
      _conformity(AdjacencyConformity::Enforce) {}

TriangulationParams::TriangulationParams(TriangulationStrategy strategy, TriangulationSimplicity simplicity,
                                         TriangulationWinding ccwWinding, TriangulationCollinearity collinearity)
    : _strategy(strategy), _simplicity(simplicity), _ccwWinding(ccwWinding), _collinearity(collinearity),
      _conformity(AdjacencyConformity::Enforce) {}

TriangulationParams::TriangulationParams(TriangulationStrategy strategy, TriangulationSimplicity simplicity,
                                         TriangulationWinding ccwWinding, TriangulationCollinearity collinearity,
                                         AdjacencyConformity conformity)
    : _strategy(strategy), _simplicity(simplicity), _ccwWinding(ccwWinding), _collinearity(collinearity),
      _conformity(conformity) {}

geompp::TriangulationParams TriangulationParams::ToNative() {
    geompp::TriangulationParams native;
    native.strategy = static_cast<geompp::TriangulationParams::Strategy>(_strategy);
    native.simplicity = static_cast<geompp::TriangulationParams::Simplicity>(_simplicity);
    native.ccw_winding = static_cast<geompp::TriangulationParams::Winding>(_ccwWinding);
    native.collinearity = static_cast<geompp::TriangulationParams::Collinearity>(_collinearity);
    native.conformity = static_cast<geompp::AdjacencyConformity>(_conformity);
    return native;
}

PolygonizationParams::PolygonizationParams()
    : _strategy(PolygonizationStrategy::HertelMehlhorn), _conformity(AdjacencyConformity::Assert) {}

PolygonizationParams::PolygonizationParams(PolygonizationStrategy strategy)
    : _strategy(strategy), _conformity(AdjacencyConformity::Assert) {}

PolygonizationParams::PolygonizationParams(PolygonizationStrategy strategy, AdjacencyConformity conformity)
    : _strategy(strategy), _conformity(conformity) {}

geompp::PolygonizationParams PolygonizationParams::ToNative() {
    geompp::PolygonizationParams native;
    native.strategy = static_cast<geompp::PolygonizationParams::Strategy>(_strategy);
    native.conformity = static_cast<geompp::AdjacencyConformity>(_conformity);
    return native;
}

System::Collections::Generic::IEnumerable<Triangle2D^>^ GeomUtil::Triangulate(
    System::Collections::Generic::List<Point2D^>^ points, TriangulationParams^ settings) {
    auto native = geompp::triangulate(ToNativePoints2D(points), settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Triangle2D^>(static_cast<int>(native.size()));
    for (auto const& t : native) {
        list->Add(gcnew Triangle2D(new geompp::Triangle2D(t)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Triangle3D^>^ GeomUtil::Triangulate(
    System::Collections::Generic::List<Point3D^>^ points, Vector3D^ normal, TriangulationParams^ settings) {
    auto native = geompp::triangulate(ToNative(points), *normal->_native, settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Triangle3D^>(static_cast<int>(native.size()));
    for (auto const& t : native) {
        list->Add(gcnew Triangle3D(new geompp::Triangle3D(t)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Triangle3D^>^ GeomUtil::Triangulate(
    System::Collections::Generic::List<Point3D^>^ points, TriangulationParams^ settings) {
    auto native = geompp::triangulate(ToNative(points), settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Triangle3D^>(static_cast<int>(native.size()));
    for (auto const& t : native) {
        list->Add(gcnew Triangle3D(new geompp::Triangle3D(t)));
    }
    return list;
}

// ── mesh-conformity checking ("every edge has at most 1 neighbor") ───────────────────────────────

static std::vector<geompp::Polygon2D> ToNativePolygons2D(array<Polygon2D^>^ polygons) {
    std::vector<geompp::Polygon2D> native;
    native.reserve(polygons->Length);
    for each (Polygon2D^ p in polygons)
        native.push_back(*p->_native);
    return native;
}

static std::vector<geompp::Polygon3D> ToNativePolygons3D(array<Polygon3D^>^ polygons) {
    std::vector<geompp::Polygon3D> native;
    native.reserve(polygons->Length);
    for each (Polygon3D^ p in polygons)
        native.push_back(*p->_native);
    return native;
}

static std::vector<geompp::Triangle2D> ToNativeTriangles2D(array<Triangle2D^>^ triangles) {
    std::vector<geompp::Triangle2D> native;
    native.reserve(triangles->Length);
    for each (Triangle2D^ t in triangles)
        native.push_back(*t->_native);
    return native;
}

static std::vector<geompp::Triangle3D> ToNativeTriangles3D(array<Triangle3D^>^ triangles) {
    std::vector<geompp::Triangle3D> native;
    native.reserve(triangles->Length);
    for each (Triangle3D^ t in triangles)
        native.push_back(*t->_native);
    return native;
}

System::Collections::Generic::IEnumerable<Polygon2D^>^ GeomUtil::Polygonize(
    array<Triangle2D^>^ triangles, PolygonizationParams^ settings) {
    auto native = geompp::polygonize(ToNativeTriangles2D(triangles), settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Polygon2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Polygon2D(new geompp::Polygon2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Polygon3D^>^ GeomUtil::Polygonize(
    array<Triangle3D^>^ triangles, PolygonizationParams^ settings) {
    auto native = geompp::polygonize(ToNativeTriangles3D(triangles), settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Polygon3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Polygon3D(new geompp::Polygon3D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Polygon2D^>^ GeomUtil::Merge(array<Polygon2D^>^ polygons) {
    auto native = geompp::merge(ToNativePolygons2D(polygons));
    auto list = gcnew System::Collections::Generic::List<Polygon2D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Polygon2D(new geompp::Polygon2D(p)));
    }
    return list;
}

System::Collections::Generic::IEnumerable<Polygon3D^>^ GeomUtil::Merge(array<Polygon3D^>^ polygons) {
    auto native = geompp::merge(ToNativePolygons3D(polygons));
    auto list = gcnew System::Collections::Generic::List<Polygon3D^>(static_cast<int>(native.size()));
    for (auto const& p : native) {
        list->Add(gcnew Polygon3D(new geompp::Polygon3D(p)));
    }
    return list;
}

static System::Collections::Generic::List<int>^ ToManagedInts(std::vector<std::size_t> const& indices) {
    auto list = gcnew System::Collections::Generic::List<int>(static_cast<int>(indices.size()));
    for (auto i : indices)
        list->Add(static_cast<int>(i));
    return list;
}

static AdjacencyViolation2D^ ToManaged(geompp::AdjacencyViolation<geompp::Point2D> const& v) {
    return gcnew AdjacencyViolation2D(gcnew Point2D(new geompp::Point2D(v.edge_p0)),
                                      gcnew Point2D(new geompp::Point2D(v.edge_p1)), ToManagedInts(v.facet_indices),
                                      v.is_non_manifold, gcnew Point2D(new geompp::Point2D(v.on_vertex)));
}

static AdjacencyViolation3D^ ToManaged(geompp::AdjacencyViolation<geompp::Point3D> const& v) {
    return gcnew AdjacencyViolation3D(gcnew Point3D(new geompp::Point3D(v.edge_p0)),
                                      gcnew Point3D(new geompp::Point3D(v.edge_p1)), ToManagedInts(v.facet_indices),
                                      v.is_non_manifold, gcnew Point3D(new geompp::Point3D(v.on_vertex)));
}

System::Collections::Generic::IEnumerable<AdjacencyViolation2D^>^ GeomUtil::ValidateAdjacency(array<Polygon2D^>^ facets) {
    auto native = geompp::validate_adjacency(ToNativePolygons2D(facets));
    auto list = gcnew System::Collections::Generic::List<AdjacencyViolation2D^>(static_cast<int>(native.size()));
    for (auto const& v : native)
        list->Add(ToManaged(v));
    return list;
}

System::Collections::Generic::IEnumerable<AdjacencyViolation2D^>^ GeomUtil::ValidateAdjacency(array<Triangle2D^>^ facets) {
    auto native = geompp::validate_adjacency(ToNativeTriangles2D(facets));
    auto list = gcnew System::Collections::Generic::List<AdjacencyViolation2D^>(static_cast<int>(native.size()));
    for (auto const& v : native)
        list->Add(ToManaged(v));
    return list;
}

System::Collections::Generic::IEnumerable<AdjacencyViolation3D^>^ GeomUtil::ValidateAdjacency(array<Polygon3D^>^ facets) {
    auto native = geompp::validate_adjacency(ToNativePolygons3D(facets));
    auto list = gcnew System::Collections::Generic::List<AdjacencyViolation3D^>(static_cast<int>(native.size()));
    for (auto const& v : native)
        list->Add(ToManaged(v));
    return list;
}

System::Collections::Generic::IEnumerable<AdjacencyViolation3D^>^ GeomUtil::ValidateAdjacency(array<Triangle3D^>^ facets) {
    auto native = geompp::validate_adjacency(ToNativeTriangles3D(facets));
    auto list = gcnew System::Collections::Generic::List<AdjacencyViolation3D^>(static_cast<int>(native.size()));
    for (auto const& v : native)
        list->Add(ToManaged(v));
    return list;
}

array<array<Point2D^>^>^ GeomUtil::FixAdjacency(array<Polygon2D^>^ facets) {
    auto native = geompp::fix_adjacency(ToNativePolygons2D(facets));
    auto result = gcnew array<array<Point2D^>^>(static_cast<int>(native.size()));
    for (int i = 0; i < native.size(); ++i) {
        auto ring = gcnew array<Point2D^>(static_cast<int>(native[i].size()));
        for (int j = 0; j < native[i].size(); ++j)
            ring[j] = gcnew Point2D(new geompp::Point2D(native[i][j]));
        result[i] = ring;
    }
    return result;
}

array<array<Point3D^>^>^ GeomUtil::FixAdjacency(array<Polygon3D^>^ facets) {
    auto native = geompp::fix_adjacency(ToNativePolygons3D(facets));
    auto result = gcnew array<array<Point3D^>^>(static_cast<int>(native.size()));
    for (int i = 0; i < native.size(); ++i) {
        auto ring = gcnew array<Point3D^>(static_cast<int>(native[i].size()));
        for (int j = 0; j < native[i].size(); ++j)
            ring[j] = gcnew Point3D(new geompp::Point3D(native[i][j]));
        result[i] = ring;
    }
    return result;
}

array<Triangle2D^>^ GeomUtil::FixAdjacency(array<Triangle2D^>^ facets) {
    auto native = geompp::fix_adjacency(ToNativeTriangles2D(facets));
    auto result = gcnew array<Triangle2D^>(static_cast<int>(native.size()));
    for (int i = 0; i < native.size(); ++i)
        result[i] = gcnew Triangle2D(new geompp::Triangle2D(native[i]));
    return result;
}

array<Triangle3D^>^ GeomUtil::FixAdjacency(array<Triangle3D^>^ facets) {
    auto native = geompp::fix_adjacency(ToNativeTriangles3D(facets));
    auto result = gcnew array<Triangle3D^>(static_cast<int>(native.size()));
    for (int i = 0; i < native.size(); ++i)
        result[i] = gcnew Triangle3D(new geompp::Triangle3D(native[i]));
    return result;
}

System::Collections::Generic::IEnumerable<Triangle2D^>^ GeomUtil::Triangulate(
    array<Polygon2D^>^ polygons, TriangulationParams^ settings) {
    auto native = geompp::triangulate(ToNativePolygons2D(polygons), settings->ToNative());
    auto list = gcnew System::Collections::Generic::List<Triangle2D^>(static_cast<int>(native.size()));
    for (auto const& t : native)
        list->Add(gcnew Triangle2D(new geompp::Triangle2D(t)));
    return list;
}

}  // namespace GeomPP
