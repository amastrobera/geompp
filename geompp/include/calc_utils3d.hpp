#pragma once

#include "constants.hpp"
#include "coordinate_frame.hpp"
#include "point3d.hpp"

#include <optional>

namespace geompp {

void distance_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0, Point3D const& L2_P1,
                           double& sc, double& tc);

std::optional<Point3D> intersection_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0,
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

}  // namespace geompp
