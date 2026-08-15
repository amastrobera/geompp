# `View2D` (C++)

## `XY`

**static** `View2D XY(double z_offset)`


**Parameters**

- `z_offset` (`double`)

## `YZ`

**static** `View2D YZ(double x_offset)`


**Parameters**

- `x_offset` (`double`)

## `ZX`

**static** `View2D ZX(double y_offset)`


**Parameters**

- `y_offset` (`double`)

## `OnPlane`

**static** `View2D OnPlane(`[`Plane`](Plane.md)` ref_plane)`


**Parameters**

- `ref_plane` ([`Plane`](Plane.md))

## `x`

`double x(`[`Point2D`](Point2D.md)` const & p) const`


**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)

`double x(`[`Point3D`](Point3D.md)` const & p) const`


**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)

## `y`

`double y(`[`Point2D`](Point2D.md)` const & p) const`


**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)

`double y(`[`Point3D`](Point3D.md)` const & p) const`


**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)

## `xy`

`std::pair< double, double > xy(`[`Point2D`](Point2D.md)` const & p) const`


**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)

`std::pair< double, double > xy(`[`Point3D`](Point3D.md)` const & p) const`


**Parameters**

- `p` ([`Point3D`](Point3D.md) const &)

## `xyz`

[`Point3D`](Point3D.md)` xyz(`[`Point2D`](Point2D.md)` const & p) const`

Reconstructs the 3D point that a (x, y) pair in this view's 2D space corresponds to — the inverse of x()/y()/xy() on [Point3D](Point3D.md) .

**Parameters**

- `p` ([`Point2D`](Point2D.md) const &)

[`Point3D`](Point3D.md)` xyz(double x, double y) const`


**Parameters**

- `x` (`double`)
- `y` (`double`)

## `type`

`ProjectionType type() const`



---

**See also:** [Plane](Plane.md), [Point2D](Point2D.md), [Point3D](Point3D.md)
