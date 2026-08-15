# `Vector2D` (Python)

## `x`

`x() -> float`


## `y`

`y() -> float`


## `to_point`

`to_point() -> `[`Point2D`](Point2D.md)


## `length`

`length() -> float`


## `almost_equals`

`almost_equals(other: Vector2D, epsilon: float) -> bool`


**Parameters**

- `other` (`Vector2D`)
- `epsilon` (`float`)

## `to_wkt`

`to_wkt() -> str`


## `to_file`

`to_file(path: str) -> None`


**Parameters**

- `path` (`str`)

## `dot`

`dot(v: Vector2D) -> float`


**Parameters**

- `v` (`Vector2D`)

## `cross`

`cross(v: Vector2D) -> float`


**Parameters**

- `v` (`Vector2D`)

## `perp`

`perp() -> Vector2D`


## `normalize`

`normalize() -> Vector2D`


## `is_parallel`

`is_parallel(other: Vector2D) -> bool`


**Parameters**

- `other` (`Vector2D`)

## `from_wkt`

**static** `from_wkt(wkt: str) -> Vector2D`


**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Vector2D`


**Parameters**

- `path` (`str`)

## `basis_x`

**static** `basis_x() -> Vector2D`


## `basis_y`

**static** `basis_y() -> Vector2D`



---

**See also:** [Point2D](Point2D.md)
