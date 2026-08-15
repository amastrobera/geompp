# `Matrix` (Python)

Fixed-size, compile-time-dimensioned (Rows x Cols known at compile time) numeric matrix, stored row-major in a flat buffer: m_data[r * Cols + c] .

See [Vector](Vector.md)<T,N> 's own docs for why size/type compatibility between operands is enforced via template parameters (a compile error on mismatch) rather than a runtime throw Matrix*Matrix/Matrix*[Vector](Vector.md) below follow the same reasoning.

## `at`

`at(r: int, c: int) -> T`

Bounds-checked element access operator() stays noexcept/unchecked (hot path, matches [Vector](Vector.md)::operator[]'s convention); use At() when (r, c) isn't already known to be in range.

**Parameters**

- `r` (`int`)
- `c` (`int`)

`at(r: int, c: int) -> T`


**Parameters**

- `r` (`int`)
- `c` (`int`)

## `data`

`data() -> T`


`data() -> T`


## `transpose`

`transpose() -> Matrix< T, Cols, Rows >`


## `determinant`

`determinant() -> T`

Determinant via recursive cofactor expansion (detail::determinant_generic) square only.

## `inverse`

`inverse() -> Matrix`

Matrix inverse via Gauss-Jordan elimination on [A | I] (detail::gauss_jordan_eliminate) square only.

## `__repr__`

`__repr__() -> str`


## `row_count`

**static** `row_count() -> int`


## `col_count`

**static** `col_count() -> int`


## `zero`

**static** `zero() -> Matrix`


## `identity`

**static** `identity() -> Matrix`


## `translation`

**static** `translation(offset: `[`Vector`](Vector.md)` < T, 3 >) -> Matrix`


**Parameters**

- `offset` ([`Vector`](Vector.md) < T, 3 >)

**static** `translation(offset: `[`Vector`](Vector.md)` < T, 2 >) -> Matrix`


**Parameters**

- `offset` ([`Vector`](Vector.md) < T, 2 >)

## `rotation`

**static** `rotation(angle_rad: T, axis: `[`Vector`](Vector.md)` < T, 3 >) -> Matrix`

Axis-angle rotation (Rodrigues' rotation formula) about axis through the origin, by angle_rad radians (right-hand rule).

**Parameters**

- `angle_rad` (`T`)
- `axis` ([`Vector`](Vector.md) < T, 3 >)

**static** `rotation(angle_rad: T) -> Matrix`


**Parameters**

- `angle_rad` (`T`)

## `scale`

**static** `scale(factor: T) -> Matrix`


**Parameters**

- `factor` (`T`)

**static** `scale(sx: T, sy: T, sz: T) -> Matrix`


**Parameters**

- `sx` (`T`)
- `sy` (`T`)
- `sz` (`T`)

**static** `scale(factor: T) -> Matrix`


**Parameters**

- `factor` (`T`)

**static** `scale(sx: T, sy: T) -> Matrix`


**Parameters**

- `sx` (`T`)
- `sy` (`T`)

## `shear`

**static** `shear(xy: T, xz: T, yx: T, yz: T, zx: T, zy: T) -> Matrix`

General 3D shear: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.) the 6 off-diagonal terms of the linear 3x3 block, embedded in the homogeneous 4x4 with no translation.

**Parameters**

- `xy` (`T`)
- `xz` (`T`)
- `yx` (`T`)
- `yz` (`T`)
- `zx` (`T`)
- `zy` (`T`)

**static** `shear(shx: T, shy: T) -> Matrix`

2D shear: shx shears X by a multiple of Y, shy shears Y by a multiple of X the 2 off-diagonal terms of the linear 2x2 block, embedded in the homogeneous 3x3 with no translation.

**Parameters**

- `shx` (`T`)
- `shy` (`T`)

## `reflection`

**static** `reflection(normal: `[`Vector`](Vector.md)` < T, 3 >) -> Matrix`

Householder reflection across the plane through the origin whose normal is normal ( R = I - 2 n n^T ), embedded in the homogeneous 4x4 with no translation.

**Parameters**

- `normal` ([`Vector`](Vector.md) < T, 3 >)

**static** `reflection(normal: `[`Vector`](Vector.md)` < T, 2 >) -> Matrix`

Householder reflection across the line through the origin whose normal is normal ( R = I - 2 n n^T ), embedded in the homogeneous 3x3 with no translation.

**Parameters**

- `normal` ([`Vector`](Vector.md) < T, 2 >)


---

**See also:** [Vector](Vector.md)
