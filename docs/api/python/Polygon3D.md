# `Polygon3D` (Python)

## `make`

**static** `make(points: list[`[`Point3D`](Point3D.md)`]) -> Polygon3D`


**Parameters**

- `points` (list[`[Point3D`](Point3D.md)])

**static** `make(points: list[`[`Point3D`](Point3D.md)`], holes: list[list[`[`Point3D`](Point3D.md)`]]) -> Polygon3D`


**Parameters**

- `points` (list[`[Point3D`](Point3D.md)])
- `holes` (list[`list[[Point3D`](Point3D.md)]])

## `from_wkt`

**static** `from_wkt(wkt: str) -> Polygon3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Polygon3D`


**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `get_plane`

`get_plane() -> `[`Plane`](Plane.md)


## `almost_equals`

`almost_equals(other: Polygon3D, epsilon: float) -> bool`


**Parameters**

- `other` (`Polygon3D`)
- `epsilon` (`float`)

## `to_segments`

`to_segments() -> `[`SegmentRange3D`](SegmentRange3D.md)


## `centroid`

`centroid() -> `[`Point3D`](Point3D.md)


## `area`

`area() -> float`


## `perimeter`

`perimeter() -> float`


## `distance_to`

`distance_to(point: `[`Point3D`](Point3D.md)`) -> float`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test. Must lie in the polygon's plane.

**Returns** — true if point is in the polygon's closed region (interior or boundary).

## `is_on_perimeter`

`is_on_perimeter(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on the polygon's boundary (any of its edges).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `intersects`

`intersects(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the polygon.

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

`intersection(other: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
