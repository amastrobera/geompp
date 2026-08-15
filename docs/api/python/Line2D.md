# `Line2D` (Python)

## `make`

**static** `make(p0: `[`Point2D`](Point2D.md)`, p1: `[`Point2D`](Point2D.md)`) -> Line2D`


**Parameters**

- `p0` ([`Point2D`](Point2D.md))
- `p1` ([`Point2D`](Point2D.md))

**static** `make(orig: `[`Point2D`](Point2D.md)`, dir: `[`Vector2D`](Vector2D.md)`) -> Line2D`


**Parameters**

- `orig` ([`Point2D`](Point2D.md))
- `dir` ([`Vector2D`](Vector2D.md))

## `from_wkt`

**static** `from_wkt(wkt: str) -> Line2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Line2D`


**Parameters**

- `path` (`str`)

## `first`

`first() -> `[`Point2D`](Point2D.md)


## `last`

`last() -> `[`Point2D`](Point2D.md)


## `origin`

`origin() -> `[`Point2D`](Point2D.md)


## `direction`

`direction() -> `[`Vector2D`](Vector2D.md)


## `almost_equals`

`almost_equals(other: Line2D, epsilon: float) -> bool`


**Parameters**

- `other` (`Line2D`)
- `epsilon` (`float`)

## `distance_to`

`distance_to(point: `[`Point2D`](Point2D.md)`) -> float`

Perpendicular distance from a point to this infinite line.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to measure distance to.

**Returns** — Length of the perpendicular from point onto this line.

## `project_onto`

`project_onto(point: `[`Point2D`](Point2D.md)`) -> `[`Point2D`](Point2D.md)

Orthogonal projection of a point onto this infinite line.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to project.

**Returns** — Closest point on this line to point .

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies on this line.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is collinear with the line within decimal precision.

## `intersects`

`intersects(other: Line2D) -> bool`


**Parameters**

- `other` (`Line2D`)

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md))

`intersects(t: `[`Triangle2D`](Triangle2D.md)`) -> bool`


**Parameters**

- `t` ([`Triangle2D`](Triangle2D.md))

`intersects(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `intersection`

`intersection(other: Line2D) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `other` (`Line2D`)

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`intersection(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md))

`intersection(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `overlaps`

`overlaps(line: Line2D) -> bool`


**Parameters**

- `line` (`Line2D`)

`overlaps(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`overlaps(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`overlaps(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `overlap`

`overlap(line: Line2D) -> Line2D | None`


**Parameters**

- `line` (`Line2D`)

`overlap(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Ray2D`](Ray2D.md)` | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`overlap(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`LineSegment2D`](LineSegment2D.md)` | None`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`overlap(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `touches`

`touches(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`touches(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`touches(polyline: `[`Polyline2D`](Polyline2D.md)`) -> bool`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))

## `touch`

`touch(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`touch(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point2D`](Point2D.md)` | None`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`touch(polyline: `[`Polyline2D`](Polyline2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md))


---

**See also:** [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md), [Vector2D](Vector2D.md)
