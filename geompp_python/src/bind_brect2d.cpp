#include "bind_helpers.hpp"

void bind_brect2d(py::module_& m) {
    py::class_<geompp::BRect2D>(m, "BRect2D",
        "2D oriented minimum bounding rectangle (rotating calipers, Freeman & Shapira 1975 / Toussaint 1983).")
        .def(py::init<const std::vector<geompp::Point2D>&>(), "points"_a)
        .def(py::init<const geompp::BRect2D&>())
        .def_property_readonly("center",     &geompp::BRect2D::center)
        .def_property_readonly("axis_u",     &geompp::BRect2D::axis_u)
        .def_property_readonly("axis_v",     &geompp::BRect2D::axis_v)
        .def_property_readonly("half_len_u", &geompp::BRect2D::half_len_u)
        .def_property_readonly("half_len_v", &geompp::BRect2D::half_len_v)
        .def_property_readonly("width",      &geompp::BRect2D::width)
        .def_property_readonly("height",     &geompp::BRect2D::height)
        .def_property_readonly("area",       &geompp::BRect2D::area)
        .def("corners",  &geompp::BRect2D::Corners)
        .def("contains", &geompp::BRect2D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BRect2D)
        .def("__eq__", [](const geompp::BRect2D& a, const geompp::BRect2D& b) { return a == b; });
}
