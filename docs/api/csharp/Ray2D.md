# `Ray2D` (C# / .NET)

## `Make`

**static** `Ray2D^ Make(`[`Point2D`](Point2D.md)`^ orig, `[`Vector2D`](Vector2D.md)`^ dir)`


**Parameters**

- `orig` ([`Point2D`](Point2D.md)^)
- `dir` ([`Vector2D`](Vector2D.md)^)

## `FromWkt`

**static** `Ray2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Ray2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Origin`

[`Point2D`](Point2D.md)`^ Origin()`


## `Direction`

[`Vector2D`](Vector2D.md)`^ Direction()`


## `AlmostEquals`

`bool AlmostEquals(Ray2D^ other, double epsilon)`


**Parameters**

- `other` (`Ray2D^`)
- `epsilon` (`double`)

## `IsAhead`

`bool IsAhead(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on the half-space ahead of (or at) the ray's origin along its direction.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if (point - origin) · direction >= 0.

## `IsBehind`

`bool IsBehind(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies strictly behind the ray's origin along its direction.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if (point - origin) · direction < 0.

## `ToLine`

[`Line2D`](Line2D.md)`^ ToLine()`

Promotes the ray to an infinite line through the same origin and direction.

**Returns** — A [Line2D](Line2D.md) that contains every point on this ray and extends backwards as well.

## `ProjectOnto`

[`Point2D`](Point2D.md)`^ ProjectOnto(`[`Point2D`](Point2D.md)`^ point)`

Orthogonal projection of a point onto this ray, clamped to the origin.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to project.

**Returns** — Closest point on the ray to point . Returns the ray's origin if point projects behind it.

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Distance from a point to this ray.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — Perpendicular distance to the ray's domain, or distance to the origin if point is behind the ray.

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on this ray.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is collinear with the ray's direction AND ahead of (or at) the origin.

## `Intersects`

`bool Intersects(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Intersects(Ray2D^ other)`


**Parameters**

- `other` (`Ray2D^`)

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Intersects(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Intersection`

[`Point2D`](Point2D.md)`^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Intersection(Ray2D^ other)`


**Parameters**

- `other` (`Ray2D^`)

[`Point2D`](Point2D.md)`^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Intersection(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Overlaps`

`bool Overlaps(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Overlaps(Ray2D^ ray)`


**Parameters**

- `ray` (`Ray2D^`)

`bool Overlaps(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Overlaps(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Overlap`

`Ray2D^  (nullable) Overlap(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`std::variant< Ray2D , `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Overlap(Ray2D^ ray)`


**Parameters**

- `ray` (`Ray2D^`)

[`LineSegment2D`](LineSegment2D.md)`^  (nullable) Overlap(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Overlap(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Touches`

`bool Touches(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

`bool Touches(Ray2D^ ray)`


**Parameters**

- `ray` (`Ray2D^`)

`bool Touches(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Touches(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Touch`

[`Point2D`](Point2D.md)`^  (nullable) Touch(`[`Line2D`](Line2D.md)`^ line)`


**Parameters**

- `line` ([`Line2D`](Line2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Touch(Ray2D^ ray)`


**Parameters**

- `ray` (`Ray2D^`)

[`Point2D`](Point2D.md)`^  (nullable) Touch(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Touch(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Vector2D](Vector2D.md)
