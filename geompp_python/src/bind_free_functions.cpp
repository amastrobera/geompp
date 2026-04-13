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
}
