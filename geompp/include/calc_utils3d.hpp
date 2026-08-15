#pragma once

/// @file calc_utils3d.hpp
/// @brief Umbrella header for geompp::geometry's 3D calculation utilities -- self-intersection detection,
/// convex hull + PCA (principal_axes/normal/direction), polygon point/distance/tangent queries, and
/// triangulation. See each calc_utils/*3d.hpp for the topic split. Polygon boolean ops and polyline
/// decimation/smoothing are declared directly in calc_utils/polygon_ops2d.hpp / calc_utils/polyline_ops2d.hpp
/// (they're generic over Point2D/Point3D via PointContainer, so there is no separate *3d.hpp for them).

#include "calc_utils/convex_hull3d.hpp"
#include "calc_utils/polygon_queries3d.hpp"
#include "calc_utils/self_intersections3d.hpp"
#include "calc_utils/triangulation3d.hpp"

#include "calc_utils2d.hpp"  // ExtremePoints<> / PolygonTangents<> / decimation / bezier / triangulate(Point2D...)
