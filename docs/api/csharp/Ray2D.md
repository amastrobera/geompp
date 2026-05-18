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

Tests whether this ray intersects a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — true if they meet at a point on the ray's domain.

`bool Intersects(Ray2D^ other)`

Tests whether this ray intersects another ray.

**Parameters**

- `other` (`Ray2D^`) — The other ray.

**Returns** — true if both rays' domains share the crossing point.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Tests whether this ray intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — true if the ray hits the segment within both domains.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Line2D`](Line2D.md)`^ line)`

Intersection point of this ray with a line.

**Parameters**

- `line` ([`Line2D`](Line2D.md)^) — The line.

**Returns** — The intersection point if it lies on the ray, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(Ray2D^ other)`

Intersection point of two rays.

**Parameters**

- `other` (`Ray2D^`) — The other ray.

**Returns** — The intersection point if it lies on both rays' domains, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Intersection point of this ray with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — The intersection point if it lies on both the ray and the segment, or std::nullopt otherwise.


---

**See also:** [Line2D](Line2D.md), [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Vector2D](Vector2D.md)
