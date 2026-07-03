#include "bind_helpers.hpp"

void bind_line3d(py::module_& m) {
    py::class_<geompp::Line3D>(m, "Line3D",
        "Infinite 3D line (origin + unit direction).")
        .def_static("make",
             py::overload_cast<const geompp::Point3D&, const geompp::Point3D&>(&geompp::Line3D::Make),
             "p0"_a, "p1"_a)
        .def_static("make",
             py::overload_cast<const geompp::Point3D&, const geompp::Vector3D&>(&geompp::Line3D::Make),
             "origin"_a, "direction"_a)
        .def(py::init<const geompp::Line3D&>())
        .def_property_readonly("first",     &geompp::Line3D::First)
        .def_property_readonly("last",      &geompp::Line3D::Last)
        .def_property_readonly("origin",    &geompp::Line3D::Origin)
        .def_property_readonly("direction", &geompp::Line3D::Direction)
        .def("distance_to",
             [](const geompp::Line3D& l, const geompp::Point3D& p) { return l.DistanceTo(p); }, "point"_a)
        .def("distance_to",
             [](const geompp::Line3D& l, const geompp::Line3D& o) { return l.DistanceTo(o); }, "other"_a)
        .def("distance_to",
             [](const geompp::Line3D& l, const geompp::Ray3D& r) { return l.DistanceTo(r); }, "ray"_a)
        .def("distance_to",
             [](const geompp::Line3D& l, const geompp::LineSegment3D& s) { return l.DistanceTo(s); }, "segment"_a)
        .def("distance",
             [](const geompp::Line3D& l, const geompp::Line3D& o) -> py::object {
                 auto r = l.Distance(o); return r.has_value() ? py::cast(*r) : py::none();
             }, "other"_a)
        .def("distance",
             [](const geompp::Line3D& l, const geompp::Ray3D& r) -> py::object {
                 auto res = l.Distance(r); return res.has_value() ? py::cast(*res) : py::none();
             }, "ray"_a)
        .def("distance",
             [](const geompp::Line3D& l, const geompp::LineSegment3D& s) -> py::object {
                 auto r = l.Distance(s); return r.has_value() ? py::cast(*r) : py::none();
             }, "segment"_a)
        .def("project_onto", &geompp::Line3D::ProjectOnto, "point"_a)
        .def("contains",     &geompp::Line3D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Line3D)
        BIND_SERIALIZATION(Line3D)
        .def("intersects",
             [](const geompp::Line3D& l, const geompp::Line3D& o)        { return l.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Line3D& l, const geompp::Ray3D& r)         { return l.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Line3D& l, const geompp::LineSegment3D& s) { return l.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Line3D& l, const geompp::Line3D& o)        -> py::object { return opt_to_py(l.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Line3D& l, const geompp::Ray3D& r)         -> py::object { return opt_to_py(l.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Line3D& l, const geompp::LineSegment3D& s) -> py::object { return opt_to_py(l.Intersection(s)); }, "segment"_a)
        .def("__eq__", [](const geompp::Line3D& a, const geompp::Line3D& b) { return a == b; });
}
