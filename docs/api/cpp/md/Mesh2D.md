# `Mesh2D` (C++)

A mesh made of adjacent triangles, stored as unique vertices plus a per-face index triple.

No adjacency structure is stored to find a face's neighbors. Each facet has no holes (a triangle cannot have one).

## `FromTriangles`

**static** `Mesh2D FromTriangles(std::vector< `[`Triangle2D`](Triangle2D.md)` > const & triangles, AdjacencyConformity conformity)`

Builds a mesh from a set of triangles, welding vertices that land in the same spatial grid cell (see [GridCell2D](GridCell2D.md) ) into a single shared vertex.

**Parameters**

- `triangles` (std::vector< [`Triangle2D`](Triangle2D.md) > const &) — The triangles to weld into a mesh. Order is not required to reflect adjacency.
- `conformity` (`AdjacencyConformity`) — decides how to handle input that doesn't make a valid adjacency (default to Assert, aka "make it fail if not perfect"; can be set to Enforce, aka "fix it if you can" via fix_adjacency(), or to Guaranteed to skip the check entirely and run at your own risk)

**Returns** — A mesh whose vertex count is ≤ 3 * triangles.size() (fewer once shared vertices are welded).

## `FromWkt`

**static** `Mesh2D FromWkt(std::string const & wkt)`

Parses a mesh written by ToWkt() (or matching its "MESH (((...)), ...)" grammar) back into a Mesh2D .

Every facet must have exactly 3 vertices (a mesh facet is always a triangle).

**Parameters**

- `wkt` (`std::string const &`)

## `FromFile`

**static** `Mesh2D FromFile(std::string const & path)`

Reads a file written by ToFile() and parses it via FromWkt() .

**Parameters**

- `path` (`std::string const &`)

## `Size`

`std::size_t Size() const`


## `Area`

`double Area() const`


## `Faces`

`auto Faces() const`

A lazy view of every facet, each rebuilt as a [Triangle2D](Triangle2D.md) on demand.

**Returns** — A std::ranges::view of [Triangle2D](Triangle2D.md) , one per facet, in the same order as the input.

## `Connect`

[`ConnectedMesh2D`](ConnectedMesh2D.md)` Connect() const`


## `Polygonize`

[`PolyMesh2D`](PolyMesh2D.md)` Polygonize(`[`PolygonizationParams`](PolygonizationParams.md)` const & params) const`

Merges coplanar, edge-adjacent facets into polygons, per params.strategy see [PolygonizationParams](PolygonizationParams.md) for what each strategy guarantees (planar boundary extraction, quads, or Hertel-Mehlhorn convex merging).

**Parameters**

- `params` ([`PolygonizationParams`](PolygonizationParams.md) const &) — Which polygonization strategy to run see [PolygonizationParams](PolygonizationParams.md)::Strategy .

**Returns** — A [PolyMesh2D](PolyMesh2D.md) of the merged polygon facets.

## `ToGeometryCollection`

[`GeometryCollection2D`](GeometryCollection2D.md)` ToGeometryCollection() const`

Every facet, as its own standalone [Triangle2D](Triangle2D.md) , packaged into one [GeometryCollection2D](GeometryCollection2D.md) .

**Returns** — A [GeometryCollection2D](GeometryCollection2D.md) with Size() entries, all [Triangle2D](Triangle2D.md) , same order as Faces() .

## `ToWkt`

`std::string ToWkt() const`

WKT-like serialization, specific to this library: "MESH ((x0 y0, x1 y1, x2 y2, x0 y0), ...)" one doubly-parenthesized ring per facet (the same syntax a bare POLYGON's own ring uses), closed by repeating its first point, comma-separated, wrapped once more in "MESH ( ... )".

Not a standard OGC WKT geometry type.

**Returns** — The serialized mesh, one triangle ring per facet, in Faces() order.

## `ToFile`

`void ToFile(std::string const & path) const`

Writes ToWkt() 's output to path (plain text, truncates any existing content).

**Parameters**

- `path` (`std::string const &`)


---

**See also:** [ConnectedMesh2D](ConnectedMesh2D.md), [GeometryCollection2D](GeometryCollection2D.md), [GridCell2D](GridCell2D.md), [PolyMesh2D](PolyMesh2D.md), [PolygonizationParams](PolygonizationParams.md), [Triangle2D](Triangle2D.md)
