#include "bind_helpers.hpp"

void bind_polyline3d(py::module_& m) {
    py::class_<geompp::Polyline3D>(m, "Polyline3D",
        "3D polyline (open chain of line segments).")
        .def_static("make", &geompp::Polyline3D::Make, "points"_a)
        .def(py::init<const geompp::Polyline3D&>())
        .def("size",        &geompp::Polyline3D::Size)
        .def("to_segments", &geompp::Polyline3D::ToSegments)
        .def("length",      &geompp::Polyline3D::Length)
        .def("distance_to", &geompp::Polyline3D::DistanceTo,  "point"_a)
        .def("location",    &geompp::Polyline3D::Location,    "point"_a)
        .def("interpolate", &geompp::Polyline3D::Interpolate, "pct"_a)
        .def("contains",    &geompp::Polyline3D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Polyline3D)
        BIND_SERIALIZATION(Polyline3D)
        .def("__len__",     &geompp::Polyline3D::Size)
        .def("__getitem__", [](const geompp::Polyline3D& p, int i) -> geompp::Point3D {
            int n = p.Size();
            if (i < 0) i += n;
            if (i < 0 || i >= n) throw py::index_error("index out of range");
            return p[static_cast<size_t>(i)];
        }, "i"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::Line3D& l)        { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::Ray3D& r)         { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::LineSegment3D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Polyline3D& p, const geompp::Polyline3D& o)    { return p.Intersects(o); }, "other"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Line3D& l)        { return opt_variant_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Ray3D& r)         { return opt_variant_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::LineSegment3D& s) { return opt_variant_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Polyline3D& o)    { return opt_variant_to_py(p.Intersection(o)); }, "other"_a)
        .def("__eq__", [](const geompp::Polyline3D& a, const geompp::Polyline3D& b) { return a == b; });
}
