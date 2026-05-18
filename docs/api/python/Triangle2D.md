# `Triangle2D` (Python)

## `make`

**static** `make(p0: `[`Point2D`](Point2D.md)`, p1: `[`Point2D`](Point2D.md)`, p2: `[`Point2D`](Point2D.md)`) -> Triangle2D`


**Parameters**

- `p0` ([`Point2D`](Point2D.md))
- `p1` ([`Point2D`](Point2D.md))
- `p2` ([`Point2D`](Point2D.md))

## `from_wkt`

**static** `from_wkt(wkt: str) -> Triangle2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Triangle2D`


**Parameters**

- `path` (`str`)

## `vertices`

`vertices() -> std::tuple< `[`Point2D`](Point2D.md)` , `[`Point2D`](Point2D.md)` , `[`Point2D`](Point2D.md)` >`


## `almost_equals`

`almost_equals(other: Triangle2D, epsilon: float) -> bool`


**Parameters**

- `other` (`Triangle2D`)
- `epsilon` (`float`)

## `centroid`

`centroid() -> `[`Point2D`](Point2D.md)


## `to_polygon`

`to_polygon() -> `[`Polygon2D`](Polygon2D.md)


## `signed_area`

`signed_area() -> float`


## `area`

`area() -> float`


## `perimeter`

`perimeter() -> float`


## `is_ccw`

`is_ccw() -> bool`


## `distance_to`

`distance_to(point: `[`Point2D`](Point2D.md)`) -> float`

Distance from a point to this triangle (interior or boundary).

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to measure distance to.

**Returns** — 0 if point is inside the triangle; otherwise the distance to the closest edge or vertex.

## `to_axis`

`to_axis() -> std::tuple< `[`Vector2D`](Vector2D.md)` , `[`Vector2D`](Vector2D.md)` >`

Local 2D basis spanning the triangle.

**Returns** — Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.

## `interpolate`

`interpolate(s: float, t: float) -> `[`Point2D`](Point2D.md)` | None`

Interpolates a point in the triangle from barycentric-like coordinates (s, t) along U and V.

**Parameters**

- `s` (`float`) — Scalar along the U axis (= P1 - P0).
- `t` (`float`) — Scalar along the V axis (= P2 - P0).

**Returns** — P0 + s·U + t·V if (s, t, s+t) all lie in [0, 1]; otherwise std::nullopt.

## `location`

`location(point: `[`Point2D`](Point2D.md)`) -> std::tuple< double, double > | None`

Inverse of Interpolate — locates a point in the (s, t) basis of ToAxis .

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to locate.

**Returns** — The (s, t) pair if point is inside the triangle; otherwise std::nullopt.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies inside the triangle (interior, edge, or vertex).

**Parameters**

- `point` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if point is in the triangle's closed region.

## `intersects`

`intersects(line: `[`Line2D`](Line2D.md)`) -> bool`

Tests whether this triangle intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — true if the line crosses the triangle's closed region.

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`

Tests whether this triangle intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — true if the ray hits the triangle within its own domain.

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`

Tests whether this triangle intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — true if any part of the segment lies inside the triangle.

`intersects(other: Triangle2D) -> bool`

Tests whether this triangle intersects another triangle.

**Parameters**

- `other` (`Triangle2D`) — The other triangle.

**Returns** — true if the two share any point.

## `intersection`

`intersection(line: `[`Line2D`](Line2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this triangle with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)) — The line.

**Returns** — A [Point2D](Point2D.md) (line touches a vertex), a [LineSegment2D](LineSegment2D.md) (line cuts through interior), or std::nullopt if disjoint.

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this triangle with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — [Point2D](Point2D.md) / [LineSegment2D](LineSegment2D.md) depending on geometry, or std::nullopt if disjoint.

`intersection(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this triangle with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — [Point2D](Point2D.md) / [LineSegment2D](LineSegment2D.md) depending on geometry, or std::nullopt if disjoint.

`intersection(other: Triangle2D) -> `[`Point3D`](Point3D.md)` | None`

Intersection of two triangles.

**Parameters**

- `other` (`Triangle2D`) — The other triangle.

**Returns** — A [Point2D](Point2D.md) , [LineSegment2D](LineSegment2D.md) , Triangle2D , or [Polygon2D](Polygon2D.md) depending on overlap, or std::nullopt if disjoint.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Ray2D](Ray2D.md), [Vector2D](Vector2D.md)
