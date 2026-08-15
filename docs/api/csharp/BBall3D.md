# `BBall3D` (C# / .NET)

## `center`

[`Point3D`](Point3D.md)`^ center()`


## `radius`

`double radius()`


## `AlmostEquals`

`bool AlmostEquals(BBall3D^ other, double epsilon)`


**Parameters**

- `other` (`BBall3D^`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ p)`

Tests whether a point lies inside this bounding ball (inclusive of the boundary).

**Parameters**

- `p` ([`Point3D`](Point3D.md)^) — The point to test.

**Returns** — true if p falls within the closed ball of radius RADIUS centered at CENTER.


---

**See also:** [Point3D](Point3D.md)
