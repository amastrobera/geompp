#pragma once

#include "calc_utils2d.hpp"  // ExtremePoints<> and detail::extreme_points (shared with 2D)
#include "constants.hpp"
#include "coordinate_frame.hpp"
#include "point3d.hpp"

#include <optional>

namespace geompp {

inline namespace geometry {

class Polygon3D;
class Line3D;
class LineSegment3D;
class Triangle3D;

namespace detail {

void distance_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0, Point3D const& L2_P1,
                           double& sc, double& tc);

std::optional<Point3D> line_intersection(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0,
                                         Point3D const& L2_P1, double& sc, double& tc);

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull. It assumes the points to be all co-planar, and
/// uses the dominant normal axis to project points in 2D
/// @param points cloud of points (ideally all coplanar)
/// @param normal the normal of the plane (ideally the plane on which the points lie)
/// @returns list of indices of the points (from the original vector) that form a convex hull
/// @throws algorithm based throw logic
std::vector<std::size_t> convex_hull_indices(std::vector<Point3D> const& points, Vector3D normal);

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull. It does NOT NEED points to be coplanar. With the
/// help of PCA, the primary and secondary axis are computed, as well as the mean point, and the plane is calculated.
/// After that, the algorithm uses the dominant normal axis to project points in 2D
/// @param points cloud of points
/// @returns list of indices of the points (from the original vector) that form a convex hull
/// @throws algorithm based throw logic
std::vector<std::size_t> convex_hull_indices(std::vector<Point3D> const& points);

}  // namespace detail

/// @brief Computes the three principal axes of a point cloud using PCA (Jacobi eigendecomposition).
/// @param points The point cloud. Must contain at least 3 non-collinear points.
/// @return A CoordinateFrame whose X is the direction of most spread, Y the second, and Z the best-fit plane normal.
CoordinateFrame principal_axes(std::vector<Point3D> const& points);

/// @brief Computes the best-fit plane normal for a cloud of points using PCA.
/// @param points The point cloud. Must contain at least 3 non-collinear points.
Vector3D principal_normal(std::vector<Point3D> const& points);

/// @brief Computes the dominant direction of a point cloud using PCA.
/// @param points The point cloud. Must contain at least 3 non-collinear points.
Vector3D principal_direction(std::vector<Point3D> const& points);

/// @brief tells if a polygon made of 1 outer loop and m-inner loops (holes) is convex. When holes are present the
/// polygon is automatically non-convex.
/// It assumes all points are on the same plane. No matter the inputs this will be
/// forced by (implicit) projection on the dominant axis (2D projection preserves convexity property)
/// @param vertices outer polygon loop
/// @param holes inner loops
/// @param normal polygon normal (plane onto which the math will be done)
bool is_convex(std::vector<Point3D> const& vertices, std::vector<std::vector<Point3D>> const& holes,
               Vector3D const& normal);

/// @brief Finds the two vertices of a polygon that are extreme (least / greatest projection) along a line.
/// Uses Daniel Sunday's O(log n) binary search when the polygon is convex, else an O(n) linear scan.
/// @param polygon The polygon whose vertices are searched (holes are ignored — only the outer ring matters).
/// @param line    The line whose Direction() defines the axis of projection.
/// @returns ExtremePoints{min_point, max_point} — the outer-ring vertices with least / greatest projection.
ExtremePoints<Point3D> find_extreme_points(Polygon3D const& polygon, Line3D const& line);

/// @brief computes the distance between a polygon and a line (the distance is zero if they intersect)
double distance_to(Polygon3D const& polygon, Line3D const& line);

/// @brief Finds the left and right tangent segments from a point to a polygon, both projected onto the
/// polygon's own plane. A tangent (a line touching a shape without crossing it) is inherently a planar
/// concept, so unlike distance_to this has no "purely 3D / skew" fallback: p must lie in the polygon's plane.
/// @pre p must lie in the polygon's plane (Polygon3D::GetPlane().Contains(p)).
/// @pre p must be strictly outside the polygon and not equal to any of its vertices.
/// @throws std::logic_error if p is not coplanar with the polygon.
PolygonTangents<LineSegment3D> tangents_to(Polygon3D const& polygon, Point3D const& p);

/// @brief Finds the common outer tangent segments between two polygons, both projected onto their shared
/// plane. Like the point overload, this requires both polygons to lie in the same plane — two polygons in
/// general (skew) 3D position don't have a single well-defined common tangent line.
/// @pre polygon and other must be coplanar (polygon.GetPlane() == other.GetPlane()).
/// @throws std::logic_error if the two polygons are not coplanar.
PolygonTangents<LineSegment3D> tangents_to(Polygon3D const& polygon, Polygon3D const& other);

/// @brief Builds the closed ring of edges connecting consecutive points, including the edge from the last
/// point back to the first — the free-function equivalent of Polygon3D::ToSegments(), for callers who have
/// a raw point loop rather than a Polygon3D instance. Native 3D, no projection: unlike is_simple() below,
/// building the segments themselves needs no plane/normal.
/// @param points ring vertices, in order (must NOT repeat the first point at the end — the closing edge is
/// added automatically).
std::vector<LineSegment3D> to_segments(std::vector<Point3D> const& points);

/// @brief Whether a closed ring of (ideally coplanar) points has no self-intersections, projected through
/// the dominant axis of @p normal — the free-function equivalent of Polygon3D::IsSimple(), for a single
/// loop with no holes and an explicit plane normal (skips the PCA fit an unknown normal would cost).
/// @param points ring vertices, in order (implicitly closed), ideally coplanar.
/// @param normal the plane normal to project along (only its dominant axis is used).
bool is_simple(std::vector<Point3D> const& points, Vector3D const& normal);

/// @brief Same as the (points, normal) overload, but fits the plane normal via PCA (principal_normal) when
/// the caller doesn't already have one on hand — mirrors triangulate()'s two-overload pattern.
/// @param points ring vertices, in order (implicitly closed). Must contain at least 3 non-collinear points.
bool is_simple(std::vector<Point3D> const& points);

/// @brief Breaks down a simple, planar polygon into triangles. @p input is assumed flat/coplanar — every
/// vertex is projected through the dominant-axis view of @p normal before triangulating, so a non-planar
/// input silently triangulates its projection, not its true 3D shape.
/// @param input polygon's outer loop of points (assumed CCW against the normal) and no holes allowed
/// @param normal the normal vector of the polygon's plane
/// @param settings options for functions inner workings
///                 (1) triangulation strategy options: user decides what algorithm to run
///                     - EarClipping clips the first valid ear it finds in scan order. Most robust and
///                                   general-purpose, and often close to O(n) in practice, but O(n²) worst-case -- and
///                                   doesn't optimize triangle shape, so it can produce a visually thin sliver purely
///                                   from scan order, even on ordinary input.
///                     - EarClippingBestFit clips the best-scoring (least sliver-prone) valid ear every step
///                                   instead of the first one. Same termination guarantee as EarClipping, but
///                                   unconditionally ~O(n²) -- a full rescan of the current ring on every single clip,
///                                   not just worst-case.
///                                   [Default: prefers shape quality over raw speed.]
///                     - MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition
///                                        into monotone pieces)
///                     - Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle
///                                 of all the angles of the triangles in the triangulation (avoiding skinny triangles)
///                 (2) simplicity: the input for the algo should be a simple polygon (no self-intersections)
///                     - Guaranteed: the input is assumed to be a good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's input will be simplified in O(n log n) time
///                 (3) winding: the input points should be in counter clockwise order (CCW)
///                     - Guaranteed: the input is assumed to be good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's input will be reversed in O(n) time
///                 (4) collinearity: the input points should not contain any collinear (including consecutive
///                      duplicates) points
///                     - Guaranteed: the input is assumed to be good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's bad vertices will be removed in O(n) time
/// @returns one Triangle3D per triangle; input.size() - 2 triangles for a simple polygon.
/// @throws std::invalid_argument if @p input has fewer than 3 points, or (Assert mode) if a precondition
/// named by @p settings is violated.
std::vector<Triangle3D> triangulate(std::vector<Point3D> const& input, Vector3D normal,
                                    TriangulationParams const& settings = TriangulationParams{});

/// @brief Same as the (input, normal, settings) overload, but fits the plane normal via PCA
/// (principal_normal) when the caller doesn't already have one on hand — mirrors is_simple()'s and
/// convex_hull_indices()'s two-overload pattern. @p input is assumed flat/coplanar — see that overload.
/// @param input polygon's outer loop of points (assumed CCW against their own normal) and no holes allowed
/// @param settings options for functions inner workings
///                 (1) triangulation strategy options: user decides what algorithm to run
///                     - EarClipping clips the first valid ear it finds in scan order. Most robust and
///                                   general-purpose, and often close to O(n) in practice, but O(n²) worst-case -- and
///                                   doesn't optimize triangle shape, so it can produce a visually thin sliver purely
///                                   from scan order, even on ordinary input.
///                     - EarClippingBestFit clips the best-scoring (least sliver-prone) valid ear every step
///                                   instead of the first one. Same termination guarantee as EarClipping, but
///                                   unconditionally ~O(n²) -- a full rescan of the current ring on every single clip,
///                                   not just worst-case.
///                                   [Default: prefers shape quality over raw speed.]
///                     - MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition
///                                        into monotone pieces)
///                     - Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle
///                                 of all the angles of the triangles in the triangulation (avoiding skinny triangles)
///                 (2) simplicity: the input for the algo should be a simple polygon (no self-intersections)
///                     - Guaranteed: the input is assumed to be a good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's input will be simplified in O(n log n) time
///                 (3) winding: the input points should be in counter clockwise order (CCW)
///                     - Guaranteed: the input is assumed to be good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's input will be reversed in O(n) time
///                 (4) collinearity: the input points should not contain any collinear (including consecutive
///                      duplicates) points
///                     - Guaranteed: the input is assumed to be good at the users's own risk
///                     - Assert: will throw if the user's input is not good
///                     - Enforce: will check, and if not good, the user's bad vertices will be removed in O(n) time
/// @returns one Triangle3D per triangle; input.size() - 2 triangles for a simple polygon.
/// @throws std::invalid_argument if @p input has fewer than 3 points, or (Assert mode) if a precondition
/// named by @p settings is violated.
std::vector<Triangle3D> triangulate(std::vector<Point3D> const& input,
                                    TriangulationParams const& settings = TriangulationParams{});

}  // namespace geometry

}  // namespace geompp
