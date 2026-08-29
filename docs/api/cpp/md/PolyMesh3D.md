# `PolyMesh3D` (C++)

A mesh made of adjacent, arbitrary-sided polygonal faces, stored as a flat index buffer for efficiency No adjacency structure is stored to find a face's neighbors.

Each facet has no holes.

## `FromPolygons`

**static** `PolyMesh3D FromPolygons(std::vector< `[`Polygon3D`](Polygon3D.md)` > const & polygons, AdjacencyConformity conformity)`

Builds a mesh from a set of (hole-free) polygons, welding vertices that land in the same spatial grid cell (see [GridCell3D](GridCell3D.md) ) into a single shared vertex.

**Parameters**

- `polygons` (std::vector< [`Polygon3D`](Polygon3D.md) > const &) — The polygon faces to weld into a mesh. Order is not required to reflect adjacency.
- `conformity` (`AdjacencyConformity`) — decides how to handle input that doesn't make a valid adjacency (default to Assert, aka "make it fail if not perfect"; can be set to Enforce, aka "fix it if you can" via fix_adjacency(), or to Guaranteed to skip the check entirely and run at your own risk)

**Returns** — A mesh whose vertex count is ≤ the sum of every polygon's Size() (fewer once shared vertices are welded).

## `FromWkt`

**static** `PolyMesh3D FromWkt(std::string const & wkt)`

Parses a mesh written by ToWkt() (or matching its "POLYMESH (((...)), ...)" grammar) back into a PolyMesh3D .

Every facet must have at least 3 vertices.

**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `PolyMesh3D FromFile(std::string const & path)`

Reads a file written by ToFile() and parses it via FromWkt() .

**Parameters**

- `path` (`std::string const &`)

## `Size`

`std::size_t Size() const`


## `Area`

`double Area() const`


## `Faces`

`auto Faces() const`

A lazy view of every facet, each rebuilt as a [Polygon3D](Polygon3D.md) on demand.

**Returns** — A std::ranges::view of [Polygon3D](Polygon3D.md) , one per facet, in the same order as the input.

## `Triangulate`

[`Mesh3D`](Mesh3D.md)` Triangulate(`[`TriangulationParams`](TriangulationParams.md)`::Strategy strategy) const`

Returns a mesh of triangles instead of n-gons — every facet is triangulated independently (its own vertices are already simple, CCW-wound, and duplicate-free by construction, since every PolyMesh3D facet came from a validated [Polygon3D](Polygon3D.md) , so every [TriangulationParams](TriangulationParams.md) check is skipped) and the results are combined into one [Mesh3D](Mesh3D.md) .

**Parameters**

- `strategy` ([`TriangulationParams`](TriangulationParams.md)::Strategy) — The triangulation strategy to use. Options: EarClipping: O(n^2) worst case, but simple and robust for small polygons MonotonePolygon: O(n log n) worst case, but requires a monotone polygon (or a decomposition into monotone pieces) Delaunay: O(n log n) worst case, but produces a triangulation that maximizes the minimum angle of all the angles of the triangles in the triangulation (avoiding skinny triangles)

**Returns** — A [Mesh3D](Mesh3D.md) with sum(facet_vertex_count - 2) triangles across every facet.

## `ToGeometryCollection`

[`GeometryCollection3D`](GeometryCollection3D.md)` ToGeometryCollection() const`

Every facet, as its own standalone [Polygon3D](Polygon3D.md) , packaged into one [GeometryCollection3D](GeometryCollection3D.md) .

**Returns** — A [GeometryCollection3D](GeometryCollection3D.md) with Size() entries, all [Polygon3D](Polygon3D.md) , same order as Faces() .

## `ToWkt`

`std::string ToWkt() const`

WKT-like serialization, specific to this library: "POLYMESH ((x0 y0 z0, ..., x0 y0 z0), ...)" one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed by repeating its first point, comma-separated, wrapped once more in "POLYMESH ( ... )".

Not a standard OGC WKT geometry type.

**Returns** — The serialized mesh, one facet ring per facet, in Faces() order.

## `ToFile`

`void ToFile(std::string const & path) const`

Writes ToWkt() 's output to path (plain text, truncates any existing content).

**Parameters**

- `path` (`std::string const &`)


---

**See also:** [GeometryCollection3D](GeometryCollection3D.md), [GridCell3D](GridCell3D.md), [Mesh3D](Mesh3D.md), [Polygon3D](Polygon3D.md), [TriangulationParams](TriangulationParams.md)
