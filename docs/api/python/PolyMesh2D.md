# `PolyMesh2D` (Python)

A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer for efficiency No adjacency structure is stored to find a face's neighbors.

Each facet has no holes.

## `from_polygons`

**static** `from_polygons(polygons: list[`[`Polygon2D`](Polygon2D.md)`]) -> PolyMesh2D`

Builds a mesh from a set of (hole-free) polygons, welding vertices that land in the same spatial grid cell (see GridCell2D ) into a single shared vertex.

**Parameters**

- `polygons` (list[`[Polygon2D`](Polygon2D.md)]) — The polygon faces to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ the sum of every polygon's Size() (fewer once shared vertices are welded).

## `size`

`size() -> int`


## `area`

`area() -> float`


## `faces`

`faces() -> auto`

A lazy view of every facet, each rebuilt as a [Polygon2D](Polygon2D.md) on demand.

**Returns** — A std::ranges::view of [Polygon2D](Polygon2D.md) , one per facet, in the same order as the input.

## `triangulate`

`triangulate(strategy: TriangulationParams::Strategy) -> `[`Mesh2D`](Mesh2D.md)

Returns a mesh of triangles instead of n-gons — every facet is triangulated independently (its own vertices are already simple, CCW-wound, and duplicate-free by construction, since every PolyMesh2D facet came from a validated [Polygon2D](Polygon2D.md) , so every TriangulationParams check is skipped) and the results are combined into one [Mesh2D](Mesh2D.md) .

**Parameters**

- `strategy` (`TriangulationParams::Strategy`) — The triangulation strategy to use. Options: EarClipping: O(n^2) worst case, but simple and robust for small polygons MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition into monotone pieces) Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle of all the angles of the triangles in the triangulation (avoiding skinny triangles)

**Returns** — A [Mesh2D](Mesh2D.md) with sum(facet_vertex_count - 2) triangles across every facet.


---

**See also:** [Mesh2D](Mesh2D.md), [Polygon2D](Polygon2D.md)
