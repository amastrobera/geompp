# `Mesh3D` (C# / .NET)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

No adjacency structure is stored to find a face's neighbors. Each facet has no holes (a triangle cannot have one).

## `FromTriangles`

**static** `Mesh3D^ FromTriangles(std::vector< `[`Triangle3D`](Triangle3D.md)` > triangles)`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell3D](GridCell3D.md) ) into a single shared vertex.

**Parameters**

- `triangles` (std::vector< [`Triangle3D`](Triangle3D.md) >) — The triangles to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `Size`

`size_t Size()`


## `Area`

`double Area()`


## `Faces`

`auto Faces()`

A lazy view of every facet, each rebuilt as a [Triangle3D](Triangle3D.md) on demand.

**Returns** — A std::ranges::view of [Triangle3D](Triangle3D.md) , one per facet, in the same order as the input.

## `Connect`

[`ConnectedMesh3D`](ConnectedMesh3D.md)`^ Connect()`



---

**See also:** [ConnectedMesh3D](ConnectedMesh3D.md), [GridCell3D](GridCell3D.md), [Triangle3D](Triangle3D.md)
