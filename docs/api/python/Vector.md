# `Vector` (Python)

Fixed-size, compile-time-dimensioned numeric vector (N known at compile time, same spirit as [Matrix](Matrix.md)<T, Rows, Cols> ).

Deliberately distinct from geompp::geometry::[Vector2D](Vector2D.md) /[Vector3D](Vector3D.md): those model a geometric displacement (Dot/Cross tied to [Point2D](Point2D.md)/3D, WKT, DECIMAL_PRECISION-aware AlmostEquals); this models a plain linear-algebra column vector for geompp::maths/geompp::transformations to build matrix machinery on top of, with no dependency on the geometry types at all. Size/type compatibility between operands (Vector+Vector, [Matrix](Matrix.md)*Vector) is enforced at compile time via template parameters rather than a runtime check: since N is part of the type, an incompatible pairing simply doesn't compile a stronger guarantee than a runtime throw, and it's caught at the call site instead of at test time. Runtime std::invalid_argument is reserved for failures that are genuinely only knowable at runtime (a zero-length vector in Normalized() , a singular matrix in [Matrix](Matrix.md)::Inverse() / solve_gauss() / solve_cramer()).

## `at`

`at(i: int) -> T`

Bounds-checked element access operator[] stays noexcept/unchecked (hot path, matches the [Matrix](Matrix.md)::operator() convention this module follows); use At() when the index isn't already known to be in range.

**Parameters**

- `i` (`int`)

`at(i: int) -> T`


**Parameters**

- `i` (`int`)

## `data`

`data() -> T`


`data() -> T`


## `x`

`x() -> T`


`x() -> T`


## `y`

`y() -> T`


`y() -> T`


## `z`

`z() -> T`


`z() -> T`


## `w`

`w() -> T`


`w() -> T`


## `dot`

`dot(o: Vector) -> T`


**Parameters**

- `o` (`Vector`)

## `cross`

`cross(o: Vector) -> Vector`

3D cross product only defined for N == 3 (a compile error otherwise, not a runtime one).

**Parameters**

- `o` (`Vector`)

## `length_squared`

`length_squared() -> T`


## `length`

`length() -> T`


## `normalized`

`normalized() -> Vector`


## `transpose`

`transpose() -> `[`Matrix`](Matrix.md)` < T, 1, N >`

Row-vector view of this (conceptually column) vector, as a 1xN [Matrix](Matrix.md) defined out-of-line in matrix.hpp , once [Matrix](Matrix.md)<T, 1, N> is a complete type.

## `__repr__`

`__repr__() -> str`


## `size`

**static** `size() -> int`


## `zero`

**static** `zero() -> Vector`



---

**See also:** [Matrix](Matrix.md)
