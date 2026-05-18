# `Triangle3D` (Python)

## `make`

**static** `make(p0: `[`Point3D`](Point3D.md)`, p1: `[`Point3D`](Point3D.md)`, p2: `[`Point3D`](Point3D.md)`) -> Triangle3D`


**Parameters**

- `p0` ([`Point3D`](Point3D.md))
- `p1` ([`Point3D`](Point3D.md))
- `p2` ([`Point3D`](Point3D.md))

## `from_wkt`

**static** `from_wkt(wkt: str) -> Triangle3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Triangle3D`


**Parameters**

- `path` (`str`)

## `vertices`

`vertices() -> std::tuple< `[`Point3D`](Point3D.md)` , `[`Point3D`](Point3D.md)` , `[`Point3D`](Point3D.md)` >`


## `almost_equals`

`almost_equals(other: Triangle3D, epsilon: float) -> bool`


**Parameters**

- `other` (`Triangle3D`)
- `epsilon` (`float`)

## `centroid`

`centroid() -> `[`Point3D`](Point3D.md)


## `to_polygon`

`to_polygon() -> `[`Polygon3D`](Polygon3D.md)


## `to_plane`

`to_plane() -> `[`Plane`](Plane.md)


## `area_vector`

`area_vector() -> `[`Vector3D`](Vector3D.md)


## `normal`

`normal() -> `[`Vector3D`](Vector3D.md)


## `signed_area`

`signed_area(ref_normal: `[`Vector3D`](Vector3D.md)`) -> float`


**Parameters**

- `ref_normal` ([`Vector3D`](Vector3D.md))

## `area`

`area() -> float`


## `perimeter`

`perimeter() -> float`


## `is_ccw`

`is_ccw(ref_normal: `[`Vector3D`](Vector3D.md)`) -> bool`


**Parameters**

- `ref_normal` ([`Vector3D`](Vector3D.md))

## `distance_to`

`distance_to(point: `[`Point3D`](Point3D.md)`) -> float`

Distance from a point to this triangle (interior or boundary).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to measure distance to.

**Returns** — 0 if point is inside the triangle's closed region; otherwise the distance to the closest edge or vertex.

## `to_axis`

`to_axis() -> std::tuple< `[`Vector3D`](Vector3D.md)` , `[`Vector3D`](Vector3D.md)` >`

Local 2D basis spanning the triangle's plane.

**Returns** — Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.

## `interpolate`

`interpolate(s: float, t: float) -> `[`Point3D`](Point3D.md)` | None`

Interpolates a point in the triangle's plane from barycentric-like coordinates (s, t) along U and V.

**Parameters**

- `s` (`float`) — Scalar along the U axis (= P1 - P0).
- `t` (`float`) — Scalar along the V axis (= P2 - P0).

**Returns** — P0 + s·U + t·V if (s, t) describes a point inside the triangle (s, t, s+t in [0, 1]); otherwise std::nullopt.

## `location`

`location(point: `[`Point3D`](Point3D.md)`) -> std::tuple< double, double > | None`

Inverse of Interpolate — locates a point in the (s, t) basis of ToAxis .

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to locate. Must lie in the triangle's plane.

**Returns** — The (s, t) pair if point is inside the triangle; otherwise std::nullopt.

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `contains`

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies inside the triangle (interior, edge, or vertex).

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test. Must lie in the triangle's plane (within decimal precision).

**Returns** — true if point is in the triangle's closed region.

## `intersects`

`intersects(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether this triangle intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if the line crosses the triangle's closed region.

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether this triangle intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if the ray hits the triangle within its own domain (sc on the ray >= 0).

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether this triangle intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if any part of the segment lies inside the triangle.

`intersects(plane: `[`Plane`](Plane.md)`) -> bool`

Tests whether this triangle intersects a plane.

**Parameters**

- `plane` ([`Plane`](Plane.md)) — The plane.

**Returns** — true if the plane cuts through the triangle or touches it.

`intersects(other: Triangle3D) -> bool`

Tests whether this triangle intersects another triangle.

**Parameters**

- `other` (`Triangle3D`) — The other triangle.

**Returns** — true if the two triangles share any point.

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this triangle with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the triangle.

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this triangle with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — The crossing point if it lies within the ray's domain, or std::nullopt otherwise.

`intersection(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this triangle with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

`intersection(plane: `[`Plane`](Plane.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this triangle with a plane.

**Parameters**

- `plane` ([`Plane`](Plane.md)) — The plane.

**Returns** — A [Point3D](Point3D.md) (touches at a vertex), a [LineSegment3D](LineSegment3D.md) (cuts through interior), or std::nullopt if disjoint.

`intersection(other: Triangle3D) -> `[`Point3D`](Point3D.md)` | None`

Intersection of two coplanar or skew triangles.

**Parameters**

- `other` (`Triangle3D`) — The other triangle.

**Returns** — A [Point3D](Point3D.md) , [LineSegment3D](LineSegment3D.md) , or std::nullopt depending on how the two triangles meet.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [Ray3D](Ray3D.md), [Vector3D](Vector3D.md)
