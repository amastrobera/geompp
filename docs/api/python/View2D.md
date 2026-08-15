# `View2D` (Python)

## `xy`

**static** `xy(z_offset: float) -> View2D`


**Parameters**

- `z_offset` (`float`)

## `yz`

**static** `yz(x_offset: float) -> View2D`


**Parameters**

- `x_offset` (`float`)

## `zx`

**static** `zx(y_offset: float) -> View2D`


**Parameters**

- `y_offset` (`float`)

## `on_plane`

**static** `on_plane(ref_plane: `[`Plane`](Plane.md)`) -> View2D`


**Parameters**

- `ref_plane` ([`Plane`](Plane.md))

## `x`

`x(p: `[`Point2D`](Point2D.md)`) -> float`


**Parameters**

- `p` ([`Point2D`](Point2D.md))

`x(p: `[`Point3D`](Point3D.md)`) -> float`


**Parameters**

- `p` ([`Point3D`](Point3D.md))

## `y`

`y(p: `[`Point2D`](Point2D.md)`) -> float`


**Parameters**

- `p` ([`Point2D`](Point2D.md))

`y(p: `[`Point3D`](Point3D.md)`) -> float`


**Parameters**

- `p` ([`Point3D`](Point3D.md))

## `xy`

`xy(p: `[`Point2D`](Point2D.md)`) -> std::pair< double, double >`


**Parameters**

- `p` ([`Point2D`](Point2D.md))

`xy(p: `[`Point3D`](Point3D.md)`) -> std::pair< double, double >`


**Parameters**

- `p` ([`Point3D`](Point3D.md))

## `xyz`

`xyz(p: `[`Point2D`](Point2D.md)`) -> `[`Point3D`](Point3D.md)

Reconstructs the 3D point that a (x, y) pair in this view's 2D space corresponds to — the inverse of x()/y()/xy() on [Point3D](Point3D.md) .

**Parameters**

- `p` ([`Point2D`](Point2D.md))

`xyz(x: float, y: float) -> `[`Point3D`](Point3D.md)


**Parameters**

- `x` (`float`)
- `y` (`float`)

## `type`

`type() -> ProjectionType`



---

**See also:** [Plane](Plane.md), [Point2D](Point2D.md), [Point3D](Point3D.md)
