# `BBall2D` (Python)

## `center`

`center() -> `[`Point2D`](Point2D.md)


## `radius`

`radius() -> float`


## `almost_equals`

`almost_equals(other: BBall2D, epsilon: float) -> bool`


**Parameters**

- `other` (`BBall2D`)
- `epsilon` (`float`)

## `contains`

`contains(p: `[`Point2D`](Point2D.md)`) -> bool`

Tests whether a point lies inside this bounding ball (inclusive of the boundary).

**Parameters**

- `p` ([`Point2D`](Point2D.md)) — The point to test.

**Returns** — true if p falls within the closed ball of radius RADIUS centered at CENTER.


---

**See also:** [Point2D](Point2D.md)
