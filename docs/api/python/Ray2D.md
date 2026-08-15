# `Ray2D` (Python)

## `make`

**static** `make(orig: `[`Point2D`](Point2D.md)`, dir: `[`Vector2D`](Vector2D.md)`) -> Ray2D`


**Parameters**

- `orig` ([`Point2D`](Point2D.md))
- `dir` ([`Vector2D`](Vector2D.md))

## `from_wkt`

**static** `from_wkt(wkt: str) -> Ray2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Ray2D`


**Parameters**

- `path` (`str`)

## `origin`

`origin() -> `[`Point2D`](Point2D.md)


## `direction`

`direction() -> `[`Vector2D`](Vector2D.md)


## `almost_equals`

`almost_equals(other: Ray2D, epsilon: float) -> bool`


**Parameters**

- `other` (`Ray2D`)
- `epsilon` (`float`)

## `is_ahead`

`is_ahead(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies on the half-space ahead of (or at) the ray's origin along its direction.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if (point - origin) · direction >= 0.

## `is_behind`

`is_behind(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies strictly behind the ray's origin along its direction.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if (point - origin) · direction < 0.

## `to_line`

`to_line() -> `[`Line2D`](Line2D.md)

Promotes the ray to an infinite line through the same origin and direction.

**Returns** — A [Line2D](Line2D.md) that contains every point on this ray and extends backwards as well.

## `project_onto`

`project_onto(point: `[`Point2D`](Point2D.md)`) -> `[`Point2D`](Point2D.md)

Orthogonal projection of a point onto this ray, clamped to the origin.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to project.

**Returns** — Closest point on the ray to point . Returns the ray's origin if point projects behind it.

## `distance_to`

`distance_to(point: `[`Point2D`](Point2D.md)`) -> float`

Distance from a point to this ray.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to measure distance to.

**Returns** — Perpendicular distance to the ray's domain, or distance to the origin if point is behind the ray.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies on this ray.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is collinear with the ray's direction AND ahead of (or at) the origin.

## `intersects`

`intersects(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`intersects(other: Ray2D) -> bool`


**Parameters**

- `other` (`Ray2D`)

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md))

`intersects(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `intersection`

`intersection(line: `[`Line2D`](Line2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`intersection(other: Ray2D) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `other` (`Ray2D`)

`intersection(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md))

`intersection(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `overlaps`

`overlaps(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`overlaps(ray: Ray2D) -> bool`


**Parameters**

- `ray` (`Ray2D`)

`overlaps(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`overlaps(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `overlap`

`overlap(line: `[`Line2D`](Line2D.md)`) -> Ray2D | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`overlap(ray: Ray2D) -> std::variant< Ray2D , `[`LineSegment2D`](LineSegment2D.md)` > | None`


**Parameters**

- `ray` (`Ray2D`)

`overlap(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`LineSegment2D`](LineSegment2D.md)` | None`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`overlap(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `touches`

`touches(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`touches(ray: Ray2D) -> bool`


**Parameters**

- `ray` (`Ray2D`)

`touches(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`touches(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `touch`

`touch(line: `[`Line2D`](Line2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`touch(ray: Ray2D) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `ray` (`Ray2D`)

`touch(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`touch(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Vector2D](Vector2D.md)
