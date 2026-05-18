# `BBox3D` (Python)

## `min`

`min() -> `[`Point3D`](Point3D.md)


## `max`

`max() -> `[`Point3D`](Point3D.md)


## `almost_equals`

`almost_equals(other: BBox3D, epsilon: float) -> bool`


**Parameters**

- `other` (`BBox3D`)
- `epsilon` (`float`)

## `contains`

`contains(p: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies inside this axis-aligned bounding box (inclusive of the boundary).

**Parameters**

- `p` ([`Point3D`](Point3D.md)) — The point to test.

**Returns** — true if p falls within the closed box [min, max] along each axis.


---

**See also:** [Point3D](Point3D.md)
