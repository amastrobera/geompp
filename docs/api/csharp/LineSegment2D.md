# `LineSegment2D` (C# / .NET)

## `Make`

**static** `LineSegment2D^ Make(`[`Point2D`](Point2D.md)`^ p0, `[`Point2D`](Point2D.md)`^ p1)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md)^)
- `p1` ([`Point2D`](Point2D.md)^)

## `FromWkt`

**static** `LineSegment2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `LineSegment2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `First`

[`Point2D`](Point2D.md)`^ First()`


## `Last`

[`Point2D`](Point2D.md)`^ Last()`


## `AlmostEquals`

`bool AlmostEquals(LineSegment2D^ other, double epsilon)`


**Parameters**

- `other` (`LineSegment2D^`)
- `epsilon` (`double`)

## `ToLine`

[`Line2D`](Line2D.md)`^ ToLine()`


## `Length`

`double Length()`


## `Reversed`

`LineSegment2D^ Reversed()`


## `ProjectOnto`

[`Point2D`](Point2D.md)`^ ProjectOnto(`[`Point2D`](Point2D.md)`^ point)`

Orthogonal projection of a point onto this segment, clamped to the segment's endpoints.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to project.

**Returns** — Closest point on the segment to point (one of the endpoints if point projects outside).

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Distance from a point to this segment.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — Perpendicular distance to the segment, or distance to the nearest endpoint if point projects outside.

## `Location`

`double Location(`[`Point2D`](Point2D.md)`^ point)`

Position of a point along the segment, normalized to [0, 1].

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to locate. Must lie on the segment's underlying line.

**Returns** — 0 at First, 1 at Last, fractional values in between, or +infinity if not on the line.

## `Interpolate`

[`Point2D`](Point2D.md)`^ Interpolate(double pct)`

Linear interpolation along the segment.

**Parameters**

- `pct` (`double`) — Normalized position in [0, 1]. Values outside are clamped to the segment's endpoints.

**Returns** — Point at the given fraction along the segment.

## `IsLeft`

`bool IsLeft(`[`Point2D`](Point2D.md)`^ p)`

Tests whether a point is on the left of the segment's direction (looking from First toward Last).

**Parameters**

- `p` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if the 2D cross product (P1 - P0) × (p - P0) is positive.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on this segment.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is collinear with the segment and falls within [First, Last].

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Intersects(LineSegment2D^ segment)`


**Parameters**

- `segment` (`LineSegment2D^`)

`bool Intersects(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Intersection`

[`Point2D`](Point2D.md)`^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Intersection(LineSegment2D^ other)`


**Parameters**

- `other` (`LineSegment2D^`)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Intersection(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Overlaps`

`bool Overlaps(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Overlaps(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Overlaps(LineSegment2D^ seg)`


**Parameters**

- `seg` (`LineSegment2D^`)

`bool Overlaps(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Overlap`

`LineSegment2D^  (nullable) Overlap(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`LineSegment2D^  (nullable) Overlap(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`LineSegment2D^  (nullable) Overlap(LineSegment2D^ seg)`


**Parameters**

- `seg` (`LineSegment2D^`)

`std::vector< LineSegment2D >^  (nullable) Overlap(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Touches`

`bool Touches(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Touches(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Touches(LineSegment2D^ seg)`


**Parameters**

- `seg` (`LineSegment2D^`)

`bool Touches(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Touch`

[`Point2D`](Point2D.md)`^  (nullable) Touch(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Touch(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Touch(LineSegment2D^ seg)`


**Parameters**

- `seg` (`LineSegment2D^`)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Touch(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)


---

**See also:** [Line2D](Line2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md)
