# `GridCell3D` (C# / .NET)

Axis-aligned spatial hash bucket: the integer grid cell a [Point3D](Point3D.md) falls into at a given resolution.

Two points map to the same GridCell3D iff floor(x / epsilon) , floor(y / epsilon) , and floor(z / epsilon) all match — an O(1) approximate-equality test used to weld near-duplicate mesh vertices. epsilon defaults to DOUBLE_EPSILON , which tracks the same thread-local DECIMAL_PRECISION as AlmostEquals , but grid-cell bucketing is a different comparison algorithm than a direct pairwise distance check (see [Mesh3D](Mesh3D.md)::FromTriangles for the boundary-case tradeoff this implies).

## Fields

- `x` (`std::int64_t`)
- `y` (`std::int64_t`)
- `z` (`std::int64_t`)

## `FromPoint`

**static** `GridCell3D^ FromPoint(`[`Point3D`](Point3D.md)`^ p, double epsilon)`

Buckets a point into its grid cell at the given resolution.

**Parameters**

- `p` ([`Point3D`](Point3D.md)^)
- `epsilon` (`double`) — The cell side length. Points within epsilon of each other are not guaranteed to land in the same cell if they straddle a cell boundary; points up to sqrt(3) * epsilon apart can land in the same cell if they don't.


---

**See also:** [Point3D](Point3D.md)
