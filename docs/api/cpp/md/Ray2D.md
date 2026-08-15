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


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`bool Intersects(Ray2D const & other) const`


**Parameters**

- `other` (`Ray2D const &`)

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Intersects(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Intersection`

`std::optional< `[`Point2D`](Point2D.md)` > Intersection(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`std::optional< `[`Point2D`](Point2D.md)` > Intersection(Ray2D const & other) const`


**Parameters**

- `other` (`Ray2D const &`)

`std::optional< `[`Point2D`](Point2D.md)` > Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Intersection(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Overlaps`

`bool Overlaps(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`bool Overlaps(Ray2D const & ray) const`


**Parameters**

- `ray` (`Ray2D const &`)

`bool Overlaps(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Overlaps(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Overlap`

`std::optional< Ray2D > Overlap(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`std::optional< std::variant< Ray2D , `[`LineSegment2D`](LineSegment2D.md)` > > Overlap(Ray2D const & ray) const`


**Parameters**

- `ray` (`Ray2D const &`)

`std::optional< `[`LineSegment2D`](LineSegment2D.md)` > Overlap(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Overlap(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Touches`

`bool Touches(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`bool Touches(Ray2D const & ray) const`


**Parameters**

- `ray` (`Ray2D const &`)

`bool Touches(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Touches(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Touch`

`std::optional< `[`Point2D`](Point2D.md)` > Touch(`[`Line2D`](Line2D.md)` const & line) const`


**Parameters**

- `line` ([`Line2D`](Line2D.md) const &)

`std::optional< `[`Point2D`](Point2D.md)` > Touch(Ray2D const & ray) const`


**Parameters**

- `ray` (`Ray2D const &`)

`std::optional< `[`Point2D`](Point2D.md)` > Touch(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Touch(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Vector2D](Vector2D.md)
