#include "bind_helpers.hpp"

void bind_plane(py::module_& m) {
    py::class_<geompp::Plane>(m, "Plane",
        "3D plane (origin + normal + two orthonormal axes).")
        .def_static("from_3_points",          &geompp::Plane::From3Points,         "p1"_a, "p2"_a, "p3"_a)
        .def_static("from_origin_and_axes",   &geompp::Plane::FromOriginAndAxes,   "origin"_a, "u"_a, "v"_a)
        .def_static("from_origin_and_normal", &geompp::Plane::FromOriginAndNormal, "origin"_a, "normal"_a)
        .def_static("xy", &geompp::Plane::XY)
        .def_static("yz", &geompp::Plane::YZ)
        .def_static("zx", &geompp::Plane::ZX)
        .def(py::init<const geompp::Plane&>())
        .def_property_readonly("origin", &geompp::Plane::origin)
        .def_property_readonly("normal", &geompp::Plane::normal)
        .def_property_readonly("axis_u", &geompp::Plane::axis_u)
        .def_property_readonly("axis_v", &geompp::Plane::axis_v)
        .def("signed_distance_to", &geompp::Plane::SignedDistanceTo, "point"_a)
        .def("distance_to",        &geompp::Plane::DistanceTo,       "point"_a)
        .def("project_onto",       &geompp::Plane::ProjectOnto,      "point"_a)
        .def("project_into",       &geompp::Plane::ProjectInto,      "point"_a)
        .def("evaluate",           &geompp::Plane::Evaluate,         "p2d"_a)
        .def("contains",           &geompp::Plane::Contains,         "point"_a)
        BIND_ALMOST_EQUALS(Plane)
        .def("intersects",
             [](const geompp::Plane& pl, const geompp::Line3D& l) { return pl.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Plane& pl, const geompp::Ray3D& r) { return pl.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Plane& pl, const geompp::LineSegment3D& s) { return pl.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Plane& pl, const geompp::Plane& other) { return pl.Intersects(other); }, "other"_a)
        .def("intersects",
             [](const geompp::Plane& pl, const geompp::Triangle3D& t) { return pl.Intersects(t); }, "triangle"_a)
        .def("intersection",
             [](const geompp::Plane& pl, const geompp::Line3D& l) -> py::object { return opt_variant_to_py(pl.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Plane& pl, const geompp::Ray3D& r) -> py::object { return opt_variant_to_py(pl.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Plane& pl, const geompp::LineSegment3D& s) -> py::object { return opt_variant_to_py(pl.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Plane& pl, const geompp::Plane& other) -> py::object { return opt_variant_to_py(pl.Intersection(other)); }, "other"_a)
        .def("intersection",
             [](const geompp::Plane& pl, const geompp::Triangle3D& t) -> py::object { return opt_variant_to_py(pl.Intersection(t)); }, "triangle"_a)
        .def("is_parallel",
             [](const geompp::Plane& pl, const geompp::Line3D& l) { return pl.IsParallel(l); }, "line"_a)
        .def("is_parallel",
             [](const geompp::Plane& pl, const geompp::Ray3D& r) { return pl.IsParallel(r); }, "ray"_a)
        .def("is_parallel",
             [](const geompp::Plane& pl, const geompp::LineSegment3D& s) { return pl.IsParallel(s); }, "segment"_a)
        .def("is_coplanar",
             [](const geompp::Plane& pl, const geompp::Line3D& l) { return pl.IsCoplanar(l); }, "line"_a)
        .def("is_coplanar",
             [](const geompp::Plane& pl, const geompp::Ray3D& r) { return pl.IsCoplanar(r); }, "ray"_a)
        .def("is_coplanar",
             [](const geompp::Plane& pl, const geompp::LineSegment3D& s) { return pl.IsCoplanar(s); }, "segment"_a)
        .def("__eq__", [](const geompp::Plane& a, const geompp::Plane& b) { return a == b; });
}
