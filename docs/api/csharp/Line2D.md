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


**Parameters**

- `other` (`Line2D^`)

`bool Intersects(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Intersects(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Intersects(`[`Triangle2D`](Triangle2D.md)`^ t)`


**Parameters**

- `t` ([`Triangle2D`](Triangle2D.md)^)

`bool Intersects(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Intersection`

[`Point2D`](Point2D.md)`^  (nullable) Intersection(Line2D^ other)`


**Parameters**

- `other` (`Line2D^`)

[`Point2D`](Point2D.md)`^  (nullable) Intersection(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Intersection(`[`LineSegment2D`](LineSegment2D.md)`^ segment)`


**Parameters**

- `segment` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Intersection(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Overlaps`

`bool Overlaps(Line2D^ line)`


**Parameters**

- `line` (`Line2D^`)

`bool Overlaps(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Overlaps(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Overlaps(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Overlap`

`Line2D^  (nullable) Overlap(Line2D^ line)`


**Parameters**

- `line` (`Line2D^`)

[`Ray2D`](Ray2D.md)`^  (nullable) Overlap(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

[`LineSegment2D`](LineSegment2D.md)`^  (nullable) Overlap(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`LineSegment2D`](LineSegment2D.md)` >^  (nullable) Overlap(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Touches`

`bool Touches(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

`bool Touches(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`bool Touches(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)

## `Touch`

[`Point2D`](Point2D.md)`^  (nullable) Touch(`[`Ray2D`](Ray2D.md)`^ ray)`


**Parameters**

- `ray` ([`Ray2D`](Ray2D.md)^)

[`Point2D`](Point2D.md)`^  (nullable) Touch(`[`LineSegment2D`](LineSegment2D.md)`^ seg)`


**Parameters**

- `seg` ([`LineSegment2D`](LineSegment2D.md)^)

`std::vector< `[`Point2D`](Point2D.md)` >^  (nullable) Touch(`[`Polyline2D`](Polyline2D.md)`^ polyline)`


**Parameters**

- `polyline` ([`Polyline2D`](Polyline2D.md)^)


---

**See also:** [LineSegment2D](LineSegment2D.md), [Point2D](Point2D.md), [Polyline2D](Polyline2D.md), [Ray2D](Ray2D.md), [Triangle2D](Triangle2D.md), [Vector2D](Vector2D.md)
