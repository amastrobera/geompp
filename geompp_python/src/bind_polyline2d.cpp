#include "bind_helpers.hpp"

void bind_polyline2d(py::module_& m) {
    py::enum_<geompp::PolylineDecimationParams::Strategy>(m, "PolylineDecimationStrategy",
        "Vertex-reduction strategy used by PolylineDecimationParams / Polyline2D.reduce() / Polyline3D.reduce(). "
        "All three interpret the same `threshold` meaning \"how much noise to remove\", but measure it "
        "differently (a plain distance, a perpendicular chord-distance, or a triangle area), so the same "
        "threshold value produces different results across strategies.")
        .value("RadialDistance", geompp::PolylineDecimationParams::Strategy::RadialDistance,
               "O(n) brute-force pass: drop a vertex if it's closer than threshold to the last *kept* vertex. "
               "Cheapest and least accurate — good as a fast noise-clustering pre-pass, not as the sole "
               "strategy when shape fidelity matters.")
        .value("RamerDouglasPeucker", geompp::PolylineDecimationParams::Strategy::RamerDouglasPeucker,
               "O(n log n) to O(n^2): recursively drop vertices closer than threshold to the chord spanning "
               "their segment. Given points P1, P2, P3, drops P2 when its perpendicular distance from the "
               "P1-P3 chord is below threshold; otherwise keeps P2 and recurses on both halves. Best "
               "general-purpose choice — preserves the vertices that most define the polyline's shape.")
        .value("VisvalingamWhyatt", geompp::PolylineDecimationParams::Strategy::VisvalingamWhyatt,
               "O(n log n) to O(n^2): repeatedly drop the vertex forming the smallest-area triangle with "
               "its neighbors, while that area stays below threshold, then re-evaluate the neighbors. Tends "
               "to preserve visually significant features (sharp spikes) better than RadialDistance while "
               "being similarly simple to reason about.")
        .export_values();

    py::class_<geompp::PolylineDecimationParams>(m, "PolylineDecimationParams",
        "Bundles the vertex-decimation strategy and its threshold for Polyline2D.reduce() / Polyline3D.reduce(). "
        "Defaults to RamerDouglasPeucker with threshold=0.5, matching reduce()'s own defaults.")
        .def(py::init([](geompp::PolylineDecimationParams::Strategy strategy, double threshold) {
                 geompp::PolylineDecimationParams p;
                 p.strategy = strategy;
                 p.threshold = threshold;
                 return p;
             }),
             "strategy"_a = geompp::PolylineDecimationParams::Strategy::RamerDouglasPeucker, "threshold"_a = 0.5)
        .def_readwrite("strategy", &geompp::PolylineDecimationParams::strategy)
        .def_readwrite("threshold", &geompp::PolylineDecimationParams::threshold);

    py::enum_<geompp::PolylineExpansionParams::Mode>(m, "PolylineExpansionMode",
        "How densely PolylineExpansionParams / Polyline2D.expand() / Polyline3D.expand() sample each "
        "corner's arc.")
        .value("FixedSegments", geompp::PolylineExpansionParams::Mode::FixedSegments,
               "Sample an exact number of segments per corner — see segments_per_corner.")
        .value("MinDistance", geompp::PolylineExpansionParams::Mode::MinDistance,
               "Sample roughly min_distance apart, however many points that takes per corner.")
        .export_values();

    py::class_<geompp::PolylineExpansionParams>(m, "PolylineExpansionParams",
        "Bundles the corner-rounding controls for Polyline2D.expand() / Polyline3D.expand(). Defaults to "
        "smoothness=0.5, FixedSegments with segments_per_corner=4, and min_segment_length=DOUBLE_EPSILON, "
        "matching expand()'s own defaults.")
        .def(py::init([](double smoothness, geompp::PolylineExpansionParams::Mode mode, int segments_per_corner,
                          double min_distance, double min_segment_length) {
                 geompp::PolylineExpansionParams p;
                 p.smoothness = smoothness;
                 p.mode = mode;
                 p.segments_per_corner = segments_per_corner;
                 p.min_distance = min_distance;
                 p.min_segment_length = min_segment_length;
                 return p;
             }),
             "smoothness"_a = 0.5, "mode"_a = geompp::PolylineExpansionParams::Mode::FixedSegments,
             "segments_per_corner"_a = 4, "min_distance"_a = 0.1, "min_segment_length"_a = static_cast<double>(geompp::DOUBLE_EPSILON))
        .def_readwrite("smoothness", &geompp::PolylineExpansionParams::smoothness)
        .def_readwrite("mode", &geompp::PolylineExpansionParams::mode)
        .def_readwrite("segments_per_corner", &geompp::PolylineExpansionParams::segments_per_corner)
        .def_readwrite("min_distance", &geompp::PolylineExpansionParams::min_distance)
        .def_readwrite("min_segment_length", &geompp::PolylineExpansionParams::min_segment_length);

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
        .def("convex_hull", &geompp::Polyline2D::ConvexHull,
             "Returns the convex hull of the polyline as a Polygon2D (Melkman's algorithm, O(n)).\n"
             "The polyline must be simple (no self-intersections). Call is_simple() first to verify;\n"
             "behaviour is undefined on non-simple input.")
        .def("reduce", &geompp::Polyline2D::Reduce,
             "settings"_a = geompp::PolylineDecimationParams{},
             "Returns a copy of this polyline with fewer vertices, per the given PolylineDecimationParams.")
        .def("expand", &geompp::Polyline2D::Expand,
             "settings"_a = geompp::PolylineExpansionParams{},
             "Returns a copy of this polyline with every inner corner rounded by a quadratic Bezier arc, "
             "per the given PolylineExpansionParams. The inverse direction of reduce(): adds vertices "
             "rather than removing them.")
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
            if (i < 0) {
                i += n;
            }
            if (i < 0 || i >= n) {
                throw py::index_error("index out of range");
            }
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
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        -> py::object { return opt_to_py(p.Intersection(l)); }, "line"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         -> py::object { return opt_to_py(p.Intersection(r)); }, "ray"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) -> py::object { return opt_to_py(p.Intersection(s)); }, "segment"_a)
        .def("intersection",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    -> py::object { return opt_to_py(p.Intersection(o)); }, "other"_a)
        .def("overlaps",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        { return p.Overlaps(l); }, "line"_a)
        .def("overlaps",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         { return p.Overlaps(r); }, "ray"_a)
        .def("overlaps",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) { return p.Overlaps(s); }, "segment"_a)
        .def("overlaps",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    { return p.Overlaps(o); }, "other"_a)
        .def("overlap",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        -> py::object { return opt_to_py(p.Overlap(l)); }, "line"_a)
        .def("overlap",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         -> py::object { return opt_to_py(p.Overlap(r)); }, "ray"_a)
        .def("overlap",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) -> py::object { return opt_to_py(p.Overlap(s)); }, "segment"_a)
        .def("overlap",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    -> py::object { return opt_to_py(p.Overlap(o)); }, "other"_a)
        .def("touches",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        { return p.Touches(l); }, "line"_a)
        .def("touches",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         { return p.Touches(r); }, "ray"_a)
        .def("touches",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) { return p.Touches(s); }, "segment"_a)
        .def("touches",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    { return p.Touches(o); }, "other"_a)
        .def("touch",
             [](const geompp::Polyline2D& p, const geompp::Line2D& l)        -> py::object { return opt_to_py(p.Touch(l)); }, "line"_a)
        .def("touch",
             [](const geompp::Polyline2D& p, const geompp::Ray2D& r)         -> py::object { return opt_to_py(p.Touch(r)); }, "ray"_a)
        .def("touch",
             [](const geompp::Polyline2D& p, const geompp::LineSegment2D& s) -> py::object { return opt_to_py(p.Touch(s)); }, "segment"_a)
        .def("touch",
             [](const geompp::Polyline2D& p, const geompp::Polyline2D& o)    -> py::object { return opt_to_py(p.Touch(o)); }, "other"_a)
        .def("__eq__", [](const geompp::Polyline2D& a, const geompp::Polyline2D& b) { return a == b; });
}
