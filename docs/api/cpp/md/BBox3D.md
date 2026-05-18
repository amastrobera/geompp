# `BBox3D` (C++)

## `min`

[`Point3D`](Point3D.md)` min() const`


## `max`

[`Point3D`](Point3D.md)` max() const`


## `AlmostEquals`

`bool AlmostEquals(BBox3D const & other, double epsilon) const`


**Parameters**

- `other` (`BBox3D const &`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & p) const`

Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &) — The point to test.

**Returns** — true if p falls within the closed box [min, max] along each axis.


---

**See also:** [Point3D](Point3D.md)
