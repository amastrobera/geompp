#include "bind_helpers.hpp"

void bind_bball3d(py::module_& m) {
    py::class_<geompp::BBall3D>(m, "BBall3D",
        "3D bounding ball (Ritter 1990 approximate algorithm).")
        .def(py::init<const geompp::Point3D&, double>(), "center"_a, "radius"_a)
        .def(py::init<const std::vector<geompp::Point3D>&>(), "points"_a)
        .def(py::init<const geompp::BBall3D&>())
        .def_property_readonly("center", &geompp::BBall3D::center)
        .def_property_readonly("radius", &geompp::BBall3D::radius)
        .def("contains", &geompp::BBall3D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BBall3D)
        .def("__eq__", [](const geompp::BBall3D& a, const geompp::BBall3D& b) { return a == b; });
}
