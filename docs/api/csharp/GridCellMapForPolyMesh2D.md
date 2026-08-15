# `GridCellMapForPolyMesh2D` (C# / .NET)

Welds a set of polygons' vertices via GridCell2D bucketing into a unique-vertex list plus a flat variable-length face-index buffer (one run of indices per input polygon).

Backs [PolyMesh2D](PolyMesh2D.md)::FromPolygons .

## `Make`

**static** `GridCellMapForPolyMesh2D^ Make(std::vector< `[`Polygon2D`](Polygon2D.md)` > polygons)`


**Parameters**

- `polygons` (std::vector< [`Polygon2D`](Polygon2D.md) >)

## `GetUniques`

`std::shared_ptr< std::vector< `[`Point2D`](Point2D.md)` > > GetUniques()`

The vector of unique points registered (shared, not copied).

## `GetFaceIndices`

`std::shared_ptr< std::vector< std::size_t > > GetFaceIndices()`

The flat index buffer: every registered polygon's vertex indices, concatenated (shared, not copied).

## `GetFaceIdxBegins`

`std::shared_ptr< std::vector< std::size_t > > GetFaceIdxBegins()`

Each polygon's starting offset into GetFaceIndices (shared, not copied).

## `GetFaceIdxOffsets`

`std::shared_ptr< std::vector< std::size_t > > GetFaceIdxOffsets()`

Each polygon's vertex count, i.e. run length into GetFaceIndices (shared, not copied).


---

**See also:** [Point2D](Point2D.md), [Polygon2D](Polygon2D.md)
