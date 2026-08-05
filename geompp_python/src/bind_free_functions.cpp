#include "bind_helpers.hpp"

#include "line_segment2d.hpp"  // has_intersections / find_intersections
#include "point2d.hpp"         // convex_hull
#include "calc_utils2d.hpp"    // ExtremePoints / find_extreme_points (2D)
#include "calc_utils3d.hpp"    // principal_axes / principal_normal / principal_direction / find_extreme_points (3D)
#include "line2d.hpp"
#include "line3d.hpp"
#include "polygon2d.hpp"
#include "polygon3d.hpp"
#include "triangle2d.hpp"
#include "triangle3d.hpp"

void bind_free_functions(py::module_& m) {
    py::class_<geompp::CoordinateFrame>(m, "CoordinateFrame",
        "Orthonormal coordinate frame from PCA: X=primary axis, Y=secondary axis, Z=best-fit plane normal.")
        .def_readonly("x", &geompp::CoordinateFrame::X, "Primary axis — direction of largest variance.")
        .def_readonly("y", &geompp::CoordinateFrame::Y, "Secondary axis — direction of second largest variance.")
        .def_readonly("z", &geompp::CoordinateFrame::Z, "Normal — direction of least variance (best-fit plane normal).");

    py::class_<geompp::ExtremePoints<geompp::Point2D>>(m, "ExtremePoints2D",
        "The two vertices of a 2D shape extreme (least / greatest projection) along a direction.")
        .def_readonly("min_point", &geompp::ExtremePoints<geompp::Point2D>::min_point, "Vertex with least projection.")
        .def_readonly("max_point", &geompp::ExtremePoints<geompp::Point2D>::max_point, "Vertex with greatest projection.");

    py::class_<geompp::ExtremePoints<geompp::Point3D>>(m, "ExtremePoints3D",
        "The two vertices of a 3D shape extreme (least / greatest projection) along a direction.")
        .def_readonly("min_point", &geompp::ExtremePoints<geompp::Point3D>::min_point, "Vertex with least projection.")
        .def_readonly("max_point", &geompp::ExtremePoints<geompp::Point3D>::max_point, "Vertex with greatest projection.");

    py::class_<geompp::PolygonTangents<geompp::LineSegment2D>>(m, "PolygonTangents2D",
        "The left and right tangent segments from a point or polygon to a 2D polygon.")
        .def_readonly("left", &geompp::PolygonTangents<geompp::LineSegment2D>::left, "The left tangent segment.")
        .def_readonly("right", &geompp::PolygonTangents<geompp::LineSegment2D>::right, "The right tangent segment.");

    py::class_<geompp::PolygonTangents<geompp::LineSegment3D>>(m, "PolygonTangents3D",
        "The left and right tangent segments from a point or polygon to a 3D polygon.")
        .def_readonly("left", &geompp::PolygonTangents<geompp::LineSegment3D>::left, "The left tangent segment.")
        .def_readonly("right", &geompp::PolygonTangents<geompp::LineSegment3D>::right, "The right tangent segment.");

    m.def("are_collinear",
          [](const geompp::Point2D& p1, const geompp::Point2D& p2, const geompp::Point2D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 2D points are collinear.");

    m.def("are_collinear",
          [](const geompp::Point3D& p1, const geompp::Point3D& p2, const geompp::Point3D& p3) {
              return geompp::are_collinear(p1, p2, p3);
          }, "p1"_a, "p2"_a, "p3"_a, "True if three 3D points are collinear.");

    m.def("remove_consecutive_duplicates",
          [](const std::vector<geompp::Point2D>& pts) {
              return geompp::remove_consecutive_duplicates(pts);
          }, "points"_a, "Remove consecutive duplicate 2D points.");

    m.def("remove_consecutive_duplicates",
          [](const std::vector<geompp::Point3D>& pts) {
              return geompp::remove_consecutive_duplicates(pts);
          }, "points"_a, "Remove consecutive duplicate 3D points.");

    m.def("remove_duplicates",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::remove_duplicates(pts); },
          "points"_a, "Remove all duplicate 2D points.");

    m.def("remove_duplicates",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::remove_duplicates(pts); },
          "points"_a, "Remove all duplicate 3D points.");

    m.def("remove_collinear",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::remove_collinear(pts); },
          "points"_a, "Remove collinear 2D points.");

    m.def("remove_collinear",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::remove_collinear(pts); },
          "points"_a, "Remove collinear 3D points.");

    m.def("linear_combination",
          [](const std::vector<geompp::Point2D>& pts, const std::vector<double>& w) {
              return geompp::linear_combination(pts, w);
          }, "points"_a, "weights"_a, "Weighted linear combination of 2D points.");

    m.def("linear_combination",
          [](const std::vector<geompp::Point3D>& pts, const std::vector<double>& w) {
              return geompp::linear_combination(pts, w);
          }, "points"_a, "weights"_a, "Weighted linear combination of 3D points.");

    m.def("average",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::average(pts); },
          "points"_a, "Arithmetic mean of 2D points.");

    m.def("average",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::average(pts); },
          "points"_a, "Arithmetic mean of 3D points.");

    m.def("lerp",
          [](const geompp::Point2D& p0, const geompp::Point2D& p1, double t) { return geompp::lerp(p0, p1, t); },
          "p0"_a, "p1"_a, "t"_a, "Linear interpolation between two 2D points. Not clamped — t outside [0, 1] extrapolates.");

    m.def("lerp",
          [](const geompp::Point3D& p0, const geompp::Point3D& p1, double t) { return geompp::lerp(p0, p1, t); },
          "p0"_a, "p1"_a, "t"_a, "Linear interpolation between two 3D points. Not clamped — t outside [0, 1] extrapolates.");

    // ── polygon clipping ──────────────────────────────────────────────────────────────────────
    m.def("clip",
          [](const std::vector<geompp::Point2D>& clipper_loop, const std::vector<geompp::Point2D>& subject_loop) {
              return geompp::clip(clipper_loop, subject_loop);
          },
          "clipper_loop"_a, "subject_loop"_a,
          "Set intersection of two 2D point loops (no holes, last point != first — implicitly closed). "
          "Returns every ring of the result (CCW outers and CW holes mixed in one flat list — even "
          "hole-less input can produce a holed intersection).");

    m.def("clip",
          [](const std::vector<geompp::Point3D>& clipper_loop, const std::vector<geompp::Point3D>& subject_loop) {
              return geompp::clip(clipper_loop, subject_loop);
          },
          "clipper_loop"_a, "subject_loop"_a,
          "Set intersection of two 3D point loops lying on the same plane (fitted from subject_loop's "
          "first three points). Raises if clipper_loop isn't coplanar with subject_loop. Returns every "
          "ring of the result, same flat-list convention as the 2D overload.");

    m.def("centroid",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::centroid(pts); },
          "points"_a, "Centroid of a 2D polygon. Throws if the points have zero area.");

    m.def("centroid",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> plane) {
              return geompp::centroid(pts, plane);
          },
          "points"_a, "plane"_a = py::none(),
          "Centroid of a 3D polygon. Throws if the points have zero area. Plane is auto-detected if omitted.");

    m.def("signed_area",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::signed_area(pts); },
          "points"_a, "Signed area of a 2D polygon; positive = CCW, negative = CW.");

    m.def("signed_area",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> plane) {
              return geompp::signed_area(pts, plane);
          },
          "points"_a, "plane"_a = py::none(),
          "Signed area of a 3D polygon; positive = CCW, negative = CW. Plane is auto-detected if omitted.");

    m.def("are_ccw",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::are_ccw(pts); },
          "points"_a, "True if 2D points are ordered counter-clockwise.");

    m.def("are_cw",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::are_cw(pts); },
          "points"_a, "True if 2D points are ordered clockwise.");

    m.def("are_coplanar",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::are_coplanar(pts); },
          "points"_a, "True if 3D points are coplanar.");

    m.def("closest_world_plane_to",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::closest_world_plane_to(pts); },
          "points"_a, "Returns the XY, YZ or ZX world plane whose normal is closest to the points' plane normal.");

    m.def("are_ccw",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> ref_plane) {
              return geompp::are_ccw(pts, ref_plane);
          },
          "points"_a, "ref_plane"_a = py::none(),
          "True if 3D points are ordered counter-clockwise. If ref_plane is omitted, the plane is fitted from the points.");

    m.def("are_cw",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Plane> ref_plane) {
              return geompp::are_cw(pts, ref_plane);
          },
          "points"_a, "ref_plane"_a = py::none(),
          "True if 3D points are ordered clockwise. If ref_plane is omitted, the plane is fitted from the points.");

    // ── segment-set intersection (Shamos–Hoey / Bentley–Ottmann) ──────────────────────────────
    m.def("has_intersections",
          [](const std::vector<geompp::LineSegment2D>& segments) { return geompp::has_intersections(segments); },
          "segments"_a, "Shamos–Hoey: True if any two of the segments intersect.");

    m.def("find_intersections",
          [](const std::vector<geompp::LineSegment2D>& segments) {
              return geompp::find_intersections(segments);
          },
          "segments"_a, "Bentley–Ottmann: list[Point2D] of all intersection points among the segments.");

    // ── convex hull ──────────────────────────────────────────────────────────────────────────
    m.def("convex_hull",
          [](const std::vector<geompp::Point2D>& pts) { return geompp::convex_hull(pts); },
          "points"_a, "Andrew's monotone chain: convex hull of a 2D point cloud, returned in CCW order.");

    m.def("convex_hull",
          [](const std::vector<geompp::Point3D>& pts, std::optional<geompp::Vector3D> normal) {
              return geompp::convex_hull(pts, normal);
          },
          "points"_a, "normal"_a = py::none(),
          "Andrew's monotone chain: convex hull of 3D points, returned in CCW order. "
          "Points do not need to be coplanar — when normal is omitted, the best-fit plane "
          "is estimated via PCA (Jacobi eigendecomposition).");

    // ── polyline decimation ──────────────────────────────────────────────────────────────────
    // Lower-level building blocks behind Polyline2D.reduce() / Polyline3D.reduce() — operate
    // directly on a list of points instead of a Polyline object.
    m.def("dist_decimation",
          [](const std::vector<geompp::Point2D>& pts, double threshold) { return geompp::dist_decimation(pts, threshold); },
          "points"_a, "threshold"_a,
          "O(n) radial-distance decimation: drop a 2D point if it's closer than threshold to the last kept point.");
    m.def("dist_decimation",
          [](const std::vector<geompp::Point3D>& pts, double threshold) { return geompp::dist_decimation(pts, threshold); },
          "points"_a, "threshold"_a,
          "O(n) radial-distance decimation: drop a 3D point if it's closer than threshold to the last kept point.");

    m.def("rdp_decimation",
          [](const std::vector<geompp::Point2D>& pts, double threshold) { return geompp::rdp_decimation(pts, threshold); },
          "points"_a, "threshold"_a,
          "Ramer-Douglas-Peucker decimation: recursively drop 2D points closer than threshold to the chord "
          "spanning their segment.");
    m.def("rdp_decimation",
          [](const std::vector<geompp::Point3D>& pts, double threshold) { return geompp::rdp_decimation(pts, threshold); },
          "points"_a, "threshold"_a,
          "Ramer-Douglas-Peucker decimation: recursively drop 3D points closer than threshold to the chord "
          "spanning their segment.");

    m.def("vw_decimation",
          [](const std::vector<geompp::Point2D>& pts, double threshold) { return geompp::vw_decimation(pts, threshold); },
          "points"_a, "threshold"_a,
          "Visvalingam-Whyatt decimation: repeatedly drops the 2D point forming the smallest-area triangle with "
          "its neighbors, while that area stays below threshold.");
    m.def("vw_decimation",
          [](const std::vector<geompp::Point3D>& pts, double threshold) { return geompp::vw_decimation(pts, threshold); },
          "points"_a, "threshold"_a,
          "Visvalingam-Whyatt decimation: repeatedly drops the 3D point forming the smallest-area triangle with "
          "its neighbors, while that area stays below threshold.");

    // ── quadratic Bezier corner smoothing ─────────────────────────────────────────────────────
    // Two overloads sharing one Python name: pybind11 resolves them the same way C++ does here —
    // a Python int matches the num_segments (int) overload with no conversion, a Python float
    // matches the min_distance (double) overload with no conversion.
    m.def("bezier_smoothing_2",
          [](const geompp::Point2D& p0, const geompp::Point2D& p1, const geompp::Point2D& p2, double smoothness,
             double min_distance, double min_segment_length) {
              return geompp::bezier_smoothing_2(p0, p1, p2, smoothness, min_distance, min_segment_length);
          },
          "p0"_a, "p1"_a, "p2"_a, "smoothness"_a, "min_distance"_a, "min_segment_length"_a = static_cast<double>(geompp::DOUBLE_EPSILON),
          "Rounds the 2D corner at p1 with a quadratic Bezier arc tangent to p0-p1 and p1-p2, sampled roughly "
          "min_distance apart. smoothness in [0,1] controls how much of the shorter adjacent edge is trimmed "
          "into the tangent points. An adjacent edge at or below min_segment_length isn't trimmed into (both "
          "at or below: the whole corner stays sharp).");
    m.def("bezier_smoothing_2",
          [](const geompp::Point3D& p0, const geompp::Point3D& p1, const geompp::Point3D& p2, double smoothness,
             double min_distance, double min_segment_length) {
              return geompp::bezier_smoothing_2(p0, p1, p2, smoothness, min_distance, min_segment_length);
          },
          "p0"_a, "p1"_a, "p2"_a, "smoothness"_a, "min_distance"_a, "min_segment_length"_a = static_cast<double>(geompp::DOUBLE_EPSILON),
          "Rounds the 3D corner at p1 with a quadratic Bezier arc tangent to p0-p1 and p1-p2, sampled roughly "
          "min_distance apart. smoothness in [0,1] controls how much of the shorter adjacent edge is trimmed "
          "into the tangent points. An adjacent edge at or below min_segment_length isn't trimmed into (both "
          "at or below: the whole corner stays sharp).");
    m.def("bezier_smoothing_2",
          [](const geompp::Point2D& p0, const geompp::Point2D& p1, const geompp::Point2D& p2, double smoothness,
             int num_segments, double min_segment_length) {
              return geompp::bezier_smoothing_2(p0, p1, p2, smoothness, num_segments, min_segment_length);
          },
          "p0"_a, "p1"_a, "p2"_a, "smoothness"_a, "num_segments"_a, "min_segment_length"_a = static_cast<double>(geompp::DOUBLE_EPSILON),
          "Same as the min_distance overload, but samples an exact num_segments + 1 points regardless of the "
          "arc's length.");
    m.def("bezier_smoothing_2",
          [](const geompp::Point3D& p0, const geompp::Point3D& p1, const geompp::Point3D& p2, double smoothness,
             int num_segments, double min_segment_length) {
              return geompp::bezier_smoothing_2(p0, p1, p2, smoothness, num_segments, min_segment_length);
          },
          "p0"_a, "p1"_a, "p2"_a, "smoothness"_a, "num_segments"_a, "min_segment_length"_a = static_cast<double>(geompp::DOUBLE_EPSILON),
          "Same as the min_distance overload, but samples an exact num_segments + 1 points regardless of the "
          "arc's length.");

    m.def("polyline_expansion",
          [](const std::vector<geompp::Point2D>& pts, const geompp::PolylineExpansionParams& settings) {
              return geompp::polyline_expansion(pts, settings);
          },
          "points"_a, "settings"_a = geompp::PolylineExpansionParams{},
          "Rounds every inner corner of a 2D point list with a quadratic Bezier arc via bezier_smoothing_2, "
          "per the given PolylineExpansionParams. The lower-level building block behind Polyline2D.expand() "
          "/ Polyline3D.expand() — call this directly when you want to round a raw point list without "
          "constructing a Polyline first.");
    m.def("polyline_expansion",
          [](const std::vector<geompp::Point3D>& pts, const geompp::PolylineExpansionParams& settings) {
              return geompp::polyline_expansion(pts, settings);
          },
          "points"_a, "settings"_a = geompp::PolylineExpansionParams{},
          "Rounds every inner corner of a 3D point list with a quadratic Bezier arc via bezier_smoothing_2, "
          "per the given PolylineExpansionParams.");

    m.def("principal_axes",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::principal_axes(pts); },
          "points"_a,
          "PCA on a 3D point cloud: returns a CoordinateFrame (X=primary, Y=secondary, Z=normal). Requires >=3 non-collinear points.");

    m.def("principal_normal",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::principal_normal(pts); },
          "points"_a,
          "Best-fit plane normal of a 3D point cloud (PCA eigenvector with smallest eigenvalue).");

    m.def("principal_direction",
          [](const std::vector<geompp::Point3D>& pts) { return geompp::principal_direction(pts); },
          "points"_a,
          "Dominant direction of a 3D point cloud (PCA eigenvector with largest eigenvalue).");

    // ── polygon extreme points along a line ───────────────────────────────────────────────────
    m.def("find_extreme_points",
          [](const geompp::Polygon2D& polygon, const geompp::Line2D& line) {
              return geompp::find_extreme_points(polygon, line);
          },
          "polygon"_a, "line"_a,
          "The outer-ring vertices of the polygon extreme (least / greatest projection) along the line's "
          "direction, as ExtremePoints2D(min_point, max_point). Uses Daniel Sunday's O(log n) binary search "
          "when the polygon is convex, else an O(n) scan. Holes are ignored.");

    m.def("find_extreme_points",
          [](const geompp::Polygon3D& polygon, const geompp::Line3D& line) {
              return geompp::find_extreme_points(polygon, line);
          },
          "polygon"_a, "line"_a,
          "The outer-ring vertices of the polygon extreme (least / greatest projection) along the line's "
          "direction, as ExtremePoints3D(min_point, max_point). Uses Daniel Sunday's O(log n) binary search "
          "when the polygon is convex, else an O(n) scan. Holes are ignored.");

    // ── polygon-to-line distance ───────────────────────────────────────────────────────────────
    m.def("distance_to",
          [](const geompp::Polygon2D& polygon, const geompp::Line2D& line) {
              return geompp::distance_to(polygon, line);
          },
          "polygon"_a, "line"_a,
          "Distance between the polygon and an infinite line (zero if they cross). Holes are ignored.");

    m.def("distance_to",
          [](const geompp::Polygon3D& polygon, const geompp::Line3D& line) {
              return geompp::distance_to(polygon, line);
          },
          "polygon"_a, "line"_a,
          "Distance between the polygon and an infinite line (zero if they cross). Handles coplanar, "
          "parallel-offset, and skew lines. Holes are ignored.");

    // ── polygon tangents ───────────────────────────────────────────────────────────────────────
    m.def("tangents_to",
          [](const geompp::Polygon2D& polygon, const geompp::Point2D& p) {
              return geompp::tangents_to(polygon, p);
          },
          "polygon"_a, "point"_a,
          "Left and right tangent segments from a point to a 2D polygon, as PolygonTangents2D(left, right). "
          "Uses Daniel Sunday's O(log n) binary search when the polygon is convex, else reduces to its convex "
          "hull first. Point must be strictly outside the polygon and not equal to any of its vertices.");

    m.def("tangents_to",
          [](const geompp::Polygon2D& polygon, const geompp::Polygon2D& other) {
              return geompp::tangents_to(polygon, other);
          },
          "polygon"_a, "other"_a,
          "The two common outer tangent segments between two 2D polygons, as PolygonTangents2D(left, right). "
          "Neither polygon needs to be convex — each is reduced to its convex hull first when needed.");

    m.def("tangents_to",
          [](const geompp::Polygon3D& polygon, const geompp::Point3D& p) {
              return geompp::tangents_to(polygon, p);
          },
          "polygon"_a, "point"_a,
          "Left and right tangent segments from a point to a 3D polygon, as PolygonTangents3D(left, right). "
          "A tangent is inherently planar, so the point must be coplanar with the polygon; raises RuntimeError "
          "otherwise. Point must be strictly outside the polygon and not equal to any of its vertices.");

    m.def("tangents_to",
          [](const geompp::Polygon3D& polygon, const geompp::Polygon3D& other) {
              return geompp::tangents_to(polygon, other);
          },
          "polygon"_a, "other"_a,
          "The two common outer tangent segments between two 3D polygons, as PolygonTangents3D(left, right). "
          "Both polygons must lie in the same plane; raises RuntimeError otherwise.");

    // ── triangulation ──────────────────────────────────────────────────────────────────────────
    // TriangulationParams and its enums are registered separately, earlier — see
    // bind_triangulation_params.cpp for why.
    m.def("triangulate",
          [](const std::vector<geompp::Point2D>& pts, const geompp::TriangulationParams& settings) {
              return geompp::triangulate(pts, settings);
          },
          "points"_a, "settings"_a = geompp::TriangulationParams{},
          "Breaks a simple 2D polygon's outer loop (no holes) down into triangles, per the given "
          "TriangulationParams. Returns list[Triangle2D], points.size() - 2 triangles for a simple polygon.");

    m.def("triangulate",
          [](const std::vector<geompp::Point3D>& pts, const geompp::Vector3D& normal,
             const geompp::TriangulationParams& settings) {
              return geompp::triangulate(pts, normal, settings);
          },
          "points"_a, "normal"_a, "settings"_a = geompp::TriangulationParams{},
          "Breaks a simple, planar 3D polygon's outer loop (no holes) down into triangles, projected via "
          "the given plane normal, per the given TriangulationParams. Input is assumed flat/coplanar. "
          "Returns list[Triangle3D], points.size() - 2 triangles for a simple polygon.");

    m.def("triangulate",
          [](const std::vector<geompp::Point3D>& pts, const geompp::TriangulationParams& settings) {
              return geompp::triangulate(pts, settings);
          },
          "points"_a, "settings"_a = geompp::TriangulationParams{},
          "Same as the (points, normal, settings) overload, but fits the plane normal via PCA "
          "(principal_normal) automatically.");

    // ── mesh-conformity checking ("every edge has at most 1 neighbor") ───────────────────────────
    // AdjacencyConformity is registered separately, earlier — see bind_triangulation_params.cpp.
    py::class_<geompp::AdjacencyViolation<geompp::Point2D>>(m, "AdjacencyViolation2D",
        "One \"more than 1 neighbor\" violation found by validate_adjacency() across a batch of 2D "
        "facets. A T-junction (a vertex partially overlapping an edge) is fixable — see on_vertex; a "
        "non-manifold edge (a full edge shared by 3+ facets) is not, since there's no principled way "
        "to pick which 2 of the 3+ facets are \"the real pair\".")
        .def_readonly("edge_p0", &geompp::AdjacencyViolation<geompp::Point2D>::edge_p0, "The shared/coarse edge's first point.")
        .def_readonly("edge_p1", &geompp::AdjacencyViolation<geompp::Point2D>::edge_p1, "The shared/coarse edge's second point.")
        .def_readonly("facet_indices", &geompp::AdjacencyViolation<geompp::Point2D>::facet_indices,
                      "Every facet (index into the input) touching this edge. For a T-junction, "
                      "facet_indices[0] owns the coarse edge — the one fix_adjacency() splices on_vertex into.")
        .def_readonly("is_non_manifold", &geompp::AdjacencyViolation<geompp::Point2D>::is_non_manifold,
                      "True: a full edge shared by 3+ facets, not fixable. False: a T-junction, fixable.")
        .def_readonly("on_vertex", &geompp::AdjacencyViolation<geompp::Point2D>::on_vertex,
                      "Meaningful only when not is_non_manifold: the foreign vertex lying on the edge.");

    py::class_<geompp::AdjacencyViolation<geompp::Point3D>>(m, "AdjacencyViolation3D",
        "3D counterpart of AdjacencyViolation2D — same fields, operating on Point3D. Native 3D "
        "collinearity/betweenness, not View2D-projected (a shared 2D projection would be wrong for a "
        "general 3D mesh whose facets aren't all coplanar).")
        .def_readonly("edge_p0", &geompp::AdjacencyViolation<geompp::Point3D>::edge_p0, "The shared/coarse edge's first point.")
        .def_readonly("edge_p1", &geompp::AdjacencyViolation<geompp::Point3D>::edge_p1, "The shared/coarse edge's second point.")
        .def_readonly("facet_indices", &geompp::AdjacencyViolation<geompp::Point3D>::facet_indices,
                      "Every facet (index into the input) touching this edge. For a T-junction, "
                      "facet_indices[0] owns the coarse edge — the one fix_adjacency() splices on_vertex into.")
        .def_readonly("is_non_manifold", &geompp::AdjacencyViolation<geompp::Point3D>::is_non_manifold,
                      "True: a full edge shared by 3+ facets, not fixable. False: a T-junction, fixable.")
        .def_readonly("on_vertex", &geompp::AdjacencyViolation<geompp::Point3D>::on_vertex,
                      "Meaningful only when not is_non_manifold: the foreign vertex lying on the edge.");

    m.def("validate_adjacency",
          [](const std::vector<geompp::Polygon2D>& facets) { return geompp::validate_adjacency(facets); },
          "facets"_a, "Checks 2D polygon facets for \"every edge has at most 1 neighbor\". "
          "Returns list[AdjacencyViolation2D], empty if conforming.");
    m.def("validate_adjacency",
          [](const std::vector<geompp::Triangle2D>& facets) { return geompp::validate_adjacency(facets); },
          "facets"_a, "Same as the Polygon2D overload, for a list of Triangle2D facets.");
    m.def("validate_adjacency",
          [](const std::vector<std::vector<geompp::Point2D>>& facet_rings) { return geompp::validate_adjacency(facet_rings); },
          "facet_rings"_a, "Same as the Polygon2D overload, for raw point rings (e.g. fix_adjacency()'s own output).");
    m.def("validate_adjacency",
          [](const std::vector<geompp::Polygon3D>& facets) { return geompp::validate_adjacency(facets); },
          "facets"_a, "Checks 3D polygon facets for \"every edge has at most 1 neighbor\". "
          "Returns list[AdjacencyViolation3D], empty if conforming.");
    m.def("validate_adjacency",
          [](const std::vector<geompp::Triangle3D>& facets) { return geompp::validate_adjacency(facets); },
          "facets"_a, "Same as the Polygon3D overload, for a list of Triangle3D facets.");
    m.def("validate_adjacency",
          [](const std::vector<std::vector<geompp::Point3D>>& facet_rings) { return geompp::validate_adjacency(facet_rings); },
          "facet_rings"_a, "Same as the Polygon3D overload, for raw point rings (e.g. fix_adjacency()'s own output).");

    m.def("fix_adjacency",
          [](const std::vector<geompp::Polygon2D>& facets) { return geompp::fix_adjacency(facets); },
          "facets"_a,
          "Repairs every T-junction validate_adjacency() would report: splices the foreign vertex into "
          "the coarse edge, then cuts a diagonal to the nearest ring vertex that forms a valid, "
          "non-crossing diagonal, splitting the facet into pieces (a facet with several T-junctions on "
          "one edge may split into several pieces). Raises ValueError on a non-manifold edge (not "
          "fixable). Returns raw point rings (list[list[Point2D]]), NOT Polygon2D — the split pieces "
          "have no guarantee of matching a valid Polygon2D winding/hole structure.");
    m.def("fix_adjacency",
          [](const std::vector<geompp::Polygon3D>& facets) { return geompp::fix_adjacency(facets); },
          "facets"_a, "Same as the Polygon2D overload, for Polygon3D facets -> list[list[Point3D]].");
    m.def("fix_adjacency",
          [](const std::vector<geompp::Triangle2D>& facets) { return geompp::fix_adjacency(facets); },
          "facets"_a,
          "Triangle2D overload: unlike a Polygon2D facet, a triangle can't just absorb a spliced-in "
          "vertex and stay a triangle -- a repaired facet is re-triangulated into 2+ triangles covering "
          "the same area as the original one. Raises ValueError on a non-manifold edge. "
          "Returns list[Triangle2D] (may be longer than the input).");
    m.def("fix_adjacency",
          [](const std::vector<geompp::Triangle3D>& facets) { return geompp::fix_adjacency(facets); },
          "facets"_a, "Same as the Triangle2D overload, for Triangle3D facets -> list[Triangle3D].");

    m.def("triangulate",
          [](const std::vector<geompp::Polygon2D>& polygons, geompp::AdjacencyConformity conformity,
             const geompp::TriangulationParams& settings) {
              return geompp::triangulate(polygons, conformity, settings);
          },
          "polygons"_a, "conformity"_a = geompp::AdjacencyConformity::Enforce, "settings"_a = geompp::TriangulationParams{},
          "Batch-triangulates a set of 2D polygon facets together (the free-function equivalent of "
          "PolyMesh2D.from_polygons(polygons).triangulate()). Unlike PolyMesh2D.from_polygons (which "
          "always raises on bad adjacency), conformity defaults to Enforce: auto-repairs a T-junction, "
          "still raises on a non-manifold edge. Returns list[Triangle2D].");
}
