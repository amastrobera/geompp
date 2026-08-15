# `Mesh2D` (Python)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

No adjacency structure is stored to find a face's neighbors. Each facet has no holes (a triangle cannot have one).

## `from_triangles`

**static** `from_triangles(triangles: list[`[`Triangle2D`](Triangle2D.md)`]) -> Mesh2D`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see GridCell2D ) into a single shared vertex.

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

## `connect`

`connect() -> `[`ConnectedMesh2D`](ConnectedMesh2D.md)



---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [Triangle2D](Triangle2D.md)
