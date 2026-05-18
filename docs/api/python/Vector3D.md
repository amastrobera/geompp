# `Vector3D` (Python)

## `x`

`x() -> float`


## `y`

`y() -> float`


## `z`

`z() -> float`


## `to_point`

`to_point() -> `[`Point3D`](Point3D.md)


## `length`

`length() -> float`


## `dominant_axis`

`dominant_axis() -> Axis`


## `almost_equals`

`almost_equals(other: Vector3D, epsilon: float) -> bool`


**Parameters**

- `other` (`Vector3D`)
- `epsilon` (`float`)

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `dot`

`dot(other: Vector3D) -> float`


**Parameters**

- `other` (`Vector3D`)

## `cross`

`cross(other: Vector3D) -> Vector3D`


**Parameters**

- `other` (`Vector3D`)

## `perp`

`perp() -> Vector3D`


## `normalize`

`normalize() -> Vector3D`


## `is_parallel`

`is_parallel(other: Vector3D) -> bool`


**Parameters**

- `other` (`Vector3D`)

## `from_wkt`

**static** `from_wkt(wkt: str) -> Vector3D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Vector3D`


**Parameters**

- `path` (`str`)

## `basis_x`

**static** `basis_x() -> Vector3D`


## `basis_y`

**static** `basis_y() -> Vector3D`


## `basis_z`

**static** `basis_z() -> Vector3D`



---

**See also:** [Point3D](Point3D.md)
