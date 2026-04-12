#include "bind_helpers.hpp"

void bind_line_segment2d(py::module_& m) {
    py::class_<geompp::LineSegment2D>(m, "LineSegment2D",
        "2D finite segment between two points.")
        .def_static("make", &geompp::LineSegment2D::Make, "p0"_a, "p1"_a)
        .def(py::init<const geompp::LineSegment2D&>())
        .def_property_readonly("first", &geompp::LineSegment2D::First)
        .def_property_readonly("last",  &geompp::LineSegment2D::Last)
        .def("to_line",     &geompp::LineSegment2D::ToLine)
        .def("length",      &geompp::LineSegment2D::Length)
        .def("distance_to", &geompp::LineSegment2D::DistanceTo,  "point"_a)
        .def("location",    &geompp::LineSegment2D::Location,    "point"_a)
        .def("interpolate", &geompp::LineSegment2D::Interpolate, "pct"_a)
        .def("contains",    &geompp::LineSegment2D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(LineSegment2D)
        BIND_SERIALIZATION(LineSegment2D)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        { return s.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         { return s.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) { return s.Intersects(o); }, "segment"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        { return opt_variant_to_py(s.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         { return opt_variant_to_py(s.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) { return opt_variant_to_py(s.Intersection(o)); }, "segment"_a)
        .def("__eq__", [](const geompp::LineSegment2D& a, const geompp::LineSegment2D& b) { return a == b; });
}
