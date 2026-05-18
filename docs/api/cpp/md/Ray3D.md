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

Tests whether this ray intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — true if they meet at a point on the ray's domain (sc on the ray >= 0).

`bool Intersects(Ray3D const & other) const`

Tests whether this ray intersects another ray.

**Parameters**

- `other` (`Ray3D const &`) — The other ray.

**Returns** — true if both rays' domains share the crossing point.

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Tests whether this ray intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — true if the ray hits the segment within both domains.

## `Intersection`

`ReturnSet Intersection(`[`Line3D`](Line3D.md)` const & line) const`

Intersection point of this ray with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — The intersection point if it lies on the ray (sc >= 0), or std::nullopt otherwise.

`ReturnSet Intersection(Ray3D const & other) const`

Intersection point of two rays.

**Parameters**

- `other` (`Ray3D const &`) — The other ray.

**Returns** — The intersection point if it lies on both rays' domains, or std::nullopt otherwise.

`ReturnSet Intersection(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Intersection point of this ray with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — The intersection point if it lies on both the ray and the segment, or std::nullopt otherwise.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Vector3D](Vector3D.md)
