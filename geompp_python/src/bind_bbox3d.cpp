#include "bind_helpers.hpp"

void bind_bbox3d(py::module_& m) {
    py::class_<geompp::BBox3D>(m, "BBox3D",
        "3D axis-aligned bounding box.")
        .def(py::init<const geompp::Point3D&,      const geompp::Point3D&>(),  "min"_a, "max"_a)
        .def(py::init<const geompp::LineSegment3D&>(), "segment"_a)
        .def(py::init<const geompp::Polyline3D&>(),    "polyline"_a)
        .def(py::init<const geompp::Polygon3D&>(),     "polygon"_a)
        .def(py::init<const geompp::Triangle3D&>(),    "triangle"_a)
        .def(py::init<const std::vector<geompp::Point3D>&>(), "points"_a)
        .def(py::init<const geompp::BBox3D&>())
        .def_property_readonly("min", &geompp::BBox3D::min)
        .def_property_readonly("max", &geompp::BBox3D::max)
        .def("contains",     &geompp::BBox3D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BBox3D)
        .def("__eq__", [](const geompp::BBox3D& a, const geompp::BBox3D& b) { return a == b; });
}
