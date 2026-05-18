# `Ray2D` (C++)

## `Make`

**static** `Ray2D Make(`[`Point2D`](Point2D.md)` const & orig, `[`Vector2D`](Vector2D.md)` const & dir)`


**Parameters**

- `orig` ([`Point2D`](Point2D.md) const &)
- `dir` ([`Vector2D`](Vector2D.md) const &)

## `FromWkt`

**static** `Ray2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Ray2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `Origin`

[`Point2D`](Point2D.md)` const & Origin() const`


## `Direction`

[`Vector2D`](Vector2D.md)` const & Direction() const`


## `AlmostEquals`

`bool AlmostEquals(Ray2D const & other, double epsilon) const`


**Parameters**

- `other` (`Ray2D const &`)
- `epsilon` (`double`)

## `IsAhead`

`bool IsAhead(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies on the half-space ahead of (or at) the ray's origin along its direction.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if (point - origin) · direction >= 0.

## `IsBehind`

`bool IsBehind(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies strictly behind the ray's origin along its direction.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if (point - origin) · direction < 0.

## `ToLine`

[`Line2D`](Line2D.md)` ToLine() const`

Promotes the ray to an infinite line through the same origin and direction.

**Returns** — A [Line2D](Line2D.md) that contains every point on this ray and extends backwards as well.

## `ProjectOnto`

[`Point2D`](Point2D.md)` ProjectOnto(`[`Point2D`](Point2D.md)` const & point) const`

Orthogonal projection of a point onto this ray, clamped to the origin.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to project.

**Returns** — Closest point on the ray to point . Returns the ray's origin if point projects behind it.

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)` const & point) const`

Distance from a point to this ray.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to measure distance to.

**Returns** — Perpendicular distance to the ray's domain, or distance to the origin if point is behind the ray.

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies on this ray.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if point is collinear with the ray's direction AND ahead of (or at) the origin.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)` const & line) const`

Tests whether this ray intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — true if they meet at a point on the ray's domain.

`bool Intersects(Ray2D const & other) const`

Tests whether this ray intersects another ray.

**Parameters**

- `other` (`Ray2D const &`) — The other ray.

**Returns** — true if both rays' domains share the crossing point.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Tests whether this ray intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — true if the ray hits the segment within both domains.

## `Intersection`

`ReturnSet Intersection(`[`Line2D`](Line2D.md)` const & line) const`

Intersection point of this ray with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md) const &) — The line.

**Returns** — The intersection point if it lies on the ray, or std::nullopt otherwise.

`ReturnSet Intersection(Ray2D const & other) const`

Intersection point of two rays.

**Parameters**

- `other` (`Ray2D const &`) — The other ray.

**Returns** — The intersection point if it lies on both rays' domains, or std::nullopt otherwise.

`ReturnSet Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Intersection point of this ray with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — The intersection point if it lies on both the ray and the segment, or std::nullopt otherwise.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Vector2D](Vector2D.md)
