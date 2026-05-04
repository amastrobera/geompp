#include "bind_helpers.hpp"

void bind_line2d(py::module_& m) {
    py::class_<geompp::Line2D>(m, "Line2D",
        "Infinite 2D line (origin + unit direction).")
        .def_static("make",
             py::overload_cast<const geompp::Point2D&, const geompp::Point2D&>(&geompp::Line2D::Make),
             "p0"_a, "p1"_a)
        .def_static("make",
             py::overload_cast<const geompp::Point2D&, const geompp::Vector2D&>(&geompp::Line2D::Make),
             "origin"_a, "direction"_a)
        .def(py::init<const geompp::Line2D&>())
        .def_property_readonly("first",     &geompp::Line2D::First)
        .def_property_readonly("last",      &geompp::Line2D::Last)
        .def_property_readonly("origin",    &geompp::Line2D::Origin)
        .def_property_readonly("direction", &geompp::Line2D::Direction)
        .def("distance_to",  &geompp::Line2D::DistanceTo,  "point"_a)
        .def("project_onto", &geompp::Line2D::ProjectOnto, "point"_a)
        .def("contains",     &geompp::Line2D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Line2D)
        BIND_SERIALIZATION(Line2D)
        .def("intersects",
             [](const geompp::Line2D& l, const geompp::Line2D& o)        { return l.Intersects(o); }, "other"_a)
        .def("intersects",
             [](const geompp::Line2D& l, const geompp::Ray2D& r)         { return l.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Line2D& l, const geompp::LineSegment2D& s) { return l.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Line2D& l, const geompp::Line2D& o)        -> py::object { return opt_variant_to_py(l.Intersection(o)); }, "other"_a)
        .def("intersection",
             [](const geompp::Line2D& l, const geompp::Ray2D& r)         -> py::object { return opt_variant_to_py(l.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Line2D& l, const geompp::LineSegment2D& s) -> py::object { return opt_variant_to_py(l.Intersection(s)); }, "segment"_a)
        // Returns (Optional[Point2D], sc, tc) where sc/tc are parametric t-values
        .def("intersection_with_params",
             [](const geompp::Line2D& l, const geompp::Line2D& other) {
                 double sc = 0.0, tc = 0.0;
                 auto result = l.Intersection(other, sc, tc);
                 return py::make_tuple(opt_variant_to_py(result), sc, tc);
             }, "other"_a,
             "Returns (point_or_None, sc, tc). sc/tc are the parametric positions on each line.")
        .def("__eq__", [](const geompp::Line2D& a, const geompp::Line2D& b) { return a == b; });
}
