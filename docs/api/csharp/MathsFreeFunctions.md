# `MathsFreeFunctions` (C# / .NET)

Free functions in `geompp::maths` (not methods on a class — call them directly).

## `SolveGauss`

[`Vector`](Vector.md)` < T, N >^ SolveGauss(`[`Matrix`](Matrix.md)` < T, N, N >^ a, `[`Vector`](Vector.md)` < T, N >^ b)`

Solves the square linear system A x = b via Gauss-Jordan elimination with partial pivoting (detail::gauss_jordan_eliminate on the augmented system [A | b]).

**Parameters**

- `a` ([`Matrix`](Matrix.md) < T, N, N >^) — the system's N x N coefficient matrix.
- `b` ([`Vector`](Vector.md) < T, N >^) — the N known terms.

**Returns** — the N unknowns x such that a * x == b.

## `SolveCramer`

[`Vector`](Vector.md)` < T, N >^ SolveCramer(`[`Matrix`](Matrix.md)` < T, N, N >^ a, `[`Vector`](Vector.md)` < T, N >^ b)`

Solves the square linear system A x = b via Cramer's rule: x_i = det(A with column i replaced
by b) / det(A).

O(N) determinant evaluations of an N x N matrix (each itself O(N!) via detail::determinant_generic) the classic textbook method, considerably more expensive than solve_gauss() for anything past N == 3 or 4, but a direct, non-iterative closed form some callers specifically want (e.g. to inspect an individual unknown's determinant ratio).

**Parameters**

- `a` ([`Matrix`](Matrix.md) < T, N, N >^) — the system's N x N coefficient matrix.
- `b` ([`Vector`](Vector.md) < T, N >^) — the N known terms.

**Returns** — the N unknowns x such that a * x == b.


---

**See also:** [Matrix](Matrix.md), [Vector](Vector.md)
