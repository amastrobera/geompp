# `Polyline2D` (C++)

## `Make`

**static** `Polyline2D Make(std::vector< `[`Point2D`](Point2D.md)` > const & points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) > const &)

## `FromWkt`

**static** `Polyline2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Polyline2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Size`

`int Size() const`


## `AlmostEquals`

`bool AlmostEquals(Polyline2D const & other, double epsilon) const`


**Parameters**

- `other` (`Polyline2D const &`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange2D`](SegmentRange2D.md)` ToSegments() const`


## `Length`

`double Length() const`


## `IsSimple`

`bool IsSimple() const`


## `ConvexHull`

[`Polygon2D`](Polygon2D.md)` ConvexHull()`

Convex Hull via the Melkman's algorithm.

It requires IsSimple() to be true in order to make sense

**Returns** — The convex hull polygon of the polyline, even if the polyline is not simple (it will be a wrong hull in that case). So please check IsSimple() before running this.

## `Reduce`

`Polyline2D Reduce(`[`PolylineDecimationParams`](PolylineDecimationParams.md)` const & settings) const`

Reduces the polyline to one with fewer vertices.

**Parameters**

- `settings` ([`PolylineDecimationParams`](PolylineDecimationParams.md) const &) — bundles the decimation strategy and its threshold — see [PolylineDecimationParams](PolylineDecimationParams.md) and [PolylineDecimationParams](PolylineDecimationParams.md)::Strategy for the per-strategy behavior and Big-O. Defaults to {RamerDouglasPeucker, 0.5} , so Reduce() with no arguments keeps working.

**Returns** — A copy of this polyline with fewer vertices.

## `Expand`

`Polyline2D Expand(`[`PolylineExpansionParams`](PolylineExpansionParams.md)` const & settings) const`

Rounds every inner corner of the polyline with a quadratic Bezier arc — the inverse direction of Reduce() : this adds vertices rather than removing them.

Each corner is delegated to bezier_smoothing_2 independently (see [PolylineExpansionParams](PolylineExpansionParams.md) for the per-corner controls); the true first/last knots are never smoothed. Corners whose sampled arc collapses to a single point (see [PolylineExpansionParams](PolylineExpansionParams.md)::min_segment_length ) are deduplicated rather than emitted as repeated/zero-length segments.

**Parameters**

- `settings` ([`PolylineExpansionParams`](PolylineExpansionParams.md) const &) — bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to {0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON} , so Expand() with no arguments works.

**Returns** — A copy of this polyline with rounded corners. Unchanged if it has fewer than 3 knots (no inner corner exists to round).

## `ProjectOnto`

[`Point2D`](Point2D.md)` ProjectOnto(`[`Point2D`](Point2D.md)` const & point) const`

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to project.

**Returns** — Point on the polyline at minimum distance from point .

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)` const & point) const`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `Location`

`double Location(`[`Point2D`](Point2D.md)` const & point) const`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, or +infinity if point is off the polyline.

## `Interpolate`

[`Point2D`](Point2D.md)` Interpolate(double pct) const`

Linear interpolation along the polyline by arc-length fraction.

**Parameters**

- `pct` (`double`) — Normalized position in [0, 1]. Values outside the range are clamped to the polyline's endpoints.

**Returns** — Point at the given arc-length fraction along the polyline.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if point is on any of the polyline's segments.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`bool Intersects(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Intersects(Polyline2D const & other) const`


**Parameters**

- `other` (`Polyline2D const &`)

## `Intersection`

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Intersection(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Intersection(Polyline2D const & other) const`


**Parameters**

- `other` (`Polyline2D const &`)

## `Overlaps`

`bool Overlaps(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`bool Overlaps(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`bool Overlaps(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Overlaps(Polyline2D const & other) const`


**Parameters**

- `other` (`Polyline2D const &`)

## `Overlap`

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Overlap(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Overlap(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Overlap(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Overlap(Polyline2D const & other) const`


**Parameters**

- `other` (`Polyline2D const &`)

## `Touches`

`bool Touches(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`bool Touches(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`bool Touches(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Touches(Polyline2D const & other) const`


**Parameters**

- `other` (`Polyline2D const &`)

## `Touch`

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Touch(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Touch(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Touch(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Touch(Polyline2D const & other) const`


**Parameters**

- `other` (`Polyline2D const &`)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [PolylineDecimationParams](PolylineDecimationParams.md), [PolylineExpansionParams](PolylineExpansionParams.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
