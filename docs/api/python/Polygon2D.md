# `Polygon2D` (Python)

## `make`

**static** `make(points: list[`[`Point2D`](Point2D.md)`]) -> Polygon2D`


**Parameters**

- `points` (list[`[Point2D`](Point2D.md)])

**static** `make(points: list[`[`Point2D`](Point2D.md)`], holes: list[list[`[`Point2D`](Point2D.md)`]]) -> Polygon2D`


**Parameters**

- `points` (list[`[Point2D`](Point2D.md)])
- `holes` (list[`list[[Point2D`](Point2D.md)]])

## `from_wkt`

**static** `from_wkt(wkt: str) -> Polygon2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Polygon2D`


**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `almost_equals`

`almost_equals(other: Polygon2D, epsilon: float) -> bool`


**Parameters**

- `other` (`Polygon2D`)
- `epsilon` (`float`)

## `to_segments`

`to_segments() -> `[`SegmentRange2D`](SegmentRange2D.md)


## `centroid`

`centroid() -> `[`Point2D`](Point2D.md)


## `area`

`area() -> float`


## `perimeter`

`perimeter() -> float`


## `distance_to`

`distance_to(point: `[`Point2D`](Point2D.md)`) -> float`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is in the polygon's closed region.

## `is_on_perimeter`

`is_on_perimeter(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies on the polygon's boundary.

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `intersects`

`intersects(line: `[`Line2D`](Line2D.md)`) -> bool`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

## `intersection`

Returns `None` on miss, [`Point2D`](Point2D.md) for a single tangent touch, or `list[`[`LineSegment2D`](LineSegment2D.md)`]` for one or more chord segments. Convex polygons use the fast Cyrus-Beck / Liang-Barsky parametric clip; concave polygons use Jordan-curve parity.

`intersection(line: `[`Line2D`](Line2D.md)`) -> `[`Point2D`](Point2D.md)` | list[`[`LineSegment2D`](LineSegment2D.md)`] | None`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — `None` on miss; `Point2D` for tangent touch; `list[LineSegment2D]` for chord(s).

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point2D`](Point2D.md)` | list[`[`LineSegment2D`](LineSegment2D.md)`] | None`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — `None` on miss; `list[LineSegment2D]` for chord(s) clipped to the ray domain (`t ≥ 0`). If the ray's origin is inside the polygon the returned segment starts there.

`intersection(other: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point2D`](Point2D.md)` | list[`[`LineSegment2D`](LineSegment2D.md)`] | None`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — `None` on miss; `list[LineSegment2D]` for chord(s) clipped to `[0, 1]` on the segment's parametric domain.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
