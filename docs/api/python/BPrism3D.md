# `BPrism3D` (Python)

## `center`

`center() -> `[`Point3D`](Point3D.md)


## `axis_u`

`axis_u() -> `[`Vector3D`](Vector3D.md)


## `axis_v`

`axis_v() -> `[`Vector3D`](Vector3D.md)


## `axis_w`

`axis_w() -> `[`Vector3D`](Vector3D.md)


## `half_len_u`

`half_len_u() -> float`


## `half_len_v`

`half_len_v() -> float`


## `half_len_w`

`half_len_w() -> float`


## `width`

`width() -> float`


## `height`

`height() -> float`


## `depth`

`depth() -> float`


## `volume`

`volume() -> float`


## `corners`

`corners() -> std::array< `[`Point3D`](Point3D.md)` , 8 >`


## `almost_equals`

`almost_equals(other: BPrism3D, epsilon: float) -> bool`


**Parameters**

- `other` (`BPrism3D`)
- `epsilon` (`float`)

## `contains`

`contains(p: `[`Point3D`](Point3D.md)`) -> bool`

Tests whether a point lies inside this oriented bounding prism (inclusive of the boundary).

Projects the point onto the prism's local axes; returns true when all three projections are within [-half_len, half_len] along each axis.

**Parameters**

- `p` ([`Point3D`](Point3D.md))


---

**See also:** [Point3D](Point3D.md), [Vector3D](Vector3D.md)
