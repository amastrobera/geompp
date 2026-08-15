# `Vector` (C# / .NET)

Fixed-size, compile-time-dimensioned numeric vector (N known at compile time, same spirit as [Matrix](Matrix.md)<T, Rows, Cols> ).

Deliberately distinct from geompp::geometry::[Vector2D](Vector2D.md) /[Vector3D](Vector3D.md): those model a geometric displacement (Dot/Cross tied to [Point2D](Point2D.md)/3D, WKT, DECIMAL_PRECISION-aware AlmostEquals); this models a plain linear-algebra column vector for geompp::maths/geompp::transformations to build matrix machinery on top of, with no dependency on the geometry types at all. Size/type compatibility between operands (Vector+Vector, [Matrix](Matrix.md)*Vector) is enforced at compile time via template parameters rather than a runtime check: since N is part of the type, an incompatible pairing simply doesn't compile a stronger guarantee than a runtime throw, and it's caught at the call site instead of at test time. Runtime std::invalid_argument is reserved for failures that are genuinely only knowable at runtime (a zero-length vector in Normalized() , a singular matrix in [Matrix](Matrix.md)::Inverse() / solve_gauss() / solve_cramer()).

## `At`

`T^ At(size_t i)`

Bounds-checked element access operator[] stays noexcept/unchecked (hot path, matches the [Matrix](Matrix.md)::operator() convention this module follows); use At() when the index isn't already known to be in range.

**Parameters**

- `i` (`size_t`)

`T^ At(size_t i)`


**Parameters**

- `i` (`size_t`)

## `data`

`T^ data()`


`T^ data()`


## `x`

`T^ x()`


`T^ x()`


## `y`

`T^ y()`


`T^ y()`


## `z`

`T^ z()`


`T^ z()`


## `w`

`T^ w()`


`T^ w()`


## `Dot`

`T^ Dot(Vector^ o)`


**Parameters**

- `o` (`Vector^`)

## `Cross`

`Vector^ Cross(Vector^ o)`

3D cross product only defined for N == 3 (a compile error otherwise, not a runtime one).

**Parameters**

- `o` (`Vector^`)

## `LengthSquared`

`T^ LengthSquared()`


## `Length`

`T^ Length()`


## `Normalized`

`Vector^ Normalized()`


## `Transpose`

[`Matrix`](Matrix.md)` < T, 1, N >^ Transpose()`

Row-vector view of this (conceptually column) vector, as a 1xN [Matrix](Matrix.md) defined out-of-line in matrix.hpp , once [Matrix](Matrix.md)<T, 1, N> is a complete type.

## `ToString`

`System::String^ ToString()`


## `Size`

**static** `size_t Size()`


## `Zero`

**static** `Vector^ Zero()`



---

**See also:** [Matrix](Matrix.md)
