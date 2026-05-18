# `BBox2D` (C++)

## `min`

[`Point2D`](Point2D.md)` min() const`


## `max`

[`Point2D`](Point2D.md)` max() const`


## `AlmostEquals`

`bool AlmostEquals(BBox2D const & other, double epsilon) const`


**Parameters**

- `other` (`BBox2D const &`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & p) const`

Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if p falls within the closed box [min, max] along each axis.


---

**See also:** [Point2D](Point2D.md)
