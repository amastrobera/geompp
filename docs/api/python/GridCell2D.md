# `GridCell2D` (Python)

Axis-aligned spatial hash bucket: the integer grid cell a [Point2D](Point2D.md) falls into at a given resolution.

Two points map to the same GridCell2D iff floor(x / epsilon) and floor(y / epsilon) both match — an O(1) approximate-equality test used to weld near-duplicate mesh vertices. epsilon defaults to DOUBLE_EPSILON , which tracks the same thread-local DECIMAL_PRECISION as AlmostEquals , but grid-cell bucketing is a different comparison algorithm than a direct pairwise distance check (see [Mesh2D](Mesh2D.md)::FromTriangles for the boundary-case tradeoff this implies).

## Fields

- `x` (`std::int64_t`)
- `y` (`std::int64_t`)

## `from_point`

**static** `from_point(p: `[`Point2D`](Point2D.md)`, epsilon: float) -> GridCell2D`

Buckets a point into its grid cell at the given resolution.

**Parameters**

- `p` ([`Point2D`](Point2D.md))
- `epsilon` (`float`) — The cell side length. Points within epsilon of each other are not guaranteed to land in the same cell if they straddle a cell boundary; points up to sqrt(2) * epsilon apart can land in the same cell if they don't.


---

**See also:** [Point2D](Point2D.md)
