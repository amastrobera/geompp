# `Triangle2D` (C# / .NET)

## `Make`

**static** `Triangle2D^ Make(`[`Point2D`](Point2D.md)`^ p0, `[`Point2D`](Point2D.md)`^ p1, `[`Point2D`](Point2D.md)`^ p2)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md)^)
- `p1` ([`Point2D`](Point2D.md)^)
- `p2` ([`Point2D`](Point2D.md)^)

## `FromWkt`

**static** `Triangle2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Triangle2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Vertices`

`std::tuple< `[`Point2D`](Point2D.md)` , `[`Point2D`](Point2D.md)` , `[`Point2D`](Point2D.md)` > Vertices()`


## `AlmostEquals`

`bool AlmostEquals(Triangle2D^ other, double epsilon)`


**Parameters**

- `other` (`Triangle2D^`)
- `epsilon` (`double`)

## `Centroid`

[`Point2D`](Point2D.md)`^ Centroid()`


## `ToPolygon`

[`Polygon2D`](Polygon2D.md)`^ ToPolygon()`


## `SignedArea`

`double SignedArea()`


## `Area`

`double Area()`


## `Perimeter`

`double Perimeter()`


## `IsCCW`

`bool IsCCW()`


## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Distance from a point to this triangle (interior or boundary).

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — 0 if point is inside the triangle; otherwise the distance to the closest edge or vertex.

## `ToAxis`

`std::tuple< `[`Vector2D`](Vector2D.md)` , `[`Vector2D`](Vector2D.md)` > ToAxis()`

Local 2D basis spanning the triangle.

**Returns** — Pair (U, V) where U = P1 - P0 and V = P2 - P0. Not orthonormalized.

## `Interpolate`

[`Point2D`](Point2D.md)`^  (nullable) Interpolate(double s, double t)`

Interpolates a point in the triangle from barycentric-like coordinates (s, t) along U and V.

**Parameters**

- `s` (`double`) — Scalar along the U axis (= P1 - P0).
- `t` (`double`) — Scalar along the V axis (= P2 - P0).

**Returns** — P0 + s·U + t·V if (s, t, s+t) all lie in [0, 1]; otherwise std::nullopt.

## `Location`

`std::tuple< double, double >^  (nullable) Location(`[`Point2D`](Point2D.md)`^ point)`

Inverse of Interpolate — locates a point in the (s, t) basis of ToAxis .

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to locate.

**Returns** — The (s, t) pair if point is inside the triangle; otherwise std::nullopt.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies inside the triangle (interior, edge, or vertex).

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is in the triangle's closed region.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)`^ line)`

Tests whether this triangle intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — true if the line crosses the triangle's closed region.

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`

Tests whether this triangle intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — true if the ray hits the triangle within its own domain.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Tests whether this triangle intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — true if any part of the segment lies inside the triangle.

`bool Intersects(Triangle2D^ other)`

Tests whether this triangle intersects another triangle.

**Parameters**

- `other` (`Triangle2D^`) — The other triangle.

**Returns** — true if the two share any point.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`

Intersection of this triangle with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — A [Point2D](Point2D.md) (line touches a vertex), a [LineSegment2D](LineSegment2D.md) (line cuts through interior), or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`

Intersection of this triangle with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — [Point2D](Point2D.md) / [LineSegment2D](LineSegment2D.md) depending on geometry, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Intersection of this triangle with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — [Point2D](Point2D.md) / [LineSegment2D](LineSegment2D.md) depending on geometry, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(Triangle2D^ other)`

Intersection of two triangles.

**Parameters**

- `other` (`Triangle2D^`) — The other triangle.

**Returns** — A [Point2D](Point2D.md) , [LineSegment2D](LineSegment2D.md) , Triangle2D , or [Polygon2D](Polygon2D.md) depending on overlap, or std::nullopt if disjoint.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polygon2D](Polygon2D.md), [Ray2D](Ray2D.md), [Vector2D](Vector2D.md)
