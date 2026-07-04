#include "bind_helpers.hpp"

void bind_bball2d(py::module_& m) {
    py::class_<geompp::BBall2D>(m, "BBall2D",
        "2D bounding ball (Ritter 1990 approximate algorithm).")
        .def(py::init<const geompp::Point2D&, double>(), "center"_a, "radius"_a)
        .def(py::init<const std::vector<geompp::Point2D>&>(), "points"_a)
        .def(py::init<const geompp::BBall2D&>())
        .def_property_readonly("center", &geompp::BBall2D::center)
        .def_property_readonly("radius", &geompp::BBall2D::radius)
        .def("contains", &geompp::BBall2D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BBall2D)
        .def("__eq__", [](const geompp::BBall2D& a, const geompp::BBall2D& b) { return a == b; });
}
