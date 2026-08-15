# `BBall3D` (Python)

## `center`

`center() -> `[`Point3D`](Point3D.md)


## `radius`

`radius() -> float`


## `almost_equals`

`almost_equals(other: BBall3D, epsilon: float) -> bool`


**Parameters**

- `other` (`BBall3D`)
- `epsilon` (`float`)

## `contains`

`contains(p: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies inside this bounding ball (inclusive of the boundary).

**Parameters**

- `p` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if p falls within the closed ball of radius RADIUS centered at CENTER.


---

**See also:** [Point3D](Point3D.md)
