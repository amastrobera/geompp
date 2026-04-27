#include "GeomUtil.hpp"
#include "Point3D.hpp"
#include "Plane.hpp"

namespace GeomPP {

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

}  // namespace GeomPP
