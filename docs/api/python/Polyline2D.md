# `Polyline2D` (Python)

## `make`

**static** `make(points: list[`[`Point2D`](Point2D.md)`]) -> Polyline2D`


**Parameters**

- `points` (list[`[Point2D`](Point2D.md)])

## `from_wkt`

**static** `from_wkt(wkt: str) -> Polyline2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Polyline2D`


**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `almost_equals`

`almost_equals(other: Polyline2D, epsilon: float) -> bool`


**Parameters**

- `other` (`Polyline2D`)
- `epsilon` (`float`)

## `to_segments`

`to_segments() -> `[`SegmentRange2D`](SegmentRange2D.md)


## `length`

`length() -> float`


## `is_simple`

`is_simple() -> bool`


## `convex_hull`

`convex_hull() -> `[`Polygon2D`](Polygon2D.md)

Convex Hull via the Melkman's algorithm.

It requires IsSimple() to be true in order to make sense

**Returns** — The convex hull polygon of the polyline, even if the polyline is not simple (it will be a wrong hull in that case). So please check IsSimple() before running this.

## `reduce`

`reduce(settings: PolylineDecimationParams) -> Polyline2D`

Reduces the polyline to one with fewer vertices.

**Parameters**

- `settings` (`PolylineDecimationParams`) — bundles the decimation strategy and its threshold — see PolylineDecimationParams and PolylineDecimationParams::Strategy for the per-strategy behavior and Big-O. Defaults to {RamerDouglasPeucker, 0.5} , so Reduce() with no arguments keeps working.

**Returns** — A copy of this polyline with fewer vertices.

## `expand`

`expand(settings: PolylineExpansionParams) -> Polyline2D`

Rounds every inner corner of the polyline with a quadratic Bezier arc — the inverse direction of Reduce() : this adds vertices rather than removing them.

Each corner is delegated to bezier_smoothing_2 independently (see PolylineExpansionParams for the per-corner controls); the true first/last knots are never smoothed. Corners whose sampled arc collapses to a single point (see PolylineExpansionParams::min_segment_length ) are deduplicated rather than emitted as repeated/zero-length segments.

**Parameters**

- `settings` (`PolylineExpansionParams`) — bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to {0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON} , so Expand() with no arguments works.

**Returns** — A copy of this polyline with rounded corners. Unchanged if it has fewer than 3 knots (no inner corner exists to round).

## `project_onto`

`project_onto(point: `[`Point2D`](Point2D.md)`) -> `[`Point2D`](Point2D.md)

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to project.

**Returns** — Point on the polyline at minimum distance from point .

## `distance_to`

`distance_to(point: `[`Point2D`](Point2D.md)`) -> float`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `location`

`location(point: `[`Point2D`](Point2D.md)`) -> float`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, or +infinity if point is off the polyline.

## `interpolate`

`interpolate(pct: float) -> `[`Point2D`](Point2D.md)

Linear interpolation along the polyline by arc-length fraction.

**Parameters**

- `pct` (`float`) — Normalized position in [0, 1]. Values outside the range are clamped to the polyline's endpoints.

**Returns** — Point at the given arc-length fraction along the polyline.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is on any of the polyline's segments.

## `intersects`

`intersects(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md))

`intersects(other: Polyline2D) -> bool`


**Parameters**

- `other` (`Polyline2D`)

## `intersection`

`intersection(line: `[`Line2D`](Line2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`intersection(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md))

`intersection(other: Polyline2D) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `other` (`Polyline2D`)

## `overlaps`

`overlaps(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`overlaps(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`overlaps(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`overlaps(other: Polyline2D) -> bool`


**Parameters**

- `other` (`Polyline2D`)

## `overlap`

`overlap(line: `[`Line2D`](Line2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`overlap(ray: `[`Ray2D`](Ray2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`overlap(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`overlap(other: Polyline2D) -> std::vector< `[`LineSegment2D`](LineSegment2D.md)` > | None`


**Parameters**

- `other` (`Polyline2D`)

## `touches`

`touches(line: `[`Line2D`](Line2D.md)`) -> bool`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`touches(ray: `[`Ray2D`](Ray2D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`touches(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`touches(other: Polyline2D) -> bool`


**Parameters**

- `other` (`Polyline2D`)

## `touch`

`touch(line: `[`Line2D`](Line2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `line` ([`Line2D`](Line2D.md))

`touch(ray: `[`Ray2D`](Ray2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md))

`touch(seg: `[`LineSegment2D`](LineSegment2D.md)`) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md))

`touch(other: Polyline2D) -> std::vector< `[`Point2D`](Point2D.md)` > | None`


**Parameters**

- `other` (`Polyline2D`)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
