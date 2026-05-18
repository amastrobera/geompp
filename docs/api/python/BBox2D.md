# `BBox2D` (Python)

## `min`

`min() -> `[`Point2D`](Point2D.md)


## `max`

`max() -> `[`Point2D`](Point2D.md)


## `almost_equals`

`almost_equals(other: BBox2D, epsilon: float) -> bool`


**Parameters**

- `other` (`BBox2D`)
- `epsilon` (`float`)

## `contains`

`contains(p: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).

**Parameters**

- `p` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if p falls within the closed box [min, max] along each axis.


---

**See also:** [Point2D](Point2D.md)
