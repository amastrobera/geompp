# `LineSegment3D` (Python)

## `make`

**static** `make(p0: `[`Point3D`](Point3D.md)`, p1: `[`Point3D`](Point3D.md)`) -> LineSegment3D`


**Parameters**

- `p0` ([`Point3D`](Point3D.md))
- `p1` ([`Point3D`](Point3D.md))

## `from_wkt`

**static** `from_wkt(wkt: str) -> LineSegment3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> LineSegment3D`


**Parameters**

- `path` (`str`)

## `first`

`first() -> `[`Point3D`](Point3D.md)


## `last`

`last() -> `[`Point3D`](Point3D.md)


## `almost_equals`

`almost_equals(other: LineSegment3D, epsilon: float) -> bool`


**Parameters**

- `other` (`LineSegment3D`)
- `epsilon` (`float`)

## `to_line`

`to_line() -> `[`Line3D`](Line3D.md)


## `length`

`length() -> float`


## `project_onto`

`project_onto(point: `[`Point3D`](Point3D.md)`) -> `[`Point3D`](Point3D.md)

Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to project.

**Returns** — Closest point on the segment to point (one of the endpoints if point projects outside the segment).

## `distance_to`

`distance_to(point: `[`Point3D`](Point3D.md)`) -> float`

Distance from a point to this segment.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the segment, or distance to the nearest endpoint if point projects outside the segment.

`distance_to(other: `[`Line3D`](Line3D.md)`) -> float`

Scalar distance between this segment and a line.

**Parameters**

- `other` ([`Line3D`](Line3D.md)) — The other line.

**Returns** — 0 if the line crosses or contains the segment; otherwise the minimum distance.

`distance_to(ray: `[`Ray3D`](Ray3D.md)`) -> float`

Scalar distance between this segment and a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — 0 if they intersect or overlap; otherwise the minimum distance.

`distance_to(seg: LineSegment3D) -> float`

Scalar distance between two segments.

**Parameters**

- `seg` (`LineSegment3D`) — The other segment.

**Returns** — 0 if they intersect or share a region; otherwise the minimum distance.

## `location`

`location(point: `[`Point3D`](Point3D.md)`) -> float`

Position of a point along the segment, normalized to [0, 1].

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to locate. Must lie on the segment's underlying line.

**Returns** — 0 at First, 1 at Last, fractional values in between, or +infinity if point is not on the segment's line.

## `interpolate`

`interpolate(pct: float) -> `[`Point3D`](Point3D.md)

Linear interpolation along the segment.

**Parameters**

- `pct` (`float`) — Normalized position in [0, 1]. Values outside that range are clamped to the segment's endpoints.

**Returns** — Point at the given fraction along the segment.

## `flip`

`flip() -> LineSegment3D`

Returns a segment with endpoints swapped.

**Returns** — A new LineSegment3D from Last to First.

## `distance`

`distance(other: `[`Line3D`](Line3D.md)`) -> LineSegment3D | None`

Directed line-segment connecting this segment's closest point to another line.

**Parameters**

- `other` ([`Line3D`](Line3D.md)) — The other line.

**Returns** — Segment from this segment's closest point to other's closest point, or std::nullopt if they intersect or are collinear.

`distance(ray: `[`Ray3D`](Ray3D.md)`) -> LineSegment3D | None`

Directed line-segment connecting this segment's closest point to a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — Segment from this segment's closest point to the ray's closest point, or std::nullopt if they intersect or overlap.

`distance(seg: LineSegment3D) -> LineSegment3D | None`

Directed line-segment connecting this segment's closest point to another segment.

**Parameters**

- `seg` (`LineSegment3D`) — The other segment.

**Returns** — Segment from this segment's closest point to seg's closest point, or std::nullopt if they intersect or share a region.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on this segment (between or at the endpoints).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if point is collinear with the segment and falls within [First, Last].

## `intersects`

`intersects(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether this segment intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if the line crosses the segment's interior or an endpoint.

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether this segment intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if the segment is met within the ray's domain (sc on ray >= 0).

`intersects(segment: LineSegment3D) -> bool`

Tests whether this segment intersects another segment.

**Parameters**

- `segment` (`LineSegment3D`) — The other segment.

**Returns** — true if both segments' domains share the crossing point.

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this segment with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — The intersection point if it lies on the segment (sc in [0, 1]), or std::nullopt otherwise.

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this segment with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — The intersection point if it lies on both the segment and the ray, or std::nullopt otherwise.

`intersection(other: LineSegment3D) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of two segments.

**Parameters**

- `other` (`LineSegment3D`) — The other segment.

**Returns** — The intersection point if it lies on both segments, or std::nullopt otherwise.


---

**See also:** [Line3D](Line3D.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md)
