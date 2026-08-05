#include "bind_helpers.hpp"

#include <utility>

void bind_polygon2d(py::module_& m) {
    py::class_<geompp::Polygon2D>(m, "Polygon2D",
        "2D polygon (ordered vertex list, open/closed by convention).")
        .def_static("make",
             // Takes `pts` by value (not const&): pybind11 already builds a fresh std::vector to
             // convert the Python list, so this is a free move into Make(vector&&) rather than an
             // extra copy on top of an already-owned temporary.
             [](std::vector<geompp::Point2D> pts) { return geompp::Polygon2D::Make(std::move(pts)); },
             "points"_a)
        .def_static("make",
             [](std::vector<geompp::Point2D> pts, std::vector<std::vector<geompp::Point2D>> holes) {
                 return geompp::Polygon2D::Make(std::move(pts), std::move(holes));
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
        .def("perimeter_size", &geompp::Polygon2D::PerimeterSize)
        .def("distance_to", &geompp::Polygon2D::DistanceTo,  "point"_a)
        .def("contains",       &geompp::Polygon2D::Contains,      "point"_a)
        .def("is_on_perimeter", &geompp::Polygon2D::IsOnPerimeter, "point"_a)
        .def("is_simple",      &geompp::Polygon2D::IsSimple, "no self-intersections, but holes are allowed")
        .def("is_convex",      &geompp::Polygon2D::IsConvex)
        .def("convex_hull",    &geompp::Polygon2D::ConvexHull, "Returns the convex hull as a new Polygon2D.")
        .def("simplify",       &geompp::Polygon2D::Simplify,
             "Decomposes a self-intersecting polygon into one or more simple polygons. "
             "Returns [self] if already simple.")
        .def("triangulate",    &geompp::Polygon2D::Triangulate,
             "strategy"_a = geompp::TriangulationParams::Strategy::EarClippingBestFit,
             "Breaks the polygon (outer ring only, holes are ignored) down into a list of Triangle2D. "
             "make() already guarantees the outer ring is simple/CCW/collinear-free, so this never "
             "re-validates.")
        .def("perimeter",      &geompp::Polygon2D::Perimeter,  "Returns the vertices as a list of Point2D.")
        .def("has_holes",      &geompp::Polygon2D::HasHoles,   "Returns True if the polygon has one or more holes.")
        .def("holes",          &geompp::Polygon2D::Holes,
             "Returns the holes as a list of rings, each a list of Point2D. Empty when the polygon has no holes.")
        BIND_ALMOST_EQUALS(Polygon2D)
        BIND_SERIALIZATION(Polygon2D)
        .def("intersects",
             [](const geompp::Polygon2D& p, const geompp::Line2D& l) { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polygon2D& p, const geompp::Ray2D& r) { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polygon2D& p, const geompp::LineSegment2D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Polygon2D& p, const geompp::Polygon2D& other) { return p.Intersects(other); }, "other"_a,
             "True if this polygon shares any area (or boundary) with other.")
        .def("intersection",
             [](const geompp::Polygon2D& p, const geompp::Line2D& l) -> py::object { return opt_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polygon2D& p, const geompp::Ray2D& r) -> py::object { return opt_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polygon2D& p, const geompp::LineSegment2D& s) -> py::object { return opt_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polygon2D& p, const geompp::Polygon2D& other) { return p.Intersection(other); }, "other"_a,
             "Set intersection with another polygon. Returns a list of Polygon2D (zero or more disjoint pieces).")
        .def("union",
             [](const geompp::Polygon2D& p, const geompp::Polygon2D& other) { return p.Union(other); }, "other"_a,
             "Set union with another polygon. Returns a list of Polygon2D (a disjoint pair yields more than one).")
        .def("difference",
             [](const geompp::Polygon2D& p, const geompp::Polygon2D& other) { return p.Difference(other); }, "other"_a,
             "Set difference (self minus other). Returns a list of Polygon2D.")
        .def("xor",
             [](const geompp::Polygon2D& p, const geompp::Polygon2D& other) { return p.Xor(other); }, "other"_a,
             "Symmetric difference (area covered by exactly one of the two polygons). Returns a list of Polygon2D.")
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
