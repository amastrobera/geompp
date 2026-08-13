#pragma once

#include "../maths.hpp"
// Explicit "../" -- see transformations2d.hpp's own comment on this include block for why a bare
// quoted include here would risk resolving against geompp_csharp/src/'s same-named managed wrappers.
#include "../connected_mesh3d.hpp"
#include "../geometry_collection3d.hpp"
#include "../line3d.hpp"
#include "../line_segment3d.hpp"
#include "../mesh3d.hpp"
#include "../point3d.hpp"
#include "../polygon3d.hpp"
#include "../polyline3d.hpp"
#include "../polymesh3d.hpp"
#include "../ray3d.hpp"
#include "../triangle3d.hpp"
#include "../vector3d.hpp"

/// @file transformations3d.hpp
/// @brief 3D counterpart of transformations2d.hpp -- see its file docs for the two-families design
/// (fast direct-arithmetic translate()/rotate()/scale()/shear()/reflect() on a single Point3D/Vector3D,
/// plus the same-named convenience wrappers for every other 3D primitive that build a Matrix4 and call
/// transform(), vs. the general transform(primitive, Matrix4) covering every 3D primitive). `rotate()`
/// here additionally takes an axis (2D has none -- rotation is a scalar angle around the implicit Z);
/// `shear()` here takes 6 terms rather than 2 (each of X/Y/Z sheared by the other two). Point3D::rotate()
/// and Vector3D::rotate() are the one exception to the "single point/vector = direct arithmetic" rule:
/// axis-angle rotation (Rodrigues' formula) isn't cheap enough to hand-expand, so both build a Matrix4
/// internally and go through transform() same as every composite type.
namespace geompp::transformations {

// -- General Matrix4-based transform() -- every 3D primitive --

/// @brief Applies a 4x4 homogeneous matrix to a point: (x, y, z, 1) -> m * (x, y, z, 1), then drops
/// back to Cartesian (x', y', z').
geometry::Point3D transform(geometry::Point3D const& p, maths::Matrix4 const& m);

/// @brief Applies a 4x4 homogeneous matrix to a direction: (x, y, z, 0) -> m * (x, y, z, 0) -- any
/// translation component of `m` has no effect, only rotation/scale/shear do (the correct behavior for a
/// displacement, which has no position to translate).
geometry::Vector3D transform(geometry::Vector3D const& v, maths::Matrix4 const& m);

/// @brief Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect
/// it) and rebuilds via Ray3D::Make().
geometry::Ray3D transform(geometry::Ray3D const& ray, maths::Matrix4 const& m);

/// @brief Transforms the origin (as a point) and direction (as a vector) and rebuilds via Line3D::Make().
geometry::Line3D transform(geometry::Line3D const& line, maths::Matrix4 const& m);

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

/// @brief Transforms every facet (via Faces(), each rebuilt as a Triangle3D) and rebuilds via
/// ConnectedMesh3D::FromTriangles() -- adjacency is recomputed from scratch, same as building a fresh
/// ConnectedMesh3D from any other triangle set.
geometry::ConnectedMesh3D transform(geometry::ConnectedMesh3D const& mesh, maths::Matrix4 const& m);

/// @brief Transforms every contained geometry (recursively, for nested GeometryCollection3D) and
/// rebuilds a fresh collection via Add().
geometry::GeometryCollection3D transform(geometry::GeometryCollection3D const& collection, maths::Matrix4 const& m);

// -- Fast direct-arithmetic Point3D/Vector3D transforms (no matrix, except rotate() -- see file docs) --

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

/// @brief Shears a point: each axis is offset by a multiple of the other two (`xy` shears X by Y,
/// `zy` shears Z by Y, etc.). Direct arithmetic -- no matrix built.
geometry::Point3D shear(geometry::Point3D const& p, double xy, double xz, double yx, double yz, double zx, double zy);

/// @brief Reflects a point across the plane through the origin whose normal is `normal` (Householder
/// reflection). Direct arithmetic -- no matrix built.
/// @throws std::invalid_argument if `normal` is zero-length.
geometry::Point3D reflect(geometry::Point3D const& p, maths::Vector3 const& normal);

/// @brief Rotates a vector (direction) about `axis` by `angle_rad` radians (Rodrigues' formula). Builds
/// a Matrix4::Rotation() internally, same as Point3D::rotate() -- see its docs and the file docs for why
/// this is the one exception to the direct-arithmetic fast path.
/// @throws std::invalid_argument if `axis` is zero-length.
geometry::Vector3D rotate(geometry::Vector3D const& v, double angle_rad, maths::Vector3 const& axis);

/// @brief Scales a vector's components by a uniform factor. Direct arithmetic -- no matrix built.
geometry::Vector3D scale(geometry::Vector3D const& v, double factor);

/// @brief Scales a vector's components independently per axis.
geometry::Vector3D scale(geometry::Vector3D const& v, double sx, double sy, double sz);

/// @brief Shears a vector: each axis is offset by a multiple of the other two. Direct arithmetic -- no
/// matrix built.
geometry::Vector3D shear(geometry::Vector3D const& v, double xy, double xz, double yx, double yz, double zx, double zy);

/// @brief Reflects a vector across the plane through the origin whose normal is `normal`. Direct
/// arithmetic -- no matrix built.
/// @throws std::invalid_argument if `normal` is zero-length.
geometry::Vector3D reflect(geometry::Vector3D const& v, maths::Vector3 const& normal);

// -- translate()/rotate()/scale()/shear()/reflect() convenience wrappers -- every 3D primitive besides
// Point3D/Vector3D. Each builds the corresponding Matrix4 and calls transform() above; see that
// function's docs for how each type is rebuilt. --

geometry::Ray3D translate(geometry::Ray3D const& ray, maths::Vector3 const& offset);
geometry::Ray3D rotate(geometry::Ray3D const& ray, double angle_rad, maths::Vector3 const& axis);
geometry::Ray3D scale(geometry::Ray3D const& ray, double factor);
geometry::Ray3D scale(geometry::Ray3D const& ray, double sx, double sy, double sz);
geometry::Ray3D shear(geometry::Ray3D const& ray, double xy, double xz, double yx, double yz, double zx, double zy);
geometry::Ray3D reflect(geometry::Ray3D const& ray, maths::Vector3 const& normal);

geometry::Line3D translate(geometry::Line3D const& line, maths::Vector3 const& offset);
geometry::Line3D rotate(geometry::Line3D const& line, double angle_rad, maths::Vector3 const& axis);
geometry::Line3D scale(geometry::Line3D const& line, double factor);
geometry::Line3D scale(geometry::Line3D const& line, double sx, double sy, double sz);
geometry::Line3D shear(geometry::Line3D const& line, double xy, double xz, double yx, double yz, double zx, double zy);
geometry::Line3D reflect(geometry::Line3D const& line, maths::Vector3 const& normal);

geometry::LineSegment3D translate(geometry::LineSegment3D const& seg, maths::Vector3 const& offset);
geometry::LineSegment3D rotate(geometry::LineSegment3D const& seg, double angle_rad, maths::Vector3 const& axis);
geometry::LineSegment3D scale(geometry::LineSegment3D const& seg, double factor);
geometry::LineSegment3D scale(geometry::LineSegment3D const& seg, double sx, double sy, double sz);
geometry::LineSegment3D shear(geometry::LineSegment3D const& seg, double xy, double xz, double yx, double yz, double zx,
                              double zy);
geometry::LineSegment3D reflect(geometry::LineSegment3D const& seg, maths::Vector3 const& normal);

geometry::GeometryCollection3D translate(geometry::GeometryCollection3D const& collection,
                                         maths::Vector3 const& offset);
geometry::GeometryCollection3D rotate(geometry::GeometryCollection3D const& collection, double angle_rad,
                                      maths::Vector3 const& axis);
geometry::GeometryCollection3D scale(geometry::GeometryCollection3D const& collection, double factor);
geometry::GeometryCollection3D scale(geometry::GeometryCollection3D const& collection, double sx, double sy, double sz);
geometry::GeometryCollection3D shear(geometry::GeometryCollection3D const& collection, double xy, double xz, double yx,
                                     double yz, double zx, double zy);
geometry::GeometryCollection3D reflect(geometry::GeometryCollection3D const& collection, maths::Vector3 const& normal);

geometry::Polygon3D translate(geometry::Polygon3D const& poly, maths::Vector3 const& offset);
geometry::Polygon3D rotate(geometry::Polygon3D const& poly, double angle_rad, maths::Vector3 const& axis);
geometry::Polygon3D scale(geometry::Polygon3D const& poly, double factor);
geometry::Polygon3D scale(geometry::Polygon3D const& poly, double sx, double sy, double sz);
geometry::Polygon3D shear(geometry::Polygon3D const& poly, double xy, double xz, double yx, double yz, double zx,
                          double zy);
geometry::Polygon3D reflect(geometry::Polygon3D const& poly, maths::Vector3 const& normal);

geometry::Polyline3D translate(geometry::Polyline3D const& polyline, maths::Vector3 const& offset);
geometry::Polyline3D rotate(geometry::Polyline3D const& polyline, double angle_rad, maths::Vector3 const& axis);
geometry::Polyline3D scale(geometry::Polyline3D const& polyline, double factor);
geometry::Polyline3D scale(geometry::Polyline3D const& polyline, double sx, double sy, double sz);
geometry::Polyline3D shear(geometry::Polyline3D const& polyline, double xy, double xz, double yx, double yz, double zx,
                           double zy);
geometry::Polyline3D reflect(geometry::Polyline3D const& polyline, maths::Vector3 const& normal);

geometry::Triangle3D translate(geometry::Triangle3D const& tri, maths::Vector3 const& offset);
geometry::Triangle3D rotate(geometry::Triangle3D const& tri, double angle_rad, maths::Vector3 const& axis);
geometry::Triangle3D scale(geometry::Triangle3D const& tri, double factor);
geometry::Triangle3D scale(geometry::Triangle3D const& tri, double sx, double sy, double sz);
geometry::Triangle3D shear(geometry::Triangle3D const& tri, double xy, double xz, double yx, double yz, double zx,
                           double zy);
geometry::Triangle3D reflect(geometry::Triangle3D const& tri, maths::Vector3 const& normal);

geometry::Mesh3D translate(geometry::Mesh3D const& mesh, maths::Vector3 const& offset);
geometry::Mesh3D rotate(geometry::Mesh3D const& mesh, double angle_rad, maths::Vector3 const& axis);
geometry::Mesh3D scale(geometry::Mesh3D const& mesh, double factor);
geometry::Mesh3D scale(geometry::Mesh3D const& mesh, double sx, double sy, double sz);
geometry::Mesh3D shear(geometry::Mesh3D const& mesh, double xy, double xz, double yx, double yz, double zx, double zy);
geometry::Mesh3D reflect(geometry::Mesh3D const& mesh, maths::Vector3 const& normal);

geometry::PolyMesh3D translate(geometry::PolyMesh3D const& mesh, maths::Vector3 const& offset);
geometry::PolyMesh3D rotate(geometry::PolyMesh3D const& mesh, double angle_rad, maths::Vector3 const& axis);
geometry::PolyMesh3D scale(geometry::PolyMesh3D const& mesh, double factor);
geometry::PolyMesh3D scale(geometry::PolyMesh3D const& mesh, double sx, double sy, double sz);
geometry::PolyMesh3D shear(geometry::PolyMesh3D const& mesh, double xy, double xz, double yx, double yz, double zx,
                           double zy);
geometry::PolyMesh3D reflect(geometry::PolyMesh3D const& mesh, maths::Vector3 const& normal);

geometry::ConnectedMesh3D translate(geometry::ConnectedMesh3D const& mesh, maths::Vector3 const& offset);
geometry::ConnectedMesh3D rotate(geometry::ConnectedMesh3D const& mesh, double angle_rad, maths::Vector3 const& axis);
geometry::ConnectedMesh3D scale(geometry::ConnectedMesh3D const& mesh, double factor);
geometry::ConnectedMesh3D scale(geometry::ConnectedMesh3D const& mesh, double sx, double sy, double sz);
geometry::ConnectedMesh3D shear(geometry::ConnectedMesh3D const& mesh, double xy, double xz, double yx, double yz,
                                double zx, double zy);
geometry::ConnectedMesh3D reflect(geometry::ConnectedMesh3D const& mesh, maths::Vector3 const& normal);

}  // namespace geompp::transformations
