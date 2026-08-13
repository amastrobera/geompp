#pragma once

#include "../maths.hpp"
// Explicit "../" -- see transformations2d.hpp's own comment on this include block for why a bare
// quoted include here would risk resolving against geompp_csharp/src/'s same-named managed wrappers.
#include "../line_segment3d.hpp"
#include "../mesh3d.hpp"
#include "../point3d.hpp"
#include "../polygon3d.hpp"
#include "../polyline3d.hpp"
#include "../polymesh3d.hpp"
#include "../triangle3d.hpp"
#include "../vector3d.hpp"

/// @file transformations3d.hpp
/// @brief 3D counterpart of transformations2d.hpp -- see its file docs for the two-families design
/// (fast direct-arithmetic translate()/rotate()/scale() on a single Point3D vs. the general
/// transform(primitive, Matrix4) covering every 3D primitive, Point3D through PolyMesh3D). `rotate()`
/// here additionally takes an axis (2D has none -- rotation is a scalar angle around the implicit Z).
namespace geompp::transformations {

// -- Fast direct-arithmetic Point3D transforms (no matrix) --

/// @brief Translates a point by a plain (x, y, z) offset. Direct arithmetic -- no matrix built.
geometry::Point3D translate(geometry::Point3D const& p, maths::Vector3 const& offset);

/// @brief Rotates a point about the origin, around `axis`, by `angle_rad` radians (Rodrigues' formula,
/// right-hand rule). Builds a Matrix4::Rotation() internally (unlike the 2D overload, a 3D rotation
/// isn't cheap enough to hand-expand point-by-point without one) and applies it via transform().
/// @throws std::invalid_argument if `axis` is zero-length.
geometry::Point3D rotate(geometry::Point3D const& p, double angle_rad, maths::Vector3 const& axis);

/// @brief Scales a point's coordinates about the origin by a uniform factor. Direct arithmetic -- no
/// matrix built.
geometry::Point3D scale(geometry::Point3D const& p, double factor);

/// @brief Scales a point's coordinates about the origin independently per axis.
geometry::Point3D scale(geometry::Point3D const& p, double sx, double sy, double sz);

// -- General Matrix4-based transform() -- every 3D primitive, Point3D through PolyMesh3D --

/// @brief Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops
/// back to Cartesian (x', y', z').
geometry::Point3D transform(geometry::Point3D const& p, maths::Matrix4 const& m);

/// @brief Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) -- any
/// translation component of `m` has no effect, only rotation/scale/shear do (the correct behavior for a
/// displacement, which has no position to translate).
geometry::Vector3D transform(geometry::Vector3D const& v, maths::Matrix4 const& m);

geometry::LineSegment3D transform(geometry::LineSegment3D const& seg, maths::Matrix4 const& m);

geometry::Polyline3D transform(geometry::Polyline3D const& polyline, maths::Matrix4 const& m);

geometry::Triangle3D transform(geometry::Triangle3D const& tri, maths::Matrix4 const& m);

/// @brief Transforms every vertex of both the outer ring and every hole ring. See the 2D overload's
/// docs for the winding-flip note under a reflection matrix.
geometry::Polygon3D transform(geometry::Polygon3D const& poly, maths::Matrix4 const& m);

/// @brief Transforms every facet independently and rebuilds via Mesh3D::FromTriangles().
geometry::Mesh3D transform(geometry::Mesh3D const& mesh, maths::Matrix4 const& m);

/// @brief Transforms every facet independently and rebuilds via PolyMesh3D::FromPolygons().
geometry::PolyMesh3D transform(geometry::PolyMesh3D const& mesh, maths::Matrix4 const& m);

}  // namespace geompp::transformations
