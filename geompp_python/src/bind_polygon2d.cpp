#include "bind_helpers.hpp"

void bind_polygon2d(py::module_& m) {
    py::class_<geompp::Polygon2D>(m, "Polygon2D",
        "2D polygon (ordered vertex list, open/closed by convention).")
        .def_static("make",
             [](const std::vector<geompp::Point2D>& pts) { return geompp::Polygon2D::Make(pts); },
             "points"_a)
        .def_static("make",
             [](const std::vector<geompp::Point2D>& pts,
                const std::vector<std::vector<geompp::Point2D>>& holes) {
                 return geompp::Polygon2D::Make(pts, holes);
             },
             "points"_a, "holes"_a)
        .def(py::init<const geompp::Polygon2D&>())
        .def("size",        &geompp::Polygon2D::Size)
        .def("to_segments", [](const geompp::Polygon2D& p) {
            auto r = p.ToSegments();
            return std::vector<geompp::LineSegment2D>(r.begin(), r.end());
        })
        .def("centroid",    &geompp::Polygon2D::Centroid)
        .def("area",        &geompp::Polygon2D::Area)
        .def("perimeter",   &geompp::Polygon2D::Perimeter)
        .def("distance_to", &geompp::Polygon2D::DistanceTo,  "point"_a)
        .def("contains",       &geompp::Polygon2D::Contains,      "point"_a)
        .def("is_on_perimeter", &geompp::Polygon2D::IsOnPerimeter, "point"_a)
        .def("is_simple",      &geompp::Polygon2D::IsSimple, "no self-intersections, but holes are allowed")
        .def("is_convex",      &geompp::Polygon2D::IsConvex)
        .def("convex_hull",    &geompp::Polygon2D::ConvexHull, "Returns the convex hull as a new Polygon2D.")
        .def("simplify",       &geompp::Polygon2D::Simplify,
             "Decomposes a self-intersecting polygon into one or more simple polygons. "
             "Returns [self] if already simple.")
        .def("to_points",      &geompp::Polygon2D::ToPoints,   "Returns the vertices as a list of Point2D.")
        BIND_ALMOST_EQUALS(Polygon2D)
        BIND_SERIALIZATION(Polygon2D)
        .def("intersects",
             [](const geompp::Polygon2D& p, const geompp::Line2D& l) { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polygon2D& p, const geompp::Ray2D& r) { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polygon2D& p, const geompp::LineSegment2D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polygon2D& p, const geompp::Line2D& l) -> py::object { return opt_variant_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polygon2D& p, const geompp::Ray2D& r) -> py::object { return opt_variant_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polygon2D& p, const geompp::LineSegment2D& s) -> py::object { return opt_variant_to_py(p.Intersection(s)); }, "segment"_a)
        .def("__len__",     &geompp::Polygon2D::Size)
        .def("__getitem__", [](const geompp::Polygon2D& p, int i) -> geompp::Point2D {
            if (i < 0) {
                i += static_cast<int>(p.Size());
            }
            if (i < 0 || i >= static_cast<int>(p.Size())) {
                throw py::index_error("index out of range");
            }
            return p[i];
        }, "i"_a)
        .def("__iter__", [](const geompp::Polygon2D& p) {
            py::list pts;
            for (std::size_t i = 0; i < p.Size(); ++i) {
                pts.append(p[static_cast<int>(i)]);
            }
            return pts.attr("__iter__")();
        })
        .def("__eq__", [](const geompp::Polygon2D& a, const geompp::Polygon2D& b) { return a == b; });
}
