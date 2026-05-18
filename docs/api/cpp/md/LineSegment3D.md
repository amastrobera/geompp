# `LineSegment3D` (C++)

## `Make`

**static** `LineSegment3D Make(`[`Point3D`](Point3D.md)` const & p0, `[`Point3D`](Point3D.md)` const & p1)`


**Parameters**

- `p0` ([`Point3D`](Point3D.md) const &)
- `p1` ([`Point3D`](Point3D.md) const &)

## `FromWkt`

**static** `LineSegment3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `LineSegment3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `First`

[`Point3D`](Point3D.md)` const & First() const`


## `Last`

[`Point3D`](Point3D.md)` const & Last() const`


## `AlmostEquals`

`bool AlmostEquals(LineSegment3D const & other, double epsilon) const`


**Parameters**

- `other` (`LineSegment3D const &`)
- `epsilon` (`double`)

## `ToLine`

[`Line3D`](Line3D.md)` ToLine() const`


## `Length`

`double Length() const`


## `ProjectOnto`

[`Point3D`](Point3D.md)` ProjectOnto(`[`Point3D`](Point3D.md)` const & point) const`

Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to project.

**Returns** — Closest point on the segment to point (one of the endpoints if point projects outside the segment).

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)` const & point) const`

Distance from a point to this segment.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the segment, or distance to the nearest endpoint if point projects outside the segment.

`double DistanceTo(`[`Line3D`](Line3D.md)` const & other) const`

Scalar distance between this segment and a line.

**Parameters**

- `other` ([`Line3D`](Line3D.md) const &) — The other line.

**Returns** — 0 if the line crosses or contains the segment; otherwise the minimum distance.

`double DistanceTo(`[`Ray3D`](Ray3D.md)` const & ray) const`

Scalar distance between this segment and a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — 0 if they intersect or overlap; otherwise the minimum distance.

`double DistanceTo(LineSegment3D const & seg) const`

Scalar distance between two segments.

**Parameters**

- `seg` (`LineSegment3D const &`) — The other segment.

**Returns** — 0 if they intersect or share a region; otherwise the minimum distance.

## `Location`

`double Location(`[`Point3D`](Point3D.md)` const & point) const`

Position of a point along the segment, normalized to [0, 1].

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to locate. Must lie on the segment's underlying line.

**Returns** — 0 at First, 1 at Last, fractional values in between, or +infinity if point is not on the segment's line.

## `Interpolate`

[`Point3D`](Point3D.md)` Interpolate(double pct) const`

Linear interpolation along the segment.

**Parameters**

- `pct` (`double`) — Normalized position in [0, 1]. Values outside that range are clamped to the segment's endpoints.

**Returns** — Point at the given fraction along the segment.

## `Flip`

`LineSegment3D Flip() const`

Returns a segment with endpoints swapped.

**Returns** — A new LineSegment3D from Last to First.

## `Distance`

`std::optional< LineSegment3D > Distance(`[`Line3D`](Line3D.md)` const & other) const`

Directed line-segment connecting this segment's closest point to another line.

**Parameters**

- `other` ([`Line3D`](Line3D.md) const &) — The other line.

**Returns** — Segment from this segment's closest point to other's closest point, or std::nullopt if they intersect or are collinear.

`std::optional< LineSegment3D > Distance(`[`Ray3D`](Ray3D.md)` const & ray) const`

Directed line-segment connecting this segment's closest point to a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — Segment from this segment's closest point to the ray's closest point, or std::nullopt if they intersect or overlap.

`std::optional< LineSegment3D > Distance(LineSegment3D const & seg) const`

Directed line-segment connecting this segment's closest point to another segment.

**Parameters**

- `seg` (`LineSegment3D const &`) — The other segment.

**Returns** — Segment from this segment's closest point to seg's closest point, or std::nullopt if they intersect or share a region.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies on this segment (between or at the endpoints).

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if point is collinear with the segment and falls within [First, Last].

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)` const & line) const`

Tests whether this segment intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — true if the line crosses the segment's interior or an endpoint.

`bool Intersects(`[`Ray3D`](Ray3D.md)` const & ray) const`

Tests whether this segment intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — true if the segment is met within the ray's domain (sc on ray >= 0).

`bool Intersects(LineSegment3D const & segment) const`

Tests whether this segment intersects another segment.

**Parameters**

- `segment` (`LineSegment3D const &`) — The other segment.

**Returns** — true if both segments' domains share the crossing point.

## `Intersection`

`ReturnSet Intersection(`[`Line3D`](Line3D.md)` const & line) const`

Intersection point of this segment with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — The intersection point if it lies on the segment (sc in [0, 1]), or std::nullopt otherwise.

`ReturnSet Intersection(`[`Ray3D`](Ray3D.md)` const & ray) const`

Intersection point of this segment with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — The intersection point if it lies on both the segment and the ray, or std::nullopt otherwise.

`ReturnSet Intersection(LineSegment3D const & other) const`

Intersection point of two segments.

**Parameters**

- `other` (`LineSegment3D const &`) — The other segment.

**Returns** — The intersection point if it lies on both segments, or std::nullopt otherwise.


---

**See also:** [Line3D](Line3D.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md)
