# `ConnectedMesh3D` (Python)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

Per-facet edge adjacency is precomputed internally (see detail::TriangleCompactNeighborRef ) and exposed via [FaceView3D](FaceView3D.md)::Neighbor() / NeighborEntryEdge() . Each facet has no holes (a triangle cannot have one).

## `from_triangles`

**static** `from_triangles(triangles: list[`[`Triangle3D`](Triangle3D.md)`]) -> ConnectedMesh3D`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell3D](GridCell3D.md) ) into a single shared vertex, and precomputing per-facet edge adjacency.

**Parameters**

- `triangles` (list[`[Triangle3D`](Triangle3D.md)]) — The triangles to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `size`

`size() -> int`


## `area`

`area() -> float`


## `faces`

`faces() -> auto`

A lazy view of every facet, each rebuilt as a [Triangle3D](Triangle3D.md) on demand.

**Returns** — A std::ranges::view of [Triangle3D](Triangle3D.md) , one per facet, in the same order as the input.

## `polygonize`

`polygonize(params: `[`PolygonizationParams`](PolygonizationParams.md)`) -> `[`PolyMesh3D`](PolyMesh3D.md)

Merges coplanar, edge-adjacent facets into polygons, per params.strategy see [PolygonizationParams](PolygonizationParams.md) for what each strategy guarantees (planar boundary extraction, quads, or Hertel-Mehlhorn convex merging).

Reads NEIGHBORS/TRIANGLES/VERTICES directly (already precomputed at FromTriangles() time), so this needs no extra adjacency-building work of its own.

**Parameters**

- `params` ([`PolygonizationParams`](PolygonizationParams.md)) — Which polygonization strategy to run see [PolygonizationParams](PolygonizationParams.md)::Strategy .

**Returns** — A [PolyMesh3D](PolyMesh3D.md) of the merged polygon facets.


---

**See also:** [GridCell3D](GridCell3D.md), [PolyMesh3D](PolyMesh3D.md), [PolygonizationParams](PolygonizationParams.md), [Triangle3D](Triangle3D.md)
