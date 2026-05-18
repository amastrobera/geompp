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

Tests whether this line intersects another line.

**Parameters**

- `other` (`Line2D`) — The other line.

**Returns** — true if they meet at a point; false for parallel non-collinear lines.

`intersects(ray: `[`Ray2D`](Ray2D.md)`) -> bool`

Tests whether this line intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — true if they meet within the ray's domain.

`intersects(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> bool`

Tests whether this line intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — true if the line crosses or touches the segment.

`intersects(t: `[`Triangle2D`](Triangle2D.md)`) -> bool`

Tests whether this line passes through a triangle.

**Parameters**

- `t` ([`Triangle2D`](Triangle2D.md)) — The triangle.

**Returns** — true if the line crosses the triangle's interior or an edge.

## `intersection`

`intersection(other: Line2D, sc: float, tc: float) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of two lines, also returning the parametric values along each line.

**Parameters**

- `other` (`Line2D`) — The other line.
- `sc` (`float`) — Output: parameter along this line at the intersection.
- `tc` (`float`) — Output: parameter along other at the intersection.

**Returns** — The intersection point, or std::nullopt for parallel lines.

`intersection(other: Line2D) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of two lines.

**Parameters**

- `other` (`Line2D`) — The other line.

**Returns** — The intersection point, or std::nullopt for parallel lines.

`intersection(ray: `[`Ray2D`](Ray2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this line with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)) — The ray.

**Returns** — The intersection point if it lies on the ray, or std::nullopt otherwise.

`intersection(segment: `[`LineSegment2D`](LineSegment2D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection point of this line with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)) — The segment.

**Returns** — The intersection point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md), [Vector2D](Vector2D.md)
