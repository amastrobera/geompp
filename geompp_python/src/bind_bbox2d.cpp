#include "bind_helpers.hpp"

void bind_bbox2d(py::module_& m) {
    py::class_<geompp::BBox2D>(m, "BBox2D",
        "2D axis-aligned bounding box.")
        .def(py::init<const geompp::Point2D&,     const geompp::Point2D&>(),     "min"_a, "max"_a)
        .def(py::init<const geompp::LineSegment2D&>(), "segment"_a)
        .def(py::init<const geompp::Polyline2D&>(),    "polyline"_a)
        .def(py::init<const geompp::Polygon2D&>(),     "polygon"_a)
        .def(py::init<const geompp::Triangle2D&>(),    "triangle"_a)
        .def(py::init<const std::vector<geompp::Point2D>&>(), "points"_a)
        .def(py::init<const geompp::BBox2D&>())
        .def_property_readonly("min", &geompp::BBox2D::min)
        .def_property_readonly("max", &geompp::BBox2D::max)
        .def("contains",     &geompp::BBox2D::Contains, "point"_a)
        BIND_ALMOST_EQUALS(BBox2D)
        .def("__eq__", [](const geompp::BBox2D& a, const geompp::BBox2D& b) { return a == b; });
}
