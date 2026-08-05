#include "bind_helpers.hpp"

void bind_triangle2d(py::module_& m) {
    py::class_<geompp::Triangle2D>(m, "Triangle2D",
        "2D triangle defined by three vertices.")
        .def_static("make", &geompp::Triangle2D::Make, "p0"_a, "p1"_a, "p2"_a)
        .def(py::init<const geompp::Triangle2D&>())
        // pybind11 3.x doesn't auto-cast std::tuple<RegisteredType...>, so we wrap explicitly
        .def_property_readonly("vertices", [](const geompp::Triangle2D& t) {
            auto [p0, p1, p2] = t.Vertices();
            return py::make_tuple(py::cast(p0), py::cast(p1), py::cast(p2));
        })
        .def("centroid",    &geompp::Triangle2D::Centroid)
        .def("to_polygon",  &geompp::Triangle2D::ToPolygon)
        .def("signed_area", &geompp::Triangle2D::SignedArea)
        .def("is_ccw",      &geompp::Triangle2D::IsCCW)
        .def("area",        &geompp::Triangle2D::Area)
        .def("perimeter",   &geompp::Triangle2D::Perimeter)
        .def("distance_to", &geompp::Triangle2D::DistanceTo, "point"_a)
        .def("to_axis", [](const geompp::Triangle2D& t) {
            auto [u, v] = t.ToAxis();
            return py::make_tuple(py::cast(u), py::cast(v));
        })
        // interpolate() → std::optional<Point2D> — single type, pybind11 handles fine
        .def("interpolate", [](const geompp::Triangle2D& t, double s, double tt) -> py::object {
            auto r = t.Interpolate(s, tt);
            if (!r.has_value()) {
                return py::none();
            }
            return py::cast(r.value());
        }, "s"_a, "t"_a)
        .def("location", [](const geompp::Triangle2D& t, const geompp::Point2D& p) -> py::object {
            auto r = t.Location(p);
            if (!r.has_value()) {
                return py::none();
            }
            auto [s, tt] = r.value();
            return py::make_tuple(s, tt);
        }, "point"_a)
        .def("contains",    &geompp::Triangle2D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Triangle2D)
        BIND_SERIALIZATION(Triangle2D)
        .def("intersects",
             [](const geompp::Triangle2D& t, const geompp::Line2D& l) { return t.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Triangle2D& t, const geompp::Ray2D& r) { return t.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Triangle2D& t, const geompp::LineSegment2D& s) { return t.Intersects(s); }, "segment"_a)
        .def("intersects_triangle",
             [](const geompp::Triangle2D& t, const geompp::Triangle2D& other) { return t.Intersects(other); }, "other"_a)
        .def("intersection",
             [](const geompp::Triangle2D& t, const geompp::Line2D& l) -> py::object { return opt_variant_to_py(t.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Triangle2D& t, const geompp::Ray2D& r) -> py::object { return opt_variant_to_py(t.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Triangle2D& t, const geompp::LineSegment2D& s) -> py::object { return opt_variant_to_py(t.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Triangle2D& t, const geompp::Triangle2D& other) -> py::object { return opt_variant_to_py(t.Intersection(other)); }, "other"_a)
        .def("__eq__", [](const geompp::Triangle2D& a, const geompp::Triangle2D& b) { return a == b; });
}
