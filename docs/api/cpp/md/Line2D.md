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

Tests whether this line intersects another line.

**Parameters**

- `other` (`Line2D const &`) — The other line.

**Returns** — true if they meet at a point; false for parallel non-collinear lines.

`bool Intersects(`[`Ray2D`](Ray2D.md)` const & ray) const`

Tests whether this line intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — true if they meet within the ray's domain.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Tests whether this line intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — true if the line crosses or touches the segment.

`bool Intersects(`[`Triangle2D`](Triangle2D.md)` const & t) const`

Tests whether this line passes through a triangle.

**Parameters**

- `t` ([`Triangle2D`](Triangle2D.md) const &) — The triangle.

**Returns** — true if the line crosses the triangle's interior or an edge.

## `Intersection`

`ReturnSet Intersection(Line2D const & other, double & sc, double & tc) const`

Intersection point of two lines, also returning the parametric values along each line.

**Parameters**

- `other` (`Line2D const &`) — The other line.
- `sc` (`double &`) — Output: parameter along this line at the intersection.
- `tc` (`double &`) — Output: parameter along other at the intersection.

**Returns** — The intersection point, or std::nullopt for parallel lines.

`ReturnSet Intersection(Line2D const & other) const`

Intersection point of two lines.

**Parameters**

- `other` (`Line2D const &`) — The other line.

**Returns** — The intersection point, or std::nullopt for parallel lines.

`ReturnSet Intersection(`[`Ray2D`](Ray2D.md)` const & ray) const`

Intersection point of this line with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md) const &) — The ray.

**Returns** — The intersection point if it lies on the ray, or std::nullopt otherwise.

`ReturnSet Intersection(`[`LineSegment2D`](LineSegment2D.md)` const & segment) const`

Intersection point of this line with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md) const &) — The segment.

**Returns** — The intersection point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md), [Vector2D](Vector2D.md)
