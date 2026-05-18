# `Triangle3D` (C# / .NET)

## `Make`

**static** `Triangle3D^ Make(`[`Point3D`](Point3D.md)`^ p0, `[`Point3D`](Point3D.md)`^ p1, `[`Point3D`](Point3D.md)`^ p2)`


**Parameters**

- `p0` ([`Point3D`](Point3D.md)^)
- `p1` ([`Point3D`](Point3D.md)^)
- `p2` ([`Point3D`](Point3D.md)^)

## `FromWkt`

**static** `Triangle3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Triangle3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Vertices`

`std::tuple< `[`Point3D`](Point3D.md)` , `[`Point3D`](Point3D.md)` , `[`Point3D`](Point3D.md)` > Vertices()`


## `AlmostEquals`

`bool AlmostEquals(Triangle3D^ other, double epsilon)`


**Parameters**

- `other` (`Triangle3D^`)
- `epsilon` (`double`)

## `Centroid`

[`Point3D`](Point3D.md)`^ Centroid()`


## `ToPolygon`

[`Polygon3D`](Polygon3D.md)`^ ToPolygon()`


## `ToPlane`

[`Plane`](Plane.md)`^ ToPlane()`


## `AreaVector`

[`Vector3D`](Vector3D.md)`^ AreaVector()`


## `Normal`

[`Vector3D`](Vector3D.md)`^ Normal()`


## `SignedArea`

`double SignedArea(`[`Vector3D`](Vector3D.md)`^ ref_normal)`


**Parameters**

- `ref_normal` ([`Vector3D`](Vector3D.md)^)

## `Area`

`double Area()`


## `Perimeter`

`double Perimeter()`


## `IsCCW`

`bool IsCCW(`[`Vector3D`](Vector3D.md)`^ ref_normal)`


**Parameters**

- `ref_normal` ([`Vector3D`](Vector3D.md)^)

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)`^ point)`

Distance from a point to this triangle (interior or boundary).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to measure distance to.

**Returns** — 0 if point is inside the triangle's closed region; otherwise the distance to the closest edge or vertex.

## `ToAxis`

`std::tuple< `[`Vector3D`](Vector3D.md)` , `[`Vector3D`](Vector3D.md)` > ToAxis()`

Local 2D basis spanning the triangle's plane.

**Returns** — Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.

## `Interpolate`

[`Point3D`](Point3D.md)`^  (nullable) Interpolate(double s, double t)`

Interpolates a point in the triangle's plane from barycentric-like coordinates (s, t) along U and V.

**Parameters**

- `s` (`double`) — Scalar along the U axis (= P1 - P0).
- `t` (`double`) — Scalar along the V axis (= P2 - P0).

**Returns** — P0 + s·U + t·V if (s, t) describes a point inside the triangle (s, t, s+t in [0, 1]); otherwise std::nullopt.

## `Location`

`std::tuple< double, double >^  (nullable) Location(`[`Point3D`](Point3D.md)`^ point)`

Inverse of Interpolate — locates a point in the (s, t) basis of ToAxis .

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to locate. Must lie in the triangle's plane.

**Returns** — The (s, t) pair if point is inside the triangle; otherwise std::nullopt.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies inside the triangle (interior, edge, or vertex).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test. Must lie in the triangle's plane (within decimal precision).

**Returns** — true if point is in the triangle's closed region.

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)`^ line)`

Tests whether this triangle intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — true if the line crosses the triangle's closed region.

`bool Intersects(`[`Ray3D`](Ray3D.md)`^ ray)`

Tests whether this triangle intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — true if the ray hits the triangle within its own domain (sc on the ray >= 0).

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`

Tests whether this triangle intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — true if any part of the segment lies inside the triangle.

`bool Intersects(`[`Plane`](Plane.md)`^ plane)`

Tests whether this triangle intersects a plane.

**Parameters**

- `plane` ([`Plane`](Plane.md)^) — The plane.

**Returns** — true if the plane cuts through the triangle or touches it.

`bool Intersects(Triangle3D^ other)`

Tests whether this triangle intersects another triangle.

**Parameters**

- `other` (`Triangle3D^`) — The other triangle.

**Returns** — true if the two triangles share any point.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line3D`](Line3D.md)`^ line)`

Intersection of this triangle with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the triangle.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray3D`](Ray3D.md)`^ ray)`

Intersection of this triangle with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — The crossing point if it lies within the ray's domain, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`

Intersection of this triangle with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Plane`](Plane.md)`^ plane)`

Intersection of this triangle with a plane.

**Parameters**

- `plane` ([`Plane`](Plane.md)^) — The plane.

**Returns** — A [Point3D](Point3D.md) (touches at a vertex), a [LineSegment3D](LineSegment3D.md) (cuts through interior), or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(Triangle3D^ other)`

Intersection of two coplanar or skew triangles.

**Parameters**

- `other` (`Triangle3D^`) — The other triangle.

**Returns** — A [Point3D](Point3D.md) , [LineSegment3D](LineSegment3D.md) , or std::nullopt depending on how the two triangles meet.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Polygon3D](Polygon3D.md), [Ray3D](Ray3D.md), [Vector3D](Vector3D.md)
