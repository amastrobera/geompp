#pragma once

#include "../constants.hpp"
#include "../point3d.hpp"

#include <vector>

namespace geompp {

inline namespace geometry {

class Triangle3D;

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
