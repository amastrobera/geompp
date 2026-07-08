#pragma once

#include "calc_utils2d.hpp"  // ExtremePoints<> and detail::extreme_points (shared with 2D)
#include "constants.hpp"
#include "coordinate_frame.hpp"
#include "point3d.hpp"

#include <optional>

namespace geompp {

class Polygon3D;
class Line3D;
class LineSegment3D;

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

}  // namespace geompp
