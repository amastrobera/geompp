# `BBall2D` (C# / .NET)

## `center`

[`Point2D`](Point2D.md)`^ center()`


## `radius`

`double radius()`


## `AlmostEquals`

`bool AlmostEquals(BBall2D^ other, double epsilon)`


**Parameters**

- `other` (`BBall2D^`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)`^ p)`

Tests whether a point lies inside this bounding ball (inclusive of the boundary).

**Parameters**

- `p` ([`Point2D`](Point2D.md)^) — The point to test.

**Returns** — true if p falls within the closed ball of radius RADIUS centered at CENTER.


---

**See also:** [Point2D](Point2D.md)
