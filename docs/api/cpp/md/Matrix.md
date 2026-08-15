# `Matrix` (C++)

Fixed-size, compile-time-dimensioned (Rows x Cols known at compile time) numeric matrix, stored row-major in a flat buffer: m_data[r * Cols + c] .

See [Vector](Vector.md)<T,N> 's own docs for why size/type compatibility between operands is enforced via template parameters (a compile error on mismatch) rather than a runtime throw Matrix*Matrix/Matrix*[Vector](Vector.md) below follow the same reasoning.

## `At`

`T & At(std::size_t r, std::size_t c)`

Bounds-checked element access operator() stays noexcept/unchecked (hot path, matches [Vector](Vector.md)::operator[]'s convention); use At() when (r, c) isn't already known to be in range.

**Parameters**

- `r` (`std::size_t`)
- `c` (`std::size_t`)

`T const & At(std::size_t r, std::size_t c) const`


**Parameters**

- `r` (`std::size_t`)
- `c` (`std::size_t`)

## `data`

`T * data()`


`T const * data() const`


## `Transpose`

`Matrix< T, Cols, Rows > Transpose() const`


## `Determinant`

`T Determinant() const`

Determinant via recursive cofactor expansion (detail::determinant_generic) square only.

## `Inverse`

`Matrix Inverse() const`

Matrix inverse via Gauss-Jordan elimination on [A | I] (detail::gauss_jordan_eliminate) square only.

## `ToString`

`std::string ToString() const`


## `RowCount`

**static** `std::size_t RowCount()`


## `ColCount`

**static** `std::size_t ColCount()`


## `Zero`

**static** `Matrix Zero()`


## `Identity`

**static** `Matrix Identity()`


## `Translation`

**static** `Matrix Translation(`[`Vector`](Vector.md)` < T, 3 > const & offset)`


**Parameters**

- `offset` ([`Vector`](Vector.md) < T, 3 > const &)

**static** `Matrix Translation(`[`Vector`](Vector.md)` < T, 2 > const & offset)`


**Parameters**

- `offset` ([`Vector`](Vector.md) < T, 2 > const &)

## `Rotation`

**static** `Matrix Rotation(T angle_rad, `[`Vector`](Vector.md)` < T, 3 > const & axis)`

Axis-angle rotation (Rodrigues' rotation formula) about axis through the origin, by angle_rad radians (right-hand rule).

**Parameters**

- `angle_rad` (`T`)
- `axis` ([`Vector`](Vector.md) < T, 3 > const &)

**static** `Matrix Rotation(T angle_rad)`


**Parameters**

- `angle_rad` (`T`)

## `Scale`

**static** `Matrix Scale(T factor)`


**Parameters**

- `factor` (`T`)

**static** `Matrix Scale(T sx, T sy, T sz)`


**Parameters**

- `sx` (`T`)
- `sy` (`T`)
- `sz` (`T`)

**static** `Matrix Scale(T factor)`


**Parameters**

- `factor` (`T`)

**static** `Matrix Scale(T sx, T sy)`


**Parameters**

- `sx` (`T`)
- `sy` (`T`)

## `Shear`

**static** `Matrix Shear(T xy, T xz, T yx, T yz, T zx, T zy)`

General 3D shear: each axis is offset by a multiple of the other two ( xy shears X by Y, zy shears Z by Y, etc.) the 6 off-diagonal terms of the linear 3x3 block, embedded in the homogeneous 4x4 with no translation.

**Parameters**

- `xy` (`T`)
- `xz` (`T`)
- `yx` (`T`)
- `yz` (`T`)
- `zx` (`T`)
- `zy` (`T`)

**static** `Matrix Shear(T shx, T shy)`

2D shear: shx shears X by a multiple of Y, shy shears Y by a multiple of X the 2 off-diagonal terms of the linear 2x2 block, embedded in the homogeneous 3x3 with no translation.

**Parameters**

- `shx` (`T`)
- `shy` (`T`)

## `Reflection`

**static** `Matrix Reflection(`[`Vector`](Vector.md)` < T, 3 > const & normal)`

Householder reflection across the plane through the origin whose normal is normal ( R = I - 2 n n^T ), embedded in the homogeneous 4x4 with no translation.

**Parameters**

- `normal` ([`Vector`](Vector.md) < T, 3 > const &)

**static** `Matrix Reflection(`[`Vector`](Vector.md)` < T, 2 > const & normal)`

Householder reflection across the line through the origin whose normal is normal ( R = I - 2 n n^T ), embedded in the homogeneous 3x3 with no translation.

**Parameters**

- `normal` ([`Vector`](Vector.md) < T, 2 > const &)


---

**See also:** [Vector](Vector.md)
