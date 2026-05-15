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
        .def("to_polygon",  &geompp::Triangle3D::ToPolygon)
        .def("to_plane",    &geompp::Triangle3D::ToPlane)
        .def("area_vector", &geompp::Triangle3D::AreaVector)
        .def("signed_area", &geompp::Triangle3D::SignedArea, "ref_normal"_a)
        .def("is_ccw",      &geompp::Triangle3D::IsCCW,      "ref_normal"_a)
        .def("area",        &geompp::Triangle3D::Area)
        .def("perimeter",   &geompp::Triangle3D::Perimeter)
        .def("distance_to", &geompp::Triangle3D::DistanceTo, "point"_a)
        .def("to_axis", [](const geompp::Triangle3D& t) {
            auto [u, v] = t.ToAxis();
            return py::make_tuple(py::cast(u), py::cast(v));
        })
        .def("interpolate", [](const geompp::Triangle3D& t, double s, double tt) -> py::object {
            auto r = t.Interpolate(s, tt);
            if (!r.has_value()) return py::none();
            return py::cast(r.value());
        }, "s"_a, "t"_a)
        .def("location", [](const geompp::Triangle3D& t, const geompp::Point3D& p) -> py::object {
            auto r = t.Location(p);
            if (!r.has_value()) return py::none();
            auto [s, tt] = r.value();
            return py::make_tuple(s, tt);
        }, "point"_a)
        .def("contains",    &geompp::Triangle3D::Contains,   "point"_a)
        BIND_ALMOST_EQUALS(Triangle3D)
        BIND_SERIALIZATION(Triangle3D)
        .def("intersects",
             [](const geompp::Triangle3D& t, const geompp::Line3D& l) { return t.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Triangle3D& t, const geompp::Ray3D& r) { return t.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Triangle3D& t, const geompp::LineSegment3D& s) { return t.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Triangle3D& t, const geompp::Plane& p) { return t.Intersects(p); }, "plane"_a)
        .def("intersects_triangle",
             [](const geompp::Triangle3D& t, const geompp::Triangle3D& other) {
                 return t.Intersects(other);
             }, "other"_a)
        .def("intersection",
             [](const geompp::Triangle3D& t, const geompp::Line3D& l) -> py::object { return opt_variant_to_py(t.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Triangle3D& t, const geompp::Ray3D& r) -> py::object { return opt_variant_to_py(t.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Triangle3D& t, const geompp::LineSegment3D& s) -> py::object { return opt_variant_to_py(t.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Triangle3D& t, const geompp::Plane& p) -> py::object { return opt_variant_to_py(t.Intersection(p)); }, "plane"_a)
        .def("intersection",
             [](const geompp::Triangle3D& t, const geompp::Triangle3D& other) -> py::object { return opt_variant_to_py(t.Intersection(other)); }, "other"_a)
        .def("__eq__", [](const geompp::Triangle3D& a, const geompp::Triangle3D& b) { return a == b; });
}
