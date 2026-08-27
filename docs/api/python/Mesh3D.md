# `Mesh3D` (Python)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

No adjacency structure is stored to find a face's neighbors. Each facet has no holes (a triangle cannot have one).

## `from_triangles`

**static** `from_triangles(triangles: list[`[`Triangle3D`](Triangle3D.md)`]) -> Mesh3D`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell3D](GridCell3D.md) ) into a single shared vertex.

**Parameters**

- `triangles` (list[`[Triangle3D`](Triangle3D.md)]) — The triangles to weld into a mesh. Order is not required to reflect adjacency.

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `from_wkt`

**static** `from_wkt(wkt: str) -> Mesh3D`

Parses a mesh written by ToWkt() (or matching its "MESH (((...)), ...)" grammar) back into a Mesh3D .

Every facet must have exactly 3 vertices (a mesh facet is always a triangle).

**Parameters**

- `wkt` (`str`)

## `from_file`

**static** `from_file(path: str) -> Mesh3D`

Reads a file written by ToFile() and parses it via FromWkt() .

**Parameters**

- `path` (`str`)

## `size`

`size() -> int`


## `area`

`area() -> float`


## `faces`

`faces() -> auto`

A lazy view of every facet, each rebuilt as a [Triangle3D](Triangle3D.md) on demand.

**Returns** — A std::ranges::view of [Triangle3D](Triangle3D.md) , one per facet, in the same order as the input.

## `connect`

`connect() -> `[`ConnectedMesh3D`](ConnectedMesh3D.md)


## `polygonize`

`polygonize(params: `[`PolygonizationParams`](PolygonizationParams.md)`) -> `[`PolyMesh3D`](PolyMesh3D.md)

Merges coplanar, edge-adjacent facets into polygons, per params.strategy see [PolygonizationParams](PolygonizationParams.md) for what each strategy guarantees.

**Parameters**

- `params` ([`PolygonizationParams`](PolygonizationParams.md)) — Which polygonization strategy to run see [PolygonizationParams](PolygonizationParams.md)::Strategy .

**Returns** — A [PolyMesh3D](PolyMesh3D.md) of the merged polygon facets.

## `to_geometry_collection`

`to_geometry_collection() -> `[`GeometryCollection3D`](GeometryCollection3D.md)

Every facet, as its own standalone [Triangle3D](Triangle3D.md) , packaged into one [GeometryCollection3D](GeometryCollection3D.md) .

**Returns** — A [GeometryCollection3D](GeometryCollection3D.md) with Size() entries, all [Triangle3D](Triangle3D.md) , same order as Faces() .

## `to_wkt`

`to_wkt() -> str`

WKT-like serialization, specific to this library: "MESH ((x0 y0 z0, ..., x0 y0 z0), ...)" one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed by repeating its first point, comma-separated, wrapped once more in "MESH ( ... )".

Not a standard OGC WKT geometry type.

**Returns** — The serialized mesh, one triangle ring per facet, in Faces() order.

## `to_file`

`to_file(path: str) -> None`

Writes ToWkt() 's output to path (plain text, truncates any existing content).

**Parameters**

- `path` (`str`)


---

**See also:** [ConnectedMesh3D](ConnectedMesh3D.md), [GeometryCollection3D](GeometryCollection3D.md), [GridCell3D](GridCell3D.md), [PolyMesh3D](PolyMesh3D.md), [PolygonizationParams](PolygonizationParams.md), [Triangle3D](Triangle3D.md)
