# `BBall3D` (C++)

## `center`

[`Point3D`](Point3D.md)` center() const`


## `radius`

`double radius() const`


## `AlmostEquals`

`bool AlmostEquals(BBall3D const & other, double epsilon) const`


**Parameters**

- `other` (`BBall3D const &`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & p) const`

Tests whether a point lies inside this bounding ball (inclusive of the boundary).

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if p falls within the closed ball of radius RADIUS centered at CENTER.


---

**See also:** [Point3D](Point3D.md)
