# `BBox3D` (C# / .NET)

## `min`

[`Point3D`](Point3D.md)`^ min()`


## `max`

[`Point3D`](Point3D.md)`^ max()`


## `AlmostEquals`

`bool AlmostEquals(BBox3D^ other, double epsilon)`


**Parameters**

- `other` (`BBox3D^`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ p)`

Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).

**Parameters**

- `p` ([`Point3D`](Point3D.md)^) — The point to test.

**Returns** — true if p falls within the closed box [min, max] along each axis.


---

**See also:** [Point3D](Point3D.md)
