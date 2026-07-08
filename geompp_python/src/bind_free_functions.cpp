#include "bind_helpers.hpp"

#include "line_segment2d.hpp"  // has_intersections / find_intersections
#include "point2d.hpp"         // convex_hull
#include "calc_utils2d.hpp"    // ExtremePoints / find_extreme_points (2D)
#include "calc_utils3d.hpp"    // principal_axes / principal_normal / principal_direction / find_extreme_points (3D)
#include "line2d.hpp"
#include "line3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"

void bind_free_functions(py::module_& m) {
    py::class_<geompp::CoordinateFrame>(m, "CoordinateFrame",
        "Orthonormal coordinate frame from PCA: X=primary axis, Y=secondary axis, Z=best-fit plane normal.")
        .def_readonly("x", &geompp::CoordinateFrame::X, "Primary axis — direction of largest variance.")
        .def_readonly("y", &geompp::CoordinateFrame::Y, "Secondary axis — direction of second largest variance.")
        .def_readonly("z", &geompp::CoordinateFrame::Z, "Normal — direction of least variance (best-fit plane normal).");

    py::class_<geompp::ExtremePoints<geompp::Point2D>>(m, "ExtremePoints2D",
        "The two vertices of a 2D shape extreme (least / greatest projection) along a direction.")
        .def_readonly("min_point", &geompp::ExtremePoints<geompp::Point2D>::min_point, "Vertex with least projection.")
        .def_readonly("max_point", &geompp::ExtremePoints<geompp::Point2D>::max_point, "Vertex with greatest projection.");

    py::class_<geompp::ExtremePoints<geompp::Point3D>>(m, "ExtremePoints3D",
        "The two vertices of a 3D shape extreme (least / greatest projection) along a direction.")
        .def_readonly("min_point", &geompp::ExtremePoints<geompp::Point3D>::min_point, "Vertex with least projection.")
        .def_readonly("max_point", &geompp::ExtremePoints<geompp::Point3D>::max_point, "Vertex with greatest projection.");

    m.def("are_collinear",
          [](const geompp::Point2D& p1, const geompp::Point2D& p2, const geompp::Point2D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 2D points are collinear.");

    m.def("are_collinear",
          [](const geompp::Point3D& p1, const geompp::Point3D& p2, const geompp::Point3D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 3D points are collinear.");

    m.def("remove_consecutive_duplicates",
          [](const std::vector<geompp::Point2D>& pts) {
              return geompp::remove_consecutive_duplicates(pts);
          }, "points"_a, "Remove consecutive duplicate 2D points.");

    m.def("remove_consecutive_duplicates",
          [](const std::vector<geompp::Point3D>& pts) {
              return geompp::remove_consecutive_duplicates(pts);
          }, "points"_a, "Remove consecutive duplicate 3D points.");

    m.def("remove_duplicates",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::remove_duplicates(pts); },
          "points"_a, "Remove all duplicate 2D points.");

    m.def("remove_duplicates",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::remove_duplicates(pts); },
          "points"_a, "Remove all duplicate 3D points.");

    m.def("remove_collinear",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::remove_collinear(pts); },
          "points"_a, "Remove collinear 2D points.");

    m.def("remove_collinear",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::remove_collinear(pts); },
          "points"_a, "Remove collinear 3D points.");

    m.def("linear_combination",
          [](const std::vector<geompp::Point2D>& pts, const std::vector<double>& w) {
              return geompp::linear_combination(pts, w);
          }, "points"_a, "weights"_a, "Weighted linear combination of 2D points.");

    m.def("linear_combination",
          [](const std::vector<geompp::Point3D>& pts, const std::vector<double>& w) {
              return geompp::linear_combination(pts, w);
          }, "points"_a, "weights"_a, "Weighted linear combination of 3D points.");

    m.def("average",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::average(pts); },
          "points"_a, "Arithmetic mean of 2D points.");

    m.def("average",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::average(pts); },
          "points"_a, "Arithmetic mean of 3D points.");

    m.def("centroid",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::centroid(pts); },
          "points"_a, "Centroid of a 2D polygon. Throws if the points have zero area.");

    m.def("centroid",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> plane) {
              return geompp::centroid(pts, plane);
          },
          "points"_a, "plane"_a = py::none(),
          "Centroid of a 3D polygon. Throws if the points have zero area. Plane is auto-detected if omitted.");

    m.def("signed_area",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::signed_area(pts); },
          "points"_a, "Signed area of a 2D polygon; positive = CCW, negative = CW.");

    m.def("signed_area",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> plane) {
              return geompp::signed_area(pts, plane);
          },
          "points"_a, "plane"_a = py::none(),
          "Signed area of a 3D polygon; positive = CCW, negative = CW. Plane is auto-detected if omitted.");

    m.def("are_ccw",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::are_ccw(pts); },
          "points"_a, "True if 2D points are ordered counter-clockwise.");

    m.def("are_cw",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::are_cw(pts); },
          "points"_a, "True if 2D points are ordered clockwise.");

    m.def("are_coplanar",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::are_coplanar(pts); },
          "points"_a, "True if 3D points are coplanar.");

    m.def("closest_world_plane_to",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::closest_world_plane_to(pts); },
          "points"_a, "Returns the XY, YZ or ZX world plane whose normal is closest to the points' plane normal.");

    m.def("are_ccw",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> ref_plane) {
              return geompp::are_ccw(pts, ref_plane);
          },
          "points"_a, "ref_plane"_a = py::none(),
          "True if 3D points are ordered counter-clockwise. If ref_plane is omitted, the plane is fitted from the points.");

    m.def("are_cw",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> ref_plane) {
              return geompp::are_cw(pts, ref_plane);
          },
          "points"_a, "ref_plane"_a = py::none(),
          "True if 3D points are ordered clockwise. If ref_plane is omitted, the plane is fitted from the points.");

    // ── segment-set intersection (Shamos–Hoey / Bentley–Ottmann) ──────────────────────────────
    m.def("has_intersections",
          [](const std::vector<geompp::LineSegment2D>& segments) { return geompp::has_intersections(segments); },
          "segments"_a, "Shamos–Hoey: True if any two of the segments intersect.");

    m.def("find_intersections",
          [](const std::vector<geompp::LineSegment2D>& segments) {
              return geompp::find_intersections(segments);
          },
          "segments"_a, "Bentley–Ottmann: list[Point2D] of all intersection points among the segments.");

    // ── convex hull ──────────────────────────────────────────────────────────────────────────
    m.def("convex_hull",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::convex_hull(pts); },
          "points"_a, "Andrew's monotone chain: convex hull of a 2D point cloud, returned in CCW order.");

    m.def("convex_hull",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Vector3D> normal) {
              return geompp::convex_hull(pts, normal);
          },
          "points"_a, "normal"_a = py::none(),
          "Andrew's monotone chain: convex hull of 3D points, returned in CCW order. "
          "Points do not need to be coplanar — when normal is omitted, the best-fit plane "
          "is estimated via PCA (Jacobi eigendecomposition).");

    m.def("principal_axes",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::principal_axes(pts); },
          "points"_a,
          "PCA on a 3D point cloud: returns a CoordinateFrame (X=primary, Y=secondary, Z=normal). Requires >=3 non-collinear points.");

    m.def("principal_normal",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::principal_normal(pts); },
          "points"_a,
          "Best-fit plane normal of a 3D point cloud (PCA eigenvector with smallest eigenvalue).");

    m.def("principal_direction",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::principal_direction(pts); },
          "points"_a,
          "Dominant direction of a 3D point cloud (PCA eigenvector with largest eigenvalue).");

    // ── polygon extreme points along a line ───────────────────────────────────────────────────
    m.def("find_extreme_points",
          [](const geompp::Polygon2D& polygon, const geompp::Line2D& line) {
              return geompp::find_extreme_points(polygon, line);
          },
          "polygon"_a, "line"_a,
          "The outer-ring vertices of the polygon extreme (least / greatest projection) along the line's "
          "direction, as ExtremePoints2D(min_point, max_point). Uses Daniel Sunday's O(log n) binary search "
          "when the polygon is convex, else an O(n) scan. Holes are ignored.");

    m.def("find_extreme_points",
          [](const geompp::Polygon3D& polygon, const geompp::Line3D& line) {
              return geompp::find_extreme_points(polygon, line);
          },
          "polygon"_a, "line"_a,
          "The outer-ring vertices of the polygon extreme (least / greatest projection) along the line's "
          "direction, as ExtremePoints3D(min_point, max_point). Uses Daniel Sunday's O(log n) binary search "
          "when the polygon is convex, else an O(n) scan. Holes are ignored.");

    // ── polygon-to-line distance ───────────────────────────────────────────────────────────────
    m.def("distance_to",
          [](const geompp::Polygon2D& polygon, const geompp::Line2D& line) {
              return geompp::distance_to(polygon, line);
          },
          "polygon"_a, "line"_a,
          "Distance between the polygon and an infinite line (zero if they cross). Holes are ignored.");

    m.def("distance_to",
          [](const geompp::Polygon3D& polygon, const geompp::Line3D& line) {
              return geompp::distance_to(polygon, line);
          },
          "polygon"_a, "line"_a,
          "Distance between the polygon and an infinite line (zero if they cross). Handles coplanar, "
          "parallel-offset, and skew lines. Holes are ignored.");
}
