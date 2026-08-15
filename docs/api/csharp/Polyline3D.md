# `Polyline3D` (C# / .NET)

## `Make`

**static** `Polyline3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

## `FromWkt`

**static** `Polyline3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polyline3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`int Size()`


## `AlmostEquals`

`bool AlmostEquals(Polyline3D^ other, double epsilon)`


**Parameters**

- `other` (`Polyline3D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange3D`](SegmentRange3D.md)`^ ToSegments()`


## `Length`

`double Length()`


## `ProjectOnto`

[`Point3D`](Point3D.md)`^ ProjectOnto(`[`Point3D`](Point3D.md)`^ point)`

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to project.

**Returns** — Point on the polyline at minimum distance from point ; falls on one of the segments' interiors or knots.

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)`^ point)`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `Location`

`double Location(`[`Point3D`](Point3D.md)`^ point)`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, fractional values in between, or +infinity if point is off the polyline.

## `Interpolate`

[`Point3D`](Point3D.md)`^ Interpolate(double pct)`

Linear interpolation along the polyline by arc-length fraction.

**Parameters**

- `pct` (`double`) — Normalized position in [0, 1]. Values outside the range are clamped to the polyline's endpoints.

**Returns** — Point at the given arc-length fraction along the polyline.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test.

**Returns** — true if point is on any of the polyline's segments (interior or knot).

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`bool Intersects(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^)

`bool Intersects(Polyline3D^ other)`


**Parameters**

- `other` (`Polyline3D^`)

## `Intersection`

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Intersection(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Intersection(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Intersection(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^)

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Intersection(Polyline3D^ other)`


**Parameters**

- `other` (`Polyline3D^`)

## `Overlaps`

`bool Overlaps(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`bool Overlaps(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`bool Overlaps(`[`LineSegment3D`](LineSegment3D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)

`bool Overlaps(Polyline3D^ other)`


**Parameters**

- `other` (`Polyline3D^`)

## `Overlap`

`std::vector< `[`LineSegment3D`](LineSegment3D.md)` >^  (nullable) Overlap(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`std::vector< `[`LineSegment3D`](LineSegment3D.md)` >^  (nullable) Overlap(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`std::vector< `[`LineSegment3D`](LineSegment3D.md)` >^  (nullable) Overlap(`[`LineSegment3D`](LineSegment3D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)

`std::vector< `[`LineSegment3D`](LineSegment3D.md)` >^  (nullable) Overlap(Polyline3D^ other)`


**Parameters**

- `other` (`Polyline3D^`)

## `Touches`

`bool Touches(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`bool Touches(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`bool Touches(`[`LineSegment3D`](LineSegment3D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)

`bool Touches(Polyline3D^ other)`


**Parameters**

- `other` (`Polyline3D^`)

## `Touch`

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Touch(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Touch(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Touch(`[`LineSegment3D`](LineSegment3D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)^)

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Touch(Polyline3D^ other)`


**Parameters**

- `other` (`Polyline3D^`)

## `IsPlanar`

`bool IsPlanar()`

Tests whether all knots of the polyline are coplanar.

**Returns** — true if all knots lie in a common plane.

## `IsSimple`

`bool IsSimple()`

Tests whether the polyline has no self-intersections (when projected onto its best-fit plane).

**Returns** — true if the polyline does not self-intersect.

## `IsConvex`

`bool IsConvex()`

Tests whether the polyline is a convex polygon boundary.

**Returns** — true if the polyline is planar, simple, and all turns go in the same direction.

## `ConvexHull`

`Polyline3D^ ConvexHull()`

Computes the convex hull of the polyline's knots.

**Returns** — A new Polyline3D containing the convex hull vertices in CCW order.

## `Reduce`

`Polyline3D^ Reduce(PolylineDecimationParams^ settings)`

Reduces the polyline to one with fewer vertices.

**Parameters**

- `settings` (`PolylineDecimationParams^`) — bundles the decimation strategy and its threshold — see PolylineDecimationParams and PolylineDecimationParams::Strategy for the per-strategy behavior and Big-O. Defaults to {RamerDouglasPeucker, 0.5} , so Reduce() with no arguments keeps working.

**Returns** — A copy of this polyline with fewer vertices.

## `Expand`

`Polyline3D^ Expand(PolylineExpansionParams^ settings)`

Rounds every inner corner of the polyline with a quadratic Bezier arc — the inverse direction of Reduce() : this adds vertices rather than removing them.

Each corner is delegated to bezier_smoothing_2 independently (see PolylineExpansionParams for the per-corner controls); the true first/last knots are never smoothed. Corners whose sampled arc collapses to a single point (see PolylineExpansionParams::min_segment_length ) are deduplicated rather than emitted as repeated/zero-length segments.

**Parameters**

- `settings` (`PolylineExpansionParams^`) — bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to {0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON} , so Expand() with no arguments works.

**Returns** — A copy of this polyline with rounded corners. Unchanged if it has fewer than 3 knots (no inner corner exists to round).

## `ToPolygon`

[`Polygon3D`](Polygon3D.md)`^ ToPolygon()`

Converts this polyline to a [Polygon3D](Polygon3D.md) .

**Returns** — A [Polygon3D](Polygon3D.md) with the same vertices.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
