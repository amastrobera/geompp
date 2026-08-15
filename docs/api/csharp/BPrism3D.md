# `BPrism3D` (C# / .NET)

## `center`

[`Point3D`](Point3D.md)`^ center()`


## `axis_u`

[`Vector3D`](Vector3D.md)`^ axis_u()`


## `axis_v`

[`Vector3D`](Vector3D.md)`^ axis_v()`


## `axis_w`

[`Vector3D`](Vector3D.md)`^ axis_w()`


## `half_len_u`

`double half_len_u()`


## `half_len_v`

`double half_len_v()`


## `half_len_w`

`double half_len_w()`


## `width`

`double width()`


## `height`

`double height()`


## `depth`

`double depth()`


## `volume`

`double volume()`


## `Corners`

`std::array< `[`Point3D`](Point3D.md)` , 8 > Corners()`


## `AlmostEquals`

`bool AlmostEquals(BPrism3D^ other, double epsilon)`


**Parameters**

- `other` (`BPrism3D^`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)`^ p)`

Tests whether a point lies inside this oriented bounding prism (inclusive of the boundary).

Projects the point onto the prism's local axes; returns true when all three projections are within [-half_len, half_len] along each axis.

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)


---

**See also:** [Point3D](Point3D.md), [Vector3D](Vector3D.md)
