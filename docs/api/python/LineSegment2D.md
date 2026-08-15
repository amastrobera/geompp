# `LineSegment2D` (Python)

## `make`

**static** `make(p0: `[`Point2D`](Point2D.md)`, p1: `[`Point2D`](Point2D.md)`) -> LineSegment2D`


**Parameters**

- `p0` ([`Point2D`](Point2D.md))
- `p1` ([`Point2D`](Point2D.md))

## `from_wkt`

**static** `from_wkt(wkt: str) -> LineSegment2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> LineSegment2D`


**Parameters**

- `path` (`str`)

## `first`

`first() -> `[`Point2D`](Point2D.md)


## `last`

`last() -> `[`Point2D`](Point2D.md)


## `almost_equals`

`almost_equals(other: LineSegment2D, epsilon: float) -> bool`


**Parameters**

- `other` (`LineSegment2D`)
- `epsilon` (`float`)

## `to_line`

`to_line() -> `[`Line2D`](Line2D.md)


## `length`

`length() -> float`


## `reversed`

`reversed() -> LineSegment2D`


## `project_onto`

`project_onto(point: `[`Point2D`](Point2D.md)`) -> `[`Point2D`](Point2D.md)

Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to project.

**Returns** — Closest point on the segment to point (one of the endpoints if point projects outside).

## `distance_to`

`distance_to(point: `[`Point2D`](Point2D.md)`) -> float`

Distance from a point to this segment.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to measure distance to.

**Returns** — Perpendicular distance to the segment, or distance to the nearest endpoint if point projects outside.

## `location`

`location(point: `[`Point2D`](Point2D.md)`) -> float`

Position of a point along the segment, normalized to [0, 1].

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to locate. Must lie on the segment's underlying line.

**Returns** — 0 at First, 1 at Last, fractional values in between, or +infinity if not on the line.

## `interpolate`

`interpolate(pct: float) -> `[`Point2D`](Point2D.md)

Linear interpolation along the segment.

**Parameters**

- `pct` (`float`) — Normalized position in [0, 1]. Values outside are clamped to the segment's endpoints.

**Returns** — Point at the given fraction along the segment.

## `is_left`

`is_left(p: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point is on the left of the segment's direction (looking from First toward Last).

**Parameters**

- `p` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if the 2D cross product (P1 - P0) × (p - P0) is positive.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies on this segment.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is collinear with the segment and falls within [First, Last].

## `intersects`

`intersects(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`intersects(segment: LineSegment2D) -> bool`


**Parameters**

- `segment` (`LineSegment2D`)

`intersects(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `intersection`

`intersection(line: `[`Line2D`](Line2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`intersection(other: LineSegment2D) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `other` (`LineSegment2D`)

`intersection(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `overlaps`

`overlaps(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`overlaps(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`overlaps(seg: LineSegment2D) -> bool`


**Parameters**

- `seg` (`LineSegment2D`)

`overlaps(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `overlap`

`overlap(line: `[`Line2D`](Line2D.md)`) -> LineSegment2D | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`overlap(ray: `[`Ray2D`](Ray2D.md)`) -> LineSegment2D | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`overlap(seg: LineSegment2D) -> LineSegment2D | None`


**Parameters**

- `seg` (`LineSegment2D`)

`overlap(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< LineSegment2D > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `touches`

`touches(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`touches(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`touches(seg: LineSegment2D) -> bool`


**Parameters**

- `seg` (`LineSegment2D`)

`touches(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `touch`

`touch(line: `[`Line2D`](Line2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`touch(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`touch(seg: LineSegment2D) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `seg` (`LineSegment2D`)

`touch(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))


---

**See also:** [Line2D](Line2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md)
