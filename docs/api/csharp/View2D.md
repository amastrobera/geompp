# `View2D` (C# / .NET)

## `XY`

**static** `View2D^ XY(double z_offset)`


**Parameters**

- `z_offset` (`double`)

## `YZ`

**static** `View2D^ YZ(double x_offset)`


**Parameters**

- `x_offset` (`double`)

## `ZX`

**static** `View2D^ ZX(double y_offset)`


**Parameters**

- `y_offset` (`double`)

## `OnPlane`

**static** `View2D^ OnPlane(`[`Plane`](Plane.md)`^ ref_plane)`


**Parameters**

- `ref_plane` ([`Plane`](Plane.md)^)

## `x`

`double x(`[`Point2D`](Point2D.md)`^ p)`


**Parameters**

- `p` ([`Point2D`](Point2D.md)^)

`double x(`[`Point3D`](Point3D.md)`^ p)`


**Parameters**

- `p` ([`Point3D`](Point3D.md)^)

## `y`

`double y(`[`Point2D`](Point2D.md)`^ p)`


**Parameters**

- `p` ([`Point2D`](Point2D.md)^)

`double y(`[`Point3D`](Point3D.md)`^ p)`


**Parameters**

- `p` ([`Point3D`](Point3D.md)^)

## `xy`

`std::pair< double, double > xy(`[`Point2D`](Point2D.md)`^ p)`


**Parameters**

- `p` ([`Point2D`](Point2D.md)^)

`std::pair< double, double > xy(`[`Point3D`](Point3D.md)`^ p)`


**Parameters**

- `p` ([`Point3D`](Point3D.md)^)

## `xyz`

[`Point3D`](Point3D.md)`^ xyz(`[`Point2D`](Point2D.md)`^ p)`

Reconstructs the 3D point that a (x, y) pair in this view's 2D space corresponds to — the inverse of x()/y()/xy() on [Point3D](Point3D.md) .

**Parameters**

- `p` ([`Point2D`](Point2D.md)^)

[`Point3D`](Point3D.md)`^ xyz(double x, double y)`


**Parameters**

- `x` (`double`)
- `y` (`double`)

## `type`

`ProjectionType^ type()`



---

**See also:** [Plane](Plane.md), [Point2D](Point2D.md), [Point3D](Point3D.md)
