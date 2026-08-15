#pragma once

#include "../point3d.hpp"

#include <optional>
#include <vector>

namespace geompp {

inline namespace geometry {

class LineSegment3D;

namespace detail {

void distance_line_to_line(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0, Point3D const& L2_P1,
                           double& sc, double& tc);

std::optional<Point3D> line_intersection(Point3D const& L1_P0, Point3D const& L1_P1, Point3D const& L2_P0,
                                         Point3D const& L2_P1, double& sc, double& tc);

}  // namespace detail

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

}  // namespace geometry

}  // namespace geompp
