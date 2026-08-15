# `Polyline3D` (C++)

## `Make`

**static** `Polyline3D Make(std::vector< `[`Point3D`](Point3D.md)` > const & points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) > const &)

## `FromWkt`

**static** `Polyline3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Polyline3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Size`

`int Size() const`


## `AlmostEquals`

`bool AlmostEquals(Polyline3D const & other, double epsilon) const`


**Parameters**

- `other` (`Polyline3D const &`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange3D`](SegmentRange3D.md)` ToSegments() const`


## `Length`

`double Length() const`


## `ProjectOnto`

[`Point3D`](Point3D.md)` ProjectOnto(`[`Point3D`](Point3D.md)` const & point) const`

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to project.

**Returns** — Point on the polyline at minimum distance from point ; falls on one of the segments' interiors or knots.

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)` const & point) const`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `Location`

`double Location(`[`Point3D`](Point3D.md)` const & point) const`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, fractional values in between, or +infinity if point is off the polyline.

## `Interpolate`

[`Point3D`](Point3D.md)` Interpolate(double pct) const`

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

`bool Contains(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if point is on any of the polyline's segments (interior or knot).

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Intersects(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &)

`bool Intersects(Polyline3D const & other) const`


**Parameters**

- `other` (`Polyline3D const &`)

## `Intersection`

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Intersection(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Intersection(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Intersection(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Intersection(Polyline3D const & other) const`


**Parameters**

- `other` (`Polyline3D const &`)

## `Overlaps`

`bool Overlaps(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Overlaps(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`bool Overlaps(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`bool Overlaps(Polyline3D const & other) const`


**Parameters**

- `other` (`Polyline3D const &`)

## `Overlap`

`std::optional< std::vector< `[`LineSegment3D`](LineSegment3D.md)` > > Overlap(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< std::vector< `[`LineSegment3D`](LineSegment3D.md)` > > Overlap(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`std::optional< std::vector< `[`LineSegment3D`](LineSegment3D.md)` > > Overlap(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`std::optional< std::vector< `[`LineSegment3D`](LineSegment3D.md)` > > Overlap(Polyline3D const & other) const`


**Parameters**

- `other` (`Polyline3D const &`)

## `Touches`

`bool Touches(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Touches(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`bool Touches(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`bool Touches(Polyline3D const & other) const`


**Parameters**

- `other` (`Polyline3D const &`)

## `Touch`

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Touch(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Touch(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Touch(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Touch(Polyline3D const & other) const`


**Parameters**

- `other` (`Polyline3D const &`)

## `IsPlanar`

`bool IsPlanar() const`

Tests whether all knots of the polyline are coplanar.

**Returns** — true if all knots lie in a common plane.

## `IsSimple`

`bool IsSimple() const`

Tests whether the polyline has no self-intersections (when projected onto its best-fit plane).

**Returns** — true if the polyline does not self-intersect.

## `IsConvex`

`bool IsConvex() const`

Tests whether the polyline is a convex polygon boundary.

**Returns** — true if the polyline is planar, simple, and all turns go in the same direction.

## `ConvexHull`

`Polyline3D ConvexHull() const`

Computes the convex hull of the polyline's knots.

**Returns** — A new Polyline3D containing the convex hull vertices in CCW order.

## `Reduce`

`Polyline3D Reduce(`[`PolylineDecimationParams`](PolylineDecimationParams.md)` const & settings) const`

Reduces the polyline to one with fewer vertices.

**Parameters**

- `settings` ([`PolylineDecimationParams`](PolylineDecimationParams.md) const &) — bundles the decimation strategy and its threshold — see [PolylineDecimationParams](PolylineDecimationParams.md) and [PolylineDecimationParams](PolylineDecimationParams.md)::Strategy for the per-strategy behavior and Big-O. Defaults to {RamerDouglasPeucker, 0.5} , so Reduce() with no arguments keeps working.

**Returns** — A copy of this polyline with fewer vertices.

## `Expand`

`Polyline3D Expand(`[`PolylineExpansionParams`](PolylineExpansionParams.md)` const & settings) const`

Rounds every inner corner of the polyline with a quadratic Bezier arc — the inverse direction of Reduce() : this adds vertices rather than removing them.

Each corner is delegated to bezier_smoothing_2 independently (see [PolylineExpansionParams](PolylineExpansionParams.md) for the per-corner controls); the true first/last knots are never smoothed. Corners whose sampled arc collapses to a single point (see [PolylineExpansionParams](PolylineExpansionParams.md)::min_segment_length ) are deduplicated rather than emitted as repeated/zero-length segments.

**Parameters**

- `settings` ([`PolylineExpansionParams`](PolylineExpansionParams.md) const &) — bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to {0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON} , so Expand() with no arguments works.

**Returns** — A copy of this polyline with rounded corners. Unchanged if it has fewer than 3 knots (no inner corner exists to round).

## `ToPolygon`

[`Polygon3D`](Polygon3D.md)` ToPolygon() const`

Converts this polyline to a [Polygon3D](Polygon3D.md) .

**Returns** — A [Polygon3D](Polygon3D.md) with the same vertices.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [PolylineDecimationParams](PolylineDecimationParams.md), [PolylineExpansionParams](PolylineExpansionParams.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
