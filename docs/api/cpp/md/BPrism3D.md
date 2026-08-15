# `BPrism3D` (C++)

## `center`

[`Point3D`](Point3D.md)` center() const`


## `axis_u`

[`Vector3D`](Vector3D.md)` axis_u() const`


## `axis_v`

[`Vector3D`](Vector3D.md)` axis_v() const`


## `axis_w`

[`Vector3D`](Vector3D.md)` axis_w() const`


## `half_len_u`

`double half_len_u() const`


## `half_len_v`

`double half_len_v() const`


## `half_len_w`

`double half_len_w() const`


## `width`

`double width() const`


## `height`

`double height() const`


## `depth`

`double depth() const`


## `volume`

`double volume() const`


## `Corners`

`std::array< `[`Point3D`](Point3D.md)` , 8 > Corners() const`


## `AlmostEquals`

`bool AlmostEquals(BPrism3D const & other, double epsilon) const`


**Parameters**

- `other` (`BPrism3D const &`)
- `epsilon` (`double`)

## `Contains`

`bool Contains(`[`Point3D`](Point3D.md)` const & p) const`

Tests whether a point lies inside this oriented bounding prism (inclusive of the boundary).

Projects the point onto the prism's local axes; returns true when all three projections are within [-half_len, half_len] along each axis.

**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)


---

**See also:** [Point3D](Point3D.md), [Vector3D](Vector3D.md)
