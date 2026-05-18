# `Polyline2D` (C# / .NET)

## `Make`

**static** `Polyline2D^ Make(std::vector< `[`Point2D`](Point2D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point2D`](Point2D.md) >)

## `FromWkt`

**static** `Polyline2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polyline2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`int Size()`


## `AlmostEquals`

`bool AlmostEquals(Polyline2D^ other, double epsilon)`


**Parameters**

- `other` (`Polyline2D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange2D`](SegmentRange2D.md)`^ ToSegments()`


## `Length`

`double Length()`


## `ProjectOnto`

[`Point2D`](Point2D.md)`^ ProjectOnto(`[`Point2D`](Point2D.md)`^ point)`

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to project.

**Returns** — Point on the polyline at minimum distance from point .

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `Location`

`double Location(`[`Point2D`](Point2D.md)`^ point)`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, or +infinity if point is off the polyline.

## `Interpolate`

[`Point2D`](Point2D.md)`^ Interpolate(double pct)`

Linear interpolation along the polyline by arc-length fraction.

**Parameters**

- `pct` (`double`) — Normalized position in [0, 1]. Values outside the range are clamped to the polyline's endpoints.

**Returns** — Point at the given arc-length fraction along the polyline.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is on any of the polyline's segments.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)`^ line)`

Tests whether this polyline intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — true if the line crosses any of the polyline's segments.

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`

Tests whether this polyline intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — true if the ray crosses any of the polyline's segments.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Tests whether this polyline intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — true if the segment crosses any of the polyline's segments.

`bool Intersects(Polyline2D^ other)`

Tests whether two polylines intersect.

**Parameters**

- `other` (`Polyline2D^`) — The other polyline.

**Returns** — true if any segment of either polyline crosses any segment of the other.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`

Intersection of this polyline with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — A single [Point2D](Point2D.md) when there's one crossing, a list when there are several, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`

Intersection of this polyline with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Intersection of this polyline with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(Polyline2D^ other)`

Intersection of two polylines.

**Parameters**

- `other` (`Polyline2D^`) — The other polyline.

**Returns** — A single [Point2D](Point2D.md) or a list of crossings, or std::nullopt if disjoint.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [SegmentRange2D](SegmentRange2D.md)
