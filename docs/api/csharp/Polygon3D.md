# `Polygon3D` (C# / .NET)

## `Make`

**static** `Polygon3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

**static** `Polygon3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points, std::vector< std::vector< `[`Point3D`](Point3D.md)` > > holes)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)
- `holes` (std::vector< std::vector< [`Point3D`](Point3D.md) > >)

## `FromWkt`

**static** `Polygon3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polygon3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`size_t Size()`


## `GetPlane`

[`Plane`](Plane.md)`^ GetPlane()`


## `AlmostEquals`

`bool AlmostEquals(Polygon3D^ other, double epsilon)`


**Parameters**

- `other` (`Polygon3D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange3D`](SegmentRange3D.md)`^ ToSegments()`


## `Centroid`

[`Point3D`](Point3D.md)`^ Centroid()`


## `Area`

`double Area()`


## `Perimeter`

`double Perimeter()`


## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)`^ point)`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test. Must lie in the polygon's plane.

**Returns** — true if point is in the polygon's closed region (interior or boundary).

## `IsOnPerimeter`

`bool IsOnPerimeter(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies on the polygon's boundary (any of its edges).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)`^ line)`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`bool Intersects(`[`Ray3D`](Ray3D.md)`^ ray)`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line3D`](Line3D.md)`^ line)`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — The crossing point as [Point3D](Point3D.md) , or std::nullopt if the line misses the polygon.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray3D`](Ray3D.md)`^ ray)`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment3D`](LineSegment3D.md)`^ other)`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Plane](Plane.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
