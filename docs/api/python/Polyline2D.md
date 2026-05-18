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

Tests whether this polyline intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — true if the line crosses any of the polyline's segments.

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`

Tests whether this polyline intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — true if the ray crosses any of the polyline's segments.

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`

Tests whether this polyline intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — true if the segment crosses any of the polyline's segments.

`intersects(other: Polyline2D) -> bool`

Tests whether two polylines intersect.

**Parameters**

- `other` (`Polyline2D`) — The other polyline.

**Returns** — true if any segment of either polyline crosses any segment of the other.

## `intersection`

`intersection(line: `[`Line2D`](Line2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polyline with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — A single [Point2D](Point2D.md) when there's one crossing, a list when there are several, or std::nullopt if disjoint.

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polyline with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.

`intersection(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polyline with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.

`intersection(other: Polyline2D) -> `[`Point3D`](Point3D.md)` | None`

Intersection of two polylines.

**Parameters**

- `other` (`Polyline2D`) — The other polyline.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
