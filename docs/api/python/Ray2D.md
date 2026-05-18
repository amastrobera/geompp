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

Tests whether this ray intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — true if they meet at a point on the ray's domain.

`intersects(other: Ray2D) -> bool`

Tests whether this ray intersects another ray.

**Parameters**

- `other` (`Ray2D`) — The other ray.

**Returns** — true if both rays' domains share the crossing point.

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`

Tests whether this ray intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — true if the ray hits the segment within both domains.

## `intersection`

`intersection(line: `[`Line2D`](Line2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this ray with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — The intersection point if it lies on the ray, or std::nullopt otherwise.

`intersection(other: Ray2D) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of two rays.

**Parameters**

- `other` (`Ray2D`) — The other ray.

**Returns** — The intersection point if it lies on both rays' domains, or std::nullopt otherwise.

`intersection(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this ray with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — The intersection point if it lies on both the ray and the segment, or std::nullopt otherwise.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Vector2D](Vector2D.md)
