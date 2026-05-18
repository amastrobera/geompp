# `Line2D` (C# / .NET)

## `Make`

**static** `Line2D^ Make(`[`Point2D`](Point2D.md)`^ p0, `[`Point2D`](Point2D.md)`^ p1)`


**Parameters**

- `p0` ([`Point2D`](Point2D.md)^)
- `p1` ([`Point2D`](Point2D.md)^)

**static** `Line2D^ Make(`[`Point2D`](Point2D.md)`^ orig, `[`Vector2D`](Vector2D.md)`^ dir)`


**Parameters**

- `orig` ([`Point2D`](Point2D.md)^)
- `dir` ([`Vector2D`](Vector2D.md)^)

## `FromWkt`

**static** `Line2D^ FromWkt(System::String^ wkt)`


**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Line2D^ FromFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `First`

[`Point2D`](Point2D.md)`^ First()`


## `Last`

[`Point2D`](Point2D.md)`^ Last()`


## `Origin`

[`Point2D`](Point2D.md)`^ Origin()`


## `Direction`

[`Vector2D`](Vector2D.md)`^ Direction()`


## `AlmostEquals`

`bool AlmostEquals(Line2D^ other, double epsilon)`


**Parameters**

- `other` (`Line2D^`)
- `epsilon` (`double`)

## `DistanceTo`

`double DistanceTo(`[`Point2D`](Point2D.md)`^ point)`

Perpendicular distance from a point to this infinite line.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to measure distance to.

**Returns** — Length of the perpendicular from point onto this line.

## `ProjectOnto`

[`Point2D`](Point2D.md)`^ ProjectOnto(`[`Point2D`](Point2D.md)`^ point)`

Orthogonal projection of a point onto this infinite line.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to project.

**Returns** — Closest point on this line to point .

## `ToWkt`

`System::String^ ToWkt()`


## `ToFile`

`void ToFile(System::String^ path)`


**Parameters**

- `path` (`System::String^`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ point)`

Tests whether a point lies on this line.

**Parameters**

- `point` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if point is collinear with the line within decimal precision.

## `Intersects`

`bool Intersects(Line2D^ other)`

Tests whether this line intersects another line.

**Parameters**

- `other` (`Line2D^`) — The other line.

**Returns** — true if they meet at a point; false for parallel non-collinear lines.

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`

Tests whether this line intersects a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — true if they meet within the ray's domain.

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Tests whether this line intersects a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — true if the line crosses or touches the segment.

`bool Intersects(`[`Triangle2D`](Triangle2D.md)`^ t)`

Tests whether this line passes through a triangle.

**Parameters**

- `t` ([`Triangle2D`](Triangle2D.md)^) — The triangle.

**Returns** — true if the line crosses the triangle's interior or an edge.

## `Intersection`

[`Point3D`](Point3D.md)`^  (nullable) Intersection(Line2D^ other, double sc, double tc)`

Intersection point of two lines, also returning the parametric values along each line.

**Parameters**

- `other` (`Line2D^`) — The other line.
- `sc` (`double`) — Output: parameter along this line at the intersection.
- `tc` (`double`) — Output: parameter along other at the intersection.

**Returns** — The intersection point, or std::nullopt for parallel lines.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(Line2D^ other)`

Intersection point of two lines.

**Parameters**

- `other` (`Line2D^`) — The other line.

**Returns** — The intersection point, or std::nullopt for parallel lines.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`

Intersection point of this line with a ray.

**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^) — The ray.

**Returns** — The intersection point if it lies on the ray, or std::nullopt otherwise.

[`Point3D`](Point3D.md)`^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`

Intersection point of this line with a segment.

**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^) — The segment.

**Returns** — The intersection point if it lies on the segment, or std::nullopt otherwise.


---

**See also:** [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md), [Vector2D](Vector2D.md)
