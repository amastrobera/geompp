#include "bind_helpers.hpp"

void bind_ray2d(py::module_& m) {
    py::class_<geompp::Ray2D>(m, "Ray2D",
        "2D ray (semi-infinite line from origin in direction).")
        .def_static("make", &geompp::Ray2D::Make, "origin"_a, "direction"_a)
        .def(py::init<const geompp::Ray2D&>())
        .def_property_readonly("origin",    &geompp::Ray2D::Origin)
        .def_property_readonly("direction", &geompp::Ray2D::Direction)
        .def("is_ahead",    &geompp::Ray2D::IsAhead,    "point"_a)
        .def("is_behind",   &geompp::Ray2D::IsBehind,   "point"_a)
        .def("to_line",     &geompp::Ray2D::ToLine)
        .def("distance_to", &geompp::Ray2D::DistanceTo, "point"_a)
        .def("contains",    &geompp::Ray2D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Ray2D)
        BIND_SERIALIZATION(Ray2D)
        .def("intersects",
             [](const geompp::Ray2D& r, const geompp::Line2D& l)        { return r.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Ray2D& r, const geompp::Ray2D& o)         { return r.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Ray2D& r, const geompp::LineSegment2D& s) { return r.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Ray2D& r, const geompp::Line2D& l)        { return opt_variant_to_py(r.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Ray2D& r, const geompp::Ray2D& o)         { return opt_variant_to_py(r.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Ray2D& r, const geompp::LineSegment2D& s) { return opt_variant_to_py(r.Intersection(s)); }, "segment"_a)
        .def("__eq__", [](const geompp::Ray2D& a, const geompp::Ray2D& b) { return a == b; });
}
