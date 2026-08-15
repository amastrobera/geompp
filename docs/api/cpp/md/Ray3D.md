# `Ray3D` (C++)

## `Make`

**static** `Ray3D Make(`[`Point3D`](Point3D.md)` const & orig, `[`Vector3D`](Vector3D.md)` const & dir)`


**Parameters**

- `orig` ([`Point3D`](Point3D.md) const &)
- `dir` ([`Vector3D`](Vector3D.md) const &)

## `FromWkt`

**static** `Ray3D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Ray3D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Origin`

[`Point3D`](Point3D.md)` const & Origin() const`


## `Direction`

[`Vector3D`](Vector3D.md)` const & Direction() const`


## `AlmostEquals`

`bool AlmostEquals(Ray3D const & other, double epsilon) const`


**Parameters**

- `other` (`Ray3D const &`)
- `epsilon` (`double`)

## `IsAhead`

`bool IsAhead(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies on the half-space ahead of (or at) the ray's origin along its direction.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if (point - origin) · direction >= 0.

## `IsBehind`

`bool IsBehind(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies strictly behind the ray's origin along its direction.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if (point - origin) · direction < 0.

## `ToLine`

[`Line3D`](Line3D.md)` ToLine() const`

Promotes the ray to an infinite line through the same origin and direction.

**Returns** — A [Line3D](Line3D.md) that contains every point on this ray and extends backwards as well.

## `ProjectOnto`

[`Point3D`](Point3D.md)` ProjectOnto(`[`Point3D`](Point3D.md)` const & point) const`

Orthogonal projection of a point onto this ray, clamped to the origin.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to project.

**Returns** — Closest point on the ray to point . Returns the ray's origin if point projects behind it.

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)` const & point) const`

Distance from a point to this ray.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to measure distance to.

**Returns** — Perpendicular distance to the ray's domain, or distance to the origin if point is behind the ray.

`double DistanceTo(`[`Line3D`](Line3D.md)` const & other) const`

Scalar distance between this ray and a line.

**Parameters**

- `other` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — 0 if they intersect or overlap; otherwise the minimum distance.

`double DistanceTo(Ray3D const & ray) const`

Scalar distance between two rays.

**Parameters**

- `ray` (`Ray3D const &`) — The other ray.

**Returns** — 0 if they intersect or share a region; otherwise the minimum distance.

`double DistanceTo(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`

Scalar distance between this ray and a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — 0 if they intersect or share a region; otherwise the minimum distance.

## `Distance`

`std::optional< `[`LineSegment3D`](LineSegment3D.md)` > Distance(`[`Line3D`](Line3D.md)` const & other) const`

Directed line-segment connecting this ray's closest point to a line.

**Parameters**

- `other` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — Segment from the ray's closest point to the line's closest point, or std::nullopt if they intersect or overlap.

`std::optional< `[`LineSegment3D`](LineSegment3D.md)` > Distance(Ray3D const & ray) const`

Directed line-segment connecting this ray's closest point to another ray.

**Parameters**

- `ray` (`Ray3D const &`) — The other ray.

**Returns** — Segment from this ray's closest point to ray's closest point, or std::nullopt if they intersect or share a region.

`std::optional< `[`LineSegment3D`](LineSegment3D.md)` > Distance(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`

Directed line-segment connecting this ray's closest point to a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — Segment from the ray's closest point to the segment's closest point, or std::nullopt if they intersect or share a region.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies on this ray.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if point is collinear with the ray's direction AND ahead of (or at) the origin.

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Intersects(Ray3D const & other) const`


**Parameters**

- `other` (`Ray3D const &`)

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &)

`bool Intersects(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Intersection`

`std::optional< `[`Point3D`](Point3D.md)` > Intersection(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< `[`Point3D`](Point3D.md)` > Intersection(Ray3D const & other) const`


**Parameters**

- `other` (`Ray3D const &`)

`std::optional< `[`Point3D`](Point3D.md)` > Intersection(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &)

`std::optional< std::variant< `[`Point3D`](Point3D.md)` , std::vector< `[`Point3D`](Point3D.md)` > > > Intersection(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Overlaps`

`bool Overlaps(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Overlaps(Ray3D const & ray) const`


**Parameters**

- `ray` (`Ray3D const &`)

`bool Overlaps(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`bool Overlaps(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Overlap`

`std::optional< Ray3D > Overlap(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< std::variant< Ray3D , `[`LineSegment3D`](LineSegment3D.md)` > > Overlap(Ray3D const & ray) const`


**Parameters**

- `ray` (`Ray3D const &`)

`std::optional< `[`LineSegment3D`](LineSegment3D.md)` > Overlap(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`std::optional< std::vector< `[`LineSegment3D`](LineSegment3D.md)` > > Overlap(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Touches`

`bool Touches(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`bool Touches(Ray3D const & ray) const`


**Parameters**

- `ray` (`Ray3D const &`)

`bool Touches(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`bool Touches(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)

## `Touch`

`std::optional< `[`Point3D`](Point3D.md)` > Touch(`[`Line3D`](Line3D.md)` const & line) const`


**Parameters**

- `line` ([`Line3D`](Line3D.md) const &)

`std::optional< `[`Point3D`](Point3D.md)` > Touch(Ray3D const & ray) const`


**Parameters**

- `ray` (`Ray3D const &`)

`std::optional< `[`Point3D`](Point3D.md)` > Touch(`[`LineSegment3D`](LineSegment3D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md) const &)

`std::optional< std::vector< `[`Point3D`](Point3D.md)` > > Touch(`[`Polyline3D`](Polyline3D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md) const &)


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polyline3D](Polyline3D.md), [Vector3D](Vector3D.md)
