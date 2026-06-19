#include "GeomUtil.hpp"
#include "Point2D.hpp"
#include "Point3D.hpp"
#include "Plane.hpp"
#include "LineSegment2D.hpp"

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

bool GeomUtil::HasIntersections(System::Collections::Generic::List<LineSegment2D^>^ segments) {
    return geompp::has_intersections(ToNativeSegments(segments));
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::FindIntersections(
    System::Collections::Generic::List<LineSegment2D^>^ segments) {
    auto native = geompp::find_intersections(ToNativeSegments(segments));
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native)
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    return list;
}

System::Collections::Generic::IEnumerable<Point2D^>^ GeomUtil::ConvexHull(
    System::Collections::Generic::List<Point2D^>^ points) {
    auto native = geompp::convex_hull(ToNativePoints2D(points));
    auto list = gcnew System::Collections::Generic::List<Point2D^>(static_cast<int>(native.size()));
    for (auto const& p : native)
        list->Add(gcnew Point2D(new geompp::Point2D(p)));
    return list;
}

}  // namespace GeomPP
