#include "bind_helpers.hpp"

void bind_bprism3d(py::module_& m) {
    py::class_<geompp::BPrism3D>(m, "BPrism3D",
        "3D oriented minimum bounding prism (rotating calipers + PCA).")
        .def(py::init<const std::vector<geompp::Point3D>&>(), "points"_a)
        .def(py::init<const geompp::BPrism3D&>())
        .def_property_readonly("center",     &geompp::BPrism3D::center)
        .def_property_readonly("axis_u",     &geompp::BPrism3D::axis_u)
        .def_property_readonly("axis_v",     &geompp::BPrism3D::axis_v)
        .def_property_readonly("axis_w",     &geompp::BPrism3D::axis_w)
        .def_property_readonly("half_len_u", &geompp::BPrism3D::half_len_u)
        .def_property_readonly("half_len_v", &geompp::BPrism3D::half_len_v)
        .def_property_readonly("half_len_w", &geompp::BPrism3D::half_len_w)
        .def_property_readonly("width",      &geompp::BPrism3D::width)
        .def_property_readonly("height",     &geompp::BPrism3D::height)
        .def_property_readonly("depth",      &geompp::BPrism3D::depth)
        .def_property_readonly("volume",     &geompp::BPrism3D::volume)
        .def("corners",  &geompp::BPrism3D::Corners)
        .def("contains", &geompp::BPrism3D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BPrism3D)
        .def("__eq__", [](const geompp::BPrism3D& a, const geompp::BPrism3D& b) { return a == b; });
}
