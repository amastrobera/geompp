# `Polygon2D` (C# / .NET)

## `Make`

**static** `Polygon2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

**static** `Polygon2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points, std::vector< std::vector< `[`Point2D`](Point2D.md)` > > holes)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)
- `holes` (std::vector< std::vector< [`Point2D`](Point2D.md) > >)

## `FromWkt`

**static** `Polygon2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polygon2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`size_t Size()`


## `AlmostEquals`

`bool AlmostEquals(Polygon2D^ other, double epsilon)`


**Parameters**

- `other` (`Polygon2D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange2D`](SegmentRange2D.md)`^ ToSegments()`


## `Centroid`

[`Point2D`](Point2D.md)`^ Centroid()`


## `Area`

`double Area()`


## `Perimeter`

`double Perimeter()`


## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Distance from a point to this polygon's closed region.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — 0 if point is inside the polygon (or on its boundary); otherwise the distance to the nearest edge.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies inside the polygon (winding-number check).

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is in the polygon's closed region.

## `IsOnBoundary`

`bool IsOnBoundary(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on the polygon's boundary.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is on any of the polygon's edges or vertices.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)`^ line)`

Tests whether this polygon intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — true if the line crosses the polygon's closed region.

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`

Tests whether this polygon intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — true if the ray hits the polygon within its domain.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Tests whether this polygon intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — true if any part of the segment is inside the polygon or crosses its boundary.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`

Intersection of this polygon with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — The crossing point, or std::nullopt if the line misses the polygon.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`

Intersection of this polygon with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — The crossing point if within the ray's domain, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ other)`

Intersection of this polygon with a segment.

**Parameters**

- `other` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — The crossing point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
