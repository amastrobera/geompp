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

Tests whether this polyline intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — true if the line crosses any of the polyline's segments.

`bool Intersects(`[`Ray2D`](Ray2D.md)` const & ray) const`

Tests whether this polyline intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — true if the ray crosses any of the polyline's segments.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Tests whether this polyline intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — true if the segment crosses any of the polyline's segments.

`bool Intersects(Polyline2D const & other) const`

Tests whether two polylines intersect.

**Parameters**

- `other` (`Polyline2D const &`) — The other polyline.

**Returns** — true if any segment of either polyline crosses any segment of the other.

## `Intersection`

`ReturnSet Intersection(`[`Line2D`](Line2D.md)` const & line) const`

Intersection of this polyline with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — A single [Point2D](Point2D.md) when there's one crossing, a list when there are several, or std::nullopt if disjoint.

`ReturnSet Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`

Intersection of this polyline with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.

`ReturnSet Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Intersection of this polyline with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.

`ReturnSet Intersection(Polyline2D const & other) const`

Intersection of two polylines.

**Parameters**

- `other` (`Polyline2D const &`) — The other polyline.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
