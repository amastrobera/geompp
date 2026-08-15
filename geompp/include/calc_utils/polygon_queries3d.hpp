#pragma once

#include "../point3d.hpp"
#include "polygon_queries2d.hpp"

namespace geompp {

inline namespace geometry {

class Polygon3D;
class Line3D;
class LineSegment3D;

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

}  // namespace geometry

}  // namespace geompp
