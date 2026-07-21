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

}  // namespace GeomPP
