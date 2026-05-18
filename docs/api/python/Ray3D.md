# `Ray3D` (Python)

## `make`

**static** `make(orig: `[`Point3D`](Point3D.md)`, dir: `[`Vector3D`](Vector3D.md)`) -> Ray3D`


**Parameters**

- `orig` ([`Point3D`](Point3D.md))
- `dir` ([`Vector3D`](Vector3D.md))

## `from_wkt`

**static** `from_wkt(wkt: str) -> Ray3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Ray3D`


**Parameters**

- `path` (`str`)

## `origin`

`origin() -> `[`Point3D`](Point3D.md)


## `direction`

`direction() -> `[`Vector3D`](Vector3D.md)


## `almost_equals`

`almost_equals(other: Ray3D, epsilon: float) -> bool`


**Parameters**

- `other` (`Ray3D`)
- `epsilon` (`float`)

## `is_ahead`

`is_ahead(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on the half-space ahead of (or at) the ray's origin along its direction.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if (point - origin) · direction >= 0.

## `is_behind`

`is_behind(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies strictly behind the ray's origin along its direction.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if (point - origin) · direction < 0.

## `to_line`

`to_line() -> `[`Line3D`](Line3D.md)

Promotes the ray to an infinite line through the same origin and direction.

**Returns** — A [Line3D](Line3D.md) that contains every point on this ray and extends backwards as well.

## `project_onto`

`project_onto(point: `[`Point3D`](Point3D.md)`) -> `[`Point3D`](Point3D.md)

Orthogonal projection of a point onto this ray, clamped to the origin.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to project.

**Returns** — Closest point on the ray to point . Returns the ray's origin if point projects behind it.

## `distance_to`

`distance_to(point: `[`Point3D`](Point3D.md)`) -> float`

Distance from a point to this ray.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to measure distance to.

**Returns** — Perpendicular distance to the ray's domain, or distance to the origin if point is behind the ray.

`distance_to(other: `[`Line3D`](Line3D.md)`) -> float`

Scalar distance between this ray and a line.

**Parameters**

- `other` ([`Line3D`](Line3D.md)) — The line.

**Returns** — 0 if they intersect or overlap; otherwise the minimum distance.

`distance_to(ray: Ray3D) -> float`

Scalar distance between two rays.

**Parameters**

- `ray` (`Ray3D`) — The other ray.

**Returns** — 0 if they intersect or share a region; otherwise the minimum distance.

`distance_to(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> float`

Scalar distance between this ray and a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — 0 if they intersect or share a region; otherwise the minimum distance.

## `distance`

`distance(other: `[`Line3D`](Line3D.md)`) -> `[`LineSegment3D`](LineSegment3D.md)` | None`

Directed line-segment connecting this ray's closest point to a line.

**Parameters**

- `other` ([`Line3D`](Line3D.md)) — The line.

**Returns** — Segment from the ray's closest point to the line's closest point, or std::nullopt if they intersect or overlap.

`distance(ray: Ray3D) -> `[`LineSegment3D`](LineSegment3D.md)` | None`

Directed line-segment connecting this ray's closest point to another ray.

**Parameters**

- `ray` (`Ray3D`) — The other ray.

**Returns** — Segment from this ray's closest point to ray's closest point, or std::nullopt if they intersect or share a region.

`distance(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`LineSegment3D`](LineSegment3D.md)` | None`

Directed line-segment connecting this ray's closest point to a segment.

**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — Segment from the ray's closest point to the segment's closest point, or std::nullopt if they intersect or share a region.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on this ray.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if point is collinear with the ray's direction AND ahead of (or at) the origin.

## `intersects`

`intersects(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether this ray intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if they meet at a point on the ray's domain (sc on the ray >= 0).

`intersects(other: Ray3D) -> bool`

Tests whether this ray intersects another ray.

**Parameters**

- `other` (`Ray3D`) — The other ray.

**Returns** — true if both rays' domains share the crossing point.

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether this ray intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if the ray hits the segment within both domains.

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this ray with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — The intersection point if it lies on the ray (sc >= 0), or std::nullopt otherwise.

`intersection(other: Ray3D) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of two rays.

**Parameters**

- `other` (`Ray3D`) — The other ray.

**Returns** — The intersection point if it lies on both rays' domains, or std::nullopt otherwise.

`intersection(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this ray with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — The intersection point if it lies on both the ray and the segment, or std::nullopt otherwise.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Vector3D](Vector3D.md)
