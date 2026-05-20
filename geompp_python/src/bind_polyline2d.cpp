#include "bind_helpers.hpp"

void bind_polyline2d(py::module_& m) {
    py::class_<geompp::Polyline2D>(m, "Polyline2D",
        "2D polyline (open chain of line segments).")
        .def_static("make", &geompp::Polyline2D::Make, "points"_a)
        .def(py::init<const geompp::Polyline2D&>())
        .def("size",        &geompp::Polyline2D::Size)
        .def("to_segments", [](const geompp::Polyline2D& p) {
            auto r = p.ToSegments();
            return std::vector<geompp::LineSegment2D>(r.begin(), r.end());
        })
        .def("length",      &geompp::Polyline2D::Length)
        .def("distance_to",  &geompp::Polyline2D::DistanceTo,  "point"_a)
        .def("project_onto", &geompp::Polyline2D::ProjectOnto, "point"_a)
        .def("location",     &geompp::Polyline2D::Location,    "point"_a)
        .def("interpolate", &geompp::Polyline2D::Interpolate, "pct"_a)
        .def("contains",    &geompp::Polyline2D::Contains,    "point"_a)
        BIND_ALMOST_EQUALS(Polyline2D)
        BIND_SERIALIZATION(Polyline2D)
        .def("__len__",     &geompp::Polyline2D::Size)
        .def("__getitem__", [](const geompp::Polyline2D& p, int i) -> geompp::Point2D {
            int n = p.Size();
            if (i < 0) i += n;
            if (i < 0 || i >= n) throw py::index_error("index out of range");
            return p[static_cast<std::size_t>(i)];
        }, "i"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    { return p.Intersects(o); }, "other"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        -> py::object { return opt_variant_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         -> py::object { return opt_variant_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) -> py::object { return opt_variant_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    -> py::object { return opt_variant_to_py(p.Intersection(o)); }, "other"_a)
        .def("__eq__", [](const geompp::Polyline2D& a, const geompp::Polyline2D& b) { return a == b; });
}
