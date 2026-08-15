# `Line2D` (C++)

## `Make`

**static** `Line2D Make(`[`Point2D`](Point2D.md)` const & p0, `[`Point2D`](Point2D.md)` const & p1)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md) const &)
- `p1` ([`Point2D`](Point2D.md) const &)

**static** `Line2D Make(`[`Point2D`](Point2D.md)` const & orig, `[`Vector2D`](Vector2D.md)` const & dir)`


**Parameters**

- `orig` ([`Point2D`](Point2D.md) const &)
- `dir` ([`Vector2D`](Vector2D.md) const &)

## `FromWkt`

**static** `Line2D FromWkt(std::string const & wkt)`


**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Line2D FromFile(std::string const & path)`


**Parameters**

- `path` (`std::string const &`)

## `First`

[`Point2D`](Point2D.md)` const & First() const`


## `Last`

[`Point2D`](Point2D.md)` const & Last() const`


## `Origin`

[`Point2D`](Point2D.md)` const & Origin() const`


## `Direction`

[`Vector2D`](Vector2D.md)` const & Direction() const`


## `AlmostEquals`

`bool AlmostEquals(Line2D const & other, double epsilon) const`


**Parameters**

- `other` (`Line2D const &`)
- `epsilon` (`double`)

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)` const & point) const`

Perpendicular distance from a point to this infinite line.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to measure distance to.

**Returns** — Length of the perpendicular from point onto this line.

## `ProjectOnto`

[`Point2D`](Point2D.md)` ProjectOnto(`[`Point2D`](Point2D.md)` const & point) const`

Orthogonal projection of a point onto this infinite line.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to project.

**Returns** — Closest point on this line to point .

## `ToWkt`

`std::string ToWkt() const`


## `ToFile`

`void ToFile(std::string const & path) const`


**Parameters**

- `path` (`std::string const &`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & point) const`

Tests whether a point lies on this line.

**Parameters**

- `point` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if point is collinear with the line within decimal precision.

## `Intersects`

`bool Intersects(Line2D const & other) const`


**Parameters**

- `other` (`Line2D const &`)

`bool Intersects(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Intersects(`[`Triangle2D`](Triangle2D.md)` const & t) const`


**Parameters**

- `t` ([`Triangle2D`](Triangle2D.md) const &)

`bool Intersects(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Intersection`

`std::optional< `[`Point2D`](Point2D.md)` > Intersection(Line2D const & other) const`


**Parameters**

- `other` (`Line2D const &`)

`std::optional< `[`Point2D`](Point2D.md)` > Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`std::optional< `[`Point2D`](Point2D.md)` > Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Intersection(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Overlaps`

`bool Overlaps(Line2D const & line) const`


**Parameters**

- `line` (`Line2D const &`)

`bool Overlaps(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`bool Overlaps(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Overlaps(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Overlap`

`std::optional< Line2D > Overlap(Line2D const & line) const`


**Parameters**

- `line` (`Line2D const &`)

`std::optional< `[`Ray2D`](Ray2D.md)` > Overlap(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`std::optional< `[`LineSegment2D`](LineSegment2D.md)` > Overlap(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`LineSegment2D`](LineSegment2D.md)` > > Overlap(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Touches`

`bool Touches(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`bool Touches(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`bool Touches(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)

## `Touch`

`std::optional< `[`Point2D`](Point2D.md)` > Touch(`[`Ray2D`](Ray2D.md)` const & ray) const`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &)

`std::optional< `[`Point2D`](Point2D.md)` > Touch(`[`LineSegment2D`](LineSegment2D.md)` const & seg) const`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md) const &)

`std::optional< std::vector< `[`Point2D`](Point2D.md)` > > Touch(`[`Polyline2D`](Polyline2D.md)` const & polyline) const`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md) const &)


---

**See also:** [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md), [Vector2D](Vector2D.md)
