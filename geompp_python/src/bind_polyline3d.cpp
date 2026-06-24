#include "bind_helpers.hpp"

void bind_polyline3d(py::module_& m) {
    py::class_<geompp::Polyline3D>(m, "Polyline3D",
        "3D polyline (open chain of line segments).")
        .def_static("make", &geompp::Polyline3D::Make, "points"_a)
        .def(py::init<const geompp::Polyline3D&>())
        .def("size",        &geompp::Polyline3D::Size)
        .def("to_segments", [](const geompp::Polyline3D& p) {
            auto r = p.ToSegments();
            return std::vector<geompp::LineSegment3D>(r.begin(), r.end());
        })
        .def("length",      &geompp::Polyline3D::Length)
        .def("distance_to",  &geompp::Polyline3D::DistanceTo,  "point"_a)
        .def("project_onto", &geompp::Polyline3D::ProjectOnto, "point"_a)
        .def("location",     &geompp::Polyline3D::Location,    "point"_a)
        .def("interpolate", &geompp::Polyline3D::Interpolate, "pct"_a)
        .def("contains",    &geompp::Polyline3D::Contains,    "point"_a)
        .def("is_planar",   &geompp::Polyline3D::IsPlanar)
        .def("is_simple",   &geompp::Polyline3D::IsSimple)
        .def("is_convex",   &geompp::Polyline3D::IsConvex)
        .def("convex_hull", &geompp::Polyline3D::ConvexHull)
        .def("to_polygon",  &geompp::Polyline3D::ToPolygon)
        BIND_ALMOST_EQUALS(Polyline3D)
        BIND_SERIALIZATION(Polyline3D)
        .def("__len__",     &geompp::Polyline3D::Size)
        .def("__getitem__", [](const geompp::Polyline3D& p, int i) -> geompp::Point3D {
            int n = p.Size();
            if (i < 0) i += n;
            if (i < 0 || i >= n) throw py::index_error("index out of range");
            return p[static_cast<std::size_t>(i)];
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
             [](const geompp::Polyline3D& p, const geompp::Line3D& l)        -> py::object { return opt_variant_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Ray3D& r)         -> py::object { return opt_variant_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::LineSegment3D& s) -> py::object { return opt_variant_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polyline3D& p, const geompp::Polyline3D& o)    -> py::object { return opt_variant_to_py(p.Intersection(o)); }, "other"_a)
        .def("__eq__", [](const geompp::Polyline3D& a, const geompp::Polyline3D& b) { return a == b; });
}
