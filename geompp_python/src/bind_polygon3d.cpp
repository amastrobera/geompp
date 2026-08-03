#include "bind_helpers.hpp"

#include <utility>

void bind_polygon3d(py::module_& m) {
    py::class_<geompp::Polygon3D>(m, "Polygon3D",
        "3D polygon (ordered vertex list).")
        .def_static("make",
             // Takes `pts` by value (not const&): pybind11 already builds a fresh std::vector to
             // convert the Python list, so this is a free move into Make(vector&&) rather than an
             // extra copy on top of an already-owned temporary.
             [](std::vector<geompp::Point3D> pts) { return geompp::Polygon3D::Make(std::move(pts)); },
             "points"_a)
        .def_static("make",
             [](std::vector<geompp::Point3D> pts, std::vector<std::vector<geompp::Point3D>> holes) {
                 return geompp::Polygon3D::Make(std::move(pts), std::move(holes));
             },
             "points"_a, "holes"_a)
        .def(py::init<const geompp::Polygon3D&>())
        .def("size",        &geompp::Polygon3D::Size)
        .def("to_segments", [](const geompp::Polygon3D& p) {
            auto r = p.ToSegments();
            return std::vector<geompp::LineSegment3D>(r.begin(), r.end());
        })
        .def("get_plane",   &geompp::Polygon3D::GetPlane)
        .def("centroid",    &geompp::Polygon3D::Centroid)
        .def("area",        &geompp::Polygon3D::Area)
        .def("perimeter_size", &geompp::Polygon3D::PerimeterSize)
        .def("is_simple",   &geompp::Polygon3D::IsSimple,    "Returns True if the polygon boundary has no self-intersections.")
        .def("is_convex",   &geompp::Polygon3D::IsConvex)
        .def("simplify",    &geompp::Polygon3D::Simplify,
             "Decomposes a self-intersecting polygon into one or more simple polygons. "
             "Returns [self] if already simple.")
        .def("triangulate", &geompp::Polygon3D::Triangulate,
             "strategy"_a = geompp::TriangulationParams::Strategy::EarClipping,
             "Breaks the polygon (outer ring only, holes are ignored) down into a list of Triangle3D, in "
             "the polygon's own plane. make() already guarantees the outer ring is simple/CCW/collinear-"
             "free, so this never re-validates.")
        .def("distance_to", &geompp::Polygon3D::DistanceTo,  "point"_a)
        .def("contains",       &geompp::Polygon3D::Contains,      "point"_a)
        .def("is_on_perimeter", &geompp::Polygon3D::IsOnPerimeter, "point"_a)
        .def("convex_hull",    &geompp::Polygon3D::ConvexHull, "Returns the convex hull as a new Polygon3D.")
        .def("perimeter",      &geompp::Polygon3D::Perimeter,  "Returns the vertices as a list of Point3D.")
        .def("has_holes",      &geompp::Polygon3D::HasHoles,   "Returns True if the polygon has one or more holes.")
        .def("holes",          &geompp::Polygon3D::Holes,
             "Returns the holes as a list of rings, each a list of Point3D. Empty when the polygon has no holes.")
        BIND_ALMOST_EQUALS(Polygon3D)
        BIND_SERIALIZATION(Polygon3D)
        .def("intersects",
             [](const geompp::Polygon3D& p, const geompp::Line3D& l) { return p.Intersects(l); }, "line"_a)
        .def("intersects",
             [](const geompp::Polygon3D& p, const geompp::Ray3D& r) { return p.Intersects(r); }, "ray"_a)
        .def("intersects",
             [](const geompp::Polygon3D& p, const geompp::LineSegment3D& s) { return p.Intersects(s); }, "segment"_a)
        .def("intersects",
             [](const geompp::Polygon3D& p, const geompp::Polygon3D& other) { return p.Intersects(other); }, "other"_a,
             "True if this polygon shares any point with other — area overlap when coplanar, or a genuine "
             "strike-through (shared segment on the two planes' common line) when not.")
        .def("intersection",
             [](const geompp::Polygon3D& p, const geompp::Line3D& l) -> py::object { return opt_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polygon3D& p, const geompp::Ray3D& r) -> py::object { return opt_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polygon3D& p, const geompp::LineSegment3D& s) -> py::object { return opt_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polygon3D& p, const geompp::Polygon3D& other) -> py::object { return opt_variant_to_py(p.Intersection(other)); },
             "other"_a,
             "Intersection with another polygon: a list[Polygon3D] when coplanar, a list[LineSegment3D] "
             "when the planes cross (the chord(s) where both bounded regions cover the shared line), or "
             "None if they share no point.")
        .def("union",
             [](const geompp::Polygon3D& p, const geompp::Polygon3D& other) { return p.Union(other); }, "other"_a,
             "Set union with a coplanar polygon. Raises if the two polygons aren't coplanar.")
        .def("difference",
             [](const geompp::Polygon3D& p, const geompp::Polygon3D& other) { return p.Difference(other); }, "other"_a,
             "Set difference (self minus other) with a coplanar polygon. Raises if not coplanar.")
        .def("xor",
             [](const geompp::Polygon3D& p, const geompp::Polygon3D& other) { return p.Xor(other); }, "other"_a,
             "Symmetric difference with a coplanar polygon. Raises if not coplanar.")
        .def("__len__",     &geompp::Polygon3D::Size)
        .def("__getitem__", [](const geompp::Polygon3D& p, int i) -> geompp::Point3D {
            if (i < 0) {
                i += static_cast<int>(p.Size());
            }
            if (i < 0 || i >= static_cast<int>(p.Size())) {
                throw py::index_error("index out of range");
            }
            return p[i];
        }, "i"_a)
        .def("__iter__", [](const geompp::Polygon3D& p) {
            py::list pts;
            for (std::size_t i = 0; i < p.Size(); ++i) {
                pts.append(p[static_cast<int>(i)]);
            }
            return pts.attr("__iter__")();
        })
        .def("__eq__", [](const geompp::Polygon3D& a, const geompp::Polygon3D& b) { return a == b; });
}
