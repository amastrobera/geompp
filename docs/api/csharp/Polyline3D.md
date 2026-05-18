# `Polyline3D` (C# / .NET)

## `Make`

**static** `Polyline3D^ Make(std::vector< `[`Point3D`](Point3D.md)` > points)`


**Parameters**

- `points` (std::vector< [`Point3D`](Point3D.md) >)

## `FromWkt`

**static** `Polyline3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Polyline3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Size`

`int Size()`


## `AlmostEquals`

`bool AlmostEquals(Polyline3D^ other, double epsilon)`


**Parameters**

- `other` (`Polyline3D^`)
- `epsilon` (`double`)

## `ToSegments`

[`SegmentRange3D`](SegmentRange3D.md)`^ ToSegments()`


## `Length`

`double Length()`


## `ProjectOnto`

[`Point3D`](Point3D.md)`^ ProjectOnto(`[`Point3D`](Point3D.md)`^ point)`

Closest point on the polyline (any of its segments) to a given point.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to project.

**Returns** — Point on the polyline at minimum distance from point ; falls on one of the segments' interiors or knots.

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)`^ point)`

Distance from a point to the polyline (minimum over all of its segments).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the closest segment.

## `Location`

`double Location(`[`Point3D`](Point3D.md)`^ point)`

Position of a point along the polyline, normalized to [0, 1] by arc length.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to locate. Must lie on the polyline.

**Returns** — 0 at the first knot, 1 at the last, fractional values in between, or +infinity if point is off the polyline.

## `Interpolate`

[`Point3D`](Point3D.md)`^ Interpolate(double pct)`

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

`bool Contains(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies on the polyline.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test.

**Returns** — true if point is on any of the polyline's segments (interior or knot).

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)`^ line)`

Tests whether this polyline intersects a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — true if the line crosses any of the polyline's segments.

`bool Intersects(`[`Ray3D`](Ray3D.md)`^ ray)`

Tests whether this polyline intersects a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — true if the ray crosses any of the polyline's segments within its domain.

`bool Intersects(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`

Tests whether this polyline intersects a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — true if the segment crosses any of the polyline's segments.

`bool Intersects(Polyline3D^ other)`

Tests whether two polylines intersect.

**Parameters**

- `other` (`Polyline3D^`) — The other polyline.

**Returns** — true if any segment of either polyline crosses any segment of the other.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line3D`](Line3D.md)`^ line)`

Intersection of this polyline with a line.

**Parameters**

- `line` ([`Line3D`](Line3D.md)^) — The line.

**Returns** — A single [Point3D](Point3D.md) when there's one crossing, a list when there are several, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray3D`](Ray3D.md)`^ ray)`

Intersection of this polyline with a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — A single [Point3D](Point3D.md) or a list of crossings, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment3D`](LineSegment3D.md)`^ segment)`

Intersection of this polyline with a segment.

**Parameters**

- `segment` ([`LineSegment3D`](LineSegment3D.md)^) — The segment.

**Returns** — A single [Point3D](Point3D.md) or a list of crossings, or std::nullopt if disjoint.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(Polyline3D^ other)`

Intersection of two polylines.

**Parameters**

- `other` (`Polyline3D^`) — The other polyline.

**Returns** — A single [Point3D](Point3D.md) or a list of crossings, or std::nullopt if disjoint.


---

**See also:** [Line3D](Line3D.md), [LineSegment3D](LineSegment3D.md), [Point3D](Point3D.md), [Ray3D](Ray3D.md), [SegmentRange3D](SegmentRange3D.md)
