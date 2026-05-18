# `BBox2D` (C# / .NET)

## `min`

[`Point2D`](Point2D.md)`^ min()`


## `max`

[`Point2D`](Point2D.md)`^ max()`


## `AlmostEquals`

`bool AlmostEquals(BBox2D^ other, double epsilon)`


**Parameters**

- `other` (`BBox2D^`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ p)`

Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).

**Parameters**

- `p` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if p falls within the closed box [min, max] along each axis.


---

**See also:** [Point2D](Point2D.md)
