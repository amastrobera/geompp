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


## `reversed`

`reversed() -> LineSegment3D`


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


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`intersects(segment: LineSegment3D) -> bool`


**Parameters**

- `segment` (`LineSegment3D`)

`intersects(polyline: `[`Polyline3D`](Polyline3D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`intersection(other: LineSegment3D) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `other` (`LineSegment3D`)

`intersection(polyline: `[`Polyline3D`](Polyline3D.md)`) -> std::variant< `[`Point3D`](Point3D.md)` , std::vector< `[`Point3D`](Point3D.md)` > > | None`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `overlaps`

`overlaps(line: `[`Line3D`](Line3D.md)`) -> bool`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`overlaps(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`overlaps(seg: LineSegment3D) -> bool`


**Parameters**

- `seg` (`LineSegment3D`)

`overlaps(polyline: `[`Polyline3D`](Polyline3D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `overlap`

`overlap(line: `[`Line3D`](Line3D.md)`) -> LineSegment3D | None`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`overlap(ray: `[`Ray3D`](Ray3D.md)`) -> LineSegment3D | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`overlap(seg: LineSegment3D) -> LineSegment3D | None`


**Parameters**

- `seg` (`LineSegment3D`)

`overlap(polyline: `[`Polyline3D`](Polyline3D.md)`) -> std::vector< LineSegment3D > | None`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `touches`

`touches(line: `[`Line3D`](Line3D.md)`) -> bool`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`touches(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`touches(seg: LineSegment3D) -> bool`


**Parameters**

- `seg` (`LineSegment3D`)

`touches(polyline: `[`Polyline3D`](Polyline3D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))

## `touch`

`touch(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`touch(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`touch(seg: LineSegment3D) -> `[`Point3D`](Point3D.md)` | None`


**Parameters**

- `seg` (`LineSegment3D`)

`touch(polyline: `[`Polyline3D`](Polyline3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md))


---

**See also:** [Line3D](Line3D.md), [Point3D](Point3D.md), [Polyline3D](Polyline3D.md), [Ray3D](Ray3D.md)
