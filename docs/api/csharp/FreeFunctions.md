# `FreeFunctions` (C# / .NET)

Free functions in `geompp::geometry` (not methods on a class — call them directly).

## `FindExtremePoints`

[`ExtremePoints`](ExtremePoints.md)` < `[`Point2D`](Point2D.md)` >^ FindExtremePoints(`[`Polygon2D`](Polygon2D.md)`^ polygon, `[`Line2D`](Line2D.md)`^ line)`

Finds the two vertices of a polygon that are extreme (least / greatest projection) along a line.

Uses Daniel Sunday's O(log n) binary search when the polygon is convex, else an O(n) linear scan.

**Parameters**

- `polygon` ([`Polygon2D`](Polygon2D.md)^) — The polygon whose vertices are searched (holes are ignored — only the outer ring matters).
- `line` ([`Line2D`](Line2D.md)^) — The line whose Direction() defines the axis of projection.

**Returns** — [ExtremePoints](ExtremePoints.md) {min_point, max_point} — the outer-ring vertices with least / greatest projection.

[`ExtremePoints`](ExtremePoints.md)` < `[`Point3D`](Point3D.md)` >^ FindExtremePoints(`[`Polygon3D`](Polygon3D.md)`^ polygon, `[`Line3D`](Line3D.md)`^ line)`

Finds the two vertices of a polygon that are extreme (least / greatest projection) along a line.

Uses Daniel Sunday's O(log n) binary search when the polygon is convex, else an O(n) linear scan.

**Parameters**

- `polygon` ([`Polygon3D`](Polygon3D.md)^) — The polygon whose vertices are searched (holes are ignored — only the outer ring matters).
- `line` ([`Line3D`](Line3D.md)^) — The line whose Direction() defines the axis of projection.

**Returns** — [ExtremePoints](ExtremePoints.md) {min_point, max_point} — the outer-ring vertices with least / greatest projection.

## `PrincipalAxes`

[`CoordinateFrame`](CoordinateFrame.md)`^ PrincipalAxes(std::vector< `[`Point3D`](Point3D.md)` > points)`

Computes the three principal axes of a point cloud using PCA (Jacobi eigendecomposition).

**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >) — The point cloud. Must contain at least 3 non-collinear points.

**Returns** — A [CoordinateFrame](CoordinateFrame.md) whose X is the direction of most spread, Y the second, and Z the best-fit plane normal.

## `PrincipalNormal`

[`Vector3D`](Vector3D.md)`^ PrincipalNormal(std::vector< `[`Point3D`](Point3D.md)` > points)`

Computes the best-fit plane normal for a cloud of points using PCA.

**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >) — The point cloud. Must contain at least 3 non-collinear points.

## `PrincipalDirection`

[`Vector3D`](Vector3D.md)`^ PrincipalDirection(std::vector< `[`Point3D`](Point3D.md)` > points)`

Computes the dominant direction of a point cloud using PCA.

**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >) — The point cloud. Must contain at least 3 non-collinear points.

## `Clip`

`std::vector< Points > Clip(Points^ clipper_loop, Points^ subject_loop)`

Clips subject_loop against clipper_loop , returning the area both loops share (their set intersection) — the classic "clip a subject polygon by a window polygon" operation, for callers who have raw point loops rather than [Polygon2D](Polygon2D.md)/[Polygon3D](Polygon3D.md) instances (no holes, no CCW/CW requirement on input).

Works for both [Point2D](Point2D.md) and [Point3D](Point3D.md) loops. Uses the same general planar-arrangement engine as [Polygon2D](Polygon2D.md)::Intersection([Polygon2D](Polygon2D.md)) — no special case for convex clippers (a convex-only caller could use the simpler/faster Sutherland-Hodgman algorithm instead, but that's a different algorithm, not offered here).

**Parameters**

- `clipper_loop` (`Points^`) — The clip region's vertices, in order. Last point must NOT repeat the first — the loop is treated as implicitly closed (an edge connects the last vertex back to the first).
- `subject_loop` (`Points^`) — The subject's vertices, same "implicitly closed, no repeated first point" convention.

**Returns** — Every ring of the intersection, CCW outer rings and CW hole rings mixed in one flat list (an intersection of two hole-less loops can still have a hole — e.g. two overlapping "L" shapes can intersect into a shape with a hole in the middle — so the caller must be prepared for that; group by signed_area()/orientation and nesting the same way simplify_rings()'s caller would). Empty if the loops don't overlap.

`template std::vector< std::vector< `[`Point2D`](Point2D.md)` > > Clip(std::vector< `[`Point2D`](Point2D.md)` > , std::vector< `[`Point2D`](Point2D.md)` > )`


**Parameters**


`template std::vector< std::vector< `[`Point3D`](Point3D.md)` > > Clip(std::vector< `[`Point3D`](Point3D.md)` > , std::vector< `[`Point3D`](Point3D.md)` > )`


**Parameters**


## `DistanceTo`

`double DistanceTo(`[`Polygon2D`](Polygon2D.md)`^ polygon, `[`Line2D`](Line2D.md)`^ line)`

computes the distance between a polygon and a line (the distance is zero if they intersect)

**Parameters**

- `polygon` ([`Polygon2D`](Polygon2D.md)^)
- `line` ([`Line2D`](Line2D.md)^)

`double DistanceTo(`[`Polygon3D`](Polygon3D.md)`^ polygon, `[`Line3D`](Line3D.md)`^ line)`

computes the distance between a polygon and a line (the distance is zero if they intersect)

**Parameters**

- `polygon` ([`Polygon3D`](Polygon3D.md)^)
- `line` ([`Line3D`](Line3D.md)^)

## `TangentsTo`

[`PolygonTangents`](PolygonTangents.md)` < `[`LineSegment2D`](LineSegment2D.md)` >^ TangentsTo(`[`Polygon2D`](Polygon2D.md)`^ polygon, `[`Point2D`](Point2D.md)`^ p)`

Finds the left and right tangent vertices from a point to a convex polygon.

**Parameters**

- `polygon` ([`Polygon2D`](Polygon2D.md)^)
- `p` ([`Point2D`](Point2D.md)^)

[`PolygonTangents`](PolygonTangents.md)` < `[`LineSegment2D`](LineSegment2D.md)` >^ TangentsTo(`[`Polygon2D`](Polygon2D.md)`^ polygon, `[`Polygon2D`](Polygon2D.md)`^ other)`

finds the tangents from a polygon to another

**Parameters**

- `polygon` ([`Polygon2D`](Polygon2D.md)^)
- `other` ([`Polygon2D`](Polygon2D.md)^)

[`PolygonTangents`](PolygonTangents.md)` < `[`LineSegment3D`](LineSegment3D.md)` >^ TangentsTo(`[`Polygon3D`](Polygon3D.md)`^ polygon, `[`Point3D`](Point3D.md)`^ p)`

Finds the left and right tangent segments from a point to a polygon, both projected onto the polygon's own plane.

A tangent (a line touching a shape without crossing it) is inherently a planar concept, so unlike distance_to this has no "purely 3D / skew" fallback: p must lie in the polygon's plane.

**Parameters**

- `polygon` ([`Polygon3D`](Polygon3D.md)^)
- `p` ([`Point3D`](Point3D.md)^)

[`PolygonTangents`](PolygonTangents.md)` < `[`LineSegment3D`](LineSegment3D.md)` >^ TangentsTo(`[`Polygon3D`](Polygon3D.md)`^ polygon, `[`Polygon3D`](Polygon3D.md)`^ other)`

Finds the common outer tangent segments between two polygons, both projected onto their shared plane.

Like the point overload, this requires both polygons to lie in the same plane — two polygons in general (skew) 3D position don't have a single well-defined common tangent line.

**Parameters**

- `polygon` ([`Polygon3D`](Polygon3D.md)^)
- `other` ([`Polygon3D`](Polygon3D.md)^)

## `DistDecimation`

`Points^ DistDecimation(Points^ points, double threshold)`


**Parameters**

- `points` (`Points^`)
- `threshold` (`double`)

`template std::vector< `[`Point2D`](Point2D.md)` > DistDecimation(std::vector< `[`Point2D`](Point2D.md)` > , double threshold)`


**Parameters**

- `threshold` (`double`)

`template std::vector< `[`Point3D`](Point3D.md)` > DistDecimation(std::vector< `[`Point3D`](Point3D.md)` > , double threshold)`


**Parameters**

- `threshold` (`double`)

## `RdpDecimation`

`Points^ RdpDecimation(Points^ points, double threshold)`


**Parameters**

- `points` (`Points^`)
- `threshold` (`double`)

`template std::vector< `[`Point2D`](Point2D.md)` > RdpDecimation(std::vector< `[`Point2D`](Point2D.md)` > , double threshold)`


**Parameters**

- `threshold` (`double`)

`template std::vector< `[`Point3D`](Point3D.md)` > RdpDecimation(std::vector< `[`Point3D`](Point3D.md)` > , double threshold)`


**Parameters**

- `threshold` (`double`)

## `VwDecimation`

`Points^ VwDecimation(Points^ points, double threshold)`


**Parameters**

- `points` (`Points^`)
- `threshold` (`double`)

`template std::vector< `[`Point2D`](Point2D.md)` > VwDecimation(std::vector< `[`Point2D`](Point2D.md)` > , double threshold)`


**Parameters**

- `threshold` (`double`)

`template std::vector< `[`Point3D`](Point3D.md)` > VwDecimation(std::vector< `[`Point3D`](Point3D.md)` > , double threshold)`


**Parameters**

- `threshold` (`double`)

## `BezierSmoothing2`

`std::vector< PointT > BezierSmoothing2(PointT^ p0, PointT^ p1, PointT^ p2, double smoothness, double min_distance, double min_segment_length)`

Rounds the corner at p1 with a quadratic Bezier arc tangent to p0-p1 and p1-p2, density-sampled.

The tangent points are trimmed in from p1 by up to smoothness fraction of the shorter adjacent edge, then the arc between them is sampled roughly min_distance apart (see bezier_trimmed_tangents / sample_quadratic_bezier in calc_utils/polyline_ops2d.cpp). p1 itself is not part of the result (it is replaced by the arc) unless smoothness is 0, in which case every sample collapses to p1 (no smoothing).

**Parameters**

- `p0` (`PointT^`) — point before the corner.
- `p1` (`PointT^`) — the corner being smoothed.
- `p2` (`PointT^`) — point after the corner.
- `smoothness` (`double`) — in [0, 1]: fraction of the shorter adjacent edge (p0-p1 or p1-p2) to trim into tangent points. 0 leaves the corner sharp (every sample collapses to p1); 1 trims half of the shorter edge.
- `min_distance` (`double`) — target spacing between consecutive sampled points along the arc.
- `min_segment_length` (`double`) — skip trimming on a side whose adjacent edge (p0-p1 or p1-p2) is at or below this length — that tangent point collapses to p1 instead (same fallback as the exact-zero-length guard). Defaults to DOUBLE_EPSILON (matching [PolylineExpansionParams](PolylineExpansionParams.md)::min_segment_length and this codebase's usual epsilon-parameter default), so an edge indistinguishable from zero at the current DECIMAL_PRECISION is never trimmed into, without the caller having to opt in. Note the effective threshold is min_segment_length + DOUBLE_EPSILON , not exactly min_segment_length — the internal compare() used for the check has its own DOUBLE_EPSILON-wide tolerance band on top of whatever value is passed here. If both adjacent edges are at or below it, the whole corner collapses to p1 (no curve at all — every sample is p1), giving the caller a way to skip smoothing tiny/noisy corners entirely (pass 0.0 to only skip on an edge that's truly, exactly zero-length).

**Returns** — points sampled from the tangent point near p0 to the tangent point near p2, inclusive.

`template std::vector< `[`Point2D`](Point2D.md)` > BezierSmoothing2(`[`Point2D`](Point2D.md)`^ p0, `[`Point2D`](Point2D.md)`^ p1, `[`Point2D`](Point2D.md)`^ p2, double smoothness, double min_distance, double min_segment_length)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md)^)
- `p1` ([`Point2D`](Point2D.md)^)
- `p2` ([`Point2D`](Point2D.md)^)
- `smoothness` (`double`)
- `min_distance` (`double`)
- `min_segment_length` (`double`)

`template std::vector< `[`Point3D`](Point3D.md)` > BezierSmoothing2(`[`Point3D`](Point3D.md)`^ p0, `[`Point3D`](Point3D.md)`^ p1, `[`Point3D`](Point3D.md)`^ p2, double smoothness, double min_distance, double min_segment_length)`


**Parameters**

- `p0` ([`Point3D`](Point3D.md)^)
- `p1` ([`Point3D`](Point3D.md)^)
- `p2` ([`Point3D`](Point3D.md)^)
- `smoothness` (`double`)
- `min_distance` (`double`)
- `min_segment_length` (`double`)

`std::vector< PointT > BezierSmoothing2(PointT^ p0, PointT^ p1, PointT^ p2, double smoothness, int num_segments, double min_segment_length)`

Same as bezier_smoothing_2(p0, p1, p2, smoothness, min_distance), but takes an exact sample count instead of a distance-derived one.

Kept as a distinct overload rather than a parameter that reinterprets min_distance — the two controls don't share a unit or a precedence rule (see the design note in calc_utils/polyline_ops2d.cpp). Note: a bare int argument always resolves here and a bare double always resolves to the min_distance overload (exact type match beats either implicit conversion); any other numeric type (e.g. std::size_t) is ambiguous between the two and requires an explicit cast at the call site.

**Parameters**

- `p0` (`PointT^`)
- `p1` (`PointT^`)
- `p2` (`PointT^`)
- `smoothness` (`double`)
- `num_segments` (`int`) — number of segments to divide the arc into; the result has num_segments + 1 points.
- `min_segment_length` (`double`) — see the min_distance overload — same skip-trim/skip-corner behavior and DOUBLE_EPSILON default.

**Returns** — num_segments + 1 points sampled evenly from the tangent point near p0 to the tangent point near p2.

`template std::vector< `[`Point2D`](Point2D.md)` > BezierSmoothing2(`[`Point2D`](Point2D.md)`^ p0, `[`Point2D`](Point2D.md)`^ p1, `[`Point2D`](Point2D.md)`^ p2, double smoothness, int num_segments, double min_segment_length)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md)^)
- `p1` ([`Point2D`](Point2D.md)^)
- `p2` ([`Point2D`](Point2D.md)^)
- `smoothness` (`double`)
- `num_segments` (`int`)
- `min_segment_length` (`double`)

`template std::vector< `[`Point3D`](Point3D.md)` > BezierSmoothing2(`[`Point3D`](Point3D.md)`^ p0, `[`Point3D`](Point3D.md)`^ p1, `[`Point3D`](Point3D.md)`^ p2, double smoothness, int num_segments, double min_segment_length)`


**Parameters**

- `p0` ([`Point3D`](Point3D.md)^)
- `p1` ([`Point3D`](Point3D.md)^)
- `p2` ([`Point3D`](Point3D.md)^)
- `smoothness` (`double`)
- `num_segments` (`int`)
- `min_segment_length` (`double`)

## `PolylineExpansion`

`std::vector< PointT > PolylineExpansion(std::vector< PointT > input, `[`PolylineExpansionParams`](PolylineExpansionParams.md)`^ settings)`

Rounds every inner corner of input (index 1 through size()-2) with a quadratic Bezier arc via bezier_smoothing_2 — the shared implementation behind [Polyline2D](Polyline2D.md)::Expand() / [Polyline3D](Polyline3D.md)::Expand() .

The true first/last points of input are always preserved unsmoothed. p0/p1/p2 for each corner are always read from input directly, never from the output already built up: bezier_smoothing_2's trim is bounded by the true adjacent edge lengths (each corner's trim is independently capped at half of its shared edge with a neighbor, so adjacent corners' arcs can touch but never cross), and substituting an already-trimmed output point would both shrink that apparent edge length (under-trimming) and stop p1 from being the actual corner vertex being rounded.

**Parameters**

- `input` (`std::vector< PointT >`) — the knots to round; must have at least 1 point (0 and 1-point input is a no-op).
- `settings` ([`PolylineExpansionParams`](PolylineExpansionParams.md)^) — bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to {0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON} .

**Returns** — input with every inner corner replaced by its rounded arc; consecutive duplicate points (e.g. from a corner fully skipped via min_segment_length) are collapsed to one.

`template std::vector< `[`Point2D`](Point2D.md)` > PolylineExpansion(std::vector< `[`Point2D`](Point2D.md)` > input, `[`PolylineExpansionParams`](PolylineExpansionParams.md)`^ settings)`


**Parameters**

- `input` (std::vector< [`Point2D`](Point2D.md) >)
- `settings` ([`PolylineExpansionParams`](PolylineExpansionParams.md)^)

`template std::vector< `[`Point3D`](Point3D.md)` > PolylineExpansion(std::vector< `[`Point3D`](Point3D.md)` > input, `[`PolylineExpansionParams`](PolylineExpansionParams.md)`^ settings)`


**Parameters**

- `input` (std::vector< [`Point3D`](Point3D.md) >)
- `settings` ([`PolylineExpansionParams`](PolylineExpansionParams.md)^)

## `Triangulate`

`std::vector< `[`Triangle2D`](Triangle2D.md)` > Triangulate(std::vector< `[`Point2D`](Point2D.md)` > input, `[`TriangulationParams`](TriangulationParams.md)`^ settings)`

Breaks down a simple polygon into triangles.

**Parameters**

- `input` (std::vector< [`Point2D`](Point2D.md) >) — polygon's outer loop of points (assumed CCW) and no holes allowed
- `settings` ([`TriangulationParams`](TriangulationParams.md)^) — options for functions inner workings (1) triangulation strategy options: user decides what algorithm to run EarClipping clips the first valid ear it finds in scan order. Most robust and general-purpose, and often close to O(n) in practice, but O(n²) worst-case and doesn't optimize triangle shape, so it can produce a visually thin sliver purely from scan order, even on ordinary input. EarClippingBestFit clips the best-scoring (least sliver-prone) valid ear every step instead of the first one. Same termination guarantee as EarClipping, but unconditionally ~O(n²) a full rescan of the current ring on every single clip, not just worst-case. [Default: prefers shape quality over raw speed.] MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition into monotone pieces) Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle of all the angles of the triangles in the triangulation (avoiding skinny triangles) (2) simplicity: the input for the algo should be a simple polygon (no self-intersections) Guaranteed: the input is assumed to be a good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's input will be simplified in O(n log n) time (3) winding: the input points should be in counter clockwise order (CCW) Guaranteed: the input is assumed to be good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's input will be reversed in O(n) time (4) collinearity: the input points should not contain any collinear (including consecutive duplicates) points Guaranteed: the input is assumed to be good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's bad vertices will be removed in O(n) time

**Returns** — one [Triangle2D](Triangle2D.md) per triangle; input.size() - 2 triangles for a simple polygon.

`std::vector< `[`Triangle2D`](Triangle2D.md)` > Triangulate(std::vector< `[`Polygon2D`](Polygon2D.md)` > polygons, `[`TriangulationParams`](TriangulationParams.md)`^ settings)`

Batch-triangulates a set of polygon facets together.

The free-function equivalent of [PolyMesh2D](PolyMesh2D.md)::FromPolygons(polygons).Triangulate() for callers who just want triangles without constructing/keeping a full [PolyMesh2D](PolyMesh2D.md) . Unlike [PolyMesh2D](PolyMesh2D.md)::FromPolygons (which always Asserts, since bad input there is a straightforward construction error), this defaults to fixing what it can.

**Parameters**

- `polygons` (std::vector< [`Polygon2D`](Polygon2D.md) >) — each facet's outer ring (no holes).
- `settings` ([`TriangulationParams`](TriangulationParams.md)^) — per-facet [TriangulationParams](TriangulationParams.md) (Strategy/Simplicity/Winding/Collinearity), same as the single-ring triangulate() overload above, plus [TriangulationParams](TriangulationParams.md)::conformity: how to handle cross-facet adjacency violations (T-junctions / non-manifold edges) before triangulating. Defaults to Enforce. Under Enforce, a facet that needed a conformity splice is always triangulated with Collinearity::Guaranteed regardless of the rest of settings otherwise the caller's own Collinearity::Enforce (the [TriangulationParams](TriangulationParams.md) default) would strip the just-spliced vertex right back out, silently undoing the repair and reintroducing the T-junction in the triangulated output.

**Returns** — every triangle from every facet, combined into one flat list.

`std::vector< `[`Triangle3D`](Triangle3D.md)` > Triangulate(std::vector< `[`Point3D`](Point3D.md)` > input, `[`Vector3D`](Vector3D.md)`^ normal, `[`TriangulationParams`](TriangulationParams.md)`^ settings)`

Breaks down a simple, planar polygon into triangles.

input is assumed flat/coplanar — every vertex is projected through the dominant-axis view of normal before triangulating, so a non-planar input silently triangulates its projection, not its true 3D shape.

**Parameters**

- `input` (std::vector< [`Point3D`](Point3D.md) >) — polygon's outer loop of points (assumed CCW against the normal) and no holes allowed
- `normal` ([`Vector3D`](Vector3D.md)^) — the normal vector of the polygon's plane
- `settings` ([`TriangulationParams`](TriangulationParams.md)^) — options for functions inner workings (1) triangulation strategy options: user decides what algorithm to run EarClipping clips the first valid ear it finds in scan order. Most robust and general-purpose, and often close to O(n) in practice, but O(n²) worst-case and doesn't optimize triangle shape, so it can produce a visually thin sliver purely from scan order, even on ordinary input. EarClippingBestFit clips the best-scoring (least sliver-prone) valid ear every step instead of the first one. Same termination guarantee as EarClipping, but unconditionally ~O(n²) a full rescan of the current ring on every single clip, not just worst-case. [Default: prefers shape quality over raw speed.] MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition into monotone pieces) Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle of all the angles of the triangles in the triangulation (avoiding skinny triangles) (2) simplicity: the input for the algo should be a simple polygon (no self-intersections) Guaranteed: the input is assumed to be a good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's input will be simplified in O(n log n) time (3) winding: the input points should be in counter clockwise order (CCW) Guaranteed: the input is assumed to be good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's input will be reversed in O(n) time (4) collinearity: the input points should not contain any collinear (including consecutive duplicates) points Guaranteed: the input is assumed to be good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's bad vertices will be removed in O(n) time

**Returns** — one [Triangle3D](Triangle3D.md) per triangle; input.size() - 2 triangles for a simple polygon.

`std::vector< `[`Triangle3D`](Triangle3D.md)` > Triangulate(std::vector< `[`Point3D`](Point3D.md)` > input, `[`TriangulationParams`](TriangulationParams.md)`^ settings)`

Same as the (input, normal, settings) overload, but fits the plane normal via PCA (principal_normal) when the caller doesn't already have one on hand — mirrors is_simple()'s and convex_hull_indices()'s two-overload pattern.

input is assumed flat/coplanar — see that overload.

**Parameters**

- `input` (std::vector< [`Point3D`](Point3D.md) >) — polygon's outer loop of points (assumed CCW against their own normal) and no holes allowed
- `settings` ([`TriangulationParams`](TriangulationParams.md)^) — options for functions inner workings (1) triangulation strategy options: user decides what algorithm to run EarClipping clips the first valid ear it finds in scan order. Most robust and general-purpose, and often close to O(n) in practice, but O(n²) worst-case and doesn't optimize triangle shape, so it can produce a visually thin sliver purely from scan order, even on ordinary input. EarClippingBestFit clips the best-scoring (least sliver-prone) valid ear every step instead of the first one. Same termination guarantee as EarClipping, but unconditionally ~O(n²) a full rescan of the current ring on every single clip, not just worst-case. [Default: prefers shape quality over raw speed.] MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition into monotone pieces) Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle of all the angles of the triangles in the triangulation (avoiding skinny triangles) (2) simplicity: the input for the algo should be a simple polygon (no self-intersections) Guaranteed: the input is assumed to be a good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's input will be simplified in O(n log n) time (3) winding: the input points should be in counter clockwise order (CCW) Guaranteed: the input is assumed to be good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's input will be reversed in O(n) time (4) collinearity: the input points should not contain any collinear (including consecutive duplicates) points Guaranteed: the input is assumed to be good at the users's own risk Assert: will throw if the user's input is not good Enforce: will check, and if not good, the user's bad vertices will be removed in O(n) time

**Returns** — one [Triangle3D](Triangle3D.md) per triangle; input.size() - 2 triangles for a simple polygon.

## `ValidateAdjacency`

`std::vector< `[`AdjacencyViolation`](AdjacencyViolation.md)` < `[`Point2D`](Point2D.md)` > > ValidateAdjacency(std::vector< `[`Polygon2D`](Polygon2D.md)` > facets)`

Checks a batch of facets for the mesh-conformity rule "every edge has at most 1 neighbor" equivalently, no facet vertex may lie in the interior of another facet's edge, only exactly at that edge's own start/end vertex.

The same rule [Mesh2D](Mesh2D.md)/3D::FromTriangles/[PolyMesh2D](PolyMesh2D.md)/3D::FromPolygons/ [ConnectedMesh2D](ConnectedMesh2D.md)/3D::FromTriangles enforce (always, via Assert) at construction time.

**Parameters**

- `facets` (std::vector< [`Polygon2D`](Polygon2D.md) >) — each facet's outer ring (e.g. [Polygon2D](Polygon2D.md)/3D::Perimeter(), or a [Triangle2D](Triangle2D.md)/3D's 3 vertices).

**Returns** — every violation found (both T-junctions and non-manifold edges); empty if conforming.

`std::vector< `[`AdjacencyViolation`](AdjacencyViolation.md)` < `[`Point2D`](Point2D.md)` > > ValidateAdjacency(std::vector< `[`Triangle2D`](Triangle2D.md)` > facets)`


**Parameters**

- `facets` (std::vector< [`Triangle2D`](Triangle2D.md) >)

`std::vector< `[`AdjacencyViolation`](AdjacencyViolation.md)` < `[`Point3D`](Point3D.md)` > > ValidateAdjacency(std::vector< `[`Polygon3D`](Polygon3D.md)` > facets)`


**Parameters**

- `facets` (std::vector< [`Polygon3D`](Polygon3D.md) >)

`std::vector< `[`AdjacencyViolation`](AdjacencyViolation.md)` < `[`Point3D`](Point3D.md)` > > ValidateAdjacency(std::vector< `[`Triangle3D`](Triangle3D.md)` > facets)`


**Parameters**

- `facets` (std::vector< [`Triangle3D`](Triangle3D.md) >)

`std::vector< `[`AdjacencyViolation`](AdjacencyViolation.md)` < `[`Point2D`](Point2D.md)` > > ValidateAdjacency(std::vector< std::vector< `[`Point2D`](Point2D.md)` > > facet_rings)`

validate_adjacency() overload for raw point rings (not yet wrapped in [Polygon2D](Polygon2D.md)/3D) what fix_adjacency() below returns, and what its own detection pass runs on internally.

**Parameters**

- `facet_rings` (std::vector< std::vector< [`Point2D`](Point2D.md) > >)

`std::vector< `[`AdjacencyViolation`](AdjacencyViolation.md)` < `[`Point3D`](Point3D.md)` > > ValidateAdjacency(std::vector< std::vector< `[`Point3D`](Point3D.md)` > > facet_rings)`


**Parameters**

- `facet_rings` (std::vector< std::vector< [`Point3D`](Point3D.md) > >)

## `FixAdjacency`

`std::vector< std::vector< `[`Point2D`](Point2D.md)` > > FixAdjacency(std::vector< `[`Polygon2D`](Polygon2D.md)` > facets)`

Repairs every T-junction validate_adjacency() would report.

For each foreign vertex, splices it into the coarse edge, then cuts a diagonal from that vertex to the nearest ring vertex that forms a valid, non-crossing diagonal (checked via proper-segment-intersection + a point-in-polygon interior test), splitting the facet into two pieces along it. A facet with several T-junctions on one edge ends up split into several pieces, not just spliced once. Does NOT attempt to fix a non-manifold edge (a full edge shared by 3+ facets); there's no principled automatic repair for that, so it throws instead same as validate_adjacency() + Assert would. Returns raw point rings, NOT reconstructed [Polygon2D](Polygon2D.md)/3D objects: the split pieces have no guarantee of matching a valid [Polygon2D](Polygon2D.md)/3D winding/hole structure. Unlike the Triangle overload below, these pieces have no leftover flat (180°) vertices, so unlike that overload they can be triangulated afterward with the default Collinearity::Enforce; no special handling needed.

**Parameters**

- `facets` (std::vector< [`Polygon2D`](Polygon2D.md) >)

`std::vector< std::vector< `[`Point3D`](Point3D.md)` > > FixAdjacency(std::vector< `[`Polygon3D`](Polygon3D.md)` > facets)`


**Parameters**

- `facets` (std::vector< [`Polygon3D`](Polygon3D.md) >)

`std::vector< `[`Triangle2D`](Triangle2D.md)` > FixAdjacency(std::vector< `[`Triangle2D`](Triangle2D.md)` > facets)`

Triangle overload of fix_adjacency(): unlike a [Polygon2D](Polygon2D.md)/3D facet, a triangle can't just absorb a spliced-in vertex and stay a triangle a repaired facet is re-triangulated (via the same ear-clipping engine triangulate() uses, with Collinearity::Guaranteed for the same reason the Polygon overload avoids [Polygon2D](Polygon2D.md)/3D::Make()) into 2+ triangles covering the exact same area as the original one.

An unaffected facet passes through unchanged (still exactly 1 triangle).

**Parameters**

- `facets` (std::vector< [`Triangle2D`](Triangle2D.md) >)

`std::vector< `[`Triangle3D`](Triangle3D.md)` > FixAdjacency(std::vector< `[`Triangle3D`](Triangle3D.md)` > facets)`


**Parameters**

- `facets` (std::vector< [`Triangle3D`](Triangle3D.md) >)

## `HasIntersections`

`bool HasIntersections(std::vector< `[`LineSegment2D`](LineSegment2D.md)` > segments)`

Shamos-Hoey: true if any two segments in the set intersect.

**Parameters**

- `segments` (std::vector< [`LineSegment2D`](LineSegment2D.md) >)

## `FindIntersections`

`std::vector< `[`Point2D`](Point2D.md)` > FindIntersections(std::vector< `[`LineSegment2D`](LineSegment2D.md)` > segments)`

Bentley-Ottmann: all intersection points among the segments, sorted bottom-left to top-right.

**Parameters**

- `segments` (std::vector< [`LineSegment2D`](LineSegment2D.md) >)

## `AreCoplanar`

`bool AreCoplanar(std::vector< `[`Point3D`](Point3D.md)` > points, `[`Plane`](Plane.md)`^  (nullable) plane)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `plane` ([`Plane`](Plane.md)^  (nullable))

## `ClosestWorldPlaneTo`

[`Plane`](Plane.md)`^ ClosestWorldPlaneTo(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

## `SignedArea`

`double SignedArea(std::vector< `[`Point3D`](Point3D.md)` > points, `[`Plane`](Plane.md)`^  (nullable) plane)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `plane` ([`Plane`](Plane.md)^  (nullable))

`double SignedArea(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

## `AreCCW`

`bool AreCCW(std::vector< `[`Point3D`](Point3D.md)` > points, `[`Plane`](Plane.md)`^  (nullable) ref_plane)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `ref_plane` ([`Plane`](Plane.md)^  (nullable))

`bool AreCCW(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

## `AreCW`

`bool AreCW(std::vector< `[`Point3D`](Point3D.md)` > points, `[`Plane`](Plane.md)`^  (nullable) ref_plane)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `ref_plane` ([`Plane`](Plane.md)^  (nullable))

`bool AreCW(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

## `Centroid`

[`Point3D`](Point3D.md)`^ Centroid(std::vector< `[`Point3D`](Point3D.md)` > points, `[`Plane`](Plane.md)`^  (nullable) plane)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `plane` ([`Plane`](Plane.md)^  (nullable))

[`Point2D`](Point2D.md)`^ Centroid(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

## `AreCollinear`

`bool AreCollinear(`[`Point2D`](Point2D.md)`^ p1, `[`Point2D`](Point2D.md)`^ p2, `[`Point2D`](Point2D.md)`^ p3)`


**Parameters**

- `p1` ([`Point2D`](Point2D.md)^)
- `p2` ([`Point2D`](Point2D.md)^)
- `p3` ([`Point2D`](Point2D.md)^)

`bool AreCollinear(`[`Point3D`](Point3D.md)`^ p1, `[`Point3D`](Point3D.md)`^ p2, `[`Point3D`](Point3D.md)`^ p3)`


**Parameters**

- `p1` ([`Point3D`](Point3D.md)^)
- `p2` ([`Point3D`](Point3D.md)^)
- `p3` ([`Point3D`](Point3D.md)^)

## `RemoveConsecutiveDuplicates`

`std::vector< `[`Point2D`](Point2D.md)` > RemoveConsecutiveDuplicates(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

`std::vector< `[`Point2D`](Point2D.md)` > RemoveConsecutiveDuplicates(std::vector< `[`Point2D`](Point2D.md)` > points)`

Same as the const& overload, but compacts points in place (no fresh vector allocated) instead of copying survivors into a new one.

**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

`std::vector< `[`Point3D`](Point3D.md)` > RemoveConsecutiveDuplicates(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

`std::vector< `[`Point3D`](Point3D.md)` > RemoveConsecutiveDuplicates(std::vector< `[`Point3D`](Point3D.md)` > points)`

Same as the const& overload, but compacts points in place (no fresh vector allocated) instead of copying survivors into a new one.

**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

## `RemoveDuplicates`

`std::vector< `[`Point2D`](Point2D.md)` > RemoveDuplicates(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

`std::vector< `[`Point3D`](Point3D.md)` > RemoveDuplicates(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

`void RemoveDuplicates(std::vector< double > sorted_vec, double epsilon)`

removes duplicates from a sorted vector (duplicates are consecutive) input {0,0,0,1,2,3,4,4,5} --> output: {0,1,2,3,4,5}

**Parameters**

- `sorted_vec` (`std::vector< double >`) — vector of sorted elements
- `epsilon` (`double`)

## `RemoveCollinear`

`std::vector< `[`Point2D`](Point2D.md)` > RemoveCollinear(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

`std::vector< `[`Point2D`](Point2D.md)` > RemoveCollinear(std::vector< `[`Point2D`](Point2D.md)` > points)`

Same as the const& overload, but compacts points in place (no fresh vector allocated) instead of copying survivors into a new one.

**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

`std::vector< `[`Point3D`](Point3D.md)` > RemoveCollinear(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

`std::vector< `[`Point3D`](Point3D.md)` > RemoveCollinear(std::vector< `[`Point3D`](Point3D.md)` > points)`

Same as the const& overload, but compacts points in place (no fresh vector allocated) instead of copying survivors into a new one.

**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

## `LinearCombination`

[`Point2D`](Point2D.md)`^ LinearCombination(std::vector< `[`Point2D`](Point2D.md)` > points, std::vector< double > weights)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)
- `weights` (`std::vector< double >`)

[`Point3D`](Point3D.md)`^ LinearCombination(std::vector< `[`Point3D`](Point3D.md)` > points, std::vector< double > weights)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `weights` (`std::vector< double >`)

## `Average`

[`Point2D`](Point2D.md)`^ Average(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

[`Point3D`](Point3D.md)`^ Average(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

## `ConvexHull`

`std::vector< `[`Point2D`](Point2D.md)` > ConvexHull(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

`std::vector< `[`Point3D`](Point3D.md)` > ConvexHull(std::vector< `[`Point3D`](Point3D.md)` > points, `[`Vector3D`](Vector3D.md)`^  (nullable) normal)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `normal` ([`Vector3D`](Vector3D.md)^  (nullable))

## `Lerp`

[`Point2D`](Point2D.md)`^ Lerp(`[`Point2D`](Point2D.md)`^ P0, `[`Point2D`](Point2D.md)`^ P1, double t)`

Linear interpolation: P0 + t * (P1 - P0). Not clamped — t outside [0, 1] extrapolates past P0/P1.

**Parameters**

- `P0` ([`Point2D`](Point2D.md)^)
- `P1` ([`Point2D`](Point2D.md)^)
- `t` (`double`)

[`Point3D`](Point3D.md)`^ Lerp(`[`Point3D`](Point3D.md)`^ P0, `[`Point3D`](Point3D.md)`^ P1, double t)`

Linear interpolation: P0 + t * (P1 - P0). Not clamped — t outside [0, 1] extrapolates past P0/P1.

**Parameters**

- `P0` ([`Point3D`](Point3D.md)^)
- `P1` ([`Point3D`](Point3D.md)^)
- `t` (`double`)


---

**See also:** [AdjacencyViolation](AdjacencyViolation.md), [ConnectedMesh2D](ConnectedMesh2D.md), [CoordinateFrame](CoordinateFrame.md), [ExtremePoints](ExtremePoints.md), [Line2D](Line2D.md), [Line3D](Line3D.md), [LineSegment2D](LineSegment2D.md), [LineSegment3D](LineSegment3D.md), [Mesh2D](Mesh2D.md), [Plane](Plane.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [PolyMesh2D](PolyMesh2D.md), [Polygon2D](Polygon2D.md), [Polygon3D](Polygon3D.md), [PolygonTangents](PolygonTangents.md), [Polyline2D](Polyline2D.md), [Polyline3D](Polyline3D.md), [PolylineExpansionParams](PolylineExpansionParams.md), [Triangle2D](Triangle2D.md), [Triangle3D](Triangle3D.md), [TriangulationParams](TriangulationParams.md), [Vector3D](Vector3D.md)
