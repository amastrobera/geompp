# `ConnectedMesh2D` (Python)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

Per-facet edge adjacency is precomputed internally (see detail::TriangleCompactNeighborRef ) and exposed via [FaceView2D](FaceView2D.md)::Neighbor() / NeighborEntryEdge() . Each facet has no holes (a triangle cannot have one).

## `from_triangles`

**static** `from_triangles(triangles: list[`[`Triangle2D`](Triangle2D.md)`]) -> ConnectedMesh2D`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see GridCell2D ) into a single shared vertex, and precomputing per-facet edge adjacency.

**Parameters**

- `triangles` (list[`[Triangle2D`](Triangle2D.md)]) — The triangles to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `size`

`size() -> int`


## `area`

`area() -> float`


## `faces`

`faces() -> auto`

A lazy view of every facet, each rebuilt as a [Triangle2D](Triangle2D.md) on demand.

**Returns** — A std::ranges::view of [Triangle2D](Triangle2D.md) , one per facet, in the same order as the input.


---

**See also:** [Triangle2D](Triangle2D.md)
