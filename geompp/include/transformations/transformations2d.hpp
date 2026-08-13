#pragma once

#include "../maths.hpp"
// Explicit "../" (rather than bare "point2d.hpp") is deliberate, not stylistic: a bare quoted include
// here would also resolve against the directories of whichever *other* currently-open headers happen to
// be mid-inclusion (MSVC quoted-include search rule 2), which bites hard when geompp_csharp/src/ (with
// its own same-named managed Mesh2D.hpp/Polygon2D.hpp/...) is one of them -- silently pulling in the
// managed C++/CLI wrapper instead of this native header, and failing with "managed type cannot be
// compiled as an unmanaged function" deep in an unrelated TU. The explicit "../" resolves unambiguously
// against this file's own directory first, every time.
#include "../line_segment2d.hpp"
#include "../mesh2d.hpp"
#include "../point2d.hpp"
#include "../polygon2d.hpp"
#include "../polyline2d.hpp"
#include "../polymesh2d.hpp"
#include "../triangle2d.hpp"
#include "../vector2d.hpp"

/// @file transformations2d.hpp
/// @brief geompp::transformations -- affine transforms for geompp::geometry's 2D primitives. Two
/// distinct families:
///   - translate()/rotate()/scale()/shear()/reflect(): fast direct arithmetic on a single Point2D, no
///     matrix built or multiplied -- the cheapest possible path for the single-point case.
///   - transform(primitive, Matrix3): every 2D primitive from Point2D to PolyMesh2D, applying a
///     caller-supplied 3x3 homogeneous matrix (built by hand, or via geompp::maths::Matrix3::
///     Translation()/Rotation()/Scale()/Shear()/Reflection()). This is the general path: composite
///     primitives are rebuilt by transforming each constituent point and re-validating through the
///     type's own Make()/FromTriangles()/FromPolygons(), so a single Matrix3 (e.g. a rotation +
///     translation combined) applies to an entire Polygon2D/Mesh2D/PolyMesh2D in one call.
/// Vector2D transforms rotation/scale/shear/reflection but never translation (homogeneous w = 0) -- a
/// displacement has no position to translate.
namespace geompp::transformations {

// -- Fast direct-arithmetic Point2D transforms (no matrix) --

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

// -- General Matrix3-based transform() -- every 2D primitive, Point2D through PolyMesh2D --

/// @brief Applies a 3x3 homogeneous matrix to a point: (x, y, 1) -> m * (x, y, 1), then drops back to
/// Cartesian (x', y').
geometry::Point2D transform(geometry::Point2D const& p, maths::Matrix3 const& m);

/// @brief Applies a 3x3 homogeneous matrix to a direction: (x, y, 0) -> m * (x, y, 0) -- the 0 in the
/// homogeneous coordinate means any translation component of `m` has no effect, only rotation/scale/
/// shear do (the correct behavior for a displacement, which has no position to translate).
geometry::Vector2D transform(geometry::Vector2D const& v, maths::Matrix3 const& m);

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

}  // namespace geompp::transformations
