# `Mesh3D` (C# / .NET)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

No adjacency structure is stored to find a face's neighbors. Each facet has no holes (a triangle cannot have one).

## `FromTriangles`

**static** `Mesh3D^ FromTriangles(std::vector< `[`Triangle3D`](Triangle3D.md)` > triangles, AdjacencyConformity^ conformity)`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell3D](GridCell3D.md) ) into a single shared vertex.

**Parameters**

- `triangles` (std::vector< [`Triangle3D`](Triangle3D.md) >) — The triangles to weld into a mesh. Order is not required to reflect adjacency.
- `conformity` (`AdjacencyConformity^`) — decides how to handle input that doesn't make a valid adjacency (default to Assert, aka "make it fail if not perfect"; can be set to Enforce, aka "fix it if you can" via fix_adjacency(), or to Guaranteed to skip the check entirely and run at your own risk)

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `FromWkt`

**static** `Mesh3D^ FromWkt(System::String^ wkt)`

Parses a mesh written by ToWkt() (or matching its "MESH (((...)), ...)" grammar) back into a Mesh3D .

Every facet must have exactly 3 vertices (a mesh facet is always a triangle).

**Parameters**

- `wkt` (`System::String^`)

## `FromFile`

**static** `Mesh3D^ FromFile(System::String^ path)`

Reads a file written by ToFile() and parses it via FromWkt() .

**Parameters**

- `path` (`System::String^`)

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


## `Polygonize`

[`PolyMesh3D`](PolyMesh3D.md)`^ Polygonize(`[`PolygonizationParams`](PolygonizationParams.md)`^ params)`

Merges coplanar, edge-adjacent facets into polygons, per params.strategy see [PolygonizationParams](PolygonizationParams.md) for what each strategy guarantees.

**Parameters**

- `params` ([`PolygonizationParams`](PolygonizationParams.md)^) — Which polygonization strategy to run see [PolygonizationParams](PolygonizationParams.md)::Strategy .

**Returns** — A [PolyMesh3D](PolyMesh3D.md) of the merged polygon facets.

## `ToGeometryCollection`

[`GeometryCollection3D`](GeometryCollection3D.md)`^ ToGeometryCollection()`

Every facet, as its own standalone [Triangle3D](Triangle3D.md) , packaged into one [GeometryCollection3D](GeometryCollection3D.md) .

**Returns** — A [GeometryCollection3D](GeometryCollection3D.md) with Size() entries, all [Triangle3D](Triangle3D.md) , same order as Faces() .

## `ToWkt`

`System::String^ ToWkt()`

WKT-like serialization, specific to this library: "MESH ((x0 y0 z0, ..., x0 y0 z0), ...)" one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed by repeating its first point, comma-separated, wrapped once more in "MESH ( ... )".

Not a standard OGC WKT geometry type.

**Returns** — The serialized mesh, one triangle ring per facet, in Faces() order.

## `ToFile`

`void ToFile(System::String^ path)`

Writes ToWkt() 's output to path (plain text, truncates any existing content).

**Parameters**

- `path` (`System::String^`)


---

**See also:** [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection3D](GeometryCollection3D.md), [GridCell3D](GridCell3D.md), [PolyMesh3D](PolyMesh3D.md), [PolygonizationParams](PolygonizationParams.md), [Triangle3D](Triangle3D.md)
