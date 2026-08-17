#pragma once

#include "../constants.hpp"
#include "../plane.hpp"
#include "../point3d.hpp"
#include "../triangle3d.hpp"
#include "polygonization2d.hpp"

#include <vector>

namespace geompp {

inline namespace geometry {

class Polygon3D;

/// @brief 3D counterpart of the Triangle2D overload (calc_utils/polygonization2d.hpp) -- see its own docs
/// for the shared machinery (welding, adjacency, detail::polygonize_impl<detail::MeshFaceView3D>, already
/// extern-template-instantiated alongside the 2D version in polygonization2d.cpp/.hpp).
/// @param triangles The triangles to polygonize. Order is not required to reflect adjacency.
/// @param params Which polygonization strategy to run -- see PolygonizationParams::Strategy.
/// @returns One Polygon3D per output piece.
/// @throws std::invalid_argument if @p triangles is empty, if any edge is shared by more than 2 triangles,
/// or if @p params names an unknown strategy enumerator.
std::vector<Polygon3D> polygonize(std::vector<Triangle3D> const& triangles,
                                  PolygonizationParams const& params = PolygonizationParams{});

/// @brief 3D counterpart of the Polygon2D overload (calc_utils/polygonization2d.hpp) -- see its own docs
/// for the shared "cancel outer edges, merge touching holes via reverse+Union+reverse" machinery. The one
/// genuinely 3D-specific step, absent from the 2D version, is grouping the input by plane first: two-phase,
/// a coarse hash bucket keyed by each polygon's plane Normal (rounded via floor(v/epsilon) into 3 int64
/// components, same idiom GridCell2D/3DHash use for vertex welding), then verified within a normal-bucket
/// against each existing sub-group's representative via Plane::AlmostEquals() -- correctly keeps two
/// same-normal-but-different-offset planes (e.g. two parallel floors) in separate groups. Cancellation and
/// hole-touch-detection then run per plane group, each projected through that group's own View2D::OnPlane,
/// with results unprojected back to Point3D via View2D::xyz() before assembly -- same round-trip
/// Polygon3D::Union()/Simplify() already use.
/// @param polygons The polygons to merge. Order doesn't matter.
/// @returns One Polygon3D per disjoint merged region.
/// @throws std::invalid_argument if a group of touching holes doesn't merge into a single Polygon3D::Union
/// result.
std::vector<Polygon3D> merge(std::vector<Polygon3D> const& polygons);

}  // namespace geometry

}  // namespace geompp
