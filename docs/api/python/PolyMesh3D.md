# `PolyMesh3D` (Python)

A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer for efficiency No adjacency structure is stored to find a face's neighbors.

Each facet has no holes.

## `from_polygons`

**static** `from_polygons(polygons: list[`[`Polygon3D`](Polygon3D.md)`]) -> PolyMesh3D`

Builds a mesh from a set of (hole-free) polygons, welding vertices that land in the same spatial grid cell (see [GridCell3D](GridCell3D.md) ) into a single shared vertex.

**Parameters**

- `polygons` (list[`[Polygon3D`](Polygon3D.md)]) — The polygon faces to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ the sum of every polygon's Size() (fewer once shared vertices are welded).

## `from_wkt`

**static** `from_wkt(wkt: str) -> PolyMesh3D`

Parses a mesh written by ToWkt() (or matching its "POLYMESH (((...)), ...)" grammar) back into a PolyMesh3D .

Every facet must have at least 3 vertices.

**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> PolyMesh3D`

Reads a file written by ToFile() and parses it via FromWkt() .

**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `area`

`area() -> float`


## `faces`

`faces() -> auto`

A lazy view of every facet, each rebuilt as a [Polygon3D](Polygon3D.md) on demand.

**Returns** — A std::ranges::view of [Polygon3D](Polygon3D.md) , one per facet, in the same order as the input.

## `triangulate`

`triangulate(strategy: `[`TriangulationParams`](TriangulationParams.md)`::Strategy) -> `[`Mesh3D`](Mesh3D.md)

Returns a mesh of triangles instead of n-gons — every facet is triangulated independently (its own vertices are already simple, CCW-wound, and duplicate-free by construction, since every PolyMesh3D facet came from a validated [Polygon3D](Polygon3D.md) , so every [TriangulationParams](TriangulationParams.md) check is skipped) and the results are combined into one [Mesh3D](Mesh3D.md) .

**Parameters**

- `strategy` ([`TriangulationParams`](TriangulationParams.md)::Strategy) — The triangulation strategy to use. Options: EarClipping: O(n^2) worst case, but simple and robust for small polygons MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition into monotone pieces) Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle of all the angles of the triangles in the triangulation (avoiding skinny triangles)

**Returns** — A [Mesh3D](Mesh3D.md) with sum(facet_vertex_count - 2) triangles across every facet.

## `to_geometry_collection`

`to_geometry_collection() -> `[`GeometryCollection3D`](GeometryCollection3D.md)

Every facet, as its own standalone [Polygon3D](Polygon3D.md) , packaged into one [GeometryCollection3D](GeometryCollection3D.md) .

**Returns** — A [GeometryCollection3D](GeometryCollection3D.md) with Size() entries, all [Polygon3D](Polygon3D.md) , same order as Faces() .

## `to_wkt`

`to_wkt() -> str`

WKT-like serialization, specific to this library: "POLYMESH ((x0 y0 z0, ..., x0 y0 z0), ...)" one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed by repeating its first point, comma-separated, wrapped once more in "POLYMESH ( ... )".

Not a standard OGC WKT geometry type.

**Returns** — The serialized mesh, one facet ring per facet, in Faces() order.

## `to_file`

`to_file(path: str) -> None`

Writes ToWkt() 's output to path (plain text, truncates any existing content).

**Parameters**

- `path` (`str`)


---

**See also:** [GeometryCollection3D](GeometryCollection3D.md), [GridCell3D](GridCell3D.md), [Mesh3D](Mesh3D.md), [Polygon3D](Polygon3D.md), [TriangulationParams](TriangulationParams.md)
