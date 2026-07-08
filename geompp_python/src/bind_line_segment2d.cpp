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
        .def("reversed",    &geompp::LineSegment2D::Reversed)
        .def("distance_to",  &geompp::LineSegment2D::DistanceTo,  "point"_a)
        .def("project_onto", &geompp::LineSegment2D::ProjectOnto, "point"_a)
        .def("location",     &geompp::LineSegment2D::Location,    "point"_a)
        .def("interpolate", &geompp::LineSegment2D::Interpolate, "pct"_a)
        .def("contains",    &geompp::LineSegment2D::Contains,    "point"_a)
        .def("is_left",     &geompp::LineSegment2D::IsLeft,      "point"_a)
        BIND_ALMOST_EQUALS(LineSegment2D)
        BIND_SERIALIZATION(LineSegment2D)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        { return s.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         { return s.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) { return s.Intersects(o); }, "segment"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        -> py::object { return opt_to_py(s.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         -> py::object { return opt_to_py(s.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) -> py::object { return opt_to_py(s.Intersection(o)); }, "segment"_a)
        .def("overlaps",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        { return s.Overlaps(l); }, "line"_a)
        .def("overlaps",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         { return s.Overlaps(r); }, "ray"_a)
        .def("overlaps",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) { return s.Overlaps(o); }, "segment"_a)
        .def("overlap",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        -> py::object { return opt_to_py(s.Overlap(l)); }, "line"_a)
        .def("overlap",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         -> py::object { return opt_to_py(s.Overlap(r)); }, "ray"_a)
        .def("overlap",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) -> py::object { return opt_to_py(s.Overlap(o)); }, "segment"_a)
        .def("touches",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        { return s.Touches(l); }, "line"_a)
        .def("touches",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         { return s.Touches(r); }, "ray"_a)
        .def("touches",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) { return s.Touches(o); }, "segment"_a)
        .def("touch",
             [](const geompp::LineSegment2D& s, const geompp::Line2D& l)        -> py::object { return opt_to_py(s.Touch(l)); }, "line"_a)
        .def("touch",
             [](const geompp::LineSegment2D& s, const geompp::Ray2D& r)         -> py::object { return opt_to_py(s.Touch(r)); }, "ray"_a)
        .def("touch",
             [](const geompp::LineSegment2D& s, const geompp::LineSegment2D& o) -> py::object { return opt_to_py(s.Touch(o)); }, "segment"_a)
        .def("overlaps",
             [](const geompp::LineSegment2D& s, const geompp::Polyline2D& p)    { return s.Overlaps(p); }, "polyline"_a)
        .def("overlap",
             [](const geompp::LineSegment2D& s, const geompp::Polyline2D& p)    -> py::object { return opt_to_py(s.Overlap(p)); }, "polyline"_a)
        .def("touches",
             [](const geompp::LineSegment2D& s, const geompp::Polyline2D& p)    { return s.Touches(p); }, "polyline"_a)
        .def("touch",
             [](const geompp::LineSegment2D& s, const geompp::Polyline2D& p)    -> py::object { return opt_to_py(s.Touch(p)); }, "polyline"_a)
        .def("__eq__", [](const geompp::LineSegment2D& a, const geompp::LineSegment2D& b) { return a == b; });
}
