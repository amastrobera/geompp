# `Plane` (Python)

## `from3_points`

**static** `from3_points(p1: `[`Point3D`](Point3D.md)`, p2: `[`Point3D`](Point3D.md)`, p3: `[`Point3D`](Point3D.md)`) -> Plane`


**Parameters**

- `p1` ([`Point3D`](Point3D.md))
- `p2` ([`Point3D`](Point3D.md))
- `p3` ([`Point3D`](Point3D.md))

## `from_origin_and_axes`

**static** `from_origin_and_axes(origin: `[`Point3D`](Point3D.md)`, u: `[`Vector3D`](Vector3D.md)`, v: `[`Vector3D`](Vector3D.md)`) -> Plane`


**Parameters**

- `origin` ([`Point3D`](Point3D.md))
- `u` ([`Vector3D`](Vector3D.md))
- `v` ([`Vector3D`](Vector3D.md))

## `from_origin_and_normal`

**static** `from_origin_and_normal(origin: `[`Point3D`](Point3D.md)`, normal: `[`Vector3D`](Vector3D.md)`) -> Plane`


**Parameters**

- `origin` ([`Point3D`](Point3D.md))
- `normal` ([`Vector3D`](Vector3D.md))

## `xy`

**static** `xy() -> Plane`


## `yz`

**static** `yz() -> Plane`


## `zx`

**static** `zx() -> Plane`


## `origin`

`origin() -> `[`Point3D`](Point3D.md)


## `normal`

`normal() -> `[`Vector3D`](Vector3D.md)


## `axis_u`

`axis_u() -> `[`Vector3D`](Vector3D.md)


## `axis_v`

`axis_v() -> `[`Vector3D`](Vector3D.md)


## `almost_equals`

`almost_equals(other: Plane, epsilon: float) -> bool`


**Parameters**

- `other` (`Plane`)
- `epsilon` (`float`)

## `signed_distance_to`

`signed_distance_to(p: `[`Point3D`](Point3D.md)`) -> float`

Signed perpendicular distance from a point to the plane.

**Parameters**

- `p` ([`Point3D`](Point3D.md)) — The point.

**Returns** — (p - origin) · normal. Positive on the normal's side, negative on the other.

## `distance_to`

`distance_to(p: `[`Point3D`](Point3D.md)`) -> float`

Unsigned perpendicular distance from a point to the plane.

**Parameters**

- `p` ([`Point3D`](Point3D.md)) — The point.

**Returns** — Absolute value of SignedDistanceTo .

## `project_onto`

`project_onto(p: `[`Point3D`](Point3D.md)`) -> `[`Point3D`](Point3D.md)

Orthogonal projection of a point onto this plane (3D position).

**Parameters**

- `p` ([`Point3D`](Point3D.md)) — The point to project.

**Returns** — Closest point on the plane to p , expressed in world (3D) coordinates.

## `project_into`

`project_into(p: `[`Point3D`](Point3D.md)`) -> `[`Point2D`](Point2D.md)

Orthogonal projection of a point into this plane's local 2D basis (axis_u, axis_v).

**Parameters**

- `p` ([`Point3D`](Point3D.md)) — The point to project.

**Returns** — 2D coordinates (u, v) such that p ≈ origin + u·axis_u + v·axis_v + (distance · normal).

## `evaluate`

`evaluate(p: `[`Point2D`](Point2D.md)`) -> `[`Point3D`](Point3D.md)

Inverse of ProjectInto — lift a local 2D point onto the plane in world coordinates.

**Parameters**

- `p` ([`Point2D`](Point2D.md)) — The local (u, v) point in the plane's basis.

**Returns** — World-space 3D point: origin + p.x · axis_u + p.y · axis_v.

## `contains`

`contains(point: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies on the plane.

**Parameters**

- `point` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if point's signed distance to the plane is zero within decimal precision.

## `intersects`

`intersects(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether a line meets this plane.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if the line crosses the plane or lies in it.

`intersects(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether a ray meets this plane within its domain.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if the ray crosses the plane ahead of its origin, or lies in it.

`intersects(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether a segment crosses or touches this plane.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if some part of the segment is on the plane.

`intersects(plane: Plane) -> bool`

Tests whether another plane meets this one.

**Parameters**

- `plane` (`Plane`) — The other plane.

**Returns** — true unless the two planes are parallel and distinct.

`intersects(triangle: `[`Triangle3D`](Triangle3D.md)`) -> bool`

Tests whether a triangle meets this plane.

**Parameters**

- `triangle` ([`Triangle3D`](Triangle3D.md)) — The triangle.

**Returns** — true if the plane cuts through the triangle or touches it.

## `intersection`

`intersection(line: `[`Line3D`](Line3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this plane with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — A [Point3D](Point3D.md) (one crossing), a [Line3D](Line3D.md) (line lies in the plane), or std::nullopt for parallel-distinct.

`intersection(ray: `[`Ray3D`](Ray3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this plane with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — [Point3D](Point3D.md) , Ray-as-[Line3D](Line3D.md) when the ray lies in the plane, or std::nullopt if the ray points away.

`intersection(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this plane with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — [Point3D](Point3D.md) crossing, [LineSegment3D](LineSegment3D.md) (the segment lies in the plane), or std::nullopt if disjoint.

`intersection(plane: Plane) -> `[`Point3D`](Point3D.md)` | None`

Intersection of two planes.

**Parameters**

- `plane` (`Plane`) — The other plane.

**Returns** — The shared line as [Line3D](Line3D.md) , or std::nullopt for parallel-distinct planes.

`intersection(triangle: `[`Triangle3D`](Triangle3D.md)`) -> `[`Point3D`](Point3D.md)` | None`

Intersection of this plane with a triangle.

**Parameters**

- `triangle` ([`Triangle3D`](Triangle3D.md)) — The triangle.

**Returns** — A [Point3D](Point3D.md) (touches a vertex), a [LineSegment3D](LineSegment3D.md) (cuts through interior), or std::nullopt if disjoint.

## `is_parallel`

`is_parallel(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether a line is parallel to this plane (no convergence).

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if line.direction · plane.normal == 0. Lines lying in the plane also return true.

`is_parallel(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether a ray is parallel to this plane.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if ray.direction · plane.normal == 0.

`is_parallel(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether a segment is parallel to this plane.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if the segment's direction is perpendicular to the plane's normal.

## `is_coplanar`

`is_coplanar(line: `[`Line3D`](Line3D.md)`) -> bool`

Tests whether a line lies entirely in this plane.

**Parameters**

- `line` ([`Line3D`](Line3D.md)) — The line.

**Returns** — true if the line is parallel to the plane AND any point of it is on the plane.

`is_coplanar(ray: `[`Ray3D`](Ray3D.md)`) -> bool`

Tests whether a ray lies entirely in this plane.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)) — The ray.

**Returns** — true if the ray is parallel to the plane AND its origin lies on the plane.

`is_coplanar(segment: `[`LineSegment3D`](LineSegment3D.md)`) -> bool`

Tests whether a segment lies entirely in this plane.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)) — The segment.

**Returns** — true if both segment endpoints lie on the plane.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [Triangle3D](Triangle3D.md), [Vector3D](Vector3D.md)
