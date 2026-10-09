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

## `Polygonize`

[`PolyMesh3D`](PolyMesh3D.md)` Polygonize(`[`PolygonizationParams`](PolygonizationParams.md)` const & params) const`

Merges coplanar, edge-adjacent facets into polygons, per params.strategy see [PolygonizationParams](PolygonizationParams.md) for what each strategy guarantees (planar boundary extraction, quads, or Hertel-Mehlhorn convex merging).

Reads NEIGHBORS/TRIANGLES/VERTICES directly (already precomputed at FromTriangles() time), so this needs no extra adjacency-building work of its own.

**Parameters**

- `params` ([`PolygonizationParams`](PolygonizationParams.md) const &) — Which polygonization strategy to run see [PolygonizationParams](PolygonizationParams.md)::Strategy .

**Returns** — A [PolyMesh3D](PolyMesh3D.md) of the merged polygon facets.

## `Disconnect`

[`Mesh3D`](Mesh3D.md)` Disconnect() const`

Drops this mesh's precomputed adjacency (NEIGHBORS), keeping the same welded vertices and facets the inverse of [Mesh3D](Mesh3D.md)::Connect().

VERTICES is reused as-is (O(1) refcount bump); only TRIANGLES' flat std::vector<size_t> layout needs repacking into [Mesh3D](Mesh3D.md)::FACE_INDICES' std::vector<array<size_t,3>> one, so this is O(n) with no re-welding or re-validation.

**Returns** — A [Mesh3D](Mesh3D.md) over the exact same vertices/facets, with no adjacency structure.


---

**See also:** [GridCell3D](GridCell3D.md), [Mesh3D](Mesh3D.md), [PolyMesh3D](PolyMesh3D.md), [PolygonizationParams](PolygonizationParams.md), [Triangle3D](Triangle3D.md)
