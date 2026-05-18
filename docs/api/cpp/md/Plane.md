# `Plane` (C++)

## `From3Points`

**static** `Plane From3Points(`[`Point3D`](Point3D.md)` p1, `[`Point3D`](Point3D.md)` p2, `[`Point3D`](Point3D.md)` p3)`


**Parameters**

- `p1` ([`Point3D`](Point3D.md))
- `p2` ([`Point3D`](Point3D.md))
- `p3` ([`Point3D`](Point3D.md))

## `FromOriginAndAxes`

**static** `Plane FromOriginAndAxes(`[`Point3D`](Point3D.md)` origin, `[`Vector3D`](Vector3D.md)` u, `[`Vector3D`](Vector3D.md)` v)`


**Parameters**

- `origin` ([`Point3D`](Point3D.md))
- `u` ([`Vector3D`](Vector3D.md))
- `v` ([`Vector3D`](Vector3D.md))

## `FromOriginAndNormal`

**static** `Plane FromOriginAndNormal(`[`Point3D`](Point3D.md)` origin, `[`Vector3D`](Vector3D.md)` normal)`


**Parameters**

- `origin` ([`Point3D`](Point3D.md))
- `normal` ([`Vector3D`](Vector3D.md))

## `XY`

**static** `Plane XY()`


## `YZ`

**static** `Plane YZ()`


## `ZX`

**static** `Plane ZX()`


## `origin`

[`Point3D`](Point3D.md)` const origin() const`


## `normal`

[`Vector3D`](Vector3D.md)` const normal() const`


## `axis_u`

[`Vector3D`](Vector3D.md)` const axis_u() const`


## `axis_v`

[`Vector3D`](Vector3D.md)` const axis_v() const`


## `AlmostEquals`

`bool AlmostEquals(Plane const & other, double epsilon) const`


**Parameters**

- `other` (`Plane const &`)
- `epsilon` (`double`)

## `SignedDistanceTo`

`double SignedDistanceTo(`[`Point3D`](Point3D.md)` const & p) const`

Signed perpendicular distance from a point to the plane.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &) — The point.

**Returns** — (p - origin) · normal. Positive on the normal's side, negative on the other.

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)` const & p) const`

Unsigned perpendicular distance from a point to the plane.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &) — The point.

**Returns** — Absolute value of SignedDistanceTo .

## `ProjectOnto`

[`Point3D`](Point3D.md)` ProjectOnto(`[`Point3D`](Point3D.md)` const & p) const`

Orthogonal projection of a point onto this plane (3D position).

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &) — The point to project.

**Returns** — Closest point on the plane to p , expressed in world (3D) coordinates.

## `ProjectInto`

[`Point2D`](Point2D.md)` ProjectInto(`[`Point3D`](Point3D.md)` const & p) const`

Orthogonal projection of a point into this plane's local 2D basis (axis_u, axis_v).

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &) — The point to project.

**Returns** — 2D coordinates (u, v) such that p ≈ origin + u·axis_u + v·axis_v + (distance · normal).

## `Evaluate`

[`Point3D`](Point3D.md)` Evaluate(`[`Point2D`](Point2D.md)` const & p) const`

Inverse of ProjectInto — lift a local 2D point onto the plane in world coordinates.

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &) — The local (u, v) point in the plane's basis.

**Returns** — World-space 3D point: origin + p.x · axis_u + p.y · axis_v.

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & point) const`

Tests whether a point lies on the plane.

**Parameters**

- `point` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if point's signed distance to the plane is zero within decimal precision.

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)` const & line) const`

Tests whether a line meets this plane.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — true if the line crosses the plane or lies in it.

`bool Intersects(`[`Ray3D`](Ray3D.md)` const & ray) const`

Tests whether a ray meets this plane within its domain.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — true if the ray crosses the plane ahead of its origin, or lies in it.

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Tests whether a segment crosses or touches this plane.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — true if some part of the segment is on the plane.

`bool Intersects(Plane const & plane) const`

Tests whether another plane meets this one.

**Parameters**

- `plane` (`Plane const &`) — The other plane.

**Returns** — true unless the two planes are parallel and distinct.

`bool Intersects(`[`Triangle3D`](Triangle3D.md)` const & triangle) const`

Tests whether a triangle meets this plane.

**Parameters**

- `triangle` ([`Triangle3D`](Triangle3D.md) const &) — The triangle.

**Returns** — true if the plane cuts through the triangle or touches it.

## `Intersection`

`ReturnSet Intersection(`[`Line3D`](Line3D.md)` const & line) const`

Intersection of this plane with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — A [Point3D](Point3D.md) (one crossing), a [Line3D](Line3D.md) (line lies in the plane), or std::nullopt for parallel-distinct.

`ReturnSet Intersection(`[`Ray3D`](Ray3D.md)` const & ray) const`

Intersection of this plane with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — [Point3D](Point3D.md) , Ray-as-[Line3D](Line3D.md) when the ray lies in the plane, or std::nullopt if the ray points away.

`ReturnSet Intersection(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Intersection of this plane with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — [Point3D](Point3D.md) crossing, [LineSegment3D](LineSegment3D.md) (the segment lies in the plane), or std::nullopt if disjoint.

`ReturnSet Intersection(Plane const & plane) const`

Intersection of two planes.

**Parameters**

- `plane` (`Plane const &`) — The other plane.

**Returns** — The shared line as [Line3D](Line3D.md) , or std::nullopt for parallel-distinct planes.

`ReturnSet Intersection(`[`Triangle3D`](Triangle3D.md)` const & triangle) const`

Intersection of this plane with a triangle.

**Parameters**

- `triangle` ([`Triangle3D`](Triangle3D.md) const &) — The triangle.

**Returns** — A [Point3D](Point3D.md) (touches a vertex), a [LineSegment3D](LineSegment3D.md) (cuts through interior), or std::nullopt if disjoint.

## `IsParallel`

`bool IsParallel(`[`Line3D`](Line3D.md)` const & line) const`

Tests whether a line is parallel to this plane (no convergence).

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — true if line.direction · plane.normal == 0. Lines lying in the plane also return true.

`bool IsParallel(`[`Ray3D`](Ray3D.md)` const & ray) const`

Tests whether a ray is parallel to this plane.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — true if ray.direction · plane.normal == 0.

`bool IsParallel(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Tests whether a segment is parallel to this plane.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — true if the segment's direction is perpendicular to the plane's normal.

## `IsCoplanar`

`bool IsCoplanar(`[`Line3D`](Line3D.md)` const & line) const`

Tests whether a line lies entirely in this plane.

**Parameters**

- `line` ([`Line3D`](Line3D.md) const &) — The line.

**Returns** — true if the line is parallel to the plane AND any point of it is on the plane.

`bool IsCoplanar(`[`Ray3D`](Ray3D.md)` const & ray) const`

Tests whether a ray lies entirely in this plane.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md) const &) — The ray.

**Returns** — true if the ray is parallel to the plane AND its origin lies on the plane.

`bool IsCoplanar(`[`LineSegment3D`](LineSegment3D.md)` const & segment) const`

Tests whether a segment lies entirely in this plane.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md) const &) — The segment.

**Returns** — true if both segment endpoints lie on the plane.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point2D](Point2D.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [Triangle3D](Triangle3D.md), [Vector3D](Vector3D.md)
