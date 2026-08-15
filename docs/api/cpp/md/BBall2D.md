# `BBall2D` (C++)

## `center`

[`Point2D`](Point2D.md)` center() const`


## `radius`

`double radius() const`


## `AlmostEquals`

`bool AlmostEquals(BBall2D const & other, double epsilon) const`


**Parameters**

- `other` (`BBall2D const &`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point2D`](Point2D.md)` const & p) const`

Tests whether a point lies inside this bounding ball (inclusive of the boundary).

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &) — The point to test.

**Returns** — true if p falls within the closed ball of radius RADIUS centered at CENTER.


---

**See also:** [Point2D](Point2D.md)
