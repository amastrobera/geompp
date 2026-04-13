#include "bind_helpers.hpp"

void bind_ray3d(py::module_& m) {
    py::class_<geompp::Ray3D>(m, "Ray3D",
        "3D ray (semi-infinite line from origin in direction).")
        .def_static("make", &geompp::Ray3D::Make, "origin"_a, "direction"_a)
        .def(py::init<const geompp::Ray3D&>())
        .def_property_readonly("origin",    &geompp::Ray3D::Origin)
        .def_property_readonly("direction", &geompp::Ray3D::Direction)
        .def("is_ahead",    &geompp::Ray3D::IsAhead,    "point"_a)
        .def("is_behind",   &geompp::Ray3D::IsBehind,   "point"_a)
        .def("to_line",     &geompp::Ray3D::ToLine)
        .def("distance_to", &geompp::Ray3D::DistanceTo, "point"_a)
        .def("contains",    &geompp::Ray3D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Ray3D)
        BIND_SERIALIZATION(Ray3D)
        .def("intersects",
             [](const geompp::Ray3D& r, const geompp::Line3D& l)        { return r.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Ray3D& r, const geompp::Ray3D& o)         { return r.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Ray3D& r, const geompp::LineSegment3D& s) { return r.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Ray3D& r, const geompp::Line3D& l)        { return opt_variant_to_py(r.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Ray3D& r, const geompp::Ray3D& o)         { return opt_variant_to_py(r.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Ray3D& r, const geompp::LineSegment3D& s) { return opt_variant_to_py(r.Intersection(s)); }, "segment"_a)
        .def("__eq__", [](const geompp::Ray3D& a, const geompp::Ray3D& b) { return a == b; });
}
