#pragma once

#include "constants.hpp"
#include "vector3d.hpp"
#include "point3d.hpp"

#include <optional>

namespace geompp {

void distance_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0, Point3D const& L2_P1,
                           double& sc, double& tc);

std::optional<Point3D> intersection_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0,
                                                 Point3D const& L2_P1, double& sc, double& tc);

/// @brief the Andrew's Monotone Chain algorithm to make a convex hull. It assumes the points to be all co-planar, and
/// uses the dominant normal axis to project points in 2D
/// @param points cloud of points
/// @param normal the normal of the points (calculated if not given)
/// @returns list of indices of the points (from the original vector) that form a convex hull
/// @throws normal not given and points are not coplanar, or algorithm based throw logic
std::vector<std::size_t> convex_hull_indices(std::vector<Point3D> const& points,
                                             std::optional<Vector3D> normal = std::nullopt);

}  // namespace geompp
