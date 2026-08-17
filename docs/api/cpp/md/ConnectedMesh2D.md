# `ConnectedMesh2D` (C++)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

Per-facet edge adjacency is precomputed internally (see detail::TriangleCompactNeighborRef ) and exposed via [FaceView2D](FaceView2D.md)::Neighbor() / NeighborEntryEdge() . Each facet has no holes (a triangle cannot have one).

## `FromTriangles`

**static** `ConnectedMesh2D FromTriangles(std::vector< `[`Triangle2D`](Triangle2D.md)` > const & triangles)`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell2D](GridCell2D.md) ) into a single shared vertex, and precomputing per-facet edge adjacency.

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

## `Polygonize`

[`PolyMesh2D`](PolyMesh2D.md)` Polygonize(`[`PolygonizationParams`](PolygonizationParams.md)` const & params) const`

Merges coplanar, edge-adjacent facets into polygons, per params.strategy see [PolygonizationParams](PolygonizationParams.md) for what each strategy guarantees (planar boundary extraction, quads, or Hertel-Mehlhorn convex merging).

Reads NEIGHBORS/TRIANGLES/VERTICES directly (already precomputed at FromTriangles() time), so this needs no extra adjacency-building work of its own.

**Parameters**

- `params` ([`PolygonizationParams`](PolygonizationParams.md) const &) — Which polygonization strategy to run see [PolygonizationParams](PolygonizationParams.md)::Strategy .

**Returns** — A [PolyMesh2D](PolyMesh2D.md) of the merged polygon facets.


---

**See also:** [GridCell2D](GridCell2D.md), [PolyMesh2D](PolyMesh2D.md), [PolygonizationParams](PolygonizationParams.md), [Triangle2D](Triangle2D.md)
