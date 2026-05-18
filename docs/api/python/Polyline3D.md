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

Tests whether this polyline intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if the line crosses any of the polyline's segments.

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether this polyline intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if the ray crosses any of the polyline's segments within its domain.

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether this polyline intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if the segment crosses any of the polyline's segments.

`intersects(other: Polyline3D) -> bool`

Tests whether two polylines intersect.

**Parameters**

- `other` (`Polyline3D`) — The other polyline.

**Returns** — true if any segment of either polyline crosses any segment of the other.

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polyline with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — A single [Point3D](Point3D.md) when there's one crossing, a list when there are several, or std::nullopt if disjoint.

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polyline with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — A single [Point3D](Point3D.md) or a list of crossings, or std::nullopt if disjoint.

`intersection(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polyline with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — A single [Point3D](Point3D.md) or a list of crossings, or std::nullopt if disjoint.

`intersection(other: Polyline3D) -> `[`Point3D`](Point3D.md)` | None`

Intersection of two polylines.

**Parameters**

- `other` (`Polyline3D`) — The other polyline.

**Returns** — A single [Point3D](Point3D.md) or a list of crossings, or std::nullopt if disjoint.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
