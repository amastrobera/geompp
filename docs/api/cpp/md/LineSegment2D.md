# `LineSegment2D` (C++)

## `Make`

**static** `LineSegment2D Make(`[`Point2D`](Point2D.md)` const & p0, `[`Point2D`](Point2D.md)` const & p1)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md) const &)
- `p1` ([`Point2D`](Point2D.md) const &)

## `FromWkt`

**static** `LineSegment2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `LineSegment2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `First`

[`Point2D`](Point2D.md)` const & First() const`


## `Last`

[`Point2D`](Point2D.md)` const & Last() const`


## `AlmostEquals`

`bool AlmostEquals(LineSegment2D const & other, double epsilon) const`


**Parameters**

- `other` (`LineSegment2D const &`)
- `epsilon` (`double`)

## `ToLine`

[`Line2D`](Line2D.md)` ToLine() const`


## `Length`

`double Length() const`


## `ProjectOnto`

[`Point2D`](Point2D.md)` ProjectOnto(`[`Point2D`](Point2D.md)` const & point) const`

Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to project.

**Returns** — Closest point on the segment to point (one of the endpoints if point projects outside).

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)` const & point) const`

Distance from a point to this segment.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to measure distance to.

**Returns** — Perpendicular distance to the segment, or distance to the nearest endpoint if point projects outside.

## `Location`

`double Location(`[`Point2D`](Point2D.md)` const & point) const`

Position of a point along the segment, normalized to [0, 1].

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to locate. Must lie on the segment's underlying line.

**Returns** — 0 at First, 1 at Last, fractional values in between, or +infinity if not on the line.

## `Interpolate`

[`Point2D`](Point2D.md)` Interpolate(double pct) const`

Linear interpolation along the segment.

**Parameters**

- `pct` (`double`) — Normalized position in [0, 1]. Values outside are clamped to the segment's endpoints.

**Returns** — Point at the given fraction along the segment.

## `IsLeft`

`bool IsLeft(`[`Point2D`](Point2D.md)` const & p) const`

Tests whether a point is on the left of the segment's direction (looking from First toward Last).

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if the 2D cross product (P1 - P0) × (p - P0) is positive.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies on this segment.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if point is collinear with the segment and falls within [First, Last].

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)` const & line) const`

Tests whether this segment intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — true if the line crosses or touches the segment.

`bool Intersects(`[`Ray2D`](Ray2D.md)` const & ray) const`

Tests whether this segment intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — true if the segment is met within the ray's domain.

`bool Intersects(LineSegment2D const & segment) const`

Tests whether this segment intersects another segment.

**Parameters**

- `segment` (`LineSegment2D const &`) — The other segment.

**Returns** — true if both segments' domains share the crossing point.

## `Intersection`

`ReturnSet Intersection(`[`Line2D`](Line2D.md)` const & line) const`

Intersection point of this segment with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — The intersection point if it lies on the segment, or std::nullopt otherwise.

`ReturnSet Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`

Intersection point of this segment with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — The intersection point if it lies on both, or std::nullopt otherwise.

`ReturnSet Intersection(LineSegment2D const & other) const`

Intersection point of two segments.

**Parameters**

- `other` (`LineSegment2D const &`) — The other segment.

**Returns** — The intersection point if it lies on both segments, or std::nullopt otherwise.


---

**See also:** [Line2D](Line2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md)
