# `Mesh2D` (C++)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

No adjacency structure is stored to find a face's neighbors. Each facet has no holes (a triangle cannot have one).

## `FromTriangles`

**static** `Mesh2D FromTriangles(std::vector< `[`Triangle2D`](Triangle2D.md)` > const & triangles)`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell2D](GridCell2D.md) ) into a single shared vertex.

**Parameters**

- `triangles` (std::vector< [`Triangle2D`](Triangle2D.md) > const &) — The triangles to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `Size`

`std::size_t Size() const`


## `Area`

`double Area() const`


## `Faces`

`auto Faces() const`

A lazy view of every facet, each rebuilt as a [Triangle2D](Triangle2D.md) on demand.

**Returns** — A std::ranges::view of [Triangle2D](Triangle2D.md) , one per facet, in the same order as the input.

## `Connect`

[`ConnectedMesh2D`](ConnectedMesh2D.md)` Connect() const`



---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [GridCell2D](GridCell2D.md), [Triangle2D](Triangle2D.md)
