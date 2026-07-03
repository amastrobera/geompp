#include "bind_helpers.hpp"

void bind_line_segment3d(py::module_& m) {
    py::class_<geompp::LineSegment3D>(m, "LineSegment3D",
        "3D finite segment between two points.")
        .def_static("make", &geompp::LineSegment3D::Make, "p0"_a, "p1"_a)
        .def(py::init<const geompp::LineSegment3D&>())
        .def_property_readonly("first", &geompp::LineSegment3D::First)
        .def_property_readonly("last",  &geompp::LineSegment3D::Last)
        .def("to_line",     &geompp::LineSegment3D::ToLine)
        .def("length",      &geompp::LineSegment3D::Length)
        .def("distance_to",
             [](const geompp::LineSegment3D& s, const geompp::Point3D& p) { return s.DistanceTo(p); }, "point"_a)
        .def("distance_to",
             [](const geompp::LineSegment3D& s, const geompp::Line3D& l) { return s.DistanceTo(l); }, "line"_a)
        .def("distance_to",
             [](const geompp::LineSegment3D& s, const geompp::Ray3D& r) { return s.DistanceTo(r); }, "ray"_a)
        .def("distance_to",
             [](const geompp::LineSegment3D& s, const geompp::LineSegment3D& o) { return s.DistanceTo(o); }, "segment"_a)
        .def("distance",
             [](const geompp::LineSegment3D& s, const geompp::Line3D& l) -> py::object {
                 auto res = s.Distance(l); return res.has_value() ? py::cast(*res) : py::none();
             }, "line"_a)
        .def("distance",
             [](const geompp::LineSegment3D& s, const geompp::Ray3D& r) -> py::object {
                 auto res = s.Distance(r); return res.has_value() ? py::cast(*res) : py::none();
             }, "ray"_a)
        .def("distance",
             [](const geompp::LineSegment3D& s, const geompp::LineSegment3D& o) -> py::object {
                 auto res = s.Distance(o); return res.has_value() ? py::cast(*res) : py::none();
             }, "segment"_a)
        .def("project_onto", &geompp::LineSegment3D::ProjectOnto, "point"_a)
        .def("location",     &geompp::LineSegment3D::Location,    "point"_a)
        .def("interpolate", &geompp::LineSegment3D::Interpolate, "pct"_a)
        .def("flip",        &geompp::LineSegment3D::Flip)
        .def("contains",    &geompp::LineSegment3D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(LineSegment3D)
        BIND_SERIALIZATION(LineSegment3D)
        .def("intersects",
             [](const geompp::LineSegment3D& s, const geompp::Line3D& l)        { return s.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::LineSegment3D& s, const geompp::Ray3D& r)         { return s.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::LineSegment3D& s, const geompp::LineSegment3D& o) { return s.Intersects(o); }, "segment"_a)
        .def("intersection",
             [](const geompp::LineSegment3D& s, const geompp::Line3D& l)        -> py::object { return opt_to_py(s.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::LineSegment3D& s, const geompp::Ray3D& r)         -> py::object { return opt_to_py(s.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::LineSegment3D& s, const geompp::LineSegment3D& o) -> py::object { return opt_to_py(s.Intersection(o)); }, "segment"_a)
        .def("__eq__", [](const geompp::LineSegment3D& a, const geompp::LineSegment3D& b) { return a == b; });
}
