#include "bind_helpers.hpp"

void bind_triangle3d(py::module_& m) {
    py::class_<geompp::Triangle3D>(m, "Triangle3D",
        "3D triangle defined by three vertices.")
        .def_static("make", &geompp::Triangle3D::Make, "p0"_a, "p1"_a, "p2"_a)
        .def(py::init<const geompp::Triangle3D&>())
        .def_property_readonly("vertices", [](const geompp::Triangle3D& t) {
            auto [p0, p1, p2] = t.Vertices();
            return py::make_tuple(py::cast(p0), py::cast(p1), py::cast(p2));
        })
        .def("centroid",    &geompp::Triangle3D::Centroid)
        .def("signed_area", &geompp::Triangle3D::SignedArea)
        .def("area",        &geompp::Triangle3D::Area)
        .def("perimeter",   &geompp::Triangle3D::Perimeter)
        .def("distance_to", &geompp::Triangle3D::DistanceTo, "point"_a)
        .def("to_axis", [](const geompp::Triangle3D& t) {
            auto [u, v] = t.ToAxis();
            return py::make_tuple(py::cast(u), py::cast(v));
        })
        .def("location", [](const geompp::Triangle3D& t, const geompp::Point3D& p) {
            auto [s, tt] = t.Location(p);
            return py::make_tuple(s, tt);
        }, "point"_a)
        .def("interpolate", [](const geompp::Triangle3D& t, double s, double tt) -> py::object {
            auto r = t.Interpolate(s, tt);
            if (!r.has_value()) return py::none();
            return py::cast(r.value());
        }, "s"_a, "t"_a)
        .def("contains",    &geompp::Triangle3D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Triangle3D)
        BIND_SERIALIZATION(Triangle3D)
        .def("intersects",
             [](const geompp::Triangle3D& t, const geompp::Line3D& l) { return t.Intersects(l); }, "line"_a)
        .def("intersection",
             [](const geompp::Triangle3D& t, const geompp::Line3D& l) { return opt_variant_to_py(t.Intersection(l)); }, "line"_a)
        .def("__eq__", [](const geompp::Triangle3D& a, const geompp::Triangle3D& b) { return a == b; });
}
