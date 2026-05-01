#include "bind_helpers.hpp"

void bind_free_functions(py::module_& m) {
    m.def("are_collinear",
          [](const geompp::Point2D& p1, const geompp::Point2D& p2, const geompp::Point2D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 2D points are collinear.");

    m.def("are_collinear",
          [](const geompp::Point3D& p1, const geompp::Point3D& p2, const geompp::Point3D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 3D points are collinear.");

    m.def("remove_duplicates_from_sorted_list",
          [](const std::vector<geompp::Point2D>& pts) {
              return geompp::remove_duplicates_from_sorted_list(pts);
          }, "points"_a, "Remove consecutive duplicate 2D points.");

    m.def("remove_duplicates_from_sorted_list",
          [](const std::vector<geompp::Point3D>& pts) {
              return geompp::remove_duplicates_from_sorted_list(pts);
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
}
