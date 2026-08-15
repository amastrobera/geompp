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


## `Reversed`

`LineSegment3D Reversed() const`


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


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Intersects(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`bool Intersects(LineSegment3D const & segment) const`


**Parameters**

- `segment` (`LineSegment3D const &`)

`bool Intersects(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Intersection`

`std::optional< `[`Point3D`](Point3D.md)` > Intersection(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< `[`Point3D`](Point3D.md)` > Intersection(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`std::optional< `[`Point3D`](Point3D.md)` > Intersection(LineSegment3D const & other) const`


**Parameters**

- `other` (`LineSegment3D const &`)

`std::optional< std::variant< `[`Point3D`](Point3D.md)` , std::vector< `[`Point3D`](Point3D.md)` > > > Intersection(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Overlaps`

`bool Overlaps(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Overlaps(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`bool Overlaps(LineSegment3D const & seg) const`


**Parameters**

- `seg` (`LineSegment3D const &`)

`bool Overlaps(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Overlap`

`std::optional< LineSegment3D > Overlap(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< LineSegment3D > Overlap(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`std::optional< LineSegment3D > Overlap(LineSegment3D const & seg) const`


**Parameters**

- `seg` (`LineSegment3D const &`)

`std::optional< std::vector< LineSegment3D > > Overlap(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Touches`

`bool Touches(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Touches(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`bool Touches(LineSegment3D const & seg) const`


**Parameters**

- `seg` (`LineSegment3D const &`)

`bool Touches(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Touch`

`std::optional< `[`Point3D`](Point3D.md)` > Touch(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< `[`Point3D`](Point3D.md)` > Touch(`[`Ray3D`](Ray3D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &)

`std::optional< `[`Point3D`](Point3D.md)` > Touch(LineSegment3D const & seg) const`


**Parameters**

- `seg` (`LineSegment3D const &`)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Touch(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)


---

**See also:** [Line3D](Line3D.md), [Point3D](Point3D.md), [Polyline3D](Polyline3D.md), [Ray3D](Ray3D.md)
