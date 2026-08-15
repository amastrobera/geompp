# `LineSegment3D` (C# / .NET)

## `Make`

**static** `LineSegment3D^ Make(`[`Point3D`](Point3D.md)`^ p0, `[`Point3D`](Point3D.md)`^ p1)`


**Parameters**

- `p0` ([`Point3D`](Point3D.md)^)
- `p1` ([`Point3D`](Point3D.md)^)

## `FromWkt`

**static** `LineSegment3D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `LineSegment3D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `First`

[`Point3D`](Point3D.md)`^ First()`


## `Last`

[`Point3D`](Point3D.md)`^ Last()`


## `AlmostEquals`

`bool AlmostEquals(LineSegment3D^ other, double epsilon)`


**Parameters**

- `other` (`LineSegment3D^`)
- `epsilon` (`double`)

## `ToLine`

[`Line3D`](Line3D.md)`^ ToLine()`


## `Length`

`double Length()`


## `Reversed`

`LineSegment3D^ Reversed()`


## `ProjectOnto`

[`Point3D`](Point3D.md)`^ ProjectOnto(`[`Point3D`](Point3D.md)`^ point)`

Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to project.

**Returns** — Closest point on the segment to point (one of the endpoints if point projects outside the segment).

## `DistanceTo`

`double DistanceTo(`[`Point3D`](Point3D.md)`^ point)`

Distance from a point to this segment.

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to measure distance to.

**Returns** — Length of the perpendicular from point to the segment, or distance to the nearest endpoint if point projects outside the segment.

`double DistanceTo(`[`Line3D`](Line3D.md)`^ other)`

Scalar distance between this segment and a line.

**Parameters**

- `other` ([`Line3D`](Line3D.md)^) — The other line.

**Returns** — 0 if the line crosses or contains the segment; otherwise the minimum distance.

`double DistanceTo(`[`Ray3D`](Ray3D.md)`^ ray)`

Scalar distance between this segment and a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — 0 if they intersect or overlap; otherwise the minimum distance.

`double DistanceTo(LineSegment3D^ seg)`

Scalar distance between two segments.

**Parameters**

- `seg` (`LineSegment3D^`) — The other segment.

**Returns** — 0 if they intersect or share a region; otherwise the minimum distance.

## `Location`

`double Location(`[`Point3D`](Point3D.md)`^ point)`

Position of a point along the segment, normalized to [0, 1].

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to locate. Must lie on the segment's underlying line.

**Returns** — 0 at First, 1 at Last, fractional values in between, or +infinity if point is not on the segment's line.

## `Interpolate`

[`Point3D`](Point3D.md)`^ Interpolate(double pct)`

Linear interpolation along the segment.

**Parameters**

- `pct` (`double`) — Normalized position in [0, 1]. Values outside that range are clamped to the segment's endpoints.

**Returns** — Point at the given fraction along the segment.

## `Distance`

`LineSegment3D^  (nullable) Distance(`[`Line3D`](Line3D.md)`^ other)`

Directed line-segment connecting this segment's closest point to another line.

**Parameters**

- `other` ([`Line3D`](Line3D.md)^) — The other line.

**Returns** — Segment from this segment's closest point to other's closest point, or std::nullopt if they intersect or are collinear.

`LineSegment3D^  (nullable) Distance(`[`Ray3D`](Ray3D.md)`^ ray)`

Directed line-segment connecting this segment's closest point to a ray.

**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^) — The ray.

**Returns** — Segment from this segment's closest point to the ray's closest point, or std::nullopt if they intersect or overlap.

`LineSegment3D^  (nullable) Distance(LineSegment3D^ seg)`

Directed line-segment connecting this segment's closest point to another segment.

**Parameters**

- `seg` (`LineSegment3D^`) — The other segment.

**Returns** — Segment from this segment's closest point to seg's closest point, or std::nullopt if they intersect or share a region.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ point)`

Tests whether a point lies on this segment (between or at the endpoints).

**Parameters**

- `point` ([`Point3D`](Point3D.md)^) — The point to test.

**Returns** — true if point is collinear with the segment and falls within [First, Last].

## `Intersects`

`bool Intersects(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`bool Intersects(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`bool Intersects(LineSegment3D^ segment)`


**Parameters**

- `segment` (`LineSegment3D^`)

`bool Intersects(`[`Polyline3D`](Polyline3D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

[`Point3D`](Point3D.md)`^  (nullable) Intersection(LineSegment3D^ other)`


**Parameters**

- `other` (`LineSegment3D^`)

`std::variant< `[`Point3D`](Point3D.md)` , std::vector< `[`Point3D`](Point3D.md)` > >^  (nullable) Intersection(`[`Polyline3D`](Polyline3D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)

## `Overlaps`

`bool Overlaps(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`bool Overlaps(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`bool Overlaps(LineSegment3D^ seg)`


**Parameters**

- `seg` (`LineSegment3D^`)

`bool Overlaps(`[`Polyline3D`](Polyline3D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)

## `Overlap`

`LineSegment3D^  (nullable) Overlap(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`LineSegment3D^  (nullable) Overlap(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`LineSegment3D^  (nullable) Overlap(LineSegment3D^ seg)`


**Parameters**

- `seg` (`LineSegment3D^`)

`std::vector< LineSegment3D >^  (nullable) Overlap(`[`Polyline3D`](Polyline3D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)

## `Touches`

`bool Touches(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

`bool Touches(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

`bool Touches(LineSegment3D^ seg)`


**Parameters**

- `seg` (`LineSegment3D^`)

`bool Touches(`[`Polyline3D`](Polyline3D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)

## `Touch`

[`Point3D`](Point3D.md)`^  (nullable) Touch(`[`Line3D`](Line3D.md)`^ line)`


**Parameters**

- `line` ([`Line3D`](Line3D.md)^)

[`Point3D`](Point3D.md)`^  (nullable) Touch(`[`Ray3D`](Ray3D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray3D`](Ray3D.md)^)

[`Point3D`](Point3D.md)`^  (nullable) Touch(LineSegment3D^ seg)`


**Parameters**

- `seg` (`LineSegment3D^`)

`std::vector< `[`Point3D`](Point3D.md)` >^  (nullable) Touch(`[`Polyline3D`](Polyline3D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline3D`](Polyline3D.md)^)


---

**See also:** [Line3D](Line3D.md), [Point3D](Point3D.md), [Polyline3D](Polyline3D.md), [Ray3D](Ray3D.md)
