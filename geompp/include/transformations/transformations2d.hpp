#pragma once

#include "../maths.hpp"
// Explicit "../" (rather than bare "point2d.hpp") is deliberate, not stylistic: a bare quoted include
// here would also resolve against the directories of whichever *other* currently-open headers happen to
// be mid-inclusion (MSVC quoted-include search rule 2), which bites hard when geompp_csharp/src/ (with
// its own same-named managed Mesh2D.hpp/Polygon2D.hpp/...) is one of them -- silently pulling in the
// managed C++/CLI wrapper instead of this native header, and failing with "managed type cannot be
// compiled as an unmanaged function" deep in an unrelated TU. The explicit "../" resolves unambiguously
// against this file's own directory first, every time.
#include "../connected_mesh2d.hpp"
#include "../geometry_collection2d.hpp"
#include "../line2d.hpp"
#include "../line_segment2d.hpp"
#include "../mesh2d.hpp"
#include "../point2d.hpp"
#include "../polygon2d.hpp"
#include "../polyline2d.hpp"
#include "../polymesh2d.hpp"
#include "../ray2d.hpp"
#include "../triangle2d.hpp"
#include "../vector2d.hpp"

/// @file transformations2d.hpp
/// @brief geompp::transformations -- affine transforms for geompp::geometry's 2D primitives. Two
/// distinct families:
///   - translate()/rotate()/scale()/shear()/reflect(): a same-named overload for every 2D primitive.
///     For Point2D and Vector2D these are fast direct arithmetic, no matrix built or multiplied -- the
///     cheapest possible path for a single point/vector, the same reasoning Point3D::rotate() explains
///     for why 3D rotation is the one exception (see transformations3d.hpp). For every other primitive
///     they are thin convenience wrappers that build the corresponding Matrix3 (Matrix3::Translation()/
///     Rotation()/Scale()/Shear()/Reflection()) and call transform() with it -- building the matrix once
///     and reusing it per-vertex beats N direct-arithmetic calls once N > 1, since direct arithmetic
///     would otherwise recompute cos()/sin() on every point instead of once -- see transform()'s own
///     docs for how each composite type is rebuilt.
///   - transform(primitive, Matrix3): every 2D primitive, applying a caller-supplied 3x3 homogeneous
///     matrix (built by hand, or via the Matrix3 factories above, or via TransformBuilder2D). This is the
///     general path: composite primitives are rebuilt by transforming each constituent point and
///     re-validating through the type's own Make()/FromTriangles()/FromPolygons(), so a single Matrix3
///     (e.g. a rotation + translation combined) applies to an entire Polygon2D/Mesh2D/PolyMesh2D in one
///     call.
/// Vector2D transforms rotation/scale/shear/reflection but never translation (homogeneous w = 0) -- a
/// displacement has no position to translate. Same reasoning excludes translate() for Vector2D below.
namespace geompp::transformations {

// -- Fast direct-arithmetic Point2D/Vector2D transforms (no matrix) --

/// @brief Translates a point by a plain (x, y) offset. Direct arithmetic -- no matrix built.
geometry::Point2D translate(geometry::Point2D const& p, maths::Vector2 const& offset);

/// @brief Rotates a point about the origin by `angle_rad` radians (CCW, right-hand rule in the XY
/// plane). Direct `cos`/`sin` arithmetic -- no matrix built. To rotate about a different pivot, first
/// translate that pivot to the origin, rotate, then translate back (or use transform() with a composed
/// Matrix3).
geometry::Point2D rotate(geometry::Point2D const& p, double angle_rad);

/// @brief Scales a point's coordinates about the origin by a uniform factor. Direct arithmetic -- no
/// matrix built.
geometry::Point2D scale(geometry::Point2D const& p, double factor);

/// @brief Scales a point's coordinates about the origin independently per axis.
geometry::Point2D scale(geometry::Point2D const& p, double sx, double sy);

/// @brief Shears a point: x' = x + shx*y, y' = y + shy*x. Direct arithmetic -- no matrix built.
geometry::Point2D shear(geometry::Point2D const& p, double shx, double shy);

/// @brief Reflects a point across the line through the origin whose normal is `normal` (Householder
/// reflection). Direct arithmetic -- no matrix built.
/// @throws std::invalid_argument if `normal` is zero-length.
geometry::Point2D reflect(geometry::Point2D const& p, maths::Vector2 const& normal);

/// @brief Rotates a vector (direction) by `angle_rad` radians (CCW). Direct arithmetic -- no matrix
/// built. No translate() overload -- a direction has no position to translate.
geometry::Vector2D rotate(geometry::Vector2D const& v, double angle_rad);

/// @brief Scales a vector's components by a uniform factor. Direct arithmetic -- no matrix built.
geometry::Vector2D scale(geometry::Vector2D const& v, double factor);

/// @brief Scales a vector's components independently per axis.
geometry::Vector2D scale(geometry::Vector2D const& v, double sx, double sy);

/// @brief Shears a vector: x' = x + shx*y, y' = y + shy*x. Direct arithmetic -- no matrix built.
geometry::Vector2D shear(geometry::Vector2D const& v, double shx, double shy);

/// @brief Reflects a vector across the line through the origin whose normal is `normal`. Direct
/// arithmetic -- no matrix built.
/// @throws std::invalid_argument if `normal` is zero-length.
geometry::Vector2D reflect(geometry::Vector2D const& v, maths::Vector2 const& normal);

// -- General Matrix3-based transform() -- every 2D primitive --

/// @brief Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to
/// Cartesian (x', y').
geometry::Point2D transform(geometry::Point2D const& p, maths::Matrix3 const& m);

/// @brief Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) -- the 0 in the
/// homogeneous coordinate means any translation component of `m` has no effect, only rotation/scale/
/// shear do (the correct behavior for a displacement, which has no position to translate).
geometry::Vector2D transform(geometry::Vector2D const& v, maths::Matrix3 const& m);

/// @brief Transforms the origin (as a point) and direction (as a vector, so translation doesn't affect
/// it) and rebuilds via Ray2D::Make().
geometry::Ray2D transform(geometry::Ray2D const& ray, maths::Matrix3 const& m);

/// @brief Transforms the origin (as a point) and direction (as a vector) and rebuilds via Line2D::Make().
geometry::Line2D transform(geometry::Line2D const& line, maths::Matrix3 const& m);

geometry::LineSegment2D transform(geometry::LineSegment2D const& seg, maths::Matrix3 const& m);

geometry::Polyline2D transform(geometry::Polyline2D const& polyline, maths::Matrix3 const& m);

geometry::Triangle2D transform(geometry::Triangle2D const& tri, maths::Matrix3 const& m);

/// @brief Transforms every vertex of both the outer ring and every hole ring. A pure rotation/
/// translation/uniform-scale preserves CCW-outer/CW-hole winding; a reflection (negative-determinant
/// `m`, e.g. `Matrix3::Scale(-1, 1)`) flips it, same as it would for a hand-reversed point list --
/// callers doing a deliberate mirror should expect to re-run IsSimple()/winding checks if they later
/// feed the result back through something that assumes CCW/CW.
geometry::Polygon2D transform(geometry::Polygon2D const& poly, maths::Matrix3 const& m);

/// @brief Transforms every facet independently and rebuilds via Mesh2D::FromTriangles() -- cheaper than
/// it sounds, since FromTriangles() re-welds shared vertices via the same spatial-hash pass any other
/// mesh construction uses.
geometry::Mesh2D transform(geometry::Mesh2D const& mesh, maths::Matrix3 const& m);

/// @brief Transforms every facet independently and rebuilds via PolyMesh2D::FromPolygons().
geometry::PolyMesh2D transform(geometry::PolyMesh2D const& mesh, maths::Matrix3 const& m);

/// @brief Transforms every facet (via Faces(), each rebuilt as a Triangle2D) and rebuilds via
/// ConnectedMesh2D::FromTriangles() -- adjacency is recomputed from scratch, same as building a fresh
/// ConnectedMesh2D from any other triangle set.
geometry::ConnectedMesh2D transform(geometry::ConnectedMesh2D const& mesh, maths::Matrix3 const& m);

/// @brief Transforms every contained geometry (recursively, for nested GeometryCollection2D) and
/// rebuilds a fresh collection via Add().
geometry::GeometryCollection2D transform(geometry::GeometryCollection2D const& collection,
                                         maths::Matrix3 const& m);

// -- translate()/rotate()/scale()/shear()/reflect() convenience wrappers -- every 2D primitive besides
// Point2D/Vector2D (see the fast-path section above for those). Each builds the corresponding Matrix3
// and calls transform() above; see that function's docs for how each type is rebuilt. --

geometry::Ray2D translate(geometry::Ray2D const& ray, maths::Vector2 const& offset);
geometry::Ray2D rotate(geometry::Ray2D const& ray, double angle_rad);
geometry::Ray2D scale(geometry::Ray2D const& ray, double factor);
geometry::Ray2D scale(geometry::Ray2D const& ray, double sx, double sy);
geometry::Ray2D shear(geometry::Ray2D const& ray, double shx, double shy);
geometry::Ray2D reflect(geometry::Ray2D const& ray, maths::Vector2 const& normal);

geometry::Line2D translate(geometry::Line2D const& line, maths::Vector2 const& offset);
geometry::Line2D rotate(geometry::Line2D const& line, double angle_rad);
geometry::Line2D scale(geometry::Line2D const& line, double factor);
geometry::Line2D scale(geometry::Line2D const& line, double sx, double sy);
geometry::Line2D shear(geometry::Line2D const& line, double shx, double shy);
geometry::Line2D reflect(geometry::Line2D const& line, maths::Vector2 const& normal);

geometry::LineSegment2D translate(geometry::LineSegment2D const& seg, maths::Vector2 const& offset);
geometry::LineSegment2D rotate(geometry::LineSegment2D const& seg, double angle_rad);
geometry::LineSegment2D scale(geometry::LineSegment2D const& seg, double factor);
geometry::LineSegment2D scale(geometry::LineSegment2D const& seg, double sx, double sy);
geometry::LineSegment2D shear(geometry::LineSegment2D const& seg, double shx, double shy);
geometry::LineSegment2D reflect(geometry::LineSegment2D const& seg, maths::Vector2 const& normal);

geometry::GeometryCollection2D translate(geometry::GeometryCollection2D const& collection,
                                         maths::Vector2 const& offset);
geometry::GeometryCollection2D rotate(geometry::GeometryCollection2D const& collection, double angle_rad);
geometry::GeometryCollection2D scale(geometry::GeometryCollection2D const& collection, double factor);
geometry::GeometryCollection2D scale(geometry::GeometryCollection2D const& collection, double sx, double sy);
geometry::GeometryCollection2D shear(geometry::GeometryCollection2D const& collection, double shx, double shy);
geometry::GeometryCollection2D reflect(geometry::GeometryCollection2D const& collection,
                                       maths::Vector2 const& normal);

geometry::Polygon2D translate(geometry::Polygon2D const& poly, maths::Vector2 const& offset);
geometry::Polygon2D rotate(geometry::Polygon2D const& poly, double angle_rad);
geometry::Polygon2D scale(geometry::Polygon2D const& poly, double factor);
geometry::Polygon2D scale(geometry::Polygon2D const& poly, double sx, double sy);
geometry::Polygon2D shear(geometry::Polygon2D const& poly, double shx, double shy);
geometry::Polygon2D reflect(geometry::Polygon2D const& poly, maths::Vector2 const& normal);

geometry::Polyline2D translate(geometry::Polyline2D const& polyline, maths::Vector2 const& offset);
geometry::Polyline2D rotate(geometry::Polyline2D const& polyline, double angle_rad);
geometry::Polyline2D scale(geometry::Polyline2D const& polyline, double factor);
geometry::Polyline2D scale(geometry::Polyline2D const& polyline, double sx, double sy);
geometry::Polyline2D shear(geometry::Polyline2D const& polyline, double shx, double shy);
geometry::Polyline2D reflect(geometry::Polyline2D const& polyline, maths::Vector2 const& normal);

geometry::Triangle2D translate(geometry::Triangle2D const& tri, maths::Vector2 const& offset);
geometry::Triangle2D rotate(geometry::Triangle2D const& tri, double angle_rad);
geometry::Triangle2D scale(geometry::Triangle2D const& tri, double factor);
geometry::Triangle2D scale(geometry::Triangle2D const& tri, double sx, double sy);
geometry::Triangle2D shear(geometry::Triangle2D const& tri, double shx, double shy);
geometry::Triangle2D reflect(geometry::Triangle2D const& tri, maths::Vector2 const& normal);

geometry::Mesh2D translate(geometry::Mesh2D const& mesh, maths::Vector2 const& offset);
geometry::Mesh2D rotate(geometry::Mesh2D const& mesh, double angle_rad);
geometry::Mesh2D scale(geometry::Mesh2D const& mesh, double factor);
geometry::Mesh2D scale(geometry::Mesh2D const& mesh, double sx, double sy);
geometry::Mesh2D shear(geometry::Mesh2D const& mesh, double shx, double shy);
geometry::Mesh2D reflect(geometry::Mesh2D const& mesh, maths::Vector2 const& normal);

geometry::PolyMesh2D translate(geometry::PolyMesh2D const& mesh, maths::Vector2 const& offset);
geometry::PolyMesh2D rotate(geometry::PolyMesh2D const& mesh, double angle_rad);
geometry::PolyMesh2D scale(geometry::PolyMesh2D const& mesh, double factor);
geometry::PolyMesh2D scale(geometry::PolyMesh2D const& mesh, double sx, double sy);
geometry::PolyMesh2D shear(geometry::PolyMesh2D const& mesh, double shx, double shy);
geometry::PolyMesh2D reflect(geometry::PolyMesh2D const& mesh, maths::Vector2 const& normal);

geometry::ConnectedMesh2D translate(geometry::ConnectedMesh2D const& mesh, maths::Vector2 const& offset);
geometry::ConnectedMesh2D rotate(geometry::ConnectedMesh2D const& mesh, double angle_rad);
geometry::ConnectedMesh2D scale(geometry::ConnectedMesh2D const& mesh, double factor);
geometry::ConnectedMesh2D scale(geometry::ConnectedMesh2D const& mesh, double sx, double sy);
geometry::ConnectedMesh2D shear(geometry::ConnectedMesh2D const& mesh, double shx, double shy);
geometry::ConnectedMesh2D reflect(geometry::ConnectedMesh2D const& mesh, maths::Vector2 const& normal);

}  // namespace geompp::transformations
