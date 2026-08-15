# `ConnectedMesh2D` (C++)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

Per-facet edge adjacency is precomputed internally (see detail::TriangleCompactNeighborRef ) and exposed via [FaceView2D](FaceView2D.md)::Neighbor() / NeighborEntryEdge() . Each facet has no holes (a triangle cannot have one).

## `FromTriangles`

**static** `ConnectedMesh2D FromTriangles(std::vector< `[`Triangle2D`](Triangle2D.md)` > const & triangles)`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see GridCell2D ) into a single shared vertex, and precomputing per-facet edge adjacency.

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


---

**See also:** [Triangle2D](Triangle2D.md)
