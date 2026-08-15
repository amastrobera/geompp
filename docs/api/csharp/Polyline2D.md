# `Polyline2D` (C# / .NET)

## `Make`

**static** `Polyline2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

## `FromWkt`

**static** `Polyline2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polyline2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`int Size()`


## `AlmostEquals`

`bool AlmostEquals(Polyline2D^ other, double epsilon)`


**Parameters**

- `other` (`Polyline2D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange2D`](SegmentRange2D.md)`^ ToSegments()`


## `Length`

`double Length()`


## `IsSimple`

`bool IsSimple()`


## `ConvexHull`

[`Polygon2D`](Polygon2D.md)`^ ConvexHull()`

Convex Hull via the Melkman's algorithm.

It requires IsSimple() to be true in order to make sense

**Returns** — The convex hull polygon of the polyline, even if the polyline is not simple (it will be a wrong hull in that case). So please check IsSimple() before running this.

## `Reduce`

`Polyline2D^ Reduce(PolylineDecimationParams^ settings)`

Reduces the polyline to one with fewer vertices.

**Parameters**

- `settings` (`PolylineDecimationParams^`) — bundles the decimation strategy and its threshold — see PolylineDecimationParams and PolylineDecimationParams::Strategy for the per-strategy behavior and Big-O. Defaults to {RamerDouglasPeucker, 0.5} , so Reduce() with no arguments keeps working.

**Returns** — A copy of this polyline with fewer vertices.

## `Expand`

`Polyline2D^ Expand(PolylineExpansionParams^ settings)`

Rounds every inner corner of the polyline with a quadratic Bezier arc — the inverse direction of Reduce() : this adds vertices rather than removing them.

Each corner is delegated to bezier_smoothing_2 independently (see PolylineExpansionParams for the per-corner controls); the true first/last knots are never smoothed. Corners whose sampled arc collapses to a single point (see PolylineExpansionParams::min_segment_length ) are deduplicated rather than emitted as repeated/zero-length segments.

**Parameters**

- `settings` (`PolylineExpansionParams^`) — bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to {0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON} , so Expand() with no arguments works.

**Returns** — A copy of this polyline with rounded corners. Unchanged if it has fewer than 3 knots (no inner corner exists to round).

## `ProjectOnto`

[`Point2D`](Point2D.md)`^ ProjectOnto(`[`Point2D`](Point2D.md)`^ point)`

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to project.

**Returns** — Point on the polyline at minimum distance from point .

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `Location`

`double Location(`[`Point2D`](Point2D.md)`^ point)`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, or +infinity if point is off the polyline.

## `Interpolate`

[`Point2D`](Point2D.md)`^ Interpolate(double pct)`

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

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is on any of the polyline's segments.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Intersects(Polyline2D^ other)`


**Parameters**

- `other` (`Polyline2D^`)

## `Intersection`

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Intersection(Polyline2D^ other)`


**Parameters**

- `other` (`Polyline2D^`)

## `Overlaps`

`bool Overlaps(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Overlaps(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Overlaps(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Overlaps(Polyline2D^ other)`


**Parameters**

- `other` (`Polyline2D^`)

## `Overlap`

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Overlap(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Overlap(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Overlap(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Overlap(Polyline2D^ other)`


**Parameters**

- `other` (`Polyline2D^`)

## `Touches`

`bool Touches(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Touches(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Touches(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Touches(Polyline2D^ other)`


**Parameters**

- `other` (`Polyline2D^`)

## `Touch`

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Touch(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Touch(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Touch(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Touch(Polyline2D^ other)`


**Parameters**

- `other` (`Polyline2D^`)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
