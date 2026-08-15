# `Polyline3D` (Python)

## `make`

**static** `make(points: list[`[`Point3D`](Point3D.md)`]) -> Polyline3D`


**Parameters**

- `points` (list[`[Point3D`](Point3D.md)])

## `from_wkt`

**static** `from_wkt(wkt: str) -> Polyline3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Polyline3D`


**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `almost_equals`

`almost_equals(other: Polyline3D, epsilon: float) -> bool`


**Parameters**

- `other` (`Polyline3D`)
- `epsilon` (`float`)

## `to_segments`

`to_segments() -> `[`SegmentRange3D`](SegmentRange3D.md)


## `length`

`length() -> float`


## `project_onto`

`project_onto(point: `[`Point3D`](Point3D.md)`) -> `[`Point3D`](Point3D.md)

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to project.

**Returns** — Point on the polyline at minimum distance from point ; falls on one of the segments' interiors or knots.

## `distance_to`

`distance_to(point: `[`Point3D`](Point3D.md)`) -> float`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `location`

`location(point: `[`Point3D`](Point3D.md)`) -> float`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, fractional values in between, or +infinity if point is off the polyline.

## `interpolate`

`interpolate(pct: float) -> `[`Point3D`](Point3D.md)

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

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if point is on any of the polyline's segments (interior or knot).

## `intersects`

`intersects(line: `[`Line3D`](Line3D.md)`) -> bool`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md))

`intersects(other: Polyline3D) -> bool`


**Parameters**

- `other` (`Polyline3D`)

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`intersection(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md))

`intersection(other: Polyline3D) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `other` (`Polyline3D`)

## `overlaps`

`overlaps(line: `[`Line3D`](Line3D.md)`) -> bool`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`overlaps(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`overlaps(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`overlaps(other: Polyline3D) -> bool`


**Parameters**

- `other` (`Polyline3D`)

## `overlap`

`overlap(line: `[`Line3D`](Line3D.md)`) -> std::vector< `[`LineSegment3D`](LineSegment3D.md)` > | None`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`overlap(ray: `[`Ray3D`](Ray3D.md)`) -> std::vector< `[`LineSegment3D`](LineSegment3D.md)` > | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`overlap(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> std::vector< `[`LineSegment3D`](LineSegment3D.md)` > | None`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`overlap(other: Polyline3D) -> std::vector< `[`LineSegment3D`](LineSegment3D.md)` > | None`


**Parameters**

- `other` (`Polyline3D`)

## `touches`

`touches(line: `[`Line3D`](Line3D.md)`) -> bool`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`touches(ray: `[`Ray3D`](Ray3D.md)`) -> bool`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`touches(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`touches(other: Polyline3D) -> bool`


**Parameters**

- `other` (`Polyline3D`)

## `touch`

`touch(line: `[`Line3D`](Line3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `line` ([`Line3D`](Line3D.md))

`touch(ray: `[`Ray3D`](Ray3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md))

`touch(seg: `[`LineSegment3D`](LineSegment3D.md)`) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `seg` ([`LineSegment3D`](LineSegment3D.md))

`touch(other: Polyline3D) -> std::vector< `[`Point3D`](Point3D.md)` > | None`


**Parameters**

- `other` (`Polyline3D`)

## `is_planar`

`is_planar() -> bool`

Tests whether all knots of the polyline are coplanar.

**Returns** — true if all knots lie in a common plane.

## `is_simple`

`is_simple() -> bool`

Tests whether the polyline has no self-intersections (when projected onto its best-fit plane).

**Returns** — true if the polyline does not self-intersect.

## `is_convex`

`is_convex() -> bool`

Tests whether the polyline is a convex polygon boundary.

**Returns** — true if the polyline is planar, simple, and all turns go in the same direction.

## `convex_hull`

`convex_hull() -> Polyline3D`

Computes the convex hull of the polyline's knots.

**Returns** — A new Polyline3D containing the convex hull vertices in CCW order.

## `reduce`

`reduce(settings: `[`PolylineDecimationParams`](PolylineDecimationParams.md)`) -> Polyline3D`

Reduces the polyline to one with fewer vertices.

**Parameters**

- `settings` ([`PolylineDecimationParams`](PolylineDecimationParams.md)) — bundles the decimation strategy and its threshold — see [PolylineDecimationParams](PolylineDecimationParams.md) and [PolylineDecimationParams](PolylineDecimationParams.md)::Strategy for the per-strategy behavior and Big-O. Defaults to {RamerDouglasPeucker, 0.5} , so Reduce() with no arguments keeps working.

**Returns** — A copy of this polyline with fewer vertices.

## `expand`

`expand(settings: `[`PolylineExpansionParams`](PolylineExpansionParams.md)`) -> Polyline3D`

Rounds every inner corner of the polyline with a quadratic Bezier arc — the inverse direction of Reduce() : this adds vertices rather than removing them.

Each corner is delegated to bezier_smoothing_2 independently (see [PolylineExpansionParams](PolylineExpansionParams.md) for the per-corner controls); the true first/last knots are never smoothed. Corners whose sampled arc collapses to a single point (see [PolylineExpansionParams](PolylineExpansionParams.md)::min_segment_length ) are deduplicated rather than emitted as repeated/zero-length segments.

**Parameters**

- `settings` ([`PolylineExpansionParams`](PolylineExpansionParams.md)) — bundles smoothness, sampling density, and the tiny-corner skip threshold. Defaults to {0.5, FixedSegments, 4, 0.1, DOUBLE_EPSILON} , so Expand() with no arguments works.

**Returns** — A copy of this polyline with rounded corners. Unchanged if it has fewer than 3 knots (no inner corner exists to round).

## `to_polygon`

`to_polygon() -> `[`Polygon3D`](Polygon3D.md)

Converts this polyline to a [Polygon3D](Polygon3D.md) .

**Returns** — A [Polygon3D](Polygon3D.md) with the same vertices.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [PolylineDecimationParams](PolylineDecimationParams.md), [PolylineExpansionParams](PolylineExpansionParams.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
