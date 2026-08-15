# `ConnectedMesh3D` (C++)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

Per-facet edge adjacency is precomputed internally (see detail::TriangleCompactNeighborRef ) and exposed via [FaceView3D](FaceView3D.md)::Neighbor() / NeighborEntryEdge() . Each facet has no holes (a triangle cannot have one).

## `FromTriangles`

**static** `ConnectedMesh3D FromTriangles(std::vector< `[`Triangle3D`](Triangle3D.md)` > const & triangles)`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell3D](GridCell3D.md) ) into a single shared vertex, and precomputing per-facet edge adjacency.

**Parameters**

- `triangles` (std::vector< [`Triangle3D`](Triangle3D.md) > const &) — The triangles to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `Size`

`std::size_t Size() const`


## `Area`

`double Area() const`


## `Faces`

`auto Faces() const`

A lazy view of every facet, each rebuilt as a [Triangle3D](Triangle3D.md) on demand.

**Returns** — A std::ranges::view of [Triangle3D](Triangle3D.md) , one per facet, in the same order as the input.


---

**See also:** [GridCell3D](GridCell3D.md), [Triangle3D](Triangle3D.md)
